#include "Wonder.h"
#include "Game.h"
#include "Player.h"
#include "Card.h"
#include <stdexcept>

std::string Wonder::getName() const {
    return name;
}

const Cost& Wonder::getCost() const {
    return cost;
}

const Effect& Wonder::getEffect() const {
    return effect;
}

bool Wonder::isBuilt() const {
    return built;
}

void Wonder::build() {
    built = true;
}

// 新增：实现奇迹效果激活
void Wonder::activateEffect(Game& game, Player& owner, Player& opponent) {
    const Effect& e = getEffect();
    
    // 处理基本资源和分数
    owner.addVictoryPoints(e.victoryPoints);
    owner.addMilitaryPower(e.militaryShields);
    owner.addCoins(e.coins);
    
    for (const auto& [type, amount] : e.resourcesProduced) {
        owner.addResource(type, amount);
    }
    
    for (const auto& symbol : e.scienceSymbols) {
        owner.addScienceSymbol(symbol);
    }
    
    // 处理特殊效果：再玩一回合
    if (e.playAgain) {
        game.setCurrentPlayer(owner);
    }
    
    // 处理特殊效果：销毁对手一张卡
    if (e.destroyOpponentCard && !opponent.getBuiltCards().empty()) {
        opponent.removeLastBuiltCard();
    }
}
