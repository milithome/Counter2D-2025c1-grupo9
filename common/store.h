#ifndef STORE_H
#define STORE_H

#include "structures.h"
#include <utility>
#include <vector>

class Store {
public:
  static std::vector<std::pair<WeaponName, int>> getStore() {
    return {
        {WeaponName::AK47, 100}, {WeaponName::AWP, 240}, {WeaponName::M3, 150}};
  }

  static int getWeaponPrice(WeaponName name) {
    for (const auto &weapon : getStore()) {
      if (weapon.first == name) {
        return weapon.second;
      }
    }
    return -1;
  }
};

#endif
