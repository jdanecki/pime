#include <raylib.h>

extern bool show_craft;

void handle_mouse(int x, int y, int button);

bool handle_events()
{
    if (IsKeyDown(KEY_ESCAPE)) {
         if (show_craft) {
                show_craft=0;
                return false;
            } 
         else
             return true;
    }

    return WindowShouldClose();


//        if (event.type == SDL_MOUSEBUTTONDOWN)
  //      {
    //      
//            handle_mouse(event.x, event.y, event.button);
      //  }
}

