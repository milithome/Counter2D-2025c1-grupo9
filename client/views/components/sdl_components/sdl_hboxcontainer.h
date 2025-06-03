#ifndef SDL_HBOX
#define SDL_HBOX

#include "sdl_widget.h"



class SdlHBoxContainer : public SdlWidget {

public:
    SdlHBoxContainer(SDL2pp::Rect& rect, std::vector<std::reference_wrapper<SdlWidget>> children) : SdlWidget(rect), rect(rect), children(children) { build(); };

    void setSpacing(int amount) { spacing = amount; }
    void build() {
        int item_container_x = rect.GetX();
        for (size_t i = 0; i < children.size(); i++) {
            SdlWidget& child = children[i];

            child.setX(item_container_x);
            child.setY(
                rect.GetY()
            );
            child.setW(
                (rect.GetW() - spacing * (children.size() - 1))/children.size()
            );
            child.setH(
                rect.GetH()
            );
            item_container_x = child.getX() + child.getW() + spacing; 
        }
    }



private:
    SDL2pp::Rect& rect;
    std::vector<std::reference_wrapper<SdlWidget>> children;
    int spacing = 0;
};

#endif