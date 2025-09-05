/// <div rustbindgen hide></div>
struct ItemLocation
{
    enum class Tag
    {
        Chunk,
        Player,
    };

    struct Chunk_Body
    {
        int32_t map_x;
        int32_t map_y;
        int32_t x;
        int32_t y;
    };

    struct Player_Body
    {
        uintptr_t id;
    };

    Tag tag;
    union
    {
        Chunk_Body chunk;
        Player_Body player;
    };

    static ItemLocation center()
    {
        ItemLocation l;
        l.tag = ItemLocation::Tag::Chunk;
        l.chunk = {128, 128, 8, 8};
        return l;
    }
    void show()
    {
        if (tag == Tag::Chunk)
        {
            printf("map_x:%d map_y:%d x:%d y:%d\n", chunk.map_x, chunk.map_y, chunk.x, chunk.y);
        } else
            printf("player: %lu\n", player.id);
    }
};
