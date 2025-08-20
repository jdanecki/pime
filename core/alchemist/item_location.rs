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

impl ItemLocation {
    pub fn add(&self, dx: i32, dy: i32) -> ItemLocation {
        match self {
            ItemLocation::Chunk(l) => {
                let map_x = l.map_x + dx / CHUNK_SIZE as i32;
                let map_y = l.map_y + dy / CHUNK_SIZE as i32;
                let x = l.x + dx % CHUNK_SIZE as i32;
                let y = l.y + dy % CHUNK_SIZE as i32;
                ItemLocation { map_x, map_y, x, y }
            }
            _ => panic!("adding to not chunk location"),
        }
    }
}
