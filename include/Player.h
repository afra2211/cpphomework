#ifndef PLAYER_H
#define PLAYER_H

#include "Board.h"
#include "Card.h"
#include "Resources.h"
#include "Wonder.h"
#include <map>
#include <string>
#include <vector>

// 前向声明 (Forward Declaration)
// 我们告诉编译器 "Game" 是一个类，但不需要在这里包含 Game.h
// 这样做可以避免循环依赖 (Circular Dependency)
class Game;

// 行动决策描述：支持建造牌、弃牌或建造奇观
enum class DecisionAction { BUILD_CARD, DISCARD, BUILD_WONDER, EXIT, INVALID };

struct Decision {
  DecisionAction action{DecisionAction::INVALID};
  int cardIndex{-1};   // 金字塔中的卡牌索引
  int wonderIndex{-1}; // 可用奇观列表中的索引（仅 BUILD_WONDER 时使用）
};

class Player {
public:
  // 构造函数：允许标记是否为 AI 玩家
  Player(std::string name, bool isAIPlayer = false);

  // === [关键点 1] 虚析构函数 (Virtual Destructor) ===
  // 确保删除 Player 指针时，子类 (AIPlayer/HumanPlayer) 的内存也能被正确释放
  virtual ~Player() = default;

  // === [关键点 2] 决策接口 (可在子类中覆盖) ===
  virtual Decision makeDecision(const Game &game);
  virtual bool isAIPlayer() const;

  // === [关键点 3] 通用逻辑 (Common Logic) ===
  // 这些方法在人类和 AI 之间是共享的，所以保留在基类中实现复用

  // Getters
  std::string getName() const;
  int getCoins() const;
  int getMilitaryPower() const;
  int getVictoryPoints() const;
  const std::map<ResourceType, int> &getResources() const;
  const std::vector<ScienceSymbol> &getScienceSymbols() const;
  const std::vector<Card> &getBuiltCards() const;
  const std::vector<Wonder *> &getBuiltWonders() const;
  const std::vector<ProgressToken> &getProgressTokens() const;

  // Modifiers
  void addCoins(int amount);
  void removeCoins(int amount);
  void addResource(ResourceType type, int amount);
  void addMilitaryPower(int amount);
  void addVictoryPoints(int amount);
  void addScienceSymbol(ScienceSymbol symbol);
  void removeScienceSymbol(ScienceSymbol symbol);
  void removeResource(ResourceType type, int amount);
  void addProgressToken(const ProgressToken &token);
  bool hasProgressToken(ProgressTokenType type) const;
  int discardForCoins();

  // 核心交易逻辑 (Core Logic)
  bool canAfford(const Cost &cost, const Player &opponent,
                 std::string chainTarget = "",
                 CardType type = CardType::CIVILIAN,
                 bool isWonder = false) const;
  int calculateCost(const Cost &cost, const Player &opponent,
                    std::string chainTarget = "",
                    CardType type = CardType::CIVILIAN,
                    bool isWonder = false) const;
  int calculateTradeCost(int totalCost, const Cost &cost,
                         const Player &opponent, std::string chainTarget,
                         CardType type = CardType::CIVILIAN,
                         bool isWonder = false) const;
  void payCost(int amount);

  void buildCard(const Card &card);
  Effect buildWonder(Wonder &wonder);
  bool removeLastBuiltCard();
  bool removeBuiltCardByType(CardType type);

  // 辅助显示
  std::map<CardType, int> getCardsByType() const;
  std::map<ResourceType, int> getTotalResourcesFromCards() const;
  std::string getCardTypeName(CardType type) const;
  std::string getResourceTypeName(ResourceType type) const;
  std::map<ScienceSymbol, int> getScienceSymbolCounts() const;
  std::map<ResourceType, TradeDiscount> getTradeDiscounts() const;
  int getYellowCardCount() const;
  bool hasLawSymbol() const;
  bool hasClaimedSciencePair(ScienceSymbol symbol) const;
  void markSciencePairClaimed(ScienceSymbol symbol);

protected:
  // === [关键点 4] 受保护的成员 (Protected Members) ===
  // 使用 protected 而不是 private，这样子类 (AIPlayer) 可以直接访问这些数据
  std::string name;
  // bool isAI;  <-- [删除]
  // 这个标志位已经被删除了，我们不再需要它，我们用后半学期的知识点Héritage和Spécialisation避免掉RTTI
  int coins;
  int militaryPower;
  int victoryPoints;
  std::map<ResourceType, int> resources;
  std::vector<ScienceSymbol> scienceSymbols;
  std::map<ScienceSymbol, int> scienceSymbolCounter; // 含 Law 进步标记
  std::vector<Card> builtCards;
  std::vector<Wonder *> builtWonders;
  std::vector<ProgressToken> progressTokens; // 已获得进步标记
  std::map<ResourceType, TradeDiscount>
      tradeDiscounts;      // 交易折扣（黄牌/奇观/进步来源）
  int yellowCardCount = 0; // 弃牌收益缓存
  bool lawSymbolUnlocked{false};
  bool isAI{false};
  std::map<ScienceSymbol, bool> sciencePairClaimed; // 已经领取过进步标记的符号

  // 进步标记常驻效果
  bool hasArchitecture{false};
  bool hasMasonry{false};
  bool hasEconomy{false};
  bool hasStrategy{false};
  bool hasTheology{false};
  bool hasUrbanism{false};
  bool hasMathematics{false};
};

#endif // PLAYER_H