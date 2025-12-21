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

// 公会卡池创建（供时代3卡组使用）
std::vector<Card*> CardFactory::createGuildCardPool() {
    std::vector<Card*> guildCards;

    // 1. 商人公会 - 依据玩家拥有的商业建筑提供胜利点
    guildCards.push_back(createGuildCard(
        "MERCHANT GUILD",
        {Resource::Gold, 2, Resource::Papyrus, 1},
        1  // vpConditionType=1 表示按商业建筑数量计算
    ));

    // 2. 工匠公会 - 依据玩家拥有的制造品建筑提供胜利点
    guildCards.push_back(createGuildCard(
        "ARTISAN GUILD",
        {Resource::Gold, 2, Resource::Glass, 1},
        2  // vpConditionType=2 表示按制造品建筑数量计算
    ));

    // 3. 战士公会 - 依据玩家拥有的军事建筑提供胜利点
    guildCards.push_back(createGuildCard(
        "WARRIOR GUILD",
        {Resource::Gold, 2, Resource::Stone, 1},
        3  // vpConditionType=3 表示按军事建筑数量计算
    ));

    // 4. 学者公会 - 依据玩家拥有的科学建筑提供胜利点
    guildCards.push_back(createGuildCard(
        "SCHOLAR GUILD",
        {Resource::Gold, 2, Resource::Wood, 1},
        4  // vpConditionType=4 表示按科学建筑数量计算
    ));

    // 5. 建筑公会 - 依据玩家拥有的平民建筑提供胜利点
    guildCards.push_back(createGuildCard(
        "BUILDER GUILD",
        {Resource::Gold, 2, Resource::Clay, 1},
        5  // vpConditionType=5 表示按平民建筑数量计算
    ));

    // 6. 贸易公会 - 依据玩家的贸易标识提供胜利点
    guildCards.push_back(createGuildCard(
        "TRADE GUILD",
        {Resource::Gold, 3},
        6  // vpConditionType=6 表示按贸易标识数量计算
    ));

    // 验证公会卡池数量（6张基础公会卡）
    assert(guildCards.size() == 6 && "Guild card pool must have exactly 6 cards");

    return guildCards;
}

// 时代1卡组创建
std::vector<Card*> CardFactory::createAge1Deck() {
    std::vector<Card*> age1Cards;

    // 1. 棕色 - 原材料卡（6张）
    age1Cards.push_back(createRawMaterialCard(
        "LUMBER YARD",
        CardColor::Brown,
        {},
        Resource::Wood,
        0
    ));
    age1Cards.push_back(createRawMaterialCard(
        "LOGGING CAMP",
        CardColor::Brown,
        {},
        Resource::Wood,
        0
    ));
    age1Cards.push_back(createRawMaterialCard(
        "CLAY POOL",
        CardColor::Brown,
        {},
        Resource::Clay,
        0
    ));
    age1Cards.push_back(createRawMaterialCard(
        "CLAY PIT",
        CardColor::Brown,
        {},
        Resource::Clay,
        0
    ));
    age1Cards.push_back(createRawMaterialCard(
        "QUARRY",
        CardColor::Brown,
        {},
        Resource::Stone,
        0
    ));
    age1Cards.push_back(createRawMaterialCard(
        "STONE PIT",
        CardColor::Brown,
        {},
        Resource::Stone,
        0
    ));

    // 2. 灰色 - 制成品卡（2张）
    age1Cards.push_back(createManufacturedGoodCard(
        "GLASSWORKS",
        CardColor::Gray,
        {Resource::Wood, Resource::Clay},
        Resource::Glass,
        1
    ));
    age1Cards.push_back(createManufacturedGoodCard(
        "PRESS",
        CardColor::Gray,
        {Resource::Wood, Resource::Stone},
        Resource::Papyrus,
        1
    ));

    // 3. 红色 - 军事建筑卡（4张）
    age1Cards.push_back(createMilitaryBuildingCard(
        "GUARD TOWER",
        CardColor::Red,
        {Resource::Wood, Resource::Stone},
        [](Game* game, Player* player) {
            player->addShield(1);
        },
        1
    ));
    age1Cards.push_back(createMilitaryBuildingCard(
        "STABLE",
        CardColor::Red,
        {Resource::Wood, Resource::Clay},
        [](Game* game, Player* player) {
            player->addCavalryToken(1);
            player->addShield(1);
        },
        1
    ));
    age1Cards.push_back(createMilitaryBuildingCard(
        "GARRISON",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Clay},
        [](Game* game, Player* player) {
            player->addSwordToken(1);
            player->addShield(1);
        },
        2
    ));
    age1Cards.push_back(createMilitaryBuildingCard(
        "PALISADE",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Wood},
        [](Game* game, Player* player) {
            player->addShieldToken(1);
            player->addShield(1);
        },
        1
    ));

    // 4. 绿色 - 科学卡（4张）
    age1Cards.push_back(createScienceCard(
        "WORKSHOP",
        CardColor::Green,
        {Resource::Wood, Resource::Clay},
        ScienceSymbol::Triangle,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
            player->addScienceSymbol(ScienceSymbol::Triangle, 1);
        },
        0
    ));
    age1Cards.push_back(createScienceCard(
        "APOTHECARY",
        CardColor::Green,
        {Resource::Clay, Resource::Stone},
        ScienceSymbol::Flask,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Flask);
            player->addScienceSymbol(ScienceSymbol::Flask, 1);
        },
        0
    ));
    age1Cards.push_back(createScienceCard(
        "SCRIPTORIUM",
        CardColor::Green,
        {Resource::Wood, Resource::Papyrus},
        ScienceSymbol::Quill,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Quill);
            player->addScienceSymbol(ScienceSymbol::Quill, 1);
        },
        0
    ));
    age1Cards.push_back(createScienceCard(
        "PHARMACIST",
        CardColor::Green,
        {Resource::Glass, Resource::Clay},
        ScienceSymbol::Flask,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Flask);
            player->addScienceSymbol(ScienceSymbol::Flask, 1);
            player->triggerGearEffect();
        },
        0
    ));

    // 5. 黄色 - 商业建筑卡（4张）
    age1Cards.push_back(createCommercialBuildingCard(
        "STONE RESERVE",
        CardColor::Yellow,
        {Resource::Stone},
        [](Game* game, Player* player) {
            player->addPerTurnResource(Resource::Stone, 1);
            player->setResourceDiscount(Resource::Stone, 1);
        },
        0
    ));
    age1Cards.push_back(createCommercialBuildingCard(
        "CLAY RESERVE",
        CardColor::Yellow,
        {Resource::Clay},
        [](Game* game, Player* player) {
            player->addPerTurnResource(Resource::Clay, 1);
            player->setResourceDiscount(Resource::Clay, 1);
        },
        0
    ));
    age1Cards.push_back(createCommercialBuildingCard(
        "WOOD RESERVE",
        CardColor::Yellow,
        {Resource::Wood},
        [](Game* game, Player* player) {
            player->addPerTurnResource(Resource::Wood, 1);
            player->setResourceDiscount(Resource::Wood, 1);
        },
        0
    ));
    age1Cards.push_back(createCommercialBuildingCard(
        "TAVERN",
        CardColor::Yellow,
        {Resource::Wood, Resource::Clay},
        [](Game* game, Player* player) {
            player->addGold(4);
        },
        1
    ));

    // 6. 蓝色 - 民用建筑卡（3张）
    age1Cards.push_back(createCivilianBuildingCard(
        "THEATER",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Clay},
        [](Game* game, Player* player) {},
        3
    ));
    age1Cards.push_back(createCivilianBuildingCard(
        "ALTAR",
        CardColor::Blue,
        {Resource::Clay, Resource::Clay, Resource::Stone},
        [](Game* game, Player* player) {},
        3
    ));
    age1Cards.push_back(createCivilianBuildingCard(
        "BATHS",
        CardColor::Blue,
        {Resource::Wood, Resource::Stone, Resource::Clay},
        [](Game* game, Player* player) {},
        3
    ));

    assert(age1Cards.size() == 23 && "Age 1 deck must have exactly 23 cards");

    // 随机洗牌并移除3张
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age1Cards.begin(), age1Cards.end(), g);
    age1Cards.erase(age1Cards.end() - 3, age1Cards.end());

    return age1Cards;
}

// 时代2卡组创建
std::vector<Card*> CardFactory::createAge2Deck() {
    std::vector<Card*> age2Cards;

    // 1. 棕色 - 原材料卡（3张，带联动标识）
    age2Cards.push_back(createRawMaterialCard(
        "SAWMILL",
        CardColor::Brown,
        {Resource::Wood, Resource::Clay},
        Resource::Wood,
        0,
        true
    ));
    age2Cards.push_back(createRawMaterialCard(
        "BRICKYARD",
        CardColor::Brown,
        {Resource::Clay, Resource::Stone},
        Resource::Clay,
        0,
        true
    ));
    age2Cards.push_back(createRawMaterialCard(
        "SHELF QUARRY",
        CardColor::Brown,
        {Resource::Stone, Resource::Wood},
        Resource::Stone,
        0,
        true
    ));

    // 2. 灰色 - 制成品卡（2张）
    age2Cards.push_back(createManufacturedGoodCard(
        "GLASS-BLOWER",
        CardColor::Gray,
        {Resource::Wood, Resource::Glass},
        Resource::Glass,
        2
    ));
    age2Cards.push_back(createManufacturedGoodCard(
        "DRYING ROOM",
        CardColor::Gray,
        {Resource::Clay, Resource::Papyrus},
        Resource::Papyrus,
        2
    ));

    // 3. 红色 - 军事建筑卡（5张）
    age2Cards.push_back(createMilitaryBuildingCard(
        "WALLS",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Clay},
        [](Game* game, Player* player) {
            player->addShield(2);
            player->addShieldToken(1);
        },
        2
    ));
    age2Cards.push_back(createMilitaryBuildingCard(
        "HORSE BREEDERS",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Clay},
        [](Game* game, Player* player) {
            player->addShield(1);
            player->addCavalryToken(2);
        },
        2
    ));
    age2Cards.push_back(createMilitaryBuildingCard(
        "BARRACKS",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Wood, Resource::Clay},
        [](Game* game, Player* player) {
            player->addShield(2);
            player->addSwordToken(2);
        },
        3
    ));
    age2Cards.push_back(createMilitaryBuildingCard(
        "ARCHERY RANGE",
        CardColor::Red,
        {Resource::Wood, Resource::Stone, Resource::Glass},
        [](Game* game, Player* player) {
            player->addShield(1);
            player->addBowToken(2);
        },
        2
    ));
    age2Cards.push_back(createMilitaryBuildingCard(
        "PARADE GROUND",
        CardColor::Red,
        {Resource::Wood, Resource::Clay, Resource::Papyrus},
        [](Game* game, Player* player) {
            player->addShield(1);
            player->triggerGearEffect();
            player->copyRandomMilitaryToken();
        },
        3
    ));

    // 4. 绿色 - 科学卡（4张）
    age2Cards.push_back(createScienceCard(
        "LIBRARY",
        CardColor::Green,
        {Resource::Wood, Resource::Glass, Resource::Papyrus},
        ScienceSymbol::Quill,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Quill);
            player->addScienceSymbol(ScienceSymbol::Quill, 2);
            player->triggerGearEffect();
        },
        0
    ));
    age2Cards.push_back(createScienceCard(
        "DISPENSARY",
        CardColor::Green,
        {Resource::Clay, Resource::Glass, Resource::Papyrus},
        ScienceSymbol::Flask,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Flask);
            player->addScienceSymbol(ScienceSymbol::Flask, 2);
            player->triggerGearEffect();
        },
        0
    ));
    age2Cards.push_back(createScienceCard(
        "SCHOOL",
        CardColor::Green,
        {Resource::Stone, Resource::Glass, Resource::Papyrus},
        ScienceSymbol::Triangle,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
            player->addScienceSymbol(ScienceSymbol::Triangle, 1);
            player->addTridentToken(1);
        },
        1
    ));
    age2Cards.push_back(createScienceCard(
        "LABORATORY",
        CardColor::Green,
        {Resource::Glass, Resource::Glass, Resource::Papyrus},
        ScienceSymbol::Triangle,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
            player->addScienceSymbol(ScienceSymbol::Triangle, 1);
            player->triggerTriangleLinkEffect();
        },
        1
    ));

    // 5. 黄色 - 商业建筑卡（4张）
    age2Cards.push_back(createCommercialBuildingCard(
        "CARAVANSERY",
        CardColor::Yellow,
        {Resource::Wood, Resource::Clay, Resource::Gold},
        [](Game* game, Player* player) {
            player->addPerTurnGold(2);
            player->addTradeBonus(1);
        },
        1
    ));
    age2Cards.push_back(createCommercialBuildingCard(
        "CUSTOMS HOUSE",
        CardColor::Yellow,
        {Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) {
            int tradeCount = player->getTradeTokenCount();
            player->addGold(tradeCount * 2);
            player->addTradeToken(1);
        },
        2
    ));
    age2Cards.push_back(createCommercialBuildingCard(
        "TAVERN",
        CardColor::Yellow,
        {Resource::Wood, Resource::Papyrus},
        [](Game* game, Player* player) {
            player->addGold(4);
        },
        1
    ));
    age2Cards.push_back(createCommercialBuildingCard(
        "BREWERY",
        CardColor::Yellow,
        {Resource::Clay, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) {
            player->addGold(6);
        },
        2
    ));

    // 6. 蓝色 - 民用建筑卡（5张）
    age2Cards.push_back(createCivilianBuildingCard(
        "COURTHOUSE",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Papyrus},
        [](Game* game, Player* player) {},
        5
    ));
    age2Cards.push_back(createCivilianBuildingCard(
        "STATUE",
        CardColor::Blue,
        {Resource::Stone, Resource::Clay, Resource::Glass},
        [](Game* game, Player* player) {},
        4
    ));
    age2Cards.push_back(createCivilianBuildingCard(
        "TEMPLE",
        CardColor::Blue,
        {Resource::Wood, Resource::Clay, Resource::Papyrus},
        [](Game* game, Player* player) {},
        4
    ));
    age2Cards.push_back(createCivilianBuildingCard(
        "AQUEDUCT",
        CardColor::Blue,
        {Resource::Wood, Resource::Wood, Resource::Stone, Resource::Glass},
        [](Game* game, Player* player) {},
        5
    ));
    age2Cards.push_back(createCivilianBuildingCard(
        "ROSTRUM",
        CardColor::Blue,
        {Resource::Wood, Resource::Stone, Resource::Papyrus},
        [](Game* game, Player* player) {},
        4
    ));

    assert(age2Cards.size() == 23 && "Age 2 deck must have exactly 23 cards");

    // 随机洗牌并移除3张
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age2Cards.begin(), age2Cards.end(), g);
    age2Cards.erase(age2Cards.end() - 3, age2Cards.end());

    return age2Cards;
}

// 时代3卡组创建
std::vector<Card*> CardFactory::createAge3Deck() {
    std::vector<Card*> age3BaseCards;

    // 1. 黄色 - 商业建筑卡（5张）
    age3BaseCards.push_back(createCommercialBuildingCard(
        "LIGHTHOUSE",
        CardColor::Yellow,
        {Resource::Glass, Resource::Glass, Resource::Papyrus, Resource::Gold},
        [](Game* game, Player* player) { player->addGold(3); },
        2
    ));
    age3BaseCards.push_back(createCommercialBuildingCard(
        "ARENA",
        CardColor::Yellow,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) { 
            player->addGold(3); 
            player->addTriangleToken(1); 
        },
        2
    ));
    age3BaseCards.push_back(createCommercialBuildingCard(
        "CHAMBER OF COMMERCE",
        CardColor::Yellow,
        {Resource::Glass, Resource::Papyrus, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) { 
            int tradeCount = player->getTradeTokenCount();
            player->addGold(tradeCount * 3); 
        },
        3
    ));
    age3BaseCards.push_back(createCommercialBuildingCard(
        "PORT",
        CardColor::Yellow,
        {Resource::Wood, Resource::Glass, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) { 
            player->setTradeGoldBonus(2);
            player->addTradeToken(1); 
        },
        3
    ));
    age3BaseCards.push_back(createCommercialBuildingCard(
        "ARMORY",
        CardColor::Yellow,
        {Resource::Stone, Resource::Stone, Resource::Papyrus, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) { 
            player->addGold(5); 
            player->triggerGearEffect(); 
        },
        4
    ));

    // 2. 蓝色 - 民用建筑卡（6张）
    age3BaseCards.push_back(createCivilianBuildingCard(
        "PALACE",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Glass, Resource::Glass, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) {},
        7
    ));
    age3BaseCards.push_back(createCivilianBuildingCard(
        "TOWN HALL",
        CardColor::Blue,
        {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Papyrus, Resource::Papyrus, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) {},
        7
    ));
    age3BaseCards.push_back(createCivilianBuildingCard(
        "OBELISK",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) {},
        5
    ));
    age3BaseCards.push_back(createCivilianBuildingCard(
        "GARDENS",
        CardColor::Blue,
        {Resource::Wood, Resource::Wood, Resource::Clay, Resource::Clay, Resource::Gold},
        [](Game* game, Player* player) {},
        6
    ));
    age3BaseCards.push_back(createCivilianBuildingCard(
        "PANTHEON",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Papyrus, Resource::Papyrus, Resource::Gold},
        [](Game* game, Player* player) {},
        6
    ));
    age3BaseCards.push_back(createCivilianBuildingCard(
        "SENATE",
        CardColor::Blue,
        {Resource::Wood, Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) {},
        5
    ));

    // 3. 红色 - 军事建筑卡（5张）
    age3BaseCards.push_back(createMilitaryBuildingCard(
        "ARSENAL",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Glass, Resource::Papyrus},
        [](Game* game, Player* player) { player->addShield(3); },
        4
    ));
    age3BaseCards.push_back(createMilitaryBuildingCard(
        "PRETORIUM",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Clay, Resource::Clay, Resource::Glass},
        [](Game* game, Player* player) { 
            player->addShield(3); 
            player->addCommandToken(1); 
        },
        4
    ));
    age3BaseCards.push_back(createMilitaryBuildingCard(
        "FORTIFICATIONS",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Stone, Resource::Stone, Resource::Gold},
        [](Game* game, Player* player) { 
            player->addShield(4); 
            player->addShieldToken(2); 
        },
        5
    ));
    age3BaseCards.push_back(createMilitaryBuildingCard(
        "SIEGE WORKSHOP",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Glass, Resource::Glass, Resource::Papyrus},
        [](Game* game, Player* player) { 
            player->addShield(3); 
            player->addBowToken(3); 
        },
        4
    ));
    age3BaseCards.push_back(createMilitaryBuildingCard(
        "CIRCUS",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Papyrus, Resource::Papyrus, Resource::Gold},
        [](Game* game, Player* player) { 
            player->addShield(3); 
            player->addCavalryToken(3); 
        },
        4
    ));

    // 4. 绿色 - 科学卡（4张）
    age3BaseCards.push_back(createScienceCard(
        "ACADEMY",
        CardColor::Green,
        {Resource::Glass, Resource::Glass, Resource::Papyrus, Resource::Papyrus, Resource::Gold},
        ScienceSymbol::Quill,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Quill);
            player->addScienceSymbol(ScienceSymbol::Quill, 3);
        },
        0
    ));
    age3BaseCards.push_back(createScienceCard(
        "STUDY",
        CardColor::Green,
        {Resource::Glass, Resource::Papyrus, Resource::Papyrus, Resource::Papyrus, Resource::Gold},
        ScienceSymbol::Quill,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Quill);
            player->addScienceSymbol(ScienceSymbol::Quill, 3);
        },
        0
    ));
    age3BaseCards.push_back(createScienceCard(
        "UNIVERSITY",
        CardColor::Green,
        {Resource::Glass, Resource::Glass, Resource::Glass, Resource::Papyrus, Resource::Gold, Resource::Gold},
        ScienceSymbol::Quill,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Quill);
            player->addScienceSymbol(ScienceSymbol::Quill, 2);
            player->triggerGearEffect();
        },
        1
    ));
    age3BaseCards.push_back(createScienceCard(
        "OBSERVATORY",
        CardColor::Green,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Glass, Resource::Gold, Resource::Gold},
        ScienceSymbol::Triangle,
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
            player->addScienceSymbol(ScienceSymbol::Triangle, 2);
            player->triggerGearEffect();
        },
        1
    ));

    assert(age3BaseCards.size() == 20 && "Age 3 base deck must have exactly 20 cards");

    // 随机洗牌并移除3张基础卡
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age3BaseCards.begin(), age3BaseCards.end(), g);
    age3BaseCards.erase(age3BaseCards.end() - 3, age3BaseCards.end());
    assert(age3BaseCards.size() == 17 && "Age 3 base deck must have 17 cards after discarding 3");

    // 添加3张公会卡
    std::vector<Card*> guildCardPool = createGuildCardPool();
    std::shuffle(guildCardPool.begin(), guildCardPool.end(), g);
    auto selectedGuildCards = std::vector<Card*>(guildCardPool.begin(), guildCardPool.begin() + 3);
    age3BaseCards.insert(age3BaseCards.end(), selectedGuildCards.begin(), selectedGuildCards.end());

    assert(age3BaseCards.size() == 20 && "Age 3 complete deck must have exactly 20 cards");

    return age3BaseCards;
}

// 所有奇迹创建
std::vector<std::unique_ptr<Wonder>> createAllWonders() {
    std::vector<std::unique_ptr<Wonder>> allWonders;

    // 1. 时代1奇迹：罗得岛太阳神巨像（Colossus of Rhodes）
    auto rhodes = std::make_unique<Wonder>(
        "Colossus of Rhodes", 
        1, // 时代1
        1, // 初始盾牌
        2  // 总阶段数
    );
    // 阶段1：成本1木材+1石头，效果+1盾牌
    rhodes->addStage(WonderStage{
        {Resource::Wood, 1}, {Resource::Stone, 1},
        [](Game* game, Player* player) {
            player->addShield(1);
        },
        0 // 阶段1无胜利点
    });
    // 阶段2：成本2木材+2石头，效果+3胜利点
    rhodes->addStage(WonderStage{
        {Resource::Wood, 2}, {Resource::Stone, 2},
        [](Game* game, Player* player) {
            // 无额外效果，仅胜利点
        },
        3 // 阶段2提供3胜利点
    });
    allWonders.push_back(std::move(rhodes));

    // 2. 时代1奇迹：亚历山大图书馆（Library of Alexandria）
    auto alexLib = std::make_unique<Wonder>(
        "Library of Alexandria", 
        1, 
        0, 
        2
    );
    // 阶段1：成本1木材+1莎草纸，效果获得1科学符号（羽毛笔）
    alexLib->addStage(WonderStage{
        {Resource::Wood, 1}, {Resource::Papyrus, 1},
        [](Game* game, Player* player) {
            player->addScienceSymbol(ScienceSymbol::Quill, 1);
        },
        0
    });
    // 阶段2：成本1玻璃+1莎草纸，效果再获得1科学符号（羽毛笔）+2胜利点
    alexLib->addStage(WonderStage{
        {Resource::Glass, 1}, {Resource::Papyrus, 1},
        [](Game* game, Player* player) {
            player->addScienceSymbol(ScienceSymbol::Quill, 1);
        },
        2
    });
    allWonders.push_back(std::move(alexLib));

    // 3. 时代2奇迹：巴比伦空中花园（Hanging Gardens）
    auto hangingGardens = std::make_unique<Wonder>(
        "Hanging Gardens", 
        2, 
        1, 
        3
    );
    // 阶段1：成本2木材+1粘土，效果每回合+1粘土
    hangingGardens->addStage(WonderStage{
        {Resource::Wood, 2}, {Resource::Clay, 1},
        [](Game* game, Player* player) {
            player->addPerTurnResource(Resource::Clay, 1);
        },
        0
    });
    // 阶段2：成本1木材+2石头，效果+1盾牌
    hangingGardens->addStage(WonderStage{
        {Resource::Wood, 1}, {Resource::Stone, 2},
        [](Game* game, Player* player) {
            player->addShield(1);
        },
        2
    });
    // 阶段3：成本1玻璃+1莎草纸+1金币，效果+4胜利点
    hangingGardens->addStage(WonderStage{
        {Resource::Glass, 1}, {Resource::Papyrus, 1}, {Resource::Gold, 1},
        [](Game* game, Player* player) {
            // 无额外效果
        },
        4
    });
    allWonders.push_back(std::move(hangingGardens));

    // 4. 时代2奇迹：宙斯神像（Statue of Zeus）
    auto zeusStatue = std::make_unique<Wonder>(
        "Statue of Zeus", 
        2, 
        2, 
        3
    );
    // 阶段1：成本2石头+1粘土，效果+1胜利点
    zeusStatue->addStage(WonderStage{
        {Resource::Stone, 2}, {Resource::Clay, 1},
        [](Game* game, Player* player) {},
        1
    });
    // 阶段2：成本1石头+1玻璃+1金币，效果+1盾牌
    zeusStatue->addStage(WonderStage{
        {Resource::Stone, 1}, {Resource::Glass, 1}, {Resource::Gold, 1},
        [](Game* game, Player* player) {
            player->addShield(1);
        },
        2
    });
    // 阶段3：成本2玻璃+1莎草纸，效果触发齿轮联动+3胜利点
    zeusStatue->addStage(WonderStage{
        {Resource::Glass, 2}, {Resource::Papyrus, 1},
        [](Game* game, Player* player) {
            player->triggerGearEffect(); // 与卡牌中齿轮效果一致
        },
        3
    });
    allWonders.push_back(std::move(zeusStatue));

    // 5. 时代3奇迹：罗马斗兽场（Colosseum）
    auto colosseum = std::make_unique<Wonder>(
        "Colosseum", 
        3, 
        3, 
        3
    );
    // 阶段1：成本3石头+1木材，效果+2盾牌
    colosseum->addStage(WonderStage{
        {Resource::Stone, 3}, {Resource::Wood, 1},
        [](Game* game, Player* player) {
            player->addShield(2);
        },
        0
    });
    // 阶段2：成本2玻璃+2莎草纸，效果每回合+2金币
    colosseum->addStage(WonderStage{
        {Resource::Glass, 2}, {Resource::Papyrus, 2},
        [](Game* game, Player* player) {
            player->addPerTurnGold(2);
        },
        3
    });
    // 阶段3：成本2金币+1石头+1玻璃，效果+5胜利点+1指挥标识
    colosseum->addStage(WonderStage{
        {Resource::Gold, 2}, {Resource::Stone, 1}, {Resource::Glass, 1},
        [](Game* game, Player* player) {
            player->addCommandToken(1); // 与军事卡牌效果一致
        },
        5
    });
    allWonders.push_back(std::move(colosseum));

    // 6. 时代3奇迹：埃及金字塔（Great Pyramid）
    auto pyramid = std::make_unique<Wonder>(
        "Great Pyramid", 
        3, 
        2, 
        4
    );
    // 阶段1：成本2石头+1粘土，效果+1胜利点
    pyramid->addStage(WonderStage{
        {Resource::Stone, 2}, {Resource::Clay, 1},
        [](Game* game, Player* player) {},
        1
    });
    // 阶段2：成本3石头+1木材，效果+1盾牌
    pyramid->addStage(WonderStage{
        {Resource::Stone, 3}, {Resource::Wood, 1},
        [](Game* game, Player* player) {
            player->addShield(1);
        },
        2
    });
    // 阶段3：成本1玻璃+1莎草纸+2金币，效果+3胜利点
    pyramid->addStage(WonderStage{
        {Resource::Glass, 1}, {Resource::Papyrus, 1}, {Resource::Gold, 2},
        [](Game* game, Player* player) {},
        3
    });
    // 阶段4：成本2石头+2玻璃，效果触发三角联动+4胜利点
    pyramid->addStage(WonderStage{
        {Resource::Stone, 2}, {Resource::Glass, 2},
        [](Game* game, Player* player) {
            player->triggerTriangleLinkEffect(); // 与科学卡牌效果一致
        },
        4
    });
    allWonders.push_back(std::move(pyramid));

    return allWonders;
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
