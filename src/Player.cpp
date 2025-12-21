#include "Game.h"
#include "Player.h"
#include <algorithm>
#include <iostream>

// === 构造函数 ===
// [规则 P.6] PREPARATION - Step 5: "Each player takes 7 coins from the Bank."
// 初始化玩家状态：7个金币，0军事，0分数
Player::Player(std::string name, bool isAIPlayer)
    : name(name), coins(7), militaryPower(0), victoryPoints(0),
      isAI(isAIPlayer) {
  // 初始化交易折扣与科学符号计数
  for (auto resource : {ResourceType::WOOD, ResourceType::STONE, ResourceType::CLAY,
                        ResourceType::PAPER, ResourceType::GLASS}) {
    tradeDiscounts[resource] = TradeDiscount{};
  }
  for (auto symbol : {ScienceSymbol::GLOBE, ScienceSymbol::TABLET,
                      ScienceSymbol::GEAR, ScienceSymbol::COMPASS,
                      ScienceSymbol::WHEEL, ScienceSymbol::MORTAR}) {
    scienceSymbolCounter[symbol] = 0;
    sciencePairClaimed[symbol] = false;
  }
}

// === Getters (访问器) ===
Decision Player::makeDecision(const Game &game) { return Decision{}; }

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

const std::vector<ProgressToken> &Player::getProgressTokens() const {
  return progressTokens;
}

bool Player::hasClaimedSciencePair(ScienceSymbol symbol) const {
  auto it = sciencePairClaimed.find(symbol);
  if (it == sciencePairClaimed.end()) {
    return false;
  }
  return it->second;
}

void Player::markSciencePairClaimed(ScienceSymbol symbol) {
  sciencePairClaimed[symbol] = true;
}

// === Setters / Modifiers (修改器) ===
void Player::addCoins(int amount) { coins += amount; }

void Player::removeCoins(int amount) {
  if (coins >= amount) {
    coins -= amount;
  } else {
    coins = 0;
  }
}

void Player::addResource(ResourceType type, int amount) { resources[type] += amount; }

void Player::removeResource(ResourceType type, int amount) {
  resources[type] -= amount;
  if (resources[type] < 0) {
    resources[type] = 0;
  }
}

void Player::addMilitaryPower(int amount) { militaryPower += amount; }

void Player::addVictoryPoints(int amount) {
  victoryPoints += amount;
  if (victoryPoints < 0) {
    victoryPoints = 0;
  }
}

void Player::addScienceSymbol(ScienceSymbol symbol) {
  scienceSymbols.push_back(symbol);
  scienceSymbolCounter[symbol]++;
  if (symbol == ScienceSymbol::NONE && lawSymbolUnlocked) {
    // Law 进步标记作为第七符号计入 NONE 占位
    scienceSymbolCounter[symbol] = 1;
  }
}

void Player::removeScienceSymbol(ScienceSymbol symbol) {
  if (scienceSymbolCounter[symbol] > 0) {
    scienceSymbolCounter[symbol]--;
  }

  for (auto it = scienceSymbols.begin(); it != scienceSymbols.end(); ++it) {
    if (*it == symbol) {
      scienceSymbols.erase(it);
      break;
    }
  }

  if (symbol == ScienceSymbol::MORTAR && scienceSymbolCounter[symbol] == 0) {
    lawSymbolUnlocked = false;
  }
}

void Player::addProgressToken(const ProgressToken &token) {
  progressTokens.push_back(token);

  switch (token.getType()) {
  case ProgressTokenType::AGRICULTURE:
    addCoins(6);
    addVictoryPoints(4);
    break;
  case ProgressTokenType::ARCHITECTURE:
    hasArchitecture = true;
    break;
  case ProgressTokenType::ECONOMY:
    hasEconomy = true;
    break;
  case ProgressTokenType::LAW:
    lawSymbolUnlocked = true;
    addScienceSymbol(ScienceSymbol::NONE);
    break;
  case ProgressTokenType::MASONRY:
    hasMasonry = true;
    break;
  case ProgressTokenType::MATHEMATICS:
    hasMathematics = true;
    break;
  case ProgressTokenType::PHILOSOPHY:
    addVictoryPoints(7);
    break;
  case ProgressTokenType::STRATEGY:
    hasStrategy = true;
    break;
  case ProgressTokenType::THEOLOGY:
    hasTheology = true;
    break;
  case ProgressTokenType::URBANISM:
    hasUrbanism = true;
    addCoins(6);
    break;
  default:
    break;
  }
}

int Player::discardForCoins() {
  int coinsGained = 2 + yellowCardCount;
  addCoins(coinsGained);
  return coinsGained;
}

// === 核心逻辑：计算建造成本 (包含交易规则) ===
// LO02考点：复杂业务逻辑封装
int Player::calculateCost(const Cost &cost, const Player &opponent,
                          std::string chainTarget) const {

  // 1. 检查免费建造链 (Chains)
  // [规则 P.9] Free construction condition (chains)
  // "If you have the Building containing this symbol... construct the new one
  // for free."
  if (!chainTarget.empty()) {
    for (const auto &card : builtCards) {
      if (card.getChainSymbol() == chainTarget) {
        return 0; // 满足链接条件，完全免费
      }
    }
  }

  int totalCoinsNeeded = cost.coins; // 部分卡牌本身需要金币成本

  // 2. 遍历每一个需要的资源类型
  for (auto const &[type, amountNeeded] : cost.resources) {
    int producedBySelf = resources.count(type) ? resources.at(type) : 0;
    int missing = std::max(0, amountNeeded - producedBySelf);

    if (missing > 0) {
      // === 交易逻辑 (Trading Rules) ===
      // [规则 P.8] Trading
      // "COST = 2 + number of symbols..."
      const TradeDiscount &discount = tradeDiscounts.at(type);

      // 默认单价：基础 2 + 对手棕/灰产量（黄牌与奇迹产量不计入）
      int opponentProduction = 0;
      if (!discount.priceToOne) {
        const std::vector<Card> &oppCards = opponent.getBuiltCards();
        for (const auto &oppCard : oppCards) {
          CardType cType = oppCard.getType();
          if (cType == CardType::RAW_MATERIAL ||
              cType == CardType::MANUFACTURED_GOOD) {
            const auto &prodMap = oppCard.getEffect().resourcesProduced;
            auto it = prodMap.find(type);
            if (it != prodMap.end()) {
              opponentProduction += it->second;
            }
          }
        }
      }

      int pricePerUnit = discount.priceToOne ? 1 : 2 + opponentProduction;
      if (!discount.priceToOne && discount.coinDiscount > 0) {
        pricePerUnit = std::max(0, pricePerUnit - discount.coinDiscount);
      }

      totalCoinsNeeded += missing * pricePerUnit;
    }
  }

  return totalCoinsNeeded;
}

// 检查是否买得起
bool Player::canAfford(const Cost &cost, const Player &opponent,
                       std::string chainTarget) const {
  return coins >= calculateCost(cost, opponent, chainTarget);
}

// 支付成本
void Player::payCost(int amount) { removeCoins(amount); }

// === 建造逻辑 ===
void Player::buildCard(const Card &card) {
  // 1. 加入已建造列表
  // [规则 P.10] Construct a Building
  // "This Building now belongs to your city."
  builtCards.push_back(card);

  if (card.getType() == CardType::COMMERCIAL) {
    yellowCardCount++;
  }

  // 2. 应用即时效果 (Effect)
  const Effect &effect = card.getEffect();

  // [规则 P.13] Civilian Victory (统计VP)
  addVictoryPoints(effect.victoryPoints);

  // [规则 P.12] Military (统计盾牌)
  addMilitaryPower(effect.militaryShields);

  // [规则 P.4] Coins (部分卡牌给予即时金币)
  addCoins(effect.coins);

  // 3. 更新资源产量
  // [规则 P.8] Production
  // "A city's resources are produced by its brown cards, its grey cards..."
  for (auto const &[type, amount] : effect.resourcesProduced) {
    addResource(type, amount);
  }

  // 4. 更新科技符号
  // [规则 P.12] Science & Progress
  for (const auto &symbol : effect.scienceSymbols) {
    addScienceSymbol(symbol);
  }
}

Effect Player::buildWonder(Wonder &wonder) {
  // [规则 P.11] Construct a Wonder
  Effect effect = wonder.build();
  builtWonders.push_back(&wonder);
  return effect;
}

bool Player::removeLastBuiltCard() {
  if (builtCards.empty()) {
    return false;
  }

  const Card &card = builtCards.back();
  const Effect &effect = card.getEffect();

  addVictoryPoints(-effect.victoryPoints);
  addMilitaryPower(-effect.militaryShields);
  for (const auto &[type, amount] : effect.resourcesProduced) {
    removeResource(type, amount);
  }
  for (const auto &symbol : effect.scienceSymbols) {
    removeScienceSymbol(symbol);
  }

  builtCards.pop_back();
  return true;
}

bool Player::removeBuiltCardByType(CardType type) {
  for (auto it = builtCards.begin(); it != builtCards.end(); ++it) {
    if (it->getType() == type) {
      const Effect &effect = it->getEffect();
      addVictoryPoints(-effect.victoryPoints);
      addMilitaryPower(-effect.militaryShields);
      for (const auto &[resource, amount] : effect.resourcesProduced) {
        removeResource(resource, amount);
      }
      for (const auto &symbol : effect.scienceSymbols) {
        removeScienceSymbol(symbol);
      }
      builtCards.erase(it);
      return true;
    }
  }
  return false;
}

// === 辅助显示方法 ===
std::map<CardType, int> Player::getCardsByType() const {
  std::map<CardType, int> result;
  // 初始化
  result[CardType::RAW_MATERIAL] = 0;
  result[CardType::MANUFACTURED_GOOD] = 0;
  result[CardType::CIVILIAN] = 0;
  result[CardType::SCIENTIFIC] = 0;
  result[CardType::COMMERCIAL] = 0;
  result[CardType::MILITARY] = 0;
  result[CardType::GUILD] = 0;

  for (const auto &card : builtCards) {
    result[card.getType()]++;
  }
  return result;
}

std::map<ResourceType, int> Player::getTotalResourcesFromCards() const {
  return resources;
}

std::map<ScienceSymbol, int> Player::getScienceSymbolCounts() const {
  std::map<ScienceSymbol, int> counts;
  // 初始化所有符号为0
  counts[ScienceSymbol::GLOBE] = 0;
  counts[ScienceSymbol::TABLET] = 0;
  counts[ScienceSymbol::GEAR] = 0;
  counts[ScienceSymbol::COMPASS] = 0;
  counts[ScienceSymbol::WHEEL] = 0;
  counts[ScienceSymbol::MORTAR] = 0;
  counts[ScienceSymbol::NONE] = lawSymbolUnlocked ? 1 : 0;

  for (const auto &symbol : scienceSymbols) {
    counts[symbol]++;
  }
  return counts;
}

std::map<ResourceType, TradeDiscount> Player::getTradeDiscounts() const {
  return tradeDiscounts;
}

int Player::getYellowCardCount() const { return yellowCardCount; }

bool Player::hasLawSymbol() const { return lawSymbolUnlocked; }

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
  default:
    return "none";
  }
}