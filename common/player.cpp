#include "player.h"
#include <iostream>

std::string Player::getName() const { return name; }
void Player::setRole(Role new_role) { role = new_role; }

float Player::getX() const { return x; }

float Player::getY() const { return y; }

void Player::move(float x, float y, float deltaTime) {
  // x,y pueden valer 0, 1 o -1 y representan hacia donde moverse
  if (x != 0 && y != 0) { // si va en diagonal, normalizo
    x /= std::sqrt(2);
    y /= std::sqrt(2);
  }
  this->x += x * deltaTime * SPEED;
  this->y += y * deltaTime * SPEED;
  hitbox.x = this->x;
  hitbox.y = this->y;
}

void Player::setPosition(float x, float y) {
  // coordenadas literales en las que quiero que se encuentre
  this->x = x;
  this->y = y;
  hitbox.x = x;
  hitbox.y = y;
}

float Player::getRotation() const { return rotation; }

void Player::setRotation(float currentRotation) { rotation = currentRotation; }

const Hitbox &Player::getHitbox() const { return hitbox; }

void Player::updateHealth(float value) {
  health += value;
  if (health < 0.0f)
    health = 0.0f;
}

void Player::setHealth(float value) {
  health = value;
  if (health < 0.0f)
    health = 0.0f;
}

void Player::updateMovement(float deltaTime) {
  move(vx, vy, deltaTime);
}

float Player::getHealth() const { return health; }

bool Player::isAlive() const { return health > 0.0f; }

void Player::updateVelocity(float vx, float vy){
  this->vx=vx;
  this->vy=vy;
}

void Player::stopShooting(){
  shooting=false;
}

void Player::startShooting(){
  shooting=true;
}

void Player::updateCooldown(float deltaTime) {
  if (shootCooldown > 0.0f){
    shootCooldown -= deltaTime;
  }
}

float Player::getShootCooldown(){
  return shootCooldown;
}

bool Player::isShooting() {
  return shooting;
}