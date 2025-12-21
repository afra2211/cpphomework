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

// 时代1卡牌组创建（23张基础牌）
std::vector<Card> CardFactory::createAge1Deck() {
    std::vector<Card> deck;
    auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

    add(createRawMaterialCard("LUMBER YARD", 0, {}, {{ResourceType::WOOD, 1}}, 0));
    add(createRawMaterialCard("LOGGING CAMP", 1, {}, {{ResourceType::WOOD, 1}}, 0));
    add(createRawMaterialCard("CLAY POOL", 0, {}, {{ResourceType::CLAY, 1}}, 0));
    add(createRawMaterialCard("CLAY PIT", 1, {}, {{ResourceType::CLAY, 1}}, 0));
    add(createRawMaterialCard("STONE PIT", 0, {}, {{ResourceType::STONE, 1}}, 0));
    add(createRawMaterialCard("QUARRY", 0, {}, {{ResourceType::STONE, 1}}, 0));

    add(createManufacturedGoodCard("GLASSWORKS", 0, {}, {{ResourceType::GLASS, 1}}, 0));
    add(createManufacturedGoodCard("PRESS", 0, {}, {{ResourceType::PAPER, 1}}, 0));

    add(createMilitaryCard("GUARD TOWER", 0, {{ResourceType::CLAY,1}}, 1));
    add(createMilitaryCard("GARRISON", 0, {{ResourceType::STONE,1}}, 1));
    add(createMilitaryCard("STABLE", 0, {{ResourceType::WOOD,1},{ResourceType::CLAY,1}}, 1));
    add(createMilitaryCard("STOCKADE", 0, {{ResourceType::WOOD,1}}, 1));

    add(createScientificCard("WORKSHOP", 0, {{ResourceType::CLAY,1}}, 0, ScienceSymbol::GEAR));
    add(createScientificCard("APOTHECARY", 0, {{ResourceType::WOOD,1}}, 0, ScienceSymbol::COMPASS));
    add(createScientificCard("SCRIPTORIUM", 0, {{ResourceType::PAPER,1}}, 0, ScienceSymbol::TABLET));
    add(createScientificCard("PHARMACIST", 0, {{ResourceType::GLASS,1}}, 0, ScienceSymbol::MORTAR));

    add(createCommercialCard("TAVERN", 0, {}, 0, 4));
    add(createCommercialCard("EAST TRADING POST", 0, {}, 0, 0));
    add(createCommercialCard("WEST TRADING POST", 0, {}, 0, 0));
    add(createCommercialCard("MARKETPLACE", 0, {}, 0, 0));

    add(createCivilianCard("BATHS", 0, {{ResourceType::STONE,1}}, 3, "", "AQUEDUCT"));
    add(createCivilianCard("ALTAR", 0, {}, 2, "", "TEMPLE"));
    add(createCivilianCard("THEATER", 0, {}, 3, "MASK", "STATUE"));
    add(createCivilianCard("PAWNSHOP", 0, {}, 3));

    return deck;
}

// 时代2卡牌组创建（23张基础牌）
std::vector<Card> CardFactory::createAge2Deck() {
    std::vector<Card> deck;
    auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

    add(createRawMaterialCard("SAWMILL", 3, {}, {{ResourceType::WOOD, 2}}, 0));
    add(createRawMaterialCard("BRICKYARD", 3, {}, {{ResourceType::CLAY, 2}}, 0));
    add(createRawMaterialCard("SHELF QUARRY", 3, {}, {{ResourceType::STONE, 2}}, 0));
    add(createManufacturedGoodCard("GLASSBLOWER", 0, {}, {{ResourceType::GLASS, 1}}, 0));
    add(createManufacturedGoodCard("DRYING ROOM", 0, {}, {{ResourceType::PAPER, 1}}, 0));

    add(createMilitaryCard("WALLS", 0, {{ResourceType::STONE,3}}, 2, "", "FORTIFICATIONS"));
    add(createMilitaryCard("PARADE GROUND", 0, {{ResourceType::CLAY,2}}, 2));
    add(createMilitaryCard("HORSE BREEDERS", 0, {{ResourceType::WOOD,1},{ResourceType::CLAY,1}}, 1, "", "CIRCUS"));
    add(createMilitaryCard("BARRACKS", 0, {{ResourceType::WOOD,1}}, 1));

    add(createScientificCard("LIBRARY", 0, {{ResourceType::GLASS,1},{ResourceType::STONE,1}}, 0, ScienceSymbol::TABLET, "THEATER", "UNIVERSITY"));
    add(createScientificCard("DISPENSARY", 0, {{ResourceType::GLASS,1},{ResourceType::CLAY,1}}, 0, ScienceSymbol::COMPASS, "APOTHECARY", "LODGING"));
    add(createScientificCard("SCHOOL", 0, {{ResourceType::PAPER,1},{ResourceType::WOOD,1}}, 1, ScienceSymbol::GEAR, "SCRIPTORIUM", "STUDY"));
    add(createScientificCard("LABORATORY", 0, {{ResourceType::PAPER,1},{ResourceType::CLAY,2}}, 0, ScienceSymbol::MORTAR, "PHARMACIST", "OBSERVATORY"));

    add(createCivilianCard("AQUEDUCT", 0, {{ResourceType::STONE,3}}, 5, "BATHS"));
    add(createCivilianCard("STATUE", 0, {{ResourceType::STONE,2}}, 4, "MASK", "GARDENS"));
    add(createCivilianCard("TEMPLE", 0, {{ResourceType::WOOD,1},{ResourceType::CLAY,1},{ResourceType::GLASS,1}}, 4, "ALTAR", "PANTHEON"));
    add(createCivilianCard("COURTHOUSE", 0, {{ResourceType::PAPER,1},{ResourceType::CLAY,2}}, 4, "", "SENATE"));

    add(createCommercialCard("CUSTOMS HOUSE", 0, {{ResourceType::GLASS,1}}, 0, 0));
    add(createCommercialCard("CARAVANSERY", 0, {{ResourceType::WOOD,2}}, 2, 0));
    add(createCommercialCard("FORUM", 0, {{ResourceType::CLAY,2}}, 0, 0));
    add(createCommercialCard("LIGHTHOUSE", 0, {{ResourceType::STONE,1},{ResourceType::GLASS,1}}, 1, 0, {}, "", "PORT"));
    add(createCommercialCard("BREWERY", 0, {}, 0, 6));

    return deck;
}

// 时代3卡牌组创建（17张基础牌，公会在 Deck 中加入）
std::vector<Card> CardFactory::createAge3Deck() {
    std::vector<Card> deck;
    auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

    add(createMilitaryCard("FORTIFICATIONS", 0, {{ResourceType::STONE,3}}, 3, "WALLS"));
    add(createMilitaryCard("CIRCUS", 0, {{ResourceType::STONE,2},{ResourceType::CLAY,1}}, 2));
    add(createMilitaryCard("ARSENAL", 0, {{ResourceType::WOOD,2},{ResourceType::CLAY,1},{ResourceType::GLASS,1}}, 3));
    add(createMilitaryCard("SIEGE WORKSHOP", 0, {{ResourceType::CLAY,3},{ResourceType::GLASS,1}}, 2));

    add(createScientificCard("UNIVERSITY", 0, {{ResourceType::GLASS,2},{ResourceType::WOOD,1}}, 2, ScienceSymbol::TABLET, "LIBRARY"));
    add(createScientificCard("OBSERVATORY", 0, {{ResourceType::GLASS,2},{ResourceType::CLAY,1}}, 2, ScienceSymbol::COMPASS, "DISPENSARY"));
    add(createScientificCard("STUDY", 0, {{ResourceType::PAPER,1},{ResourceType::WOOD,2}}, 2, ScienceSymbol::GEAR, "SCHOOL"));
    add(createScientificCard("LODGING", 0, {{ResourceType::PAPER,1},{ResourceType::CLAY,2}}, 2, ScienceSymbol::MORTAR, "DISPENSARY"));

    add(createCivilianCard("PANTHEON", 0, {{ResourceType::GLASS,1},{ResourceType::PAPER,1},{ResourceType::CLAY,2}}, 7, "TEMPLE"));
    add(createCivilianCard("GARDENS", 0, {{ResourceType::WOOD,2},{ResourceType::CLAY,1}}, 6, "STATUE"));
    add(createCivilianCard("TOWN HALL", 0, {{ResourceType::STONE,2},{ResourceType::GLASS,1}}, 7));
    add(createCivilianCard("PALACE", 0, {{ResourceType::WOOD,1},{ResourceType::STONE,1},{ResourceType::CLAY,1},{ResourceType::GLASS,1},{ResourceType::PAPER,1}}, 8));
    add(createCivilianCard("SENATE", 0, {{ResourceType::STONE,2},{ResourceType::WOOD,1},{ResourceType::PAPER,1}}, 6, "COURTHOUSE"));

    add(createCommercialCard("PORT", 0, {{ResourceType::WOOD,1},{ResourceType::GLASS,1}}, 3, 0));
    add(createCommercialCard("ARMORY", 0, {{ResourceType::CLAY,2},{ResourceType::WOOD,1}}, 3, 0));
    add(createCommercialCard("CHAMBER OF COMMERCE", 0, {{ResourceType::PAPER,1},{ResourceType::STONE,2}}, 3, 0));
    add(createCommercialCard("ARENA", 0, {{ResourceType::STONE,1},{ResourceType::CLAY,1},{ResourceType::WOOD,1}}, 3, 0));

    return deck;
}

std::vector<Card> CardFactory::createGuildCards() {
    std::vector<Card> deck;
    auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };
    add(createGuildCard("MERCHANTS' GUILD", {{ResourceType::GLASS,1},{ResourceType::PAPER,1}}, 0));
    add(createGuildCard("SHIPOWNERS' GUILD", {{ResourceType::WOOD,2},{ResourceType::GLASS,1}}, 0));
    add(createGuildCard("BUILDERS' GUILD", {{ResourceType::STONE,2},{ResourceType::PAPER,1}}, 0));
    add(createGuildCard("MAGISTRATES' GUILD", {{ResourceType::STONE,2},{ResourceType::GLASS,1}}, 0));
    add(createGuildCard("TACTICIANS' GUILD", {{ResourceType::CLAY,2},{ResourceType::GLASS,1}}, 0));
    add(createGuildCard("SCIENTISTS' GUILD", {{ResourceType::WOOD,2},{ResourceType::PAPER,1}}, 0));
    add(createGuildCard("MONEY LENDERS' GUILD", {{ResourceType::STONE,1},{ResourceType::CLAY,1},{ResourceType::WOOD,1}}, 0));
    return deck;
}

// 所有奇迹创建
std::vector<std::unique_ptr<Wonder>> CardFactory::createAllWonders() {
    std::vector<std::unique_ptr<Wonder>> wonders;

    Effect colossusEffect;
    colossusEffect.militaryShields = 2;
    colossusEffect.victoryPoints = 3;
    wonders.push_back(createWonder("COLOSSUS OF RHODES", 3,
                                   {{ResourceType::STONE, 2}, {ResourceType::WOOD, 1}},
                                   colossusEffect));

    Effect lighthouseEffect;
    lighthouseEffect.victoryPoints = 4;
    lighthouseEffect.coins = 6;
    wonders.push_back(createWonder("LIGHTHOUSE OF ALEXANDRIA", 2,
                                   {{ResourceType::GLASS, 1}, {ResourceType::STONE, 1}},
                                   lighthouseEffect));

    Effect statueZeus;
    statueZeus.militaryShields = 1;
    statueZeus.victoryPoints = 3;
    wonders.push_back(createWonder("STATUE OF ZEUS", 2,
                                   {{ResourceType::CLAY, 1}, {ResourceType::WOOD, 1}, {ResourceType::STONE, 1}},
                                   statueZeus));

    Effect templeArtemis;
    templeArtemis.coins = 12;
    templeArtemis.playAgain = true;
    wonders.push_back(createWonder("TEMPLE OF ARTEMIS", 2,
                                   {{ResourceType::PAPER, 1}, {ResourceType::GLASS, 1}},
                                   templeArtemis));

    Effect greatLibrary;
    greatLibrary.victoryPoints = 4;
    wonders.push_back(createWonder("GREAT LIBRARY", 2,
                                   {{ResourceType::PAPER, 1}, {ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}},
                                   greatLibrary));

    Effect greatLighthouse;
    greatLighthouse.victoryPoints = 4;
    wonders.push_back(createWonder("GREAT LIGHTHOUSE", 2,
                                   {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}},
                                   greatLighthouse));

    Effect mausoleum;
    mausoleum.victoryPoints = 2;
    wonders.push_back(createWonder("MAUSOLEUM", 1,
                                   {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}, {ResourceType::STONE, 1}},
                                   mausoleum));

    Effect sphinx;
    sphinx.victoryPoints = 6;
    sphinx.playAgain = true;
    wonders.push_back(createWonder("SPHINX", 2,
                                   {{ResourceType::CLAY, 2}, {ResourceType::PAPER, 1}},
                                   sphinx));

    Effect pyramids;
    pyramids.victoryPoints = 9;
    wonders.push_back(createWonder("PYRAMIDS", 3,
                                   {{ResourceType::STONE, 3}, {ResourceType::WOOD, 1}},
                                   pyramids));

    Effect hangingGardens;
    hangingGardens.victoryPoints = 3;
    hangingGardens.coins = 6;
    hangingGardens.playAgain = true;
    wonders.push_back(createWonder("HANGING GARDENS", 2,
                                   {{ResourceType::WOOD, 2}, {ResourceType::CLAY, 1}},
                                   hangingGardens));

    Effect greatWall;
    greatWall.militaryShields = 2;
    wonders.push_back(createWonder("GREAT WALL", 3,
                                   {{ResourceType::STONE, 3}, {ResourceType::WOOD, 1}},
                                   greatWall));

    Effect appianWay;
    appianWay.victoryPoints = 3;
    appianWay.coins = 3;
    appianWay.playAgain = true;
    wonders.push_back(createWonder("APPIAN WAY", 2,
                                   {{ResourceType::STONE, 1}, {ResourceType::CLAY, 1}, {ResourceType::PAPER, 1}},
                                   appianWay));

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
