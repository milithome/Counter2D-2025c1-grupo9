#ifndef WEAPON_TYPES_H
#define WEAPON_TYPES_H
#include "structures.h"
#include <string>

enum class EquippedWeapon {
  KNIFE,
  PRIMARY,
  SECONDARY
};

struct Weapon {
  WeaponName name;
  int minDamage;
  int maxDamage;
  bool burstFire; //solo ak-47 true
  float burstDelay;
  float spreadAngle; //0: sin dispersión, >0: dispersión angular (en grados)
  int bulletsPerShoot;// (M3 dispara varias de una)
  float maxRange; // distancia máxima efectiva del arma
  int maxAmmo;
  float cooldown;
};

namespace Weapons {

  const Weapon Knife = {
    WeaponName::KNIFE,
    20, 40,  
    false,
    0.0f, 
    0.0f, 
    1,               
    1.0f,   
    0,
    1.0f
  };
const Weapon Glock = {
    WeaponName::GLOCK,
    25, 35,
    false,
    0.0f,
    2.0f,
    1,
    30.0f,
    15,
    1.0f
  };

  const Weapon AK47 = {
    WeaponName::AK47,
    15, 25,
    true,
    0.2f,//delay entre balas
    4.0f,
    3,
    40.0f,
    30,
    2.0f
  };

  const Weapon M3 = {
    WeaponName::M3,
    10, 50,
    false,
    0.0f,
    4.0f,
    5,                 
    40.0f,
    50,
    2.0f
  };

  const Weapon AWP = {
    WeaponName::AWP,
    80, 100,
    false,
    0.0f,
    0.0f,
    1,
    100.0f,
    10,
    4.0f
  };
  const Weapon getWeapon(WeaponName name) {
    switch (name) {
      case WeaponName::KNIFE: return Knife;
      case WeaponName::GLOCK: return Glock;
      case WeaponName::AK47:  return AK47;
      case WeaponName::M3:    return M3;
      case WeaponName::AWP:   return AWP;
      default:
        return Knife;
    }
  }
}
#endif
