use std::collections::HashMap;
use std::error::Error;
use std::net::SocketAddr;
use std::net::UdpSocket;

mod convert_types;
mod core;
mod generator;
mod types;

#[no_mangle]
extern "C" fn update_location(
    id: usize,
    old_location: core::ItemLocation,
    location: core::ItemLocation,
) {
    unsafe {
        LOCATION_UPDATES.push(types::LocationUpdateData {
            id,
            old: old_location,
            new: location,
        });
    }
}

#[no_mangle]
extern "C" fn notify_destroy(id: usize, location: core::ItemLocation) {
    unsafe {
        DESTROY_ITEMS.push((id, location));
    }
}

pub static mut SEED: i64 = 0;
pub static mut LOCATION_UPDATES: Vec<types::LocationUpdateData> = vec![];

pub static mut DESTROY_ITEMS: Vec<(usize, core::ItemLocation)> = vec![];

pub enum ClientEvent<'a> {
    Move {
        x: i32,
        y: i32,
    },
    Pickup {
        id: usize,
    },
    Drop {
        id: usize,
    },
    ItemUsedOnObject {
        iid: usize,
        oid: usize,
    },
    Craft {
        product_id: usize,
        ingredients_num: u32,
        ingredients_ids: &'a [u8],
    },
    ItemUsedOnTile {
        iid: usize,
        map_x: i32,
        map_y: i32,
        x: i32,
        y: i32,
    },
    RequestChunk {
        x: i32,
        y: i32,
    },
    Whatever,
}

pub struct Server {
    socket: UdpSocket,
    clients: HashMap<SocketAddr, usize>,
}

impl Server {
    fn broadcast(&self, data: &[u8]) {
        for client in self.clients.keys() {
            self.socket.send_to(data, client).unwrap();
        }
    }
}

impl<'a> From<&'a [u8]> for ClientEvent<'a> {
    fn from(value: &'a [u8]) -> Self {
        match value[0] {
            core::PACKET_PLAYER_MOVE => ClientEvent::Move {
                x: (value[1] as i8).into(),
                y: (value[2] as i8).into(),
            },
            core::PACKET_PLAYER_ACTION_PICKUP => ClientEvent::Pickup {
                id: usize::from_le_bytes(value[1..9].try_into().unwrap()),
            },
            core::PACKET_PLAYER_ACTION_DROP => ClientEvent::Drop {
                id: usize::from_le_bytes(value[1..9].try_into().unwrap()),
            },
            core::PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT => ClientEvent::ItemUsedOnObject {
                iid: usize::from_le_bytes(value[1..9].try_into().unwrap()),
                oid: usize::from_le_bytes(value[9..17].try_into().unwrap()),
            },
            core::PACKET_PLAYER_ACTION_CRAFT => ClientEvent::Craft {
                product_id: usize::from_le_bytes(value[1..9].try_into().unwrap()),
                ingredients_num: 2/*((value.len()-1)/8)*/ as u32,
                ingredients_ids: &value[9..],
            },
            core::PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE => ClientEvent::ItemUsedOnTile {
                iid: usize::from_le_bytes(value[1..9].try_into().unwrap()),
                map_x: i32::from_le_bytes(value[9..13].try_into().unwrap()),
                map_y: i32::from_le_bytes(value[13..17].try_into().unwrap()),
                x: i32::from_le_bytes(value[17..21].try_into().unwrap()),
                y: i32::from_le_bytes(value[21..25].try_into().unwrap()),
            },
            core::PACKET_REQUEST_CHUNK => ClientEvent::RequestChunk {
                x: i32::from_le_bytes(value[1..5].try_into().unwrap()),
                y: i32::from_le_bytes(value[5..9].try_into().unwrap()),
            },
            1 => ClientEvent::Whatever,
            _ => panic!("invalid event {:?}", value),
        }
    }
}

pub fn init_server() -> Result<Server, Box<dyn Error>> {
    let socket = UdpSocket::bind("127.0.0.1:1234")?;
    socket.set_nonblocking(true).unwrap();
    generator::generate();

    Ok(Server {
        socket,
        clients: HashMap::new(),
    })
}

pub fn main_loop(server: &mut Server) {
    let mut players: Vec<core::PlayerServer> = vec![];
    loop {
        handle_network(server, &mut players);
        unsafe {
            core::update();
        }
        send_game_updates(server, &mut players);
        std::thread::sleep(std::time::Duration::from_millis(100));
    }
}

fn add_player(
    server: &mut Server,
    mut peer: std::net::SocketAddr,
    players: &mut Vec<core::PlayerServer>,
) {
    // TODO
    //peer.set_data(Some(players.len()));
    server.clients.insert(peer, players.len());

    let mut response = vec![core::PACKET_PLAYER_ID as u8];
    response.extend_from_slice(&players.len().to_le_bytes());
    let data = generator::get_world_data();
    response.extend_from_slice(&data);
    println!("SEND {:?}", data.len());
    generator::show_world();

    server.socket.send_to(&response, peer).unwrap();

    unsafe {
        let p = core::PlayerServer::new(players.len() as i32);
        players.push(p);
    }
    //println!("{:?} , players {:?}", peer, players);

    update_chunk_for_player(server, &mut peer, (128, 128));
}

fn update_players(server: &Server, players: &mut Vec<core::PlayerServer>) {
    for (i, p) in players.iter().enumerate() {
        let mut data = [0 as u8; 25];
        data[0] = core::PACKET_PLAYER_UPDATE;
        data[1..9].clone_from_slice(&i.to_le_bytes());
        data[9..13].clone_from_slice(&p._base.map_x.to_le_bytes());
        data[13..17].clone_from_slice(&p._base.map_y.to_le_bytes());
        data[17..21].clone_from_slice(&p._base.x.to_le_bytes());
        data[21..25].clone_from_slice(&p._base.y.to_le_bytes());

        server.broadcast(&data);
    }
}

fn update_chunk_for_player(server: &Server, peer: &SocketAddr, coords: (u8, u8)) {
    let mut data = vec![
        0 as u8;
        3 + (core::CHUNK_SIZE * core::CHUNK_SIZE) as usize
            * size_of::<core::game_tiles>()
    ];
    data[0] = core::PACKET_CHUNK_UPDATE;
    data[1] = coords.0;
    data[2] = coords.1;

    unsafe {
        let mut chunk = *core::world_table[coords.1 as usize][coords.0 as usize];
        let tile = core::tile { tile: 1 };
        chunk.table[5][5] = tile;
        let table = chunk.table.as_mut_ptr() as *mut u8;
        let table = std::slice::from_raw_parts(table, 1024);

        let dest = data[3..].as_mut_ptr();
        let dest = std::slice::from_raw_parts_mut(dest, 1024);
        dest.clone_from_slice(table);
    }

    server.socket.send_to(&data, peer).unwrap();
    create_objects_in_chunk_for_player(server, peer, coords);
}

fn create_objects_in_chunk_for_player(server: &Server, peer: &SocketAddr, coords: (u8, u8)) {
    let chunk;
    unsafe {
        chunk = *core::world_table[coords.1 as usize][coords.0 as usize];

        let mut le = chunk.objects._base.head;
        while le != std::ptr::null_mut() {
            let mut data = vec![core::PACKET_OBJECT_CREATE];
            let obj = convert_types::convert_to_data(&*(*le).el);
            let obj_data = &bincode::serialize(&obj).unwrap()[..];
            //          println!("data {:?}", obj_data);
            data.extend_from_slice(obj_data);

            le = (*le).next;
            server.socket.send_to(&data, peer).unwrap();
        }
    }
}

fn handle_network(server: &mut Server, players: &mut Vec<core::PlayerServer>) {
    let mut buf = [0; 100];
    loop {
        if let Ok((amt, src)) = server.socket.recv_from(&mut buf) {
            if amt == 0 {
                break;
            }
            match buf[0] {
                core::PACKET_JOIN_REQUEST => {
                    println!("connected");
                    add_player(server, src, players);
                }
                _ => match server.clients.get(&src) {
                    Some(id) => {
                        if *id < players.len() {
                            handle_packet(server, &mut players[*id], &buf, *id, &src);
                        } else {
                            println!("invalid player idx {}", *id);
                        }
                    }
                    None => println!("player without idx"),
                },
            }
        } else {
            break;
        }
    }
}

fn handle_packet(
    server: &Server,
    player: &mut core::PlayerServer,
    packet: &[u8],
    _player_id: usize,
    peer: &SocketAddr,
) {
    let tag = ClientEvent::from(packet);
    match tag {
        ClientEvent::Move { x, y } => {
            println!("moved {x} {y}");
            unsafe { player.move_(x, y) }
        }
        ClientEvent::Pickup { id } => {
            println!("player picked {id}");
            unsafe {
                let item = (*core::world_table[player._base.map_y as usize]
                    [player._base.map_x as usize])
                    .find_by_id(id);
                if item != std::ptr::null_mut() {
                    player.pickup(item);
                }
                //let mut buf = vec![core::PACKET_PLAYER_ACTION_PICKUP];
                //buf.extend_from_slice(&id.to_le_bytes());
                //buf.extend_from_slice(&player_id.to_le_bytes());
                //server.broadcast(&buf);
            }
        }
        ClientEvent::Drop { id } => {
            println!("player dropped {id}");
            unsafe {
                let item = player._base.get_item_by_uid(id);
                if item != std::ptr::null_mut() {
                    let loc = (*item).location;
                    (*core::world_table[player._base.map_y as usize][player._base.map_x as usize])
                        .add_object(item, player._base.x, player._base.y);
                    player._base.drop(item);
                    core::update_location((*item).uid, loc, (*item).location);
                    //let mut buf = vec![core::PACKET_PLAYER_ACTION_DROP];
                    //buf.extend_from_slice(&id.to_le_bytes());
                    //buf.extend_from_slice(&player_id.to_le_bytes());
                    //server.broadcast(&buf);
                }
            }
        }
        ClientEvent::ItemUsedOnObject { iid, oid } => {
            println!("player used {iid} on {oid}");
            unsafe {
                let item = player._base.get_item_by_uid(iid);
                let object = (*core::world_table[player._base.map_y as usize]
                    [player._base.map_x as usize])
                    .find_by_id(oid);
                if !player.use_item_on_object(item, object) {
                    let response = [core::PACKET_FAILED_CRAFT];
                    server.socket.send_to(&response, peer).unwrap();
                }
            }
        }
        ClientEvent::Craft {
            product_id,
            ingredients_num,
            ingredients_ids,
        } => {
            unsafe {
                if core::craft(
                    product_id as i32,
                    ingredients_num as i32,
                    ingredients_ids.as_ptr() as *const usize,
                    (&mut player._base) as *mut core::Player,
                ) {
                    let id = usize::from_le_bytes(ingredients_ids[0..8].try_into().unwrap());
                    let el = player._base.get_item_by_uid(id);
                    println!("id after CRAFT{:?}", el);
                    if !el.is_null() {
                        destroy_object(server, (*el).uid, (*el).location);
                        player._base.drop(el);
                        println!("deleted {}", id);
                    } else {
                        println!("invalid id {}", id);
                    }
                    if product_id >= 4 {
                        let id2 = usize::from_le_bytes(ingredients_ids[8..16].try_into().unwrap());
                        let el2 = player._base.get_item_by_uid(id2);
                        if !el2.is_null() {
                            destroy_object(server, (*el2).uid, (*el2).location);
                            player._base.drop(el2);
                        } else {
                            println!("invalid id2 {}", id2);
                        }
                    }
                } else {
                    let response = [core::PACKET_FAILED_CRAFT];
                    server.socket.send_to(&response, peer).unwrap();
                }
            };
        }
        ClientEvent::ItemUsedOnTile {
            iid,
            map_x,
            map_y,
            x,
            y,
        } => unsafe {
            let item = player._base.get_item_by_uid(iid);
            if player.plant_with_seed(item, map_x, map_y, x, y) {
                println!("planted OK");
            } else {
                println!("failed to plant");
            }
        },
        ClientEvent::RequestChunk { x, y } => unsafe {
            println!("received {} {}", x, y);
            if !(x < 0 || x > core::WORLD_SIZE as i32 || y < 0 || y > core::WORLD_SIZE as i32) {
                if core::world_table[y as usize][x as usize] != std::ptr::null_mut() {
                    update_chunk_for_player(server, peer, (x as u8, y as u8));
                    println!("request received {} {}", x, y);
                }
            }
        },
        ClientEvent::Whatever => println!("whatever"),
    }
}

fn send_game_updates(server: &Server, players: &mut Vec<core::PlayerServer>) {
    update_players(server, players);
    unsafe {
        let list = std::ptr::addr_of_mut!(core::objects_to_create);
        let mut le = (*list)._base.head;
        while le != std::ptr::null_mut() {
            let mut data = vec![core::PACKET_OBJECT_CREATE];
            let obj = convert_types::convert_to_data(&*(*le).el);
            let obj_data = &bincode::serialize(&obj).unwrap()[..];
            //            println!("data {:?}", obj_data);
            data.extend_from_slice(obj_data);

            le = (*le).next;
            server.broadcast(&data);
        }

        while (*list)._base.head != std::ptr::null_mut() {
            (*list).remove((*(*list)._base.head).el);
        }

        let el = std::ptr::addr_of_mut!(core::objects_to_update);
        let mut le = (*el)._base.head;
        while le != std::ptr::null_mut() {
            let mut data = vec![core::PACKET_OBJECT_UPDATE];
            let obj = convert_types::convert_to_data(&*(*le).el);
            let obj_data = &bincode::serialize(&obj).unwrap()[..];
            //            println!("data {:?}", obj_data);
            data.extend_from_slice(obj_data);

            le = (*le).next;
            server.broadcast(&data);
        }
        while (*el)._base.head != std::ptr::null_mut() {
            (*el).remove((*(*el)._base.head).el);
        }
    }
    send_location_updates(server);
    send_destroy_updates(server);
}

fn send_location_updates(server: &Server) {
    unsafe {
        if LOCATION_UPDATES.len() > 0 {
            for update in LOCATION_UPDATES.iter() {
                let mut data = vec![core::PACKET_LOCATION_UPDATE];
                data.extend_from_slice(&bincode::serialize(update).unwrap()[..]);
                server.broadcast(&data);
            }
            LOCATION_UPDATES.clear();
        }
    }
}

fn send_destroy_updates(server: &Server) {
    unsafe {
        if DESTROY_ITEMS.len() > 0 {
            for (id, location) in DESTROY_ITEMS.iter() {
                destroy_object(server, *id, *location);
            }
            DESTROY_ITEMS.clear();
        }
    }
}

fn destroy_object(server: &Server, id: usize, location: core::ItemLocation) {
    let mut buf = vec![core::PACKET_OBJECT_DESTROY];
    buf.extend_from_slice(&id.to_le_bytes());
    buf.extend_from_slice(&bincode::serialize(&location).unwrap()[..]);
    server.broadcast(&buf);
}
