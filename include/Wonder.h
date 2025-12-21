#ifndef WONDER_H
#define WONDER_H

#include "Resources.h"
#include <string>
#include <vector>

// 前向声明
class Game;
class Player;

class Wonder {
public:
  // 工厂将调用此构造函数
  Wonder(std::string name, Cost cost, Effect effect)
      : name(name), cost(cost), effect(effect), built(false) {}

  std::string getName() const;
  const Cost &getCost() const;
  const Effect &getEffect() const;
  bool isBuilt() const;
  Effect build();
  // 新增：执行奇迹效果
  void activateEffect(Game& game, Player& owner, Player& opponent);

private:
  std::string name;
  Cost cost;
  Effect effect;
  bool built;
};

#endif // WONDER_H
