#include "Player.h"
#include <algorithm>
#include <iostream>

Player::Player(std::string name, bool isAI)
    : name(name), isAI(isAI), coins(7), militaryPower(0), victoryPoints(0) {
  // Initial resources (none usually, but maybe some base production depending
  // on rules/wonders later)
}

bool Player::isAIPlayer() const { return isAI; }

std::string Player::getName() const { return name; }

int Player::getCoins() const { return coins; }

int Player::getMilitaryPower() const { return militaryPower; }

int Player::getVictoryPoints() const { return victoryPoints; }

const std::map<ResourceType, int> &Player::getResources() const {
  return resources;
}

const std::vector<ScienceSymbol> &Player::getScienceSymbols() const {
  return scienceSymbols;
}

const std::vector<Card> &Player::getBuiltCards() const { return builtCards; }

const std::vector<Wonder *> &Player::getBuiltWonders() const {
  return builtWonders;
}

void Player::addCoins(int amount) { coins += amount; }

void Player::removeCoins(int amount) {
  if (coins >= amount) {
    coins -= amount;
  } else {
    // Should check before calling, but handle safely
    coins = 0;
  }
}

void Player::addResource(ResourceType type, int amount) {
  resources[type] += amount;
}

void Player::addMilitaryPower(int amount) { militaryPower += amount; }

void Player::addVictoryPoints(int amount) { victoryPoints += amount; }

void Player::addScienceSymbol(ScienceSymbol symbol) {
  scienceSymbols.push_back(symbol);
}

bool Player::canAfford(const Cost &cost, const Player &opponent,
                       std::string chainTarget) const {
  if (!chainTarget.empty()) {
    for (const auto &card : builtCards) {
      if (card.getChainSymbol() == chainTarget)
        return true;
    }
  }
  int totalCost = calculateCost(cost, opponent, chainTarget);
  return coins >= totalCost;
}

int Player::calculateCost(const Cost &cost, const Player &opponent,
                          std::string chainTarget) const {
  if (!chainTarget.empty()) {
    for (const auto &card : builtCards) {
      if (card.getChainSymbol() == chainTarget)
        return 0;
    }
  }

  int totalCoinsNeeded = cost.coins;

  for (auto const &[type, amount] : cost.resources) {
    int produced = resources.count(type) ? resources.at(type) : 0;
    int missing = std::max(0, amount - produced);

    if (missing > 0) {
      // Trading rule (R4.5-8): Cost per missing resource = 2 + Opponent's
      // production of that resource Note: Opponent's brown/grey cards
      // production. Simplified: We use opponent's total resource count for that
      // type.
      const auto &oppResources = opponent.getResources();
      int opponentProduction =
          oppResources.count(type) ? oppResources.at(type) : 0;

      int pricePerUnit = 2 + opponentProduction;
      totalCoinsNeeded += missing * pricePerUnit;
    }
  }
  return totalCoinsNeeded;
}

void Player::payCost(int amount) { removeCoins(amount); }

void Player::buildCard(const Card &card) {
  builtCards.push_back(card);

  // Apply immediate effects
  const Effect &effect = card.getEffect();
  addVictoryPoints(effect.victoryPoints);
  addMilitaryPower(effect.militaryShields);
  addCoins(effect.coins);

  for (auto const &[type, amount] : effect.resourcesProduced) {
    addResource(type, amount);
  }

  for (const auto &symbol : effect.scienceSymbols) {
    addScienceSymbol(symbol);
  }
}

void Player::buildWonder(Wonder &wonder) {
  wonder.build();
  builtWonders.push_back(&wonder);

  const Effect &effect = wonder.getEffect();
  addVictoryPoints(effect.victoryPoints);
  addMilitaryPower(effect.militaryShields);
  addCoins(effect.coins);

  for (auto const &[type, amount] : effect.resourcesProduced) {
    addResource(type, amount);
  }

  for (const auto &symbol : effect.scienceSymbols) {
    addScienceSymbol(symbol);
  }
}

std::map<CardType, int> Player::getCardsByType() const {
  std::map<CardType, int> result;

  // 初始化所有卡牌类型为0
  result[CardType::RAW_MATERIAL] = 0;
  result[CardType::MANUFACTURED_GOOD] = 0;
  result[CardType::CIVILIAN] = 0;
  result[CardType::SCIENTIFIC] = 0;
  result[CardType::COMMERCIAL] = 0;
  result[CardType::MILITARY] = 0;
  result[CardType::GUILD] = 0;

  // 统计每种类型的卡牌数量
  for (const auto &card : builtCards) {
    result[card.getType()]++;
  }

  return result;
}

// 新增方法：从卡牌获取总资源产量
std::map<ResourceType, int> Player::getTotalResourcesFromCards() const {
  std::map<ResourceType, int> totalResources;

  // 初始化所有资源类型为0
  totalResources[ResourceType::WOOD] = 0;
  totalResources[ResourceType::CLAY] = 0;
  totalResources[ResourceType::STONE] = 0;
  totalResources[ResourceType::GLASS] = 0;
  totalResources[ResourceType::PAPER] = 0;
  totalResources[ResourceType::NONE] = 0;

  // 从卡牌中统计资源
  for (const auto &card : builtCards) {
    const Effect &effect = card.getEffect();
    for (const auto &[type, amount] : effect.resourcesProduced) {
      totalResources[type] += amount;
    }
  }

  // 从奇迹中统计资源
  for (const auto &wonder : builtWonders) {
    if (wonder && wonder->isBuilt()) {
      const Effect &effect = wonder->getEffect();
      for (const auto &[type, amount] : effect.resourcesProduced) {
        totalResources[type] += amount;
      }
    }
  }

  return totalResources;
}

// 新增方法：获取卡牌类型名称
std::string Player::getCardTypeName(CardType type) const {
  switch (type) {
  case CardType::RAW_MATERIAL:
    return "Brown";
  case CardType::MANUFACTURED_GOOD:
    return "Grey";
  case CardType::CIVILIAN:
    return "Blue";
  case CardType::SCIENTIFIC:
    return "Green";
  case CardType::COMMERCIAL:
    return "Yellow";
  case CardType::MILITARY:
    return "Red";
  case CardType::GUILD:
    return "Purple";
  default:
    return "Unknown";
  }
}

// 新增方法：获取资源类型名称
std::string Player::getResourceTypeName(ResourceType type) const {
  switch (type) {
  case ResourceType::WOOD:
    return "wood";
  case ResourceType::CLAY:
    return "clay";
  case ResourceType::STONE:
    return "stone";
  case ResourceType::GLASS:
    return "glass";
  case ResourceType::PAPER:
    return "paper";
  case ResourceType::NONE:
    return "none";
  default:
    return "unknown";
  }
}

std::map<ScienceSymbol, int> Player::getScienceSymbolCounts() const {
  std::map<ScienceSymbol, int> counts;

  // 初始化所有ScienceSymbol为0
  counts[ScienceSymbol::GLOBE] = 0;
  counts[ScienceSymbol::TABLET] = 0;
  counts[ScienceSymbol::GEAR] = 0;
  counts[ScienceSymbol::COMPASS] = 0;
  counts[ScienceSymbol::WHEEL] = 0;
  counts[ScienceSymbol::MORTAR] = 0;
  counts[ScienceSymbol::NONE] = 0;

  // 统计每个符号的数量
  for (const auto &symbol : scienceSymbols) {
    counts[symbol]++;
  }

  return counts;
}