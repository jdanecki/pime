use std::borrow::{Borrow, BorrowMut};
use std::cell::RefCell;
use std::collections::HashMap;
use std::error::Error;
use std::net::UdpSocket;
use std::rc::Rc;

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

impl NetClient {
    fn send(&mut self, data: &[u8]) {
        let mut buf = Vec::with_capacity(12 + data.len());
        self.local_seq_num += 1;
        buf.extend_from_slice(&self.local_seq_num.to_le_bytes());
        buf.extend_from_slice(&self.remote_seq_num.to_le_bytes());
        buf.extend_from_slice(&self.my_acks_bitmap.to_le_bytes());
        buf.extend_from_slice(&data);
        self.not_confirmed_packets
            .insert(self.local_seq_num, buf.to_vec());
        self.socket.send(&buf).unwrap();
    }
}

#[no_mangle]
pub extern "C" fn init() -> *mut NetClient {
    unsafe {
        let a = core::Player::new(1);
    }
    let client = Box::new(init_internal().expect("failed to init NetClient"));

    let mut buf = [0; 13];
    buf[12] = core::PACKET_JOIN_REQUEST;
    client.socket.send(&buf).unwrap();
    let mut buf = [0; 4096];
    let amt = client.socket.recv(&mut buf).unwrap();
    println!("{:?}", buf);

    let buf = &buf[12..];

    if buf[0] == core::PACKET_PLAYER_ID {
        unsafe {
            events::got_id(
                usize::from_le_bytes(buf[1..9].try_into().unwrap()),
                0, //i64::from_le_bytes(buf[9..17].try_into().unwrap()),
            );
            println!("RECEIVED {:?}", &buf[9..amt]);
            WORLD.set(bincode::deserialize::<World>(&buf[9..amt]).expect("failed to create world"));
            WORLD.with_borrow(|world| {
                println!("{:#?}", world);
            })
        };
    } else {
        println!("did not get id");
        panic!();
    }

    client.socket.set_nonblocking(true).unwrap();

    Box::into_raw(client)
}

fn init_internal() -> Result<NetClient, Box<dyn Error>> {
    let socket = UdpSocket::bind("127.0.0.1:0")?;
    socket.connect("127.0.0.1:1234")?;
    // let socket = UdpSocket::bind("0.0.0.0:0")?;
    // socket.connect("141.147.35.247:1234")?;

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

#[no_mangle]
pub extern "C" fn network_tick(client: &mut NetClient) {
    let mut buf = [0; 2048];
    loop {
        if let Ok((amt, _src)) = client.socket.recv_from(&mut buf) {
            let seq = u32::from_le_bytes(buf[0..4].try_into().unwrap());
            let ack = u32::from_le_bytes(buf[4..8].try_into().unwrap());
            let acks = u32::from_le_bytes(buf[8..12].try_into().unwrap());
            if seq > client.remote_seq_num {
                let diff = seq - client.remote_seq_num;
                client.my_acks_bitmap = (client.my_acks_bitmap << 1) | 1;
                if diff > 1 {
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
                    println!("PACKET NOT CONFIRMED {}", id);
                    to_resend.push(d.clone());
                    to_remove.push(id);
                }
            }
            for i in to_remove {
                client.not_confirmed_packets.remove(&i);
            }
            for d in to_resend {
                client.send(&d);
            }
            client.unsent_acks += 1;
            if client.unsent_acks > 20 {
                client.send(&vec![core::PACKET_KEEP_ALIVE as u8]);
                client.unsent_acks = 0;
            }

            //println!("{:?}", &buf);
            let value = &mut buf[12..];
            // println!("{seq} {}", value[0]);
            match value[0] {
                core::PACKET_PLAYER_UPDATE => {
                    if amt == (25 + 12) {
                        unsafe {
                            events::update_player(
                                usize::from_le_bytes(value[1..9].try_into().unwrap()),
                                i32::from_le_bytes(value[9..13].try_into().unwrap()),
                                i32::from_le_bytes(value[13..17].try_into().unwrap()),
                                i32::from_le_bytes(value[17..21].try_into().unwrap()),
                                i32::from_le_bytes(value[21..25].try_into().unwrap()),
                            );
                        }
                    } else {
                        println!("invalid player update");
                    }
                }
                core::PACKET_CHUNK_UPDATE => {
                    println!("chunk update {}", amt);
                    if amt == size_of::<core::chunk_table>() + 3 + 12 {
                        unsafe {
                            println!("OK");
                            events::update_chunk(
                                //i32::from_le_bytes(value[1..5].try_into().unwrap()),
                                //i32::from_le_bytes(value[5..9].try_into().unwrap()),
                                i32::from(value[1]),
                                i32::from(value[2]),
                                &mut *(&mut value[0] as *mut u8 as *mut core::chunk_table),
                            )
                        }
                    } else {
                        println!("BAD");
                    }
                }
                core::PACKET_OBJECT_UPDATE => unsafe {
                    events::update_object(
                        &bincode::deserialize(&value[1..amt]).expect("bad object update"),
                    );
                },
                core::PACKET_LOCATION_UPDATE => unsafe {
                    events::update_item_location(
                        bincode::deserialize(&value[1..amt]).expect("bad location update"),
                    );
                },
                core::PACKET_OBJECT_CREATE => unsafe {
                    // println!("value {:?}", &value[1..amt]);
                    let obj = bincode::deserialize(&value[1..amt])
                        .expect("Failed to create item from data");
                    // println!("{:?}", obj);
                    events::create_object(&obj);
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
                _ => {
                    println!("invalid packet type {:?}", value);
                }
            }
        } else {
            break;
        }
    }
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
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
