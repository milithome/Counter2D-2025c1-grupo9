#include "weapons.h"

namespace Weapons {
  const Weapon Knife = {};
  const Weapon Glock = {};
  const Weapon AK47 = {};
  const Weapon M3 = {};
  const Weapon AWP = {};
  const Weapon None = {};
  /*
  const Weapon Knife = {
      WeaponName::KNIFE, 20, 40, false, 0, 0.0f, 0.0f, 1, 1.0f, 0, 0.5f};
  const Weapon Glock = {
      WeaponName::GLOCK, 25, 35, false, 0, 0.0f, 0.0f, 1, 30.0f, 15, 0.4f};
  const Weapon AK47 = {
      WeaponName::AK47, 15, 25, true, 3, 0.05f, 0.0f, 1, 40.0f, 30, 0.4f};
  const Weapon M3 = {WeaponName::M3, 10, 50,    false, 0,   0.0f,
                    10.0f,          5,  40.0f, 50,    1.0f};
  const Weapon AWP = {WeaponName::AWP, 80, 100, false, 0, 0.0f, 0.0f, 1,
                      100.0f,          10, 2.0f};
  const Weapon None = {WeaponName::NONE, 0, 0, false, 0, 0.0f, 0.0f, 0,
                      0.0f,          0, 0.0f};
  */
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
} // namespace Weapons
