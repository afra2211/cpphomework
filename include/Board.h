#ifndef BOARD_H
#define BOARD_H

#include "Card.h"
#include "Wonder.h"
#include <map>
#include <memory>
#include <vector>

struct CardSlot {
  Card card;
  bool isFaceUp;
  bool isTaken;
  std::vector<int> coveredBy; // Indices of slots that cover this one
};

class Board {
public:
  Board();

  void setupAge(int age, std::vector<Card> deck);
  const std::vector<CardSlot> &getPyramid() const;
  bool isCardAccessible(int index) const;
  Card takeCard(int index);
  void revealAvailableCards();

  int getMilitaryPosition()
      const; // 0 is center, negative for P1, positive for P2
  void moveMilitary(
      int amount); // amount > 0 moves towards P2 (P1 advantage), < 0 towards P1

  const std::vector<Wonder *> &getAvailableWonders() const;
  void setAvailableWonders(const std::vector<Wonder *> &wonders);
  Wonder *takeWonder(int index);

private:
  std::vector<CardSlot> pyramid;
  int militaryPosition; // Range -9 to 9, 0 is start
  std::vector<Wonder *> availableWonders;

  void setupAge1(std::vector<Card> &deck);
  void setupAge2(std::vector<Card> &deck);
  void setupAge3(std::vector<Card> &deck);
};

#endif // BOARD_H
