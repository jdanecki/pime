use std::error::Error;
use std::net::UdpSocket;

mod core;
mod events;
mod generator;
mod send_packets;
mod types;

#[repr(C)]
pub struct NetClient {
    socket: UdpSocket,
}

impl NetClient {
    fn send(&self, data: &[u8]) {
        self.socket.send(data).unwrap();
    }
}

#[no_mangle]
pub extern "C" fn init() -> *const NetClient {
    unsafe {
        let a = core::Player::new(1);
    }
    Box::into_raw(Box::new(init_internal().expect("failed to init NetClient")))
}

fn init_internal() -> Result<NetClient, Box<dyn Error>> {
    let socket = UdpSocket::bind("127.0.0.1:0")?;
    socket.connect("127.0.0.1:1234")?;
    let mut buf = [0];
    buf[0] = core::PACKET_JOIN_REQUEST;
    socket.send(&buf)?;
    let mut buf = [0; 17];
    socket.recv(&mut buf)?;
    if buf[0] == core::PACKET_PLAYER_ID {
        unsafe {
            events::got_id(
                usize::from_le_bytes(buf[1..9].try_into().unwrap()),
                i64::from_le_bytes(buf[9..17].try_into().unwrap()),
            );
        };
    } else {
        println!("did not get id");
        panic!();
    }

    socket.set_nonblocking(true)?;

    Ok(NetClient { socket })
}

// #[no_mangle]
// pub extern "C" fn foo(a: &NetClient) {
//     println!("{:?}", a.socket);
//     a.socket.send(&[1, 2, 3]).expect("foo");
// }

#[no_mangle]
pub extern "C" fn network_tick(client: &NetClient) {
    let socket = &client.socket;

    let mut buf = [0; 2048];
    loop {
        if let Ok((amt, _src)) = socket.recv_from(&mut buf) {
            //println!("{:?}", &buf);
            let value = &mut buf;
            //println!("{:?}", &value);
            match value[0] {
                core::PACKET_PLAYER_UPDATE => {
                    if amt == 25 {
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
                    if amt == size_of::<core::chunk_table>() + 3 {
                        unsafe {
                            println!("OK");
                            events::update_chunk(
                                //i32::from_le_bytes(value[1..5].try_into().unwrap()),
                                //i32::from_le_bytes(value[5..9].try_into().unwrap()),
                                i32::from(value[1]),
                                i32::from(value[2]),
                                &mut *(&mut value[3] as *mut u8 as *mut core::chunk_table),
                            )
                        }
                    } else {
                        println!("BAD");
                    }
                }
                core::PACKET_OBJECT_UPDATE => unsafe {
                    events::update_object(
                        bincode::deserialize(&value[1..amt]).expect("bad object update"),
                    );
                },
                core::PACKET_LOCATION_UPDATE => unsafe {
                    events::update_item_location(
                        bincode::deserialize(&value[1..amt]).expect("bad location update"),
                    );
                },
                core::PACKET_OBJECT_CREATE => unsafe {
                    //println!("value {:?}", &value[1..amt]);
                    events::create_object(
                        bincode::deserialize(&value[1..amt])
                            .expect("Failed to create item from data"),
                    );
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
