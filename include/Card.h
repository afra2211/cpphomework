#ifndef CARD_H
#define CARD_H

#include "Resources.h"
#include <map>
#include <string>

class Card {
public:
  Card();
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
