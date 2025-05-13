#include "bullet.h"
#include "gameConstants.h"
#include <cmath>
#include <numbers>

Bullet::Bullet(float startX, float startY, float angle, float speed)
    : x(startX), y(startY), hitbox{x, y, BULLET_WIDTH, BULLET_HEIGHT}, rotation(angle), speed(speed), destroyed(false)
{
}

void Bullet::update(float deltaTime) {
    float radians = rotation * std::numbers::pi_v<float> / 180.0f;
    float dx = std::cos(radians) * speed * deltaTime;
    float dy = std::sin(radians) * speed * deltaTime;

    x += dx;
    y += dy;
}

bool Bullet::isDestroyed() const {
    return destroyed;
}

void Bullet::destroy(){
    destroyed=true;
}

float Bullet::getX() const {
    return x;
}

float Bullet::getY() const {
    return y;
}

float Bullet::getRotation() const {
    return rotation;
}
