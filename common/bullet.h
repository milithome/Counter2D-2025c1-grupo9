#ifndef BULLET_H
#define BULLET_H
#include "hitBox.h"

class Bullet {
private:
    float x, y;
    Hitbox hitbox;
    float rotation;
    float speed;
    bool destroyed;

public:
    Bullet(float startX, float startY, float angle, float speed = 5);

    void update(float deltaTime);
    bool isDestroyed() const;
    void destroy();

    float getX() const;
    float getY() const;
    float getRotation() const;
};

#endif
