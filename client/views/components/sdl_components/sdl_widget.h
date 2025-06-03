#ifndef SDL_WIDGET_H
#define SDL_WIDGET_H
#include <memory>
#include <SDL2pp/Rect.hh>

class SdlWidget {
public:

    Widget(const SDL2pp::Rect& rect)
        : rect_(rect) {}

    virtual ~Widget() = default;


    const SDL2pp::Rect& GetRect() const { return rect_; }
    void SetRect(const SDL2pp::Rect& rect) { rect_ = rect; }


protected:
    SDL2pp::Rect rect_;
};

#endif