#[repr(C)]
#[derive(serde::Serialize, serde::Deserialize, Copy, Clone, Debug)]
pub enum ItemLocation {
    Chunk {
        map_x: i32,
        map_y: i32,
        x: i32,
        y: i32,
    },
    Player {
        id: usize,
    },
}
