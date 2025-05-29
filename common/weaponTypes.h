#ifndef WEAPON_TYPES_H
#define WEAPON_TYPES_H

#include <string>

enum class EquippedWeapon {
  KNIFE,
  PRIMARY,
  SECONDARY
};

struct WeaponType {
  const char* name;
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

namespace WeaponTypes {
  const WeaponType Knife = {
    "knife",
    20, 40,  
    false,
    0.0f, 
    0.0f, 
    1,               
    1.0f,   
    0,
    1.0f
  };
const WeaponType Glock = {
    "Glock",
    25, 35,
    false,
    0.0f,
    2.0f,
    1,
    30.0f,
    15,
    1.0f
  };

  const WeaponType AK47 = {
    "AK-47",
    15, 25,
    true,
    0.2f,//delay entre balas
    4.0f,
    3,
    40.0f,
    30,
    2.0f
  };

  const WeaponType M3 = {
    "M3",
    10, 50,
    false,
    0.0f,
    10.0f,
    5,                 
    15.0f,
    50,
    2.0f
  };

  const WeaponType AWP = {
    "AWP",
    80, 100,
    false,
    0.0f,
    0.0f,
    1,
    100.0f,
    10,
    4.0f
  };
}
#endif
