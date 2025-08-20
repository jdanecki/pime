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

};
