#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include "Wonder.h"
#include <memory>
#include <vector>

class Deck {
public:
  static std::vector<std::shared_ptr<Card>> getAge1Deck() {
    return CardFactory::getInstance().createAge1Deck();
  }
  static std::vector<std::shared_ptr<Card>> getAge2Deck() {
    return CardFactory::getInstance().createAge2Deck();
  }
  static std::vector<std::shared_ptr<Card>> getAge3Deck() {
    return CardFactory::getInstance().createAge3Deck();
  }
  static std::vector<Wonder> getWonders();
};

#endif // DECK_H