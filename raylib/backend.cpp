#include "backend.inl"

Texture2D Backend_Create_Texture_From_Surface(Image image)
{
    return LoadTextureFromImage(image);
}

const char * Backend_Get_Error() {
    return "not implemented yet";
}
