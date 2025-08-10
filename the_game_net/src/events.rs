use crate::core;
use crate::types;

extern "C" {
    pub fn update_player(id: usize, map_x: i32, map_y: i32, x: i32, y: i32, thirst: i32, hunger: i32);
    pub fn update_chunk(x: i32, y: i32, data: &core::chunk_table);
    pub fn got_id(id: usize, seed: i64);
    pub fn update_object(data: &types::ObjectData);
    pub fn update_item_location(data: types::LocationUpdateData);
    pub fn create_object(data: &types::ObjectData);
    pub fn destroy_object(id: usize, location: core::ItemLocation);
    pub fn failed_craft();
    pub fn action_failed();
    pub fn knowledge_update(pl_id: usize, cid: core::Class_id, id: i32);
}
