#ifndef SDL_WIDGET_H
#define SDL_WIDGET_H
#include <memory>
#include <SDL2pp/Rect.hh>

class SdlWidget {
public:

    Widget(const SDL2pp::Rect& rect)
        : rect_(rect) {}

    virtual ~Widget() = default;

    virtual void Draw() = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;

    const SDL2pp::Rect& GetRect() const { return rect_; }
    void SetRect(const SDL2pp::Rect& rect) { rect_ = rect; }

    Ptr GetParent() const { return parent_; }

protected:
    SDL2pp::Rect rect_;
    SdlWidget& parent_;
};

#endif