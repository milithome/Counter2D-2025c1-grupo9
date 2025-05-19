#ifndef HITBOX_H
#define HITBOX_H
struct Hitbox {
  float x, y; // esquina superior izquierda
  float width;
  float height;

  bool intersects(const Hitbox &other) const { // colisión
    return !(x + width < other.x || x > other.x + other.width ||
             y + height < other.y || y > other.y + other.height);
  }
  bool intersectsRay(float ox, float oy, float tx, float ty) const {
    // interpreto a la bala como un rayo que impacta inmediatamente
    float x1 = x;
    float y1 = y;
    float x2 = x + width;
    float y2 = y + height;

    auto outside = [](float a, float b, float min, float max) {
      return (a < min && b < min) || (a > max && b > max);
    };

    if (outside(ox, tx, x1, x2) || outside(oy, ty, y1, y2))
      return false;

    return true;
  }
};

#endif
