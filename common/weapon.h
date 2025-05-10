#ifndef WEAPON_H
#define WEAPON_H

#include <string>

class Weapon {
protected:
  std::string name;
  int minDamage;
  int maxDamage;
  int bulletsPerShot; // rafaga
  float fireRate;     // balas por segundo
  float accuracy;     // precisión
  float range;        // rango de disparo
  int ammo;
  bool isMelee; // true solo si es cuchillo

public:
  Weapon(const std::string &name, int minDamage, int maxDamage,
         int bulletsPerShot, float accuracy, float range, int ammo,
         bool isMelee);

  virtual ~Weapon() = default;

  virtual void shoot(float distance) = 0; // el daño depende de la distancia

  virtual void reload(int bullets);

  std::string getName() const;
  int getAmmo() const;
  bool isMeleeWeapon() const;
};

#endif
