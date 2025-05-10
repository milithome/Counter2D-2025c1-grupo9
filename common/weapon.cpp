#include "weapon.h"
#include <iostream>

Weapon::Weapon(const std::string &name, int minDamage, int maxDamage,
               int bulletsPerShot, float accuracy, float range, int ammo,
               bool isMelee)
    : name(name), minDamage(minDamage), maxDamage(maxDamage),
      bulletsPerShot(bulletsPerShot), accuracy(accuracy), range(range),
      ammo(ammo), isMelee(isMelee) {}

void Weapon::reload(int bullets) { ammo += bullets; }

std::string Weapon::getName() const { return name; }

int Weapon::getAmmo() const { return ammo; }

bool Weapon::isMeleeWeapon() const { return isMelee; }
