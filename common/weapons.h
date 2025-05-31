#ifndef WEAPONS_H
#define WEAPONS_H

#include "structures.h"

enum class EquippedWeapon {
  KNIFE,
  PRIMARY,
  SECONDARY
};

struct Weapon {
  WeaponName name;
  int minDamage;
  int maxDamage;
  bool burstFire;
  int bulletsPerBurst;
  float burstDelay;
  float spreadAngle;
  int bulletsPerShoot;
  float maxRange;
  int maxAmmo;
  float cooldown;
};

namespace Weapons {
  extern const Weapon Knife;
  extern const Weapon Glock;
  extern const Weapon AK47;
  extern const Weapon M3;
  extern const Weapon AWP;

  const Weapon& getWeapon(WeaponName name);
}

#endif
