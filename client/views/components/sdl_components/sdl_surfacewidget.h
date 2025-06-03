#ifndef SDL_SURFACEWIDGET
#define SDL_SURFACEWIDGET

#include "sdl_widget.h"



class SdlSurfaceWidget : public SdlWidget {

public:
    SdlSurfaceWidget(SDL2pp::Surface& child) : child(child) { build(); };

    void build() {
        rect.SetH(child.GetHeight());
        rect.SetW(child.GetWidth());
    }
     
    void draw(SDL2pp::Renderer& renderer) override {
        SDL2pp::Texture texture(renderer, child);
        renderer.Copy(
            texture, 
            SDL2pp::NullOpt, 
            rect);
    }


private:
    SDL2pp::Surface& child;
    SDL2pp::Rect rect;
};


#endif