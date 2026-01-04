#include "Deck.h"
#include "CardFactory.h"
#include <algorithm>
#include <chrono>
#include <random>

namespace {
void removeThreeRandom(std::vector<Card> &deck, std::vector<Card> *removed) {
  if (deck.size() <= 3)
    return;
  static std::mt19937 rng(static_cast<unsigned>(
      std::chrono::system_clock::now().time_since_epoch().count()));
  std::shuffle(deck.begin(), deck.end(), rng);
  if (removed) {
    removed->insert(removed->end(), deck.begin(), deck.begin() + 3);
  }
  deck.erase(deck.begin(), deck.begin() + 3);
}
} // namespace

std::vector<Card> Deck::getAge1Deck(bool shuffle, std::vector<Card> *removed) {
  auto deck = CardFactory::getInstance().createAge1Deck();
  if (shuffle)
    shuffleDeck(deck);
  removeThreeRandom(deck, removed);
  return deck;
}

std::vector<Card> Deck::getAge2Deck(bool shuffle, std::vector<Card> *removed) {
  auto deck = CardFactory::getInstance().createAge2Deck();
  if (shuffle)
    shuffleDeck(deck);
  removeThreeRandom(deck, removed);
  return deck;
}

// 确保抽取公会牌时是随机抽取
std::vector<Card> Deck::getAge3Deck(bool shuffle, std::vector<Card> *removed) {
  auto deck = CardFactory::getInstance().createAge3Deck();
  if (shuffle)
    shuffleDeck(deck);
  removeThreeRandom(deck, removed);

  auto guilds = CardFactory::getInstance().createGuildCards();
  // Ensure random shuffle with time-based seed
  static std::mt19937 rng(static_cast<unsigned>(
      std::chrono::system_clock::now().time_since_epoch().count()));
  std::shuffle(guilds.begin(), guilds.end(), rng);
  for (int i = 0; i < 3 && i < static_cast<int>(guilds.size()); ++i) {
    deck.push_back(guilds[i]);
  }
  shuffleDeck(deck);
  return deck;
}

std::vector<Wonder> Deck::getWonders(bool shuffle) {
  auto wonders = createWonders();
  if (shuffle)
    shuffleDeck(wonders);
  return wonders;
}

std::vector<Wonder> Deck::createWonders() {
  std::vector<Wonder> wonders;
  auto uniqueWonders = CardFactory::getInstance().createAllWonders();
  for (auto &wonder : uniqueWonders) {
    wonders.push_back(*wonder);
  }
  return wonders;
}
