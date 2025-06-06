#ifndef SDL_VBOX
#define SDL_VBOX

#include "sdl_widget.h"



class SdlVBoxContainer : public SdlWidget {

public:
    SdlVBoxContainer(SDL2pp::Rect rect, std::vector<std::reference_wrapper<SdlWidget>> children) : SdlWidget(rect), rect(rect), children(children) { build(); };

    SdlVBoxContainer() {}

    void setSpacing(int amount) { spacing = amount; }
    void build() {
        int item_container_y = rect.GetX();
        for (size_t i = 0; i < children.size(); i++) {
            SdlWidget& child = children[i];

            child.setX(rect.GetX());
            child.setY(item_container_y);
            child.setW(
                rect.GetW()
            );
            child.setH(
                (rect.GetH() - spacing * (children.size() - 1))/children.size()
            );
            item_container_y = child.getY() + child.getH() + spacing;

        
        }
    }

    void addChild(std::reference_wrapper<SdlWidget> child) {
        children.push_back(child);
        build();
    }


private:
    SDL2pp::Rect rect;
    std::vector<std::reference_wrapper<SdlWidget>> children;
    int spacing = 0;
};


#endif