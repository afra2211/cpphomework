#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include "Wonder.h"
#include <vector>


class Deck {
public:
  static std::vector<Card> getAge1Deck();
  static std::vector<Card> getAge2Deck();
  static std::vector<Card> getAge3Deck();
  static std::vector<Wonder> getWonders();
};

#endif // DECK_H
