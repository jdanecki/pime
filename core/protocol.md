# THE GAME PROTOCOL LOL

## Packet types

    enum PacketType {
        PACKET_PLAYER_UPDATE = 0,
        PACKET_PLAYER_ID,
        PACKET_MOVE,
        PACKET_CHUNK_UPDATE,
        PACKET_INIT_INVENTORY_ELEMENT,
    };

### player update

packet size: 25

| type  | bits  | value |
| u8    | 0     | PACKET\_PLAYER\_UPDATE |
| usize | 1-8   | player id |
| i32   | 9-12  | player map\_x |
| i32   | 13-16 | player map\_y |
| i32   | 17-20 | player x |
| i32   | 21-24 | player y |

### player id

packet size: 9

| type  | bits  | value |
| u8    | 0     | PACKET\_PLAYER\_ID |
| usize | 1-8   | id for player |

### move player

packet size: 3

| type  | bits  | value |
| u8    | 0     | PACKET\_MOVE |
| i8    | 0     | x     |
| i8    | 0     | y     |

### chunk update

packet size: 3 + CHUNK\_SIZE * CHUNK\_SIZE * sizeof(enum game\_tiles)

| type  | bits  | value |
| u8    | 0     | PACKET\_CHUNK\_UPDATE |
| u8    | 1     | chunk x |
| u8    | 2     | chunk y |
| enum game\_tiles | 3-size | data |
| objects | ? | |

### object serialization

packet size: 

| type  | bits  | value |
| u8    | 0     | PACKET\_INIT\_INVENTORY\_ELEMENT |


### server send                                                 ### client receive                    
```                                                              
let mut buf = Vec::with_capacity(12 + data.len());              
client.local_seq_num += 1;                                      
buf.extend_from_slice(&client.local_seq_num.to_le_bytes());     let seq = u32::from_le_bytes(buf[0..4].try_into().unwrap());       
buf.extend_from_slice(&client.remote_seq_num.to_le_bytes());    let ack = u32::from_le_bytes(buf[4..8].try_into().unwrap());
buf.extend_from_slice(&client.acks_bitmap.to_le_bytes());       let acks = u32::from_le_bytes(buf[8..12].try_into().unwrap());
buf.extend_from_slice(&data);                                   client.remote_seq_num = seq;
```

### client send                                                 server receive
```
let mut buf = Vec::with_capacity(12 + data.len());                        
self.local_seq_num += 1;
buf.extend_from_slice(&self.local_seq_num.to_le_bytes());        let seq = u32::from_le_bytes(packet[0..4].try_into().unwrap());
buf.extend_from_slice(&self.remote_seq_num.to_le_bytes());       let ack = u32::from_le_bytes(packet[4..8].try_into().unwrap());
buf.extend_from_slice(&self.my_acks_bitmap.to_le_bytes());       let acks = u32::from_le_bytes(packet[8..12].try_into().unwrap());
buf.extend_from_slice(&data);                                    client_data.remote_seq_num = seq;
```

