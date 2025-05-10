#include "player.h"

Player::Player(const std::string &playerName, int playerId)
    : name(playerName), id(playerId) {}

std::string Player::getName() const { return name; }
int Player::getId() const { return id; }
void Player::setRole(Role new_role) { role = new_role; }

float Player::getX() const { return x; }

float Player::getY() const { return y; }

void Player::move(float x, float y) {
  // siempre mover será para una sola dirección
  // porque se vincula con el presionar de una sola tecla
  this->x += x;
  this->y += y;
}
