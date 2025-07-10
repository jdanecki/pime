use crate::core;
use crate::NetClient;

#[no_mangle]
pub extern "C" fn send_packet_move(client: &NetClient, x: i32, y: i32) {
    let buf = [core::PACKET_PLAYER_MOVE, x as u8, y as u8];
//    println!("player_move {} {}", x, y);
    client.send(&buf);
}

#[no_mangle]
pub extern "C" fn send_packet_pickup(client: &NetClient, id: usize) {
    let mut buf = vec![core::PACKET_PLAYER_ACTION_PICKUP];
    buf.extend_from_slice(&id.to_le_bytes());
    client.send(&buf);
}

#[no_mangle]
pub extern "C" fn send_packet_drop(client: &NetClient, id: usize) {
    let mut buf = vec![core::PACKET_PLAYER_ACTION_DROP];
    buf.extend_from_slice(&id.to_le_bytes());
    client.send(&buf);
}

#[no_mangle]
pub extern "C" fn send_packet_item_used_on_object(client: &NetClient, iid: usize, oid: usize) {
    let mut buf = vec![core::PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT];
    buf.extend_from_slice(&iid.to_le_bytes());
    buf.extend_from_slice(&oid.to_le_bytes());
    client.send(&buf);
}

#[no_mangle]
pub extern "C" fn send_packet_item_used_on_tile(
    client: &NetClient,
    iid: usize,
    map_x: i32,
    map_y: i32,
    x: i32,
    y: i32,
) {
    let mut buf = vec![core::PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE];
    buf.extend_from_slice(&iid.to_le_bytes());
    buf.extend_from_slice(&map_x.to_le_bytes());
    buf.extend_from_slice(&map_y.to_le_bytes());
    buf.extend_from_slice(&x.to_le_bytes());
    buf.extend_from_slice(&y.to_le_bytes());
    client.send(&buf);
}

#[no_mangle]
pub extern "C" fn send_packet_craft(
    client: &NetClient,
    prod_id: usize,
    ingredients_num: usize,
    iid: *const usize,
) {
    let mut buf = vec![core::PACKET_PLAYER_ACTION_CRAFT];
    buf.extend_from_slice(&prod_id.to_le_bytes());
    for i in 0..ingredients_num {
        unsafe {
            buf.extend_from_slice(&(*iid.add(i)).to_le_bytes());
            println!("extended");
        }
    }
    //  println!("{:?}", buf);
    client.send(&buf);
}

#[no_mangle]
pub extern "C" fn send_packet_request_chunk(client: &NetClient, x: i32, y: i32) {
    let mut buf = vec![core::PACKET_REQUEST_CHUNK];
    buf.extend_from_slice(&x.to_le_bytes());
    buf.extend_from_slice(&y.to_le_bytes());
    println!("request {} {}", x, y);

    client.send(&buf);
}
