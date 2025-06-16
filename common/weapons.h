#ifndef WEAPONS_H
#define WEAPONS_H

#include "structures.h"

enum class EquippedWeapon { KNIFE, PRIMARY, SECONDARY };

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
  uint32_t maxAmmo;
  float cooldown;
};

namespace Weapons {
  extern const Weapon Knife;
  extern const Weapon Glock;
  extern const Weapon AK47;
  extern const Weapon M3;
  extern const Weapon AWP;
  extern const Weapon None;

  const Weapon &getWeapon(WeaponName name);
}

#endif
