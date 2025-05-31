#ifndef HITBOX_H
#define HITBOX_H
#include <optional>
#include <utility>
#include <limits>
#include <algorithm>


struct Hitbox {
  float x, y; // esquina superior izquierda
  float width;
  float height;
  float getX() const { return x; }
  float getY() const { return y; }
  float getWidth() const { return width; }
  float getHeight() const { return height; }

  bool intersects(const Hitbox &other) const { // colisión entre hitboxes
    return !(x + width < other.x || x > other.x + other.width ||
             y + height < other.y || y > other.y + other.height);
  }
  std::optional<std::pair<float, float>> intersectsRay(float ox, float oy, float tx, float ty) const {
    float x1 = x;
    float y1 = y;
    float x2 = x + width;
    float y2 = y + height;

    float dx = tx - ox;
    float dy = ty - oy;

    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::infinity();

    if (dx != 0.0f) {
        float tx1 = (x1 - ox) / dx;
        float tx2 = (x2 - ox) / dx;

        tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
    } else if (ox < x1 || ox > x2) {
        return std::nullopt;
    }

    if (dy != 0.0f) {
        float ty1 = (y1 - oy) / dy;
        float ty2 = (y2 - oy) / dy;

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    } else if (oy < y1 || oy > y2) {
        return std::nullopt;
    }

    if (tmax >= tmin && tmax >= 0.0f) {
        float ix = ox + dx * tmin;
        float iy = oy + dy * tmin;
        return std::make_pair(ix, iy);
    }

    return std::nullopt;
  }
};

#endif
