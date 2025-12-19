#include "CardFactory.h"
#include "Wonder.h"
#include <stdexcept>
#include <memory>

// 单例模式实现
CardFactory& CardFactory::getInstance() {
    static CardFactory instance;
    return instance;
}

// 原材料卡创建
std::unique_ptr<Card> CardFactory::createRawMaterialCard(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes,
    const std::map<ResourceType, int>& prodRes,
    int coins) {
    
    Cost cost{costCoins, costRes};
    Effect effect;
    effect.resourcesProduced = prodRes;
    effect.coins = coins;
    
    return createCardInternal(name, CardType::RAW_MATERIAL, cost, effect, "", "");
}

// 制造品卡创建
std::unique_ptr<Card> CardFactory::createManufacturedGoodCard(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes,
    const std::map<ResourceType, int>& prodRes,
    int coins) {
    
    Cost cost{costCoins, costRes};
    Effect effect;
    effect.resourcesProduced = prodRes;
    effect.coins = coins;
    
    return createCardInternal(name, CardType::MANUFACTURED_GOOD, cost, effect, "", "");
}

// 平民建筑卡创建
std::unique_ptr<Card> CardFactory::createCivilianCard(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes, 
    int vp,
    const std::string& chainSymbol,
    const std::string& chainTarget) {
    
    Cost cost{costCoins, costRes};
    Effect effect;
    effect.victoryPoints = vp;
    
    return createCardInternal(name, CardType::CIVILIAN, cost, effect, chainSymbol, chainTarget);
}

// 军事建筑卡创建
std::unique_ptr<Card> CardFactory::createMilitaryCard(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes, 
    int shields,
    const std::string& chainSymbol,
    const std::string& chainTarget) {
    
    Cost cost{costCoins, costRes};
    Effect effect;
    effect.militaryShields = shields;
    
    return createCardInternal(name, CardType::MILITARY, cost, effect, chainSymbol, chainTarget);
}

// 科学建筑卡创建
std::unique_ptr<Card> CardFactory::createScientificCard(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes, 
    int vp, 
    ScienceSymbol symbol,
    const std::string& chainSymbol,
    const std::string& chainTarget) {
    
    Cost cost{costCoins, costRes};
    Effect effect;
    effect.victoryPoints = vp;
    effect.scienceSymbols = {symbol};
    
    return createCardInternal(name, CardType::SCIENTIFIC, cost, effect, chainSymbol, chainTarget);
}

// 商业建筑卡创建
std::unique_ptr<Card> CardFactory::createCommercialCard(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes, 
    int vp, 
    int immediateCoins,
    const std::map<ResourceType, int>& prodRes,
    const std::string& chainSymbol,
    const std::string& chainTarget) {
    
    Cost cost{costCoins, costRes};
    Effect effect;
    effect.victoryPoints = vp;
    effect.coins = immediateCoins;
    effect.resourcesProduced = prodRes;
    
    return createCardInternal(name, CardType::COMMERCIAL, cost, effect, chainSymbol, chainTarget);
}

// 公会卡创建
std::unique_ptr<Card> CardFactory::createGuildCard(
    const std::string& name,
    const std::map<ResourceType, int>& costRes,
    int vpConditionType) {
    
    Cost cost{0, costRes};
    Effect effect;
    
    return createCardInternal(name, CardType::GUILD, cost, effect, "", "");
}

// 奇迹创建
std::unique_ptr<Wonder> CardFactory::createWonder(
    const std::string& name, 
    int costCoins,
    const std::map<ResourceType, int>& costRes,
    const Effect& effect) {
    
    Cost cost{costCoins, costRes};
    return std::make_unique<Wonder>(name, cost, effect);
}

// 时代1卡牌组创建
std::vector<std::unique_ptr<Card>> CardFactory::createAge1Deck() {
    std::vector<std::unique_ptr<Card>> deck;
    
    // 原材料卡
    deck.push_back(createRawMaterialCard(
        "LUMBER YARD", 0, {}, {{ResourceType::WOOD, 1}}, 0));
    deck.push_back(createRawMaterialCard(
        "LOGGING CAMP", 0, {}, {{ResourceType::WOOD, 1}}, 0));
    deck.push_back(createRawMaterialCard(
        "CLAY POOL", 0, {}, {{ResourceType::CLAY, 1}}, 0));
    deck.push_back(createRawMaterialCard(
        "CLAY PIT", 0, {}, {{ResourceType::CLAY, 1}}, 0));
    deck.push_back(createRawMaterialCard(
        "QUARRY", 0, {}, {{ResourceType::STONE, 1}}, 0));
    deck.push_back(createRawMaterialCard(
        "STONE PIT", 0, {}, {{ResourceType::STONE, 1}}, 0));
    
    return deck;
}

// 时代2卡牌组创建
std::vector<std::unique_ptr<Card>> CardFactory::createAge2Deck() {
    std::vector<std::unique_ptr<Card>> deck;
    return deck;
}

// 时代3卡牌组创建
std::vector<std::unique_ptr<Card>> CardFactory::createAge3Deck() {
    std::vector<std::unique_ptr<Card>> deck;
    return deck;
}

// 所有奇迹创建
std::vector<std::unique_ptr<Wonder>> CardFactory::createAllWonders() {
    std::vector<std::unique_ptr<Wonder>> wonders;
    
    Effect rhodesEffect;
    rhodesEffect.victoryPoints = 5;
    rhodesEffect.playAgain = true;
    wonders.push_back(createWonder(
        "Colossus of Rhodes",
        3,
        {{ResourceType::STONE, 2}, {ResourceType::WOOD, 1}},
        rhodesEffect
    ));
    
    Effect alexEffect;
    alexEffect.victoryPoints = 3;
    alexEffect.destroyOpponentCard = true;
    wonders.push_back(createWonder(
        "Lighthouse of Alexandria",
        2,
        {{ResourceType::STONE, 1}, {ResourceType::GLASS, 1}},
        alexEffect
    ));
    
    return wonders;
}

// 内部卡牌创建辅助函数
std::unique_ptr<Card> CardFactory::createCardInternal(
    const std::string& name,
    CardType type,
    Cost cost,
    Effect effect,
    const std::string& chainSymbol,
    const std::string& chainTarget) {
    
    return std::make_unique<Card>(name, type, cost, effect, chainSymbol, chainTarget);
}
