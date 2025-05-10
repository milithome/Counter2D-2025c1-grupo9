#include "player.h"
#include <iostream>

std::string Player::getName() const { return name; }
uint Player::getId() const { return id; }
void Player::setRole(Role new_role) { role = new_role; }

float Player::getX() const { return x; }

float Player::getY() const { return y; }

void Player::move(float x, float y) {
  // siempre mover será para una sola dirección
  // porque se vincula con el presionar de una sola tecla
  this->x += x;
  this->y += y;
}

void Player::setPosition(float x, float y) {
  this->x = x;
  this->y = y;
}
