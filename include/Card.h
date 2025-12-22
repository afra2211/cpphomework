#ifndef CARD_H
#define CARD_H

#include "Resources.h"
#include <map>
#include <string>
#include <vector>


class Card {
public:
  // 默认构造函数
  Card();

  // 完整的构造函数（成员 C 的工厂将调用这个）
  Card(std::string name, CardType type, Cost cost, Effect effect,
       std::string chainSymbol = "", std::string chainTarget = "");

  std::string getName() const;
  CardType getType() const;
  const Cost &getCost() const;
  const Effect &getEffect() const;

  std::string getChainSymbol() const;
  std::string getChainTarget() const;

private:
  std::string name;
  CardType type;
  Cost cost;
  Effect effect;
  std::string chainSymbol; // Symbol this card provides
  std::string chainTarget; // Symbol required to build this card for free
};

#endif // CARD_H
