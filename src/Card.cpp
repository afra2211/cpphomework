#include "Card.h"

Card::Card() : name(""), type(CardType::RAW_MATERIAL) {}

Card::Card(std::string name, CardType type, Cost cost, Effect effect,
           std::string chainSymbol, std::string chainTarget)
    : name(name), type(type), cost(cost), effect(effect),
      chainSymbol(chainSymbol), chainTarget(chainTarget) {}

std::string Card::getName() const { return name; }

CardType Card::getType() const { return type; }

const Cost &Card::getCost() const { return cost; }

const Effect &Card::getEffect() const { return effect; }

std::string Card::getChainSymbol() const { return chainSymbol; }

std::string Card::getChainTarget() const { return chainTarget; }
