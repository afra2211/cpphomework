#ifndef WONDER_H
#define WONDER_H

#include "Resources.h"
#include <string>
#include <vector>


class Wonder {
public:
  Wonder(std::string name, Cost cost, Effect effect);

  std::string getName() const;
  const Cost &getCost() const;
  const Effect &getEffect() const;
  bool isBuilt() const;
  void build();

private:
  std::string name;
  Cost cost;
  Effect effect;
  bool built;
};

#endif // WONDER_H
