#ifndef WONDER_H
#define WONDER_H

#include "Resources.h"
#include <string>
#include <vector>

// 前向声明
class Game;
class Player;

// 新增：定义效果激活后的结果状态
struct TurnResult {
    bool needsPlayerSwitch; // 指示是否需要切换玩家
};

class Wonder {
public:
  // 工厂将调用此构造函数
  Wonder(std::string name, Cost cost, Effect effect)
      : name(name), cost(cost), effect(effect), built(false) {}

  std::string getName() const;
  const Cost &getCost() const;
  const Effect &getEffect() const;
  bool isBuilt() const;
  void build();
  // 修改：返回TurnResult类型
  TurnResult activateEffect(Game& game, Player& owner, Player& opponent);

private:
  std::string name;
  Cost cost;
  Effect effect;
  bool built;
};

#endif // WONDER_H
