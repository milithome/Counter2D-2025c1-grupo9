#ifndef SDL_CONTAINER
#define SDL_CONTAINER

#include "sdl_widget.h"
#include "sdl_nullwidget.h"

enum VAlignment {
    TOP,
    MIDDLE,
    BOTTOM
};

enum HAlignment {
    RIGHT,
    CENTER,
    LEFT,
};

struct Alignment {
    VAlignment vertical;
    HAlignment horizontal;
};

struct RGBA {
    int r;
    int g;
    int b;
    int a;
};


class SdlContainer : public SdlWidget {

public:
    SdlContainer(SDL2pp::Rect rect, Alignment alignment, SdlWidget& child) : SdlWidget(rect), rect(rect), child(child), alignment(alignment) { build(); };
    SdlContainer(SDL2pp::Rect rect, SdlWidget& null) : rect(rect), child(null) {}



    void setMargin(int amount) { margin = amount; }
    void setPadding(int amount) { padding = amount; }
    void build() {
        switch (child.getSizePolicy()) {
            case FIXED: {
                child.setH(std::min(rect.GetH() - margin*2 - padding*2, child.getH()));
                child.setW(std::min(rect.GetW() - margin*2 - padding*2, child.getW()));
            }
            case EXPAND: {
                child.setH(rect.GetH() - margin*2 - padding*2);
                child.setW(rect.GetW() - margin*2 - padding*2);
            }
            case EXPAND_BUT_MAINTAIN_RATIO: {

                uint32_t max_w = rect.GetW() - margin * 2 - padding * 2;
                uint32_t max_h = rect.GetH() - margin * 2 - padding * 2;

                float width_to_height_scalar = static_cast<float>(child.getH())/ static_cast<float>(child.getW());

                if (max_w * width_to_height_scalar > max_h) {
                    child.setH(max_h);
                    child.setW(max_h / width_to_height_scalar);

                } else if (max_h / width_to_height_scalar > max_w) {
                    child.setW(max_w);
                    child.setH(max_w * width_to_height_scalar);

                }
            }
        }

        switch (alignment.vertical) {
            case BOTTOM: {
                child.setY(rect.GetY() + rect.GetH() - child.getH() - margin - padding);
                break;
            }
            case TOP: {
                child.setY(rect.GetY() + margin + padding);
                break;
            }
            case MIDDLE: {
                child.setY((rect.GetY() + rect.GetH())/2);
                break;
            }
        }
        switch (alignment.horizontal) {
            case LEFT: {
                child.setX(rect.GetX() + margin + padding);
                break;
            }
            case CENTER: {
                child.setX(rect.GetX() + rect.GetW()/2);
                break;
            }
            case RIGHT: {
                child.setX(rect.GetX() + rect.GetW() - child.getW() - margin - padding);
                break;
            }
        }
    }
    void setColor(RGBA color) {
        this->color = color;
    }
    void draw(SDL2pp::Renderer& renderer) override {
        renderer.SetDrawColor(color.r, color.b, color.g, color.a);
        renderer.FillRect(rect);
    }


    void setChild(Alignment alignment, SdlWidget& child) {
        this->child = child;
        this->alignment = alignment;
        build();
    }



private:
    SDL2pp::Rect rect;
    SdlWidget& child;
    Alignment alignment;
    int margin = 0;
    int padding = 0;

    RGBA color = RGBA{0, 0, 0, 0};
};

#endif