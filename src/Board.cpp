#include "Board.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

Board::Board() : militaryPosition(0) {}

void Board::setupAge(int age, std::vector<Card> deck) {
  pyramid.clear();
  if (age == 1)
    setupAge1(deck);
  else if (age == 2)
    setupAge2(deck);
  else if (age == 3)
    setupAge3(deck);

  revealAvailableCards();
}

const std::vector<CardSlot> &Board::getPyramid() const { return pyramid; }

bool Board::isCardAccessible(int index) const {
  if (index < 0 || index >= pyramid.size())
    return false;
  if (pyramid[index].isTaken)
    return false;

  for (int coverIndex : pyramid[index].coveredBy) {
    if (!pyramid[coverIndex].isTaken) {
      return false;
    }
  }
  return true;
}

Card Board::takeCard(int index) {
  if (!isCardAccessible(index)) {
    throw std::runtime_error("Card is not accessible");
  }
  pyramid[index].isTaken = true;
  revealAvailableCards();
  return pyramid[index].card;
}

void Board::revealAvailableCards() {
  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (!pyramid[i].isTaken && !pyramid[i].isFaceUp) {
      if (isCardAccessible(i)) {
        pyramid[i].isFaceUp = true;
      }
    }
  }
}

int Board::getMilitaryPosition() const { return militaryPosition; }

void Board::moveMilitary(int amount) {
  militaryPosition += amount;
  // Clamp logic or victory check should be handled by Game class
}

const std::vector<Wonder *> &Board::getAvailableWonders() const {
  return availableWonders;
}

void Board::setAvailableWonders(const std::vector<Wonder *> &wonders) {
  availableWonders = wonders;
}

Wonder *Board::takeWonder(int index) {
  if (index < 0 || index >= availableWonders.size())
    return nullptr;
  Wonder *w = availableWonders[index];
  availableWonders.erase(availableWonders.begin() + index);
  return w;
}

// Helper to add a slot
void addSlot(std::vector<CardSlot> &p, const Card &c, bool faceUp,
             const std::vector<int> &coveredBy) {
  p.push_back({c, faceUp, false, coveredBy});
}

void Board::setupAge1(std::vector<Card> &deck) {
  // Age I Structure: 20 cards
  // Row 0: 2 cards (Indices 0-1) - Covered by 2-4
  // Row 1: 3 cards (Indices 2-4) - Covered by 5-9
  // Row 2: 4 cards (Indices 5-8) - Covered by 9-14
  // Row 3: 5 cards (Indices 9-13) - Covered by 14-19
  // Row 4: 6 cards (Indices 14-19) - Uncovered

  // Note: Indices need to be mapped correctly.
  // Let's build from bottom up to make dependency defining easier?
  // Or just hardcode.

  // 20 cards needed.
  if (deck.size() < 20)
    return; // Error handling needed

  int cardIdx = 0;

  // Row 0 (Top)
  addSlot(pyramid, deck[cardIdx++], true, {2, 3});
  addSlot(pyramid, deck[cardIdx++], true, {3, 4});

  // Row 1
  addSlot(pyramid, deck[cardIdx++], false, {5, 6});
  addSlot(pyramid, deck[cardIdx++], false, {6, 7});
  addSlot(pyramid, deck[cardIdx++], false, {7, 8});

  // Row 2
  addSlot(pyramid, deck[cardIdx++], true, {9, 10});
  addSlot(pyramid, deck[cardIdx++], true, {10, 11});
  addSlot(pyramid, deck[cardIdx++], true, {11, 12});
  addSlot(pyramid, deck[cardIdx++], true, {12, 13});

  // Row 3
  addSlot(pyramid, deck[cardIdx++], false, {14, 15});
  addSlot(pyramid, deck[cardIdx++], false, {15, 16});
  addSlot(pyramid, deck[cardIdx++], false, {16, 17});
  addSlot(pyramid, deck[cardIdx++], false, {17, 18});
  addSlot(pyramid, deck[cardIdx++], false, {18, 19});

  // Row 4 (Bottom) - No covers
  for (int i = 0; i < 6; ++i) {
    addSlot(pyramid, deck[cardIdx++], true, {});
  }
}

void Board::setupAge2(std::vector<Card> &deck) {
  // Age II Structure (Inverted Pyramid)
  // Shape: 6 -> 5 -> 4 -> 3 -> 2 (20 cards total)
  // Visual:
  // 0 1 2 3 4 5      (Row 0 - Top)
  //  6 7 8 9 10      (Row 1)
  //   11 12 13 14    (Row 2)
  //    15 16 17      (Row 3)
  //     18 19        (Row 4 - Bottom, accessible)

  if (deck.size() < 20)
    return;
  int cardIdx = 0;

  // Row 0 (0-5) - Covered by Row 1
  addSlot(pyramid, deck[cardIdx++], true, {6});
  addSlot(pyramid, deck[cardIdx++], true, {6, 7});
  addSlot(pyramid, deck[cardIdx++], true, {7, 8});
  addSlot(pyramid, deck[cardIdx++], true, {8, 9});
  addSlot(pyramid, deck[cardIdx++], true, {9, 10});
  addSlot(pyramid, deck[cardIdx++], true, {10});

  // Row 1 (6-10) - Covered by Row 2
  addSlot(pyramid, deck[cardIdx++], false, {11});
  addSlot(pyramid, deck[cardIdx++], false, {11, 12});
  addSlot(pyramid, deck[cardIdx++], false, {12, 13});
  addSlot(pyramid, deck[cardIdx++], false, {13, 14});
  addSlot(pyramid, deck[cardIdx++], false, {14});

  // Row 2 (11-14) - Covered by Row 3
  addSlot(pyramid, deck[cardIdx++], true, {15});
  addSlot(pyramid, deck[cardIdx++], true, {15, 16});
  addSlot(pyramid, deck[cardIdx++], true, {16, 17});
  addSlot(pyramid, deck[cardIdx++], true, {17});

  // Row 3 (15-17) - Covered by Row 4
  addSlot(pyramid, deck[cardIdx++], false, {18});
  addSlot(pyramid, deck[cardIdx++], false, {18, 19});
  addSlot(pyramid, deck[cardIdx++], false, {19});

  // Row 4 (18-19) - Bottom, accessible (no covers)
  addSlot(pyramid, deck[cardIdx++], true, {});
  addSlot(pyramid, deck[cardIdx++], true, {});
}

void Board::setupAge3(std::vector<Card> &deck) {
  // Age 3 is like Age 1 but slightly bigger? Or same?
  // Duel Age 3 is same shape as Age 1 usually.
  setupAge1(deck);
}
