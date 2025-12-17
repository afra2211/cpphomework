#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include "Wonder.h"
#include <memory>
#include <vector>
#include <random>  // 用于随机洗牌
#include <algorithm> // 用于std::shuffle

class Deck {
public:
  // 获取对应时代的牌组（新增洗牌功能）
  static std::vector<std::shared_ptr<Card>> getAge1Deck(bool shuffle = false) {
    auto deck = CardFactory::getInstance().createAge1Deck();
    if (shuffle) shuffleDeck(deck);
    return deck;
  }

  static std::vector<std::shared_ptr<Card>> getAge2Deck(bool shuffle = false) {
    auto deck = CardFactory::getInstance().createAge2Deck();
    if (shuffle) shuffleDeck(deck);
    return deck;
  }

  static std::vector<std::shared_ptr<Card>> getAge3Deck(bool shuffle = false) {
    auto deck = CardFactory::getInstance().createAge3Deck();
    if (shuffle) shuffleDeck(deck);
    return deck;
  }

  // 获取奇迹牌组
  static std::vector<Wonder> getWonders(bool shuffle = false) {
    auto wonders = createWonders();  // 假设内部实现牌组创建
    if (shuffle) shuffleDeck(wonders);
    return wonders;
  }

private:
  // 洗牌函数（
  template <typename T>
  static void shuffleDeck(std::vector<T>& deck) {
    static std::random_device rd;  // 随机设备，用于种子
    static std::mt19937 g(rd());   // 梅森旋转算法随机数生成器
    std::shuffle(deck.begin(), deck.end(), g);  // 洗牌
  }

  // 补充createWonders的声明
  static std::vector<Wonder> createWonders();
  
};

#endif // DECK_H