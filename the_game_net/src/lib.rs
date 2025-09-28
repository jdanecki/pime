use nix::libc::{c_int, FIONREAD};
use std::cell::RefCell;
use std::collections::HashMap;
use std::error::Error;
use std::ffi::CStr;
use std::mem::MaybeUninit;
use std::net::UdpSocket;
use std::os::unix::io::AsRawFd;
use std::rc::Rc;
use std::sync::{Arc, Mutex, RwLock};
use types::ObjectData;

mod core;
mod events;
mod send_packets;
mod types;

/// cbindgen:no-export
#[repr(C)]
pub struct NetClient {
    socket: UdpSocket,
    local_seq_num: u32,
    remote_seq_num: u32,
    my_acks_bitmap: u32,
    unsent_acks: u32,
    not_confirmed_packets: HashMap<u32, Vec<u8>>,
}

static mut total_sent: usize = 0;

impl NetClient {
    fn send(&mut self, data: &[u8]) {
        let mut buf = Vec::with_capacity(12 + data.len());
        self.local_seq_num += 1;
        buf.extend_from_slice(&self.local_seq_num.to_le_bytes());
        buf.extend_from_slice(&self.remote_seq_num.to_le_bytes());
        buf.extend_from_slice(&self.my_acks_bitmap.to_le_bytes());
        buf.extend_from_slice(&data);
        unsafe {
            if core::trace_network > 0 {
                println!(
                    "SDL: send len={} local_seq={} remote_seq={}",
                    12 + data.len(),
                    self.local_seq_num,
                    self.remote_seq_num
                );
                core::show_packet_type_name('S' as i8, data[0] as u8);
            }
        }
        self.not_confirmed_packets
            .insert(self.local_seq_num, data.to_vec());
        self.socket.send(&buf).unwrap();
        unsafe {
            total_sent += 1;
        }
    }
}

#[no_mangle]
pub extern "C" fn init(
    server_ip: *const std::os::raw::c_char,
    port: *const std::os::raw::c_char,
) -> *mut NetClient {
    OBJECTS.write().unwrap().replace(HashMap::new());
    let mut ip;
    unsafe {
        // let a = core::Player::new(1);
        ip = CStr::from_ptr(server_ip).to_str().unwrap().to_owned();
        ip.push(':');
        ip.push_str(CStr::from_ptr(port).to_str().unwrap())
    }
    let client = Box::new(init_internal(&ip).expect("failed to init NetClient"));

    let mut buf = [0; 13];
    buf[12] = core::PACKET_JOIN_REQUEST;
    client.socket.send(&buf).unwrap();

    let mut buf = [0; 8192];
    let mut time_to_resend = 50;
    loop {
        let amt = client.socket.recv(&mut buf).unwrap();
        unsafe {
            if core::trace_network > 0 {
                println!("{:?}", buf[12]);
            }
        }
        let buf = &buf[12..];
        let amt = amt - 12;
        if buf[0] == core::PACKET_PLAYER_ID {
            unsafe {
                events::got_id(
                    usize::from_le_bytes(buf[1..9].try_into().unwrap()),
                    0, //i64::from_le_bytes(buf[9..17].try_into().unwrap()),
                );
                if core::trace_network > 0 {
                    println!("RECEIVED {:?}", &buf[9..amt]);
                }
                println!("{:?}", buf);
                WORLD.set(
                    bincode::deserialize::<World>(&buf[9..amt]).expect("failed to create world"),
                );
                WORLD.with_borrow(|world| {
                    //                    println!("{:#?}", world);
                });
                break;
            };
        } else {
            println!("did not get id");
            time_to_resend -= 1;
            if time_to_resend == 0 {
                let mut buf = [0; 13];
                buf[12] = core::PACKET_JOIN_REQUEST;
                client.socket.send(&buf).unwrap();
                println!("RESENT REQUEST");
            }
        }
    }

    client.socket.set_nonblocking(true).unwrap();

    Box::into_raw(client)
}

fn init_internal(server_ip: &str) -> Result<NetClient, Box<dyn Error>> {
    let socket = UdpSocket::bind("0.0.0.0:0")?;
    println!("{server_ip}");
    socket.connect(server_ip)?;

    Ok(NetClient {
        socket,
        local_seq_num: 0,
        remote_seq_num: 0,
        my_acks_bitmap: 0,
        unsent_acks: 0,
        not_confirmed_packets: HashMap::new(),
    })
}

// #[no_mangle]
// pub extern "C" fn foo(a: &NetClient) {
//     println!("{:?}", a.socket);
//     a.socket.send(&[1, 2, 3]).expect("foo");
// }

fn get_pending_bytes(socket: &UdpSocket) -> std::io::Result<usize> {
    let fd = socket.as_raw_fd();
    let mut bytes: c_int = 0;

    let res = unsafe { nix::libc::ioctl(fd, FIONREAD, &mut bytes) };

    if res < 0 {
        Err(std::io::Error::last_os_error())
    } else {
        Ok(bytes as usize)
    }
}

static mut max_pending: usize = 0;
static mut total_resv: usize = 0;
static mut prev_resv: usize = 0;
static mut prev_send: usize = 0;

#[no_mangle]
pub extern "C" fn network_tick(client: &mut NetClient) -> u32 {
    let mut buf = [0; 8096];
    let mut cnt = 0;
    loop {
        if let Ok(pending) = get_pending_bytes(&client.socket) {
            unsafe {
                if pending > max_pending {
                    println!("Bufor kernela zawiera {} max={}", pending, max_pending);
                    max_pending = pending;
                }
            }
        }

        if let Ok((amt, src)) = client.socket.recv_from(&mut buf) {
            cnt += 1;
            unsafe {
                total_resv += 1;
            }
            let seq = u32::from_le_bytes(buf[0..4].try_into().unwrap());
            let ack = u32::from_le_bytes(buf[4..8].try_into().unwrap());
            let acks = u32::from_le_bytes(buf[8..12].try_into().unwrap());
            unsafe {
                if core::trace_network > 0 {
                    println!(
                        "SDL: network_tick amt={amt} seq={seq} remote_seq_num={} ack={ack}",
                        client.remote_seq_num
                    );
                }
            }
            if seq > client.remote_seq_num {
                let diff = seq - client.remote_seq_num;
                client.my_acks_bitmap = (client.my_acks_bitmap << 1) | 1;
                if diff > 1 && diff < 32 {
                    client.my_acks_bitmap = client.my_acks_bitmap << (diff - 1);
                }
                // for i in 0..diff - 1 {
                //     if client.acks_bitmap & (1 << (31 - i)) > 0 {
                //         println!("PACKET NOT CONFIRMED {}", client.remote_seq_num - (31 - i));
                //     }
                // }
                client.remote_seq_num = seq;
            } else if seq < client.remote_seq_num {
                let diff = client.remote_seq_num - seq;
                if diff <= 32 {
                    client.my_acks_bitmap = client.my_acks_bitmap | (1 << (diff - 1));
                }
            } else {
                panic!("Shouldn't ever receive 2 packets with same seq_num");
            }
            // Detect packet loss
            for i in 0..31 {
                if (acks & (1 << i)) > 0 {
                    client.not_confirmed_packets.remove(&(ack - i));
                }
            }
            client.not_confirmed_packets.remove(&ack);

            let mut to_remove = vec![];
            let mut to_resend = vec![];
            for (&id, d) in client.not_confirmed_packets.iter() {
                if id + 32 < ack {
                    println!("SDL: PACKET NOT CONFIRMED id={id} ack={ack}");
                    unsafe {
                        core::trace_network = 1;
                        core::show_packet_type_name('S' as i8, d[0] as u8);
                        core::trace_network = 0;
                    }

                    to_resend.push(d.clone());
                    to_remove.push(id);
                }
            }
            for i in to_remove {
                client.not_confirmed_packets.remove(&i);
            }
            //FIXME should we uncomment it now?
            /*   for d in to_resend {
                client.send(&d);
            }*/

            client.unsent_acks += 1;
            if client.unsent_acks > 20 {
                client.send(&vec![core::PACKET_KEEP_ALIVE as u8]);
                client.unsent_acks = 0;
            }

            //println!("{:?}", &buf);
            let value = &mut buf[12..];
            let amt = amt - 12;

            //FIXME
            unsafe {
                core::show_packet_type_name('C' as i8, value[0] as u8);
            }
            match value[0] {
                core::PACKET_PLAYER_UPDATE => {
                    if amt == (33 + 12) {
                        unsafe {
                            events::update_player(
                                usize::from_le_bytes(value[1..9].try_into().unwrap()),
                                i32::from_le_bytes(value[9..13].try_into().unwrap()),
                                i32::from_le_bytes(value[13..17].try_into().unwrap()),
                                i32::from_le_bytes(value[17..21].try_into().unwrap()),
                                i32::from_le_bytes(value[21..25].try_into().unwrap()),
                                i32::from_le_bytes(value[25..29].try_into().unwrap()),
                                i32::from_le_bytes(value[29..33].try_into().unwrap()),
                            );
                        }
                    } else {
                        println!("invalid player update");
                    }
                }
                core::PACKET_CHUNK_UPDATE => {
                    unsafe {
                        if core::trace_network > 0 {
                            println!("SDL: PACKET_CHUNK_UPDATE {}", amt);
                        }
                    }
                    if amt == size_of::<core::chunk_table>() + 3 {
                        unsafe {
                            events::update_chunk(
                                //i32::from_le_bytes(value[1..5].try_into().unwrap()),
                                //i32::from_le_bytes(value[5..9].try_into().unwrap()),
                                i32::from(value[1]),
                                i32::from(value[2]),
                                &mut *(&mut value[0] as *mut u8 as *mut core::chunk_table),
                            );
                            unsafe {
                                if core::trace_network > 0 {
                                    println!("SDL: PACKET_CHUNK_UPDATE OK");
                                }
                            }
                        }
                    } else {
                        println!("SDL: PACKET_CHUNK_UPDATE BAD");
                    }
                }
                core::PACKET_OBJECT_UPDATE => unsafe {
                    let updates: Vec<ObjectData> = bincode::deserialize(&value[1..amt])
                        .expect(&format!("bad object update size {}", amt));
                    for u in updates {
                        events::update_object(&u);
                    }
                },
                core::PACKET_LOCATION_UPDATE => unsafe {
                    events::update_item_location(
                        bincode::deserialize(&value[1..amt]).expect("bad location update"),
                    );
                },
                core::PACKET_OBJECT_CREATE => unsafe {
                    println!("len {}, value {:?}", amt, &value[1..amt]);
                    let obj: Vec<ObjectData> =
                        bincode::deserialize(&value[1..amt]).expect(&format!(
                            "Failed to create item from data amt {} {:?}",
                            amt,
                            &value[1..amt]
                        ));
                    println!("{:?}", obj);
                    for o in obj {
                        events::create_object(&o);
                    }
                    // events::create_object(
                    //     bincode::deserialize(&value[1..amt])
                    //         .expect("Failed to create item from data"),
                    // );
                },
                core::PACKET_OBJECT_DESTROY => unsafe {
                    events::destroy_object(
                        usize::from_le_bytes(value[1..9].try_into().unwrap()),
                        bincode::deserialize(&value[9..]).expect("bad item location for destroy"),
                    );
                },
                core::PACKET_FAILED_CRAFT => unsafe {
                    events::failed_craft();
                },
                core::PACKET_ACTION_FAILED => unsafe {
                    events::action_failed();
                },
                core::PACKET_KNOWLEDGE_UPDATE => unsafe {
                    events::knowledge_update(
                        i32::from_le_bytes(value[1..5].try_into().unwrap()),
                        u32::from_le_bytes(value[5..9].try_into().unwrap()),
                        i32::from_le_bytes(value[9..13].try_into().unwrap()),
                    );
                },
                core::PACKET_CHECKED_UPDATE => unsafe {
                    events::checked_update(
                        i32::from_le_bytes(value[1..5].try_into().unwrap()),
                        usize::from_le_bytes(value[5..13].try_into().unwrap()),
                    );
                },

                _ => {
                    println!("invalid packet type {:?}", value);
                }
            }
        } else {
            break;
        }
    }
    /*
    unsafe {
        if prev_send != total_sent || prev_resv != total_resv
        {
            println!("sent {} recv {}", total_sent, total_resv);
            prev_send = total_sent;
            prev_resv = total_resv;
        }
    }*/
    cnt
}

pub fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[derive(Debug, serde::Deserialize)]
struct World {
    terrains: Vec<Rc<core::BaseElement>>,
    plants: Vec<Rc<core::BasePlant>>,
    animals: Vec<Rc<core::BaseAnimal>>,
}

thread_local! {
static WORLD: RefCell<World> = panic!("world not created yet");
}

struct CorePointer(*mut core::InventoryElement);
unsafe impl Send for CorePointer {}
unsafe impl Sync for CorePointer {}

static OBJECTS: RwLock<Option<HashMap<usize, CorePointer>>> = RwLock::new(None);

#[no_mangle]
pub extern "C" fn get_object_by_id(uid: usize) -> *mut core::InventoryElement {
    match OBJECTS.read().unwrap().as_ref().unwrap().get(&uid) {
        Some(obj) => obj.0,
        None => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn register_object(o: *mut core::InventoryElement) {
    let uid = unsafe { (*o)._base.uid };
    OBJECTS
        .write()
        .unwrap()
        .as_mut()
        .unwrap()
        .insert(uid, CorePointer(o));
}

#[no_mangle]
pub extern "C" fn deregister_object(o: *mut core::InventoryElement) {
    let uid = unsafe { (*o)._base.uid };
    OBJECTS.write().unwrap().as_mut().unwrap().remove(&uid);
}

#[no_mangle]
pub extern "C" fn get_base_element(id: i32) -> *mut core::BaseElement {
    WORLD.with_borrow(|world| {
        Rc::downgrade(&world.terrains[id as usize].clone())
            .as_ptr()
            .cast_mut()
    })
}

#[no_mangle]
pub extern "C" fn get_base_plant(id: i32) -> *mut core::BasePlant {
    WORLD.with_borrow(|world| {
        Rc::downgrade(&world.plants[id as usize].clone())
            .as_ptr()
            .cast_mut()
    })
}

#[no_mangle]
pub extern "C" fn get_base_animal(id: i32) -> *mut core::BaseAnimal {
    WORLD.with_borrow(|world| {
        Rc::downgrade(&world.animals[id as usize].clone())
            .as_ptr()
            .cast_mut()
    })
}
#[no_mangle]
pub extern "C" fn get_base(c_id: u32, id: i32) -> *mut core::Base {
    match c_id {
        core::Class_id_Class_BaseElement => get_base_element(id) as *mut core::Base,
        core::Class_id_Class_BasePlant => get_base_plant(id) as *mut core::Base,
        core::Class_id_Class_BaseAnimal => get_base_animal(id) as *mut core::Base,
        _ => panic!("get_base: wrong c_id"),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
