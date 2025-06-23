#include "weapons.h"
namespace Weapons {
const Weapon Knife = {};
const Weapon Glock = {};
const Weapon AK47 = {};
const Weapon M3 = {};
const Weapon AWP = {};
const Weapon None = {};

const Weapon &getWeapon(WeaponName name) {
  switch (name) {
  case WeaponName::KNIFE:
    return Knife;
  case WeaponName::GLOCK:
    return Glock;
  case WeaponName::AK47:
    return AK47;
  case WeaponName::M3:
    return M3;
  case WeaponName::AWP:
    return AWP;
  default:
    return None;
  }
}
}
