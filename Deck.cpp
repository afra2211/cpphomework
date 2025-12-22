#include "Deck.h"
#include <random>
#include <algorithm>
#include <stdexcept>

Deck::Deck() {
    // 初始化所有卡组
    initAge1Deck();
    initAge2Deck();
    initWonders();
}

void Deck::initAge1Deck() {
    // 从CardFactory获取时代1卡组（原始指针）
    std::vector<Card*> rawCards = CardFactory::getInstance().createAge1Deck();
    // 转换为unique_ptr管理，存储到成员变量
    age1Cards = convertToUniquePtrs(rawCards);
}

void Deck::initAge2Deck() {
    // 从CardFactory获取时代2卡组（原始指针）
    std::vector<Card*> rawCards = CardFactory::getInstance().createAge2Deck();
    // 转换为unique_ptr管理，存储到成员变量
    age2Cards = convertToUniquePtrs(rawCards);
}

void Deck::initWonders() {
    // 假设CardFactory提供创建所有奇迹的方法
    wonders = CardFactory::getInstance().createAllWonders();
}

void Deck::shuffleAge1Deck() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age1Cards.begin(), age1Cards.end(), g);
}

void Deck::shuffleAge2Deck() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age2Cards.begin(), age2Cards.end(), g);
}

std::unique_ptr<Card> Deck::drawAge1Card() {
    if (isAge1DeckEmpty()) {
        throw std::runtime_error("Age 1 deck is empty, cannot draw card");
    }
    // 转移最后一张卡牌的所有权
    std::unique_ptr<Card> card = std::move(age1Cards.back());
    age1Cards.pop_back();
    return card;
}

std::unique_ptr<Card> Deck::drawAge2Card() {
    if (isAge2DeckEmpty()) {
        throw std::runtime_error("Age 2 deck is empty, cannot draw card");
    }
    // 转移最后一张卡牌的所有权
    std::unique_ptr<Card> card = std::move(age2Cards.back());
    age2Cards.pop_back();
    return card;
}

std::vector<Wonder*> Deck::getWonders() const {
    std::vector<Wonder*> wonderPtrs;
    for (const auto& wonder : wonders) {
        wonderPtrs.push_back(wonder.get());
    }
    return wonderPtrs;
}

bool Deck::isAge1DeckEmpty() const {
    return age1Cards.empty();
}

bool Deck::isAge2DeckEmpty() const {
    return age2Cards.empty();
}

size_t Deck::getAge1DeckSize() const {
    return age1Cards.size();
}

size_t Deck::getAge2DeckSize() const {
    return age2Cards.size();
}
