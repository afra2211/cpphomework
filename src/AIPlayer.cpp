// AIPlayer.cpp - 成员D (HY) 完成
#include "AIPlayer.h"
#include "Board.h"
#include "Card.h"
#include "ConsoleView.h" // 使用你定义的视图接口来输出
#include "Game.h"
#include <cstdlib> // 用于 rand()
#include <ctime>   // 用于 srand()
#include <limits>
#include <vector>

AIPlayer::AIPlayer(std::string name)
    : Player(name, true) // 调用基类构造函数，标记为AI
{
  // 初始化随机数种子，用于随机选择
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

AIPlayer::~AIPlayer() {
  // 没有动态分配的资源，留空
}

// 核心AI决策函数 - 完整实现
Decision AIPlayer::makeDecision(const Game &game) {
  ConsoleView::printInfo(getName() + "AI begin to take action...");

  const Board &board = game.getBoard();
  const Player *selfPtr = this;
  const Player &opponent =
      (&game.getPlayer1() == selfPtr) ? game.getPlayer2() : game.getPlayer1();
  const auto &pyramid = board.getPyramid();

  std::vector<int> accessibleIndices;
  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (board.isCardAccessible(i) && !pyramid.at(i).isTaken) {
      accessibleIndices.push_back(static_cast<int>(i));
    }
  }

  Decision decision;
  if (accessibleIndices.empty()) {
    ConsoleView::printError("AI error: No cards are accessible.");
    decision.action = DecisionAction::EXIT;
    return decision;
  }

  auto evaluateCard = [&](const Card &card) {
    int score = 0;
    const Effect &effect = card.getEffect();

    const int VP_WEIGHT = 3;
    const int MILITARY_WEIGHT = 6;
    const int SCIENCE_WEIGHT = 5;
    const int RESOURCE_WEIGHT = 2;
    const int COIN_WEIGHT = 1;

    score += effect.victoryPoints * VP_WEIGHT;
    score += effect.militaryShields * MILITARY_WEIGHT;
    if (!effect.scienceSymbols.empty()) {
      score += SCIENCE_WEIGHT;
    }
    for (const auto &[resource, amount] : effect.resourcesProduced) {
      score += amount * RESOURCE_WEIGHT;
    }
    score += effect.coins * COIN_WEIGHT;

    if (game.getCurrentAge() == 1) {
      score += effect.militaryShields * 2;
    } else if (game.getCurrentAge() == 3) {
      score += effect.victoryPoints * 2;
    }

    score -= static_cast<int>(card.getCost().coins * 0.5);
    return score;
  };

  int bestCardIndex = -1;
  int bestCardScore = std::numeric_limits<int>::min();
  for (int cardIndex : accessibleIndices) {
    const Card &card = pyramid.at(cardIndex).card;
    if (canAfford(card.getCost(), opponent)) {
      int score = evaluateCard(card);
      // ConsoleView::printInfo("  评估卡牌 [" + std::to_string(cardIndex) + "]:
      // " +
      //                         card.getName() + "，得分: " +
      //                         std::to_string(score));
      if (score > bestCardScore) {
        bestCardScore = score;
        bestCardIndex = cardIndex;
      }
    }
  }

  const auto &availableWonders = board.getAvailableWonders();
  int affordableWonderIndex = -1;
  for (size_t i = 0; i < availableWonders.size(); ++i) {
    if (availableWonders[i] != nullptr && !availableWonders[i]->isBuilt() &&
        canAfford(availableWonders[i]->getCost(), opponent)) {
      affordableWonderIndex = static_cast<int>(i);
      break;
    }
  }

  if (bestCardIndex != -1 && bestCardScore > 0) {
    decision.action = DecisionAction::BUILD_CARD;
    decision.cardIndex = bestCardIndex;
    return decision;
  }

  if (affordableWonderIndex != -1) {
    decision.action = DecisionAction::BUILD_WONDER;
    decision.cardIndex = accessibleIndices.front();
    decision.wonderIndex = affordableWonderIndex;
    return decision;
  }

  decision.action = DecisionAction::DISCARD;
  decision.cardIndex = accessibleIndices.front();
  return decision;
}