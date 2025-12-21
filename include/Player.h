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

class Player {
public:
  // 构造函数：移除 isAI 参数，因为现在我们用类本身来区分
  Player(std::string name);

  // === [关键点 1] 虚析构函数 (Virtual Destructor) ===
  // 确保删除 Player 指针时，子类 (AIPlayer/HumanPlayer) 的内存也能被正确释放
  virtual ~Player() = default;

  // === [关键点 2] 纯虚函数 (Pure Virtual Function) ===
  // 这是一个"接口"，任何继承 Player 的类都 *必须* 实现这个函数
  // 这里的 "= 0" 意味着 Player 现在是一个抽象类 (Abstract
  // Class)，不能直接实例化 这实现了设计模式中的“策略模式” (Strategy Pattern)
  // 这部分建立了一个契约，确定任何玩家都可以在游戏中做出决策，但是具体怎么决策，需要后续对接这部分的同学在子类中实现
  // 这里是为了扩展性得分点，此处符合(Open/Closed
  // Principle)原则，想加超强AI的时候可以直接新建一个超强AI类，提高可拓展性
  virtual int makeDecision(const Game &game) = 0;

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
  void addProgressToken(const ProgressToken &token);

  // 核心交易逻辑 (Core Logic)
  bool canAfford(const Cost &cost, const Player &opponent,
                 std::string chainTarget = "") const;
  int calculateCost(const Cost &cost, const Player &opponent,
                    std::string chainTarget = "") const;
  void payCost(int amount);

  void buildCard(const Card &card);
  void buildWonder(Wonder &wonder);

  // 辅助显示
  std::map<CardType, int> getCardsByType() const;
  std::map<ResourceType, int> getTotalResourcesFromCards() const;
  std::string getCardTypeName(CardType type) const;
  std::string getResourceTypeName(ResourceType type) const;
  std::map<ScienceSymbol, int> getScienceSymbolCounts() const;
  std::map<ResourceType, int> getTradeDiscounts() const;
  int getYellowCardCount() const;
  bool hasLawSymbol() const;

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
  std::map<ResourceType, int>
      tradeDiscounts;        // 交易折扣（黄牌/奇观/进步来源）
  int yellowCardCount = 0;   // 弃牌收益缓存
  bool lawSymbolUnlocked{false};
};

#endif // PLAYER_H