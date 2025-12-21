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
  // 获取对应时代的牌组（新增洗牌功能），可选返回被移除的3张卡牌
  static std::vector<Card> getAge1Deck(bool shuffle = false,
                                       std::vector<Card> *removed = nullptr);

  static std::vector<Card> getAge2Deck(bool shuffle = false,
                                       std::vector<Card> *removed = nullptr);

  static std::vector<Card> getAge3Deck(bool shuffle = false,
                                       std::vector<Card> *removed = nullptr);

  // 获取奇迹牌组
  static std::vector<Wonder> getWonders(bool shuffle = false);

private:
  // 洗牌函数（
  template <typename T>
  static void shuffleDeck(std::vector<T> &deck) {
    static std::random_device rd;   // 随机设备，用于种子
    static std::mt19937 g(rd());    // 梅森旋转算法随机数生成器
    std::shuffle(deck.begin(), deck.end(), g); // 洗牌
  }

  // 补充createWonders的声明
  static std::vector<Wonder> createWonders();
  
};

#endif // DECK_H