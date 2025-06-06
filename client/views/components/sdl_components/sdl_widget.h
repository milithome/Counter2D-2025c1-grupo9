#ifndef SDL_WIDGET_H
#define SDL_WIDGET_H
#include <memory>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Surface.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Renderer.hh>


enum SizePolicy {
    EXPAND,
    FIXED,
    EXPAND_BUT_MAINTAIN_RATIO

};



class SdlWidget {
public:

    SdlWidget() {}


    SdlWidget(const SDL2pp::Rect& rect)
        : rect_(rect) {}

    virtual ~SdlWidget() = default;


    const SDL2pp::Rect& getRect() const { return rect_; }
    void setRect(const SDL2pp::Rect& rect) { rect_ = rect; }


    virtual void draw(SDL2pp::Renderer& renderer);
    void setX(int x) {
        rect_.SetX(x);
    }
    void setY(int y) {
        rect_.SetY(y);
    }
    void setW(int w) {
        rect_.SetW(w);
    }
    void setH(int h) {
        rect_.SetH(h);
    }

    int getX() {
        return rect_.GetX();
    }
    int getY() {
        return rect_.GetY();
    }
    int getW() {
        return rect_.GetW();
    }
    int getH() {
        return rect_.GetH();
    }

    SizePolicy getSizePolicy() { return sizePolicy; }
    void setSizePolicy(SizePolicy policy) { sizePolicy = policy; }

protected:
    SDL2pp::Rect rect_;
    SizePolicy sizePolicy;

};

#endif