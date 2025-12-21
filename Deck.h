#ifndef DECK_H
#define DECK_H

#include <vector>
#include <memory>
#include "Card.h"
#include "CardFactory.h"

class Deck {
private:
    // 存储各时代卡牌（使用智能指针管理内存）
    std::vector<std::unique_ptr<Card>> age1Cards;
    std::vector<std::unique_ptr<Card>> age2Cards;
    std::vector<std::unique_ptr<Wonder>> wonders;

    // 从工厂获取卡牌并转换为unique_ptr
    template <typename CardType>
    std::vector<std::unique_ptr<CardType>> convertToUniquePtrs(std::vector<CardType*> rawPtrs) {
        std::vector<std::unique_ptr<CardType>> uniquePtrs;
        for (auto* ptr : rawPtrs) {
            uniquePtrs.emplace_back(ptr);
        }
        return uniquePtrs;
    }

public:
    Deck();

    // 初始化各时代卡组
    void initAge1Deck();
    void initAge2Deck();
    void initWonders();

    // 洗牌
    void shuffleAge1Deck();
    void shuffleAge2Deck();

    // 抽卡（返回unique_ptr转移所有权）
    std::unique_ptr<Card> drawAge1Card();
    std::unique_ptr<Card> drawAge2Card();

    // 获取奇迹卡牌（返回原始指针，所有权仍由Deck管理）
    std::vector<Wonder*> getWonders() const;

    // 检查卡组是否为空
    bool isAge1DeckEmpty() const;
    bool isAge2DeckEmpty() const;

    // 获取当前卡组大小
    size_t getAge1DeckSize() const;
    size_t getAge2DeckSize() const;
};

#endif // DECK_H
