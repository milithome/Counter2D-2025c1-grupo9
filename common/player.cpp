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

void Player::resetCooldown(){ //cuando acaba de salir una bala
  shootCooldown=equipped.cooldown;
}

bool Player::isShooting() {
  return shooting;
}

std::tuple<float, float, float, float, float, float> Player::shoot() {
  
      Hitbox hb= getHitbox();
      
      std::cout << "Hitbox de " << name << ": "
              << "desde (" << hb.x << ", " << hb.y << ") "
              << "hasta (" << (hb.x + hb.width) << ", " << (hb.y + hb.height) << ")\n";
      
      float origin_x = hb.x + hb.width / 2.0f;
      float origin_y = hb.y + hb.height / 2.0f;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<float> dist(getRotation() - getSpreadAngle(), getRotation() + getSpreadAngle());
      float angle = dist(gen); 
      float angle_rad = angle * M_PI / 180.0f;
      float max_distance;

      if (weaponEquipped==EquippedWeapon::SECONDARY){
        max_distance=secondaryWeapon.maxRange;
      }else if(weaponEquipped==EquippedWeapon::PRIMARY){
        max_distance=primaryWeapon.maxRange; 
      }else{
        max_distance=knife.maxRange;
      }
      float target_x = origin_x + cos(angle_rad) * max_distance;
      float target_y = origin_y + sin(angle_rad) * max_distance;
      
      /*
      std::cout << "Disparo desde (" << origin_x << ", " << origin_y << ") hasta ("
              << target_x << ", " << target_y << ")\n";
      */
      return std::make_tuple(max_distance, origin_x, origin_y, target_x, target_y, angle);
}

int Player::getBulletsPerShoot(){
  return equipped.bulletsPerShoot;
}

float Player::getSpreadAngle(){
  return equipped.spreadAngle;
}

std::pair<float, float> Player::getDamageRange(){
  return std::make_pair(equipped.minDamage, equipped.maxDamage);
}

void Player::changeWeapon(EquippedWeapon newEquippedWeapon){
  if (newEquippedWeapon== EquippedWeapon::PRIMARY){
    equipped=primaryWeapon;
  }else if(newEquippedWeapon== EquippedWeapon::SECONDARY){
    equipped=secondaryWeapon;
  }else{
    equipped=knife;
  }
}