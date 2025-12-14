#include "AI.h"
#include <cstdlib>
#include <iostream>
#include <vector>

int AI::getWonderChoice(const std::vector<Wonder *> &wonders) {
  // 简单的AI奇迹选择策略：随机选择
  if (wonders.empty())
    return 0;
  return (rand() % wonders.size()) + 1;
}

int AI::getBestMove(const Game &game, const Player &player) {
  const Board &board = game.getBoard();
  const auto &pyramid = board.getPyramid();

  std::vector<int> accessibleIndices;
  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (board.isCardAccessible(i)) {
      accessibleIndices.push_back(i);
    }
  }

  if (accessibleIndices.empty())
    return -1;

  // Simple Heuristic:
  // 1. Can I afford it?
  // 2. Does it give VP?
  // 3. Does it give Military?
  // 4. Does it give Science?

  int bestIndex = -1;
  int bestScore = -100;

  const Player &opponent =
      (&player == &game.getPlayer1()) ? game.getPlayer2() : game.getPlayer1();

  for (int idx : accessibleIndices) {
    const Card &card = pyramid[idx].card;
    int score = 0;

    if (player.canAfford(card.getCost(), opponent)) {
      score += 10; // Base score for being affordable

      const Effect &e = card.getEffect();
      score += e.victoryPoints * 2;
      score += e.militaryShields * 3;
      if (!e.scienceSymbols.empty())
        score += 5;
      score += e.resourcesProduced.size() * 2;

      // Prioritize military if losing
      if (board.getMilitaryPosition() < -2 && &player == &game.getPlayer1())
        score += e.militaryShields * 5;
      if (board.getMilitaryPosition() > 2 && &player == &game.getPlayer2())
        score += e.militaryShields * 5;

    } else {
      // If can't afford, we discard it for money.
      // Score based on denying opponent?
      // For now, low score.
      score = 0;
    }

    if (score > bestScore) {
      bestScore = score;
      bestIndex = idx;
    }
  }

  // If no affordable card found, pick random to discard
  if (bestIndex == -1) {
    bestIndex = accessibleIndices[rand() % accessibleIndices.size()];
  }

  return bestIndex;
}
