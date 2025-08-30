use core::add_object_to_world;
use core::find_in_world;
use std::collections::HashMap;
use std::error::Error;
use std::net::SocketAddr;
use std::net::UdpSocket;
use std::slice;

use convert_types::convert_to_data;

mod convert_types;
mod core;
mod generator;
mod types;

pub static mut SEED: i64 = 0;
pub static mut LOCATION_UPDATES: Vec<types::LocationUpdateData> = vec![];
pub static mut OBJECT_UPDATES: Vec<types::ObjectData> = vec![];

pub static mut DESTROY_ITEMS: Vec<(usize, core::ItemLocation)> = vec![];
pub static mut KNOWN_UPDATES: Vec<(i32, core::Class_id, i32)> = vec![];
pub static mut CHECKED_UPDATES: Vec<(i32, usize)> = vec![];

#[no_mangle]
extern "C" fn notify_update(el: *const core::InventoryElement) {
    let data = convert_to_data(el);
    unsafe { OBJECT_UPDATES.push(data) }
}

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

#[no_mangle]
extern "C" fn notify_knowledge(pl_id: i32, cid: core::Class_id, id: i32) {
    unsafe {
        KNOWN_UPDATES.push((pl_id, cid, id));
    }
}

#[no_mangle]
extern "C" fn notify_checked(pl_id: i32, el: usize) {
    unsafe {
        CHECKED_UPDATES.push((pl_id, el));
    }
}

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
    ActionOnObject {
        a: u32,
        oid: usize,
    },
    ServerActionOnObject {
        a: u32,
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

#[derive(Clone)]
struct ClientData {
    id: usize,
    local_seq_num: u32,
    remote_seq_num: u32,
    acks_bitmap: u32,
    not_confirmed_packets: HashMap<u32, Vec<u8>>,
}

impl ClientData {
    fn new(id: usize) -> ClientData {
        ClientData {
            id,
            local_seq_num: 0,
            remote_seq_num: 0,
            acks_bitmap: 0,
            not_confirmed_packets: HashMap::new(),
        }
    }
}

pub struct Server {
    socket: UdpSocket,
    clients: HashMap<SocketAddr, ClientData>,
}

impl Server {
    fn broadcast(&mut self, data: &[u8]) {
        let clients = self.clients.clone();
        //    println!("SERV: clients.len={}", self.clients.len());
        //  p self.clients.base.table.table.items
        for client in clients.keys() {
            self.send_to_reliable(data, client);
        }
    }

    fn send_to_reliable(&mut self, data: &[u8], to: &SocketAddr) {
        let client = self.clients.get_mut(&to).unwrap();
        client.local_seq_num += 1;
        // println!("{} {}", client.local_seq_num, data[0]);
        if data.len() > 65507 {
            panic!("data too big");
        }
        let mut buf = Vec::with_capacity(12 + data.len());
        buf.extend_from_slice(&client.local_seq_num.to_le_bytes());
        buf.extend_from_slice(&client.remote_seq_num.to_le_bytes());
        buf.extend_from_slice(&client.acks_bitmap.to_le_bytes());
        buf.extend_from_slice(&data);
        unsafe {
            if core::trace_network > 0 {
                println!(
                    "SERV: send len={} local_seq={} remote_seq={}",
                    12 + data.len(),
                    client.local_seq_num,
                    client.remote_seq_num
                );
                core::show_packet_type_name('S' as std::os::raw::c_char, data[0] as u8);
            }
        }

        client
            .not_confirmed_packets
            .insert(client.local_seq_num, data.to_vec());
        self.socket.send_to(&buf, to).unwrap();
    }
}

impl<'a> From<&'a [u8]> for ClientEvent<'a> {
    fn from(value: &'a [u8]) -> Self {
        match value[0] {
            core::PACKET_PLAYER_MOVE => ClientEvent::Move {
                x: i32::from(value[1] as i8),
                y: i32::from(value[2] as i8),
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
            core::PACKET_PLAYER_ACTION_ON_OBJECT => ClientEvent::ActionOnObject {
                a: u32::from_le_bytes(value[1..5].try_into().unwrap()),
                oid: usize::from_le_bytes(value[5..13].try_into().unwrap()),
            },
            core::PACKET_SERVER_ACTION_ON_OBJECT => ClientEvent::ServerActionOnObject {
                a: u32::from_le_bytes(value[1..5].try_into().unwrap()),
                oid: usize::from_le_bytes(value[5..13].try_into().unwrap()),
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
            core::PACKET_PLAYER_UPDATE => ClientEvent::Whatever,
            core::PACKET_KEEP_ALIVE => ClientEvent::Whatever,
            _ => panic!("invalid event {:?}", value),
        }
    }
}

pub fn init_server() -> Result<Server, Box<dyn Error>> {
    let socket = UdpSocket::bind("0.0.0.0:1234")?;
    socket.set_nonblocking(true).unwrap();
    generator::generate();

    Ok(Server {
        socket,
        clients: HashMap::new(),
    })
}

pub fn test_server() {
    generator::generate();
    //generator::show_world();
}

//static mut max_recv: u32 = 0;

pub fn main_loop(server: &mut Server) {
    let mut players: Vec<*mut core::PlayerServer> = vec![];
    loop {
        // let start = std::time::Instant::now();
        let _recv = handle_network(server, &mut players);
        // let net = start.elapsed();

        // let start = std::time::Instant::now();
        unsafe {
            /*if recv != 0 {
                if recv > max_recv
                {
                    max_recv = recv;
                }
                println!("recv={} / {}", recv, max_recv);
            }*/
            core::update();
        }
        // let update = start.elapsed();
        // let start = std::time::Instant::now();
        send_game_updates(server);
        // let send = start.elapsed();
        // println!(
        //     "net {:5?} update {:5?} send {:5?}",
        //     net.as_millis(),
        //     update.as_millis(),
        //     send.as_millis()
        // );
        std::thread::sleep(std::time::Duration::from_millis(20));
    }
}

fn add_player(
    server: &mut Server,
    peer: std::net::SocketAddr,
    players: &mut Vec<*mut core::PlayerServer>,
) {
    server.clients.insert(peer, ClientData::new(players.len()));

    let mut response = vec![0; 13];
    response[12] = core::PACKET_PLAYER_ID as u8;
    response.extend_from_slice(&players.len().to_le_bytes());
    let data = generator::get_world_data();
    response.extend_from_slice(&data);
    println!("SEND {:?}", data.len());
    // generator::show_world();
    println!("PLAYER {} CONNECTED", &players.len());

    server.socket.send_to(&response, &peer).unwrap();

    unsafe {
        // let p = core::PlayerServer::new(players.len() as i32);
        let p = core::create_player(players.len() as i32);
        players.push(p);
        // FIXME
        // let axe = Box::into_raw(Box::new(core::Axe::new(
        //     std::ptr::null_mut(),
        //     std::ptr::null_mut(),
        // ))) as *mut core::InventoryElement;
        // (*core::world_table[128][128]).add_object1(axe);
        // p.pickup(axe);
        // players.push(p);
        // update_chunk_for_player(server, &mut peer, (128, 128));
        // core::objects_to_create.add(axe);
        update_chunk_for_player(server, &peer, (128, 128));
        core::add_object_to_world(p as *mut core::InventoryElement, (*p)._base._base.location);
    }
    //println!("{:?} , players {:?}", peer, players);
}

fn update_chunk_for_player(server: &mut Server, peer: &SocketAddr, coords: (u8, u8)) {
    let mut data =
        vec![0 as u8; 3 + (core::CHUNK_SIZE * core::CHUNK_SIZE) as usize * size_of::<i32>()];
    data[0] = core::PACKET_CHUNK_UPDATE;
    data[1] = coords.0;
    data[2] = coords.1;

    unsafe {
        let chunk = core::world_table[coords.1 as usize][coords.0 as usize]
            .as_mut()
            .unwrap();
        let table_ptr = chunk.table.as_mut_ptr() as *mut u8;
        let table_slice = std::slice::from_raw_parts(
            table_ptr,
            (core::CHUNK_SIZE * core::CHUNK_SIZE * core::TILE_SIZE) as usize,
        );

        let dest_ptr = data[3..].as_mut_ptr();
        let dest_slice = std::slice::from_raw_parts_mut(
            dest_ptr,
            (core::CHUNK_SIZE * core::CHUNK_SIZE * core::TILE_SIZE) as usize,
        );
        dest_slice.clone_from_slice(table_slice);
    }

    server.send_to_reliable(&data, peer);
    create_objects_in_chunk_for_player(server, peer, coords);
}

fn create_objects_in_chunk_for_player(server: &mut Server, peer: &SocketAddr, coords: (u8, u8)) {
    let chunk;
    unsafe {
        chunk = core::world_table[coords.1 as usize][coords.0 as usize]
            .as_mut()
            .unwrap();

        let mut le = chunk.objects._base.head;
        while le != std::ptr::null_mut() {
            let mut data = vec![core::PACKET_OBJECT_CREATE];
            //  println!("create_objects_in_chunk_for_player PACKET_OBJECT_CREATE");
            let obj = convert_types::convert_to_data(&*(*le).el);
            let obj_data = &bincode::serialize(&obj).unwrap()[..];
            data.extend_from_slice(obj_data);

            le = (*le).next;
            server.send_to_reliable(&data, peer);
        }
    }
}

fn handle_network(server: &mut Server, players: &mut Vec<*mut core::PlayerServer>) {
    let mut buf = [0; 100];
    loop {
        if let Ok((amt, src)) = server.socket.recv_from(&mut buf) {
            if amt == 0 {
                panic!("SERV: PACKET empty!!!");
            }
            if amt < 12 {
                panic!("SERV: PACKET too short!!!");
            }
            unsafe {
                if core::trace_network > 0 {
                    println!("SERV: handle_network amt={amt}");
                }
            }

            match server.clients.get_mut(&src) {
                Some(data) => {
                    let id = data.id;
                    unsafe {
                        core::show_packet_type_name('S' as std::os::raw::c_char, buf[12] as u8);
                    }
                    if buf[12] == core::PACKET_JOIN_REQUEST {
                        let mut response = vec![0; 13];
                        response[12] = core::PACKET_PLAYER_ID as u8;
                        response.extend_from_slice(&id.to_le_bytes());
                        let data = generator::get_world_data();
                        response.extend_from_slice(&data);
                        println!("SEND {:?}", data.len());
                        println!("PLAYER {} RECONNECTED", id);

                        server.socket.send_to(&response, &src).unwrap();
                        update_chunk_for_player(server, &src, (128, 128))
                    } else {
                        if id < players.len() {
                            handle_packet(
                                server,
                                unsafe {
                                    slice::from_raw_parts_mut(players[id], 1)
                                        .get_mut(0)
                                        .unwrap()
                                },
                                &buf,
                                &src,
                            );
                        } else {
                            println!("invalid player idx {}", id);
                        }
                    }
                }
                None => {
                    if buf[12] == core::PACKET_JOIN_REQUEST {
                        println!("connected");
                        add_player(server, src, players);
                    } else {
                        println!("player without idx")
                    }
                }
            }
        } else {
            //            println!("SERV: handle_network END");
            break;
        }
    }
}

fn handle_packet(
    server: &mut Server,
    player: &mut core::PlayerServer,
    packet: &[u8],
    peer: &SocketAddr,
) {
    let client_data = server.clients.get_mut(&peer).unwrap();
    let seq = u32::from_le_bytes(packet[0..4].try_into().unwrap());
    let ack = u32::from_le_bytes(packet[4..8].try_into().unwrap());
    let acks = u32::from_le_bytes(packet[8..12].try_into().unwrap());
    unsafe {
        if core::trace_network > 0 {
            println!(
                "SERV: handle_packet seq={seq} remote_seq_num={} ack={ack}",
                client_data.remote_seq_num
            );
        }
    }
    if seq > client_data.remote_seq_num {
        let diff = seq - client_data.remote_seq_num;
        client_data.acks_bitmap = (client_data.acks_bitmap << 1) | 1;
        if diff > 1 && diff <= 32 {
            client_data.acks_bitmap = client_data.acks_bitmap << (diff - 1);
        }
        client_data.remote_seq_num = seq;
    } else if seq < client_data.remote_seq_num {
        let diff = client_data.remote_seq_num - seq;
        client_data.acks_bitmap = client_data.acks_bitmap | (1 << (diff - 1));
    } else {
        panic!("Shouldn't ever receive 2 packets with same seq_num");
    }
    // Detect packet loss
    for i in 0..32 {
        if (acks & (1 << i)) > 0 {
            client_data.not_confirmed_packets.remove(&(ack - i));
        }
    }
    client_data.not_confirmed_packets.remove(&ack);

    let mut to_remove = vec![];
    let mut to_resend = vec![];
    for (&id, d) in client_data.not_confirmed_packets.iter() {
        if id + 32 < ack {
            println!("SERV: PACKET NOT CONFIRMED id={id} ack={ack}");
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
        client_data.not_confirmed_packets.remove(&i);
    }
    /* for d in to_resend {
            server.send_to_reliable(&d, peer);
        }
    */
    if packet.len() < 13 {
        panic!("packet to short len={} {:?}", packet.len(), packet);
    }

    let packet = &packet[12..];

    let tag: ClientEvent = ClientEvent::from(packet);

    match tag {
        ClientEvent::Move { x, y } => {
            println!("SERV: moved {x} {y}");
            unsafe { player.move_(x, y) }
        }
        ClientEvent::Pickup { id } => {
            println!("SERV: player picking up {id}");
            unsafe {
                let item = find_in_world(
                    &mut player._base._base.location as *mut core::ItemLocation,
                    id,
                );
                if item != std::ptr::null_mut() {
                    if !player.pickup(item) {
                        let response = [core::PACKET_ACTION_FAILED];
                        server.send_to_reliable(&response, peer);
                    }
                }
                //let mut buf = vec![core::PACKET_PLAYER_ACTION_PICKUP];
                //buf.extend_from_slice(&id.to_le_bytes());
                //buf.extend_from_slice(&player_id.to_le_bytes());
                //server.broadcast(&buf);
            }
        }
        ClientEvent::Drop { id } => {
            println!("SERV: player dropped {id}");
            unsafe {
                let item = player._base.get_item_by_uid(id);
                if item != std::ptr::null_mut() {
                    let loc = (*item).location;
                    add_object_to_world(item, player._base._base.location);
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
            println!("SERV: player used {iid} on {oid}");
            unsafe {
                let item = player._base.get_item_by_uid(iid);
                let object = find_in_world(
                    &mut player._base._base.location as *mut core::ItemLocation,
                    oid,
                );
                if !player.use_item_on_object(item, object) {
                    let response = [core::PACKET_ACTION_FAILED];
                    server.send_to_reliable(&response, peer);
                }
            }
        }
        ClientEvent::ActionOnObject { a, oid } => {
            println!("SERV: player action {a} on {oid}");
            unsafe {
                let object = find_in_world(
                    &mut player._base._base.location as *mut core::ItemLocation,
                    oid,
                );
                if object != std::ptr::null_mut() {
                    if !player.action_on_object(a, object) {
                        let response = [core::PACKET_ACTION_FAILED];
                        server.send_to_reliable(&response, peer);
                    }
                }
            }
        }
        ClientEvent::ServerActionOnObject { a, oid } => {
            println!("SERV: server action {a} on {oid}");
            unsafe {
                let mut object = 0 as *mut core::InventoryElement;
                if oid != 0 {
                    object = find_in_world(
                        &mut player._base._base.location as *mut core::ItemLocation,
                        oid,
                    );
                }
                if !player.server_action_on_object(a, object) {
                    let response = [core::PACKET_ACTION_FAILED];
                    server.send_to_reliable(&response, peer);
                }
            }
        }
        ClientEvent::Craft {
            product_id,
            ingredients_num,
            ingredients_ids,
        } => {
            println!("SERV: craft product={product_id} ing_num={ingredients_num}");
            unsafe {
                if core::craft_entry(
                    product_id as i32,
                    ingredients_num as i32,
                    ingredients_ids.as_ptr() as *const usize,
                    (&mut player._base) as *mut core::Player,
                ) {
                    let id = usize::from_le_bytes(ingredients_ids[0..8].try_into().unwrap());
                    let el = player._base.get_item_by_uid(id);
                    println!("SERV: id after CRAFT{:?}", el);
                    if !el.is_null() {
                        destroy_object(server, (*el).uid, (*el).location);
                        player._base.drop(el);
                        println!("SERV: deleted {}", id);
                    } else {
                        println!("SERV: invalid id {}", id);
                    }
                    if product_id >= 4 {
                        let id2 = usize::from_le_bytes(ingredients_ids[8..16].try_into().unwrap());
                        let el2 = player._base.get_item_by_uid(id2);
                        if !el2.is_null() {
                            destroy_object(server, (*el2).uid, (*el2).location);
                            player._base.drop(el2);
                        } else {
                            println!("SERV: invalid id2 {}", id2);
                        }
                    }
                } else {
                    let response = [core::PACKET_FAILED_CRAFT];
                    server.send_to_reliable(&response, peer);
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
            println!("SERV: ItemUsedOnTile iid={iid} map_x={map_x} map_y={map_y} x={x} y={y}");
            let item = player._base.get_item_by_uid(iid);
            if player.plant_with_seed(item, map_x, map_y, x, y) {
                println!("SERV: planted OK");
            } else {
                println!("SERV: failed to plant");
            }
        },
        ClientEvent::RequestChunk { x, y } => unsafe {
            println!("SERV: RequestChunk: received {} {}", x, y);
            if !(x < 0 || x > core::WORLD_SIZE as i32 || y < 0 || y > core::WORLD_SIZE as i32) {
                if core::world_table[y as usize][x as usize] != std::ptr::null_mut() {
                    println!("SERV: update chunk for {} {}", x, y);
                    update_chunk_for_player(server, peer, (x as u8, y as u8));
                }
            }
        },
        //FIXME change id to get_id()
        ClientEvent::Whatever => {} // println!("player {} alive", player._base.id) },
    }
}

fn send_game_updates(server: &mut Server) {
    unsafe {
        let list = std::ptr::addr_of_mut!(core::objects_to_create);
        let mut le = (*list)._base.head;
        while le != std::ptr::null_mut() {
            let mut data = vec![core::PACKET_OBJECT_CREATE];
            //         println!("send_game_updates PACKET_OBJECT_CREATE");
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

        if OBJECT_UPDATES.len() > 0 {
            let mut data = vec![core::PACKET_OBJECT_UPDATE];
            let obj_data = &bincode::serialize(&OBJECT_UPDATES[..]).unwrap()[..];
            println!(
                "sending {} updates size {}",
                OBJECT_UPDATES.len(),
                obj_data.len()
            );
            data.extend_from_slice(obj_data);
            server.broadcast(&data);
            OBJECT_UPDATES.clear();
        }
    }
    send_location_updates(server);
    send_knowledge_updates(server);
    send_destroy_updates(server);
    send_checked_updates(server);
}

fn send_location_updates(server: &mut Server) {
    unsafe {
        if LOCATION_UPDATES.len() > 0 {
            //  println!("SERV:send_location_updates len={}", LOCATION_UPDATES.len());
            for update in LOCATION_UPDATES.iter() {
                let mut data = vec![core::PACKET_LOCATION_UPDATE];
                //    println!("SERV: send_location_updates id={}", update.id);
                data.extend_from_slice(&bincode::serialize(update).unwrap()[..]);
                server.broadcast(&data);
            }
            LOCATION_UPDATES.clear();
        }
    }
}

fn send_checked_updates(server: &mut Server) {
    unsafe {
        if CHECKED_UPDATES.len() > 0 {
            for update in CHECKED_UPDATES.iter() {
                let mut data = vec![core::PACKET_CHECKED_UPDATE];
                data.extend_from_slice(&bincode::serialize(update).unwrap()[..]);
                server.broadcast(&data);
            }
            CHECKED_UPDATES.clear();
        }
    }
}

fn send_knowledge_updates(server: &mut Server) {
    unsafe {
        if KNOWN_UPDATES.len() > 0 {
            for update in KNOWN_UPDATES.iter() {
                let mut data = vec![core::PACKET_KNOWLEDGE_UPDATE];
                data.extend_from_slice(&bincode::serialize(update).unwrap()[..]);
                server.broadcast(&data);
            }
            KNOWN_UPDATES.clear();
        }
    }
}

fn send_destroy_updates(server: &mut Server) {
    unsafe {
        if DESTROY_ITEMS.len() > 0 {
            for (id, location) in DESTROY_ITEMS.iter() {
                destroy_object(server, *id, *location);
            }
            DESTROY_ITEMS.clear();
        }
    }
}

fn destroy_object(server: &mut Server, id: usize, location: core::ItemLocation) {
    let mut buf = vec![core::PACKET_OBJECT_DESTROY];
    buf.extend_from_slice(&id.to_le_bytes());
    buf.extend_from_slice(&bincode::serialize(&location).unwrap()[..]);
    server.broadcast(&buf);
}
