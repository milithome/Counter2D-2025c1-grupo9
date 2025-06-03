#ifndef SDL_SURFACEWIDGET
#define SDL_SURFACEWIDGET

#include "sdl_widget.h"



class SdlSurfaceWidget : public SdlWidget {

public:
    SdlSurfaceWidget(SDL2pp::Rect& rect, SDL2pp::Surface& child) : SdlWidget(rect), rect(rect), child(child) { build(); };

    void build() {
        rect.SetH(child.GetHeight());
        rect.SetW(child.GetWidth());
        
    }



private:
    SDL2pp::Surface& child;
    SDL2pp::Rect& rect;
};


#endif