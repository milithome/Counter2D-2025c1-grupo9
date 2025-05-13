#ifndef HITBOX_H
#define HITBOX_H

struct Hitbox {
    float x, y; // el centro
    float width;
    float height;

    bool intersects(const Hitbox& other) const { //colisión
        float halfWidthA = width / 2.0f;
        float halfHeightA = height / 2.0f;
        float halfWidthB = other.width / 2.0f;
        float halfHeightB = other.height / 2.0f;

        return !(x + halfWidthA  < other.x - halfWidthB ||
                 x - halfWidthA  > other.x + halfWidthB ||
                 y + halfHeightA < other.y - halfHeightB ||
                 y - halfHeightA > other.y + halfHeightB);
    }
};

#endif
