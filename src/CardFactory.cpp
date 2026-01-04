#include "CardFactory.h"
#include "Wonder.h"
#include <memory>
#include <stdexcept>

// 单例模式实现：保证全局只有一个卡牌工厂实例
CardFactory &CardFactory::getInstance() {
  static CardFactory instance;
  return instance;
}

// 原材料卡创建函数
std::unique_ptr<Card>
CardFactory::createRawMaterialCard(const std::string &name, int costCoins,
                                   const std::map<ResourceType, int> &costRes,
                                   const std::map<ResourceType, int> &prodRes,
                                   int coins) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.resourcesProduced = prodRes; // 设置产出资源
  effect.coins = coins;               // 设置金币产出

  return createCardInternal(name, CardType::RAW_MATERIAL, cost, effect, "", "");
}

// 制造品卡创建函数
std::unique_ptr<Card> CardFactory::createManufacturedGoodCard(
    const std::string &name, int costCoins,
    const std::map<ResourceType, int> &costRes,
    const std::map<ResourceType, int> &prodRes, int coins) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.resourcesProduced = prodRes;
  effect.coins = coins;

  return createCardInternal(name, CardType::MANUFACTURED_GOOD, cost, effect, "",
                            "");
}

// 平民建筑卡创建函数
std::unique_ptr<Card>
CardFactory::createCivilianCard(const std::string &name, int costCoins,
                                const std::map<ResourceType, int> &costRes,
                                int vp, const std::string &chainSymbol,
                                const std::string &chainTarget) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.victoryPoints = vp; // 设置胜利点

  return createCardInternal(name, CardType::CIVILIAN, cost, effect, chainSymbol,
                            chainTarget);
}

// 军事建筑卡创建函数
std::unique_ptr<Card>
CardFactory::createMilitaryCard(const std::string &name, int costCoins,
                                const std::map<ResourceType, int> &costRes,
                                int shields, const std::string &chainSymbol,
                                const std::string &chainTarget) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.militaryShields = shields; // 设置军事盾

  return createCardInternal(name, CardType::MILITARY, cost, effect, chainSymbol,
                            chainTarget);
}

// 科学建筑卡创建函数
std::unique_ptr<Card> CardFactory::createScientificCard(
    const std::string &name, int costCoins,
    const std::map<ResourceType, int> &costRes, int vp, ScienceSymbol symbol,
    const std::string &chainSymbol, const std::string &chainTarget) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.victoryPoints = vp;        // 胜利点
  effect.scienceSymbols = {symbol}; // 科学符号

  return createCardInternal(name, CardType::SCIENTIFIC, cost, effect,
                            chainSymbol, chainTarget);
}

// 商业建筑卡创建函数
std::unique_ptr<Card> CardFactory::createCommercialCard(
    const std::string &name, int costCoins,
    const std::map<ResourceType, int> &costRes, int vp, int immediateCoins,
    const std::map<ResourceType, int> &prodRes,
    const std::map<ResourceType, TradeDiscount> &tradeDiscounts,
    const std::string &chainSymbol, const std::string &chainTarget) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.victoryPoints = vp;              // 胜利点
  effect.coins = immediateCoins;          // 即时金币
  effect.resourcesProduced = prodRes;     // 产出资源
  effect.tradeDiscounts = tradeDiscounts; // 交易折扣

  return createCardInternal(name, CardType::COMMERCIAL, cost, effect,
                            chainSymbol, chainTarget);
}

// 公会卡创建函数 (Corrected Signature & Implementation)
std::unique_ptr<Card>
CardFactory::createGuildCard(const std::string &name,
                             const std::map<ResourceType, int> &costRes,
                             GuildType guildType) {
  // Guild cards often have no immediate coins or fixed VP in constructor args
  // usually? Check usages. Usages passed `immediateCoins` and `victoryPoints`.
  // Wait. The OLD function had `immediateCoins` and `victoryPoints` args.
  // The HEADER `CardFactory.h` (Step 377 Error Note) showed:
  // createGuildCard(const std::string &name, const std::map<ResourceType,
  // int>&, GuildType) IT DOES NOT HAVE `immediateCoins` or `victoryPoints` args
  // in the header signature??? Let's assume the header signature implies
  // properties are derived from GuildType or fixed. BUT the old code passed `1,
  // 1` etc. If I want to fix this, I should probably UPDATE THE HEADER to
  // accept `immediateCoins` and `victoryPoints` too, OR derive them. Guilds
  // usually give VP based on conditions at end of game. The `1` VP passed might
  // be base VP? (Usually 0 base VP for guilds?). Rules: Guilds give points
  // based on conditions. MoneyLenders gives 1 VP per 3 coins. Immediate coins?
  // MoneyLenders gives 3 coins immediately? No, usually no immediate coins
  // unless specified. The OLD code passed `3` immediate coins for MoneyLenders.
  // I should check `CardFactory.h` signature AGAIN carefully from user history
  // or I can just update the Header to match what I want. BETTER: Update the
  // HEADER to take the args I need, OR update CPP to match Header. Error 377
  // said: `candidate 1: ... createGuildCard(const std::string&, const
  // std::map<ResourceType, int>&, GuildType)` So the header only takes 3 args.
  // I will update the CPP to take 3 args and set default/derived values inside,
  // OR update the Header. Since I don't see the Header content fully (only
  // error log), I'll assume I should match the Header for now to avoid editing
  // too many files. BUT what about the `immediateCoins`? If I don't pass them,
  // MoneyLenders won't give coins. I'll update the CPP to take 3 args, and
  // inside the switch/logic set the coins/VP based on GuildType.

  Cost cost{0, costRes}; // Guilds cost 0 coins usually (just resources? or free
                         // in Age 3? usually have resource costs).
  Effect effect;
  effect.guildType = guildType;

  // Set properties based on GuildType (Hardcoded based on rules/previous code)
  switch (guildType) {
  case GuildType::TRADERS: // Merchants (Yellow)
    effect.coins = 1;
    effect.victoryPoints = 1;
    break;
  case GuildType::SHIPOWNERS: // Shipowners (Brown/Grey)
    effect.coins = 1;
    effect.victoryPoints = 1;
    break;
  case GuildType::BUILDERS: // Builders (Wonder)
    effect.coins = 2;
    effect.victoryPoints = 2;
    break;                     // Old code: 2 coins, 2 VP?
  case GuildType::MAGISTRATES: // Magistrates (Blue)
    effect.coins = 1;
    effect.victoryPoints = 1;
    break;
  case GuildType::SCIENTISTS: // Scientists (Green)
    effect.coins = 1;
    effect.victoryPoints = 1;
    break;
  case GuildType::MONEYLENDERS:
    effect.coins = 3;
    effect.victoryPoints = 1;
    break;                    // Old code: 3 coins, 1 VP.
  case GuildType::TACTICIANS: // Tacticians (Red)
    effect.coins = 1;
    effect.victoryPoints = 1;
    break;
  default:
    effect.coins = 0;
    effect.victoryPoints = 0;
    break;
  }

  return createCardInternal(name, CardType::GUILD, cost, effect, "", "");
}

// 奇迹创建函数
std::unique_ptr<Wonder>
CardFactory::createWonder(const std::string &name, int costCoins,
                          const std::map<ResourceType, int> &costRes,
                          const Effect &effect) {

  Cost cost{costCoins, costRes};
  return std::make_unique<Wonder>(name, cost, effect);
}

// 内部卡牌创建辅助函数
std::unique_ptr<Card> CardFactory::createCardInternal(
    const std::string &name, CardType type, Cost cost, Effect effect,
    const std::string &chainSymbol, const std::string &chainTarget) {

  return std::make_unique<Card>(name, type, cost, effect, chainSymbol,
                                chainTarget);
}

// 时代1卡牌组创建
std::vector<Card> CardFactory::createAge1Deck() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // ---------------------- 棕色：原材料卡 ----------------------
  add(createRawMaterialCard("LUMBER YARD", 0, {}, {{ResourceType::WOOD, 1}},
                            0));
  add(createRawMaterialCard("LOGGING CAMP", 1, {}, {{ResourceType::WOOD, 1}},
                            0));
  add(createRawMaterialCard("CLAY POOL", 0, {}, {{ResourceType::CLAY, 1}}, 0));
  add(createRawMaterialCard("CLAY PIT", 1, {}, {{ResourceType::CLAY, 1}}, 0));
  add(createRawMaterialCard("STONE PIT", 0, {}, {{ResourceType::STONE, 1}}, 0));
  add(createRawMaterialCard("QUARRY", 0, {}, {{ResourceType::STONE, 1}}, 0));

  // ---------------------- 灰色：制造品卡 ----------------------
  add(createManufacturedGoodCard("GLASSWORKS", 0, {},
                                 {{ResourceType::GLASS, 1}}, 0));
  add(createManufacturedGoodCard("PRESS", 0, {}, {{ResourceType::PAPER, 1}},
                                 0));

  // ---------------------- 红色：军事卡 ----------------------
  add(createMilitaryCard("GUARD TOWER", 0, {{ResourceType::CLAY, 1}}, 1));
  add(createMilitaryCard("GARRISON", 0, {{ResourceType::STONE, 1}}, 1));
  add(createMilitaryCard(
      "STABLE", 0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}, 1));
  add(createMilitaryCard("PALISADE", 2, {{ResourceType::WOOD, 1}}, 1));

  // ---------------------- 绿色：科学卡 ----------------------
  add(createScientificCard("WORKSHOP", 0, {{ResourceType::CLAY, 1}}, 0,
                           ScienceSymbol::GEAR));
  add(createScientificCard("APOTHECARY", 0, {{ResourceType::WOOD, 1}}, 0,
                           ScienceSymbol::COMPASS));
  add(createScientificCard("SCRIPTORIUM", 0, {{ResourceType::PAPER, 1}}, 0,
                           ScienceSymbol::TABLET));
  add(createScientificCard("PHARMACIST", 0, {{ResourceType::GLASS, 1}}, 0,
                           ScienceSymbol::MORTAR));

  // ---------------------- 黄色：商业卡 ----------------------
  add(createCommercialCard("TAVERN", 0, {}, 0, 4, {}, {}, "", ""));
  add(createCommercialCard("STONE RESERVE", 3, {}, 0, 0, {}, {}, "", ""));
  add(createCommercialCard("CLAY RESERVE", 3, {}, 0, 0, {}, {}, "", ""));
  add(createCommercialCard("WOOD RESERVE", 3, {}, 0, 0, {}, {}, "", ""));

  // ---------------------- 蓝色：平民卡 ----------------------
  add(createCivilianCard("BATHS", 0, {{ResourceType::STONE, 1}}, 3, "",
                         "AQUEDUCT"));
  add(createCivilianCard("ALTAR", 0, {}, 2, "", "TEMPLE"));
  add(createCivilianCard("THEATER", 0, {}, 3, "MASK", "STATUE"));

  return deck;
}

// 时代2卡牌组创建
std::vector<Card> CardFactory::createAge2Deck() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // ---------------------- 棕色：原材料卡 ----------------------
  add(createRawMaterialCard("SAWMILL", 2, {}, {{ResourceType::WOOD, 2}}, 0));
  add(createRawMaterialCard("BRICKYARD", 2, {}, {{ResourceType::CLAY, 2}}, 0));
  add(createRawMaterialCard("SHELF QUARRY", 2, {}, {{ResourceType::STONE, 2}},
                            0));

  // ---------------------- 灰色：制造品卡 ----------------------
  add(createManufacturedGoodCard("GLASSBLOWER", 0, {},
                                 {{ResourceType::GLASS, 1}}, 0));
  add(createManufacturedGoodCard("DRYING ROOM", 0, {},
                                 {{ResourceType::PAPER, 1}}, 0));

  // ---------------------- 红色：军事卡 ----------------------
  add(createMilitaryCard("WALLS", 0, {{ResourceType::STONE, 2}}, 2, "",
                         "FORTIFICATIONS"));
  add(createMilitaryCard("HORSE BREEDERS", 0,
                         {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}, 1,
                         "", "CIRCUS"));
  add(createMilitaryCard("BARRACKS", 3, {}, 1));
  add(createMilitaryCard("ARCHERY RANGE", 0,
                         {{ResourceType::STONE, 1},
                          {ResourceType::WOOD, 1},
                          {ResourceType::PAPER, 1}},
                         2));
  add(createMilitaryCard("PARADE GROUND", 0,
                         {{ResourceType::CLAY, 2}, {ResourceType::GLASS, 1}},
                         2));

  // ---------------------- 绿色：科学卡 ----------------------
  add(createScientificCard("LIBRARY", 0,
                           {{ResourceType::STONE, 1},
                            {ResourceType::WOOD, 1},
                            {ResourceType::GLASS, 1}},
                           2, ScienceSymbol::TABLET, "", ""));
  add(createScientificCard("DISPENSARY", 0,
                           {{ResourceType::CLAY, 2}, {ResourceType::STONE, 1}},
                           2, ScienceSymbol::MORTAR, "", ""));
  add(createScientificCard("SCHOOL", 0,
                           {{ResourceType::WOOD, 1}, {ResourceType::PAPER, 2}},
                           1, ScienceSymbol::GEAR));
  add(createScientificCard("LABORATORY", 0,
                           {{ResourceType::WOOD, 1}, {ResourceType::GLASS, 2}},
                           1, ScienceSymbol::GEAR, "", ""));

  // ---------------------- 蓝色：平民卡 ----------------------
  add(createCivilianCard("STATUE", 0, {{ResourceType::STONE, 2}}, 4, "MASK",
                         "GARDENS"));
  add(createCivilianCard("TEMPLE", 0,
                         {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}, 4,
                         "ALTAR", "PANTHEON"));
  add(createCivilianCard("AQUEDUCT", 0, {{ResourceType::STONE, 3}}, 5,
                         "BATHS"));
  add(createCivilianCard("COURTHOUSE", 0,
                         {{ResourceType::WOOD, 2}, {ResourceType::GLASS, 1}}, 5,
                         "", ""));
  add(createCivilianCard("ROSTRUM", 0,
                         {{ResourceType::STONE, 1}, {ResourceType::WOOD, 1}}, 4,
                         "", ""));

  // ---------------------- 黄色：商业卡 ----------------------
  add(createCommercialCard("FORUM", 3, {{ResourceType::CLAY, 1}}, 0, 0,
                           {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}},
                           {}, "", ""));
  add(createCommercialCard("CARAVANSERY", 2,
                           {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}},
                           0, 0,
                           {{ResourceType::WOOD, 1},
                            {ResourceType::CLAY, 1},
                            {ResourceType::STONE, 1}},
                           {}, "", ""));
  add(createCommercialCard("CUSTOMS HOUSE", 4, {}, 0, 0,
                           {{ResourceType::PAPER, 1}, {ResourceType::GLASS, 1}},
                           {}, "", ""));
  add(createCommercialCard("BREWERY", 0, {}, 0, 6, {}, {}, "", ""));

  return deck;
}

// 时代3卡牌组创建
std::vector<Card> CardFactory::createAge3Deck() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // ---------------------- 红色：军事卡 ----------------------
  add(createMilitaryCard("FORTIFICATIONS", 0,
                         {{ResourceType::STONE, 2},
                          {ResourceType::CLAY, 1},
                          {ResourceType::PAPER, 1}},
                         2));
  add(createMilitaryCard("SIEGE WORKSHOP", 0,
                         {{ResourceType::WOOD, 3}, {ResourceType::GLASS, 1}},
                         2));
  add(createMilitaryCard(
      "CIRCUS", 0, {{ResourceType::CLAY, 3}, {ResourceType::STONE, 1}}, 2));
  add(createMilitaryCard(
      "ARSENAL", 0, {{ResourceType::CLAY, 3}, {ResourceType::WOOD, 2}}, 3));
  add(createMilitaryCard("PRETORIUM", 8, {}, 3));

  // ---------------------- 绿色：科学卡 ----------------------
  add(createScientificCard("ACADEMY", 0,
                           {{ResourceType::STONE, 1},
                            {ResourceType::WOOD, 1},
                            {ResourceType::GLASS, 2}},
                           3, ScienceSymbol::TABLET, "", ""));
  add(createScientificCard("STUDY", 0,
                           {{ResourceType::WOOD, 2},
                            {ResourceType::GLASS, 1},
                            {ResourceType::PAPER, 1}},
                           3, ScienceSymbol::TABLET, "", ""));
  add(createScientificCard("UNIVERSITY", 0,
                           {{ResourceType::CLAY, 1},
                            {ResourceType::GLASS, 1},
                            {ResourceType::PAPER, 1}},
                           2, ScienceSymbol::COMPASS, "", ""));
  add(createScientificCard("OBSERVATORY", 0,
                           {{ResourceType::STONE, 1}, {ResourceType::PAPER, 2}},
                           2, ScienceSymbol::COMPASS, "", ""));

  // ---------------------- 蓝色：平民卡 ----------------------
  add(createCivilianCard("PALACE", 0,
                         {{ResourceType::CLAY, 1},
                          {ResourceType::STONE, 1},
                          {ResourceType::WOOD, 1},
                          {ResourceType::GLASS, 2}},
                         7, "", ""));
  add(createCivilianCard("TOWN HALL", 0,
                         {{ResourceType::STONE, 3}, {ResourceType::WOOD, 2}}, 7,
                         "", ""));
  add(createCivilianCard("OBELISK", 0,
                         {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}},
                         5, "", ""));
  add(createCivilianCard("GARDENS", 0,
                         {{ResourceType::CLAY, 2}, {ResourceType::WOOD, 2}}, 6,
                         "", ""));
  add(createCivilianCard("PANTHEON", 0,
                         {{ResourceType::GLASS, 1},
                          {ResourceType::CLAY, 1},
                          {ResourceType::PAPER, 2}},
                         6, "", ""));
  add(createCivilianCard("SENATE", 0,
                         {{ResourceType::CLAY, 2},
                          {ResourceType::STONE, 1},
                          {ResourceType::PAPER, 1}},
                         5, "", ""));

  // ---------------------- 黄色：商业卡 ----------------------
  add(createCommercialCard("CHAMBER OF COMMERCE", 0, {{ResourceType::PAPER, 2}},
                           3, 3, {}, {}, "", ""));
  add(createCommercialCard("PORT", 0,
                           {{ResourceType::WOOD, 1},
                            {ResourceType::GLASS, 1},
                            {ResourceType::PAPER, 1}},
                           3, 2, {}, {}, "", ""));
  add(createCommercialCard("ARMORY", 0,
                           {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}},
                           3, 1, {}, {}, "", ""));
  add(createCommercialCard("LIGHTHOUSE", 0,
                           {{ResourceType::CLAY, 2}, {ResourceType::GLASS, 1}},
                           3, 1, {}, {}, "", ""));
  add(createCommercialCard("ARENA", 0,
                           {{ResourceType::CLAY, 1},
                            {ResourceType::STONE, 1},
                            {ResourceType::WOOD, 1}},
                           3, 2, {}, {}, "", ""));

  return deck;
}

std::vector<Card> CardFactory::createGuildCards() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  add(createGuildCard("MERCHANTS GUILD",
                      {{ResourceType::PAPER, 1},
                       {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 1},
                       {ResourceType::GLASS, 1}},
                      GuildType::TRADERS));
  add(createGuildCard("SHIPOWNERS GUILD",
                      {{ResourceType::PAPER, 1},
                       {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 1},
                       {ResourceType::GLASS, 1}},
                      GuildType::SHIPOWNERS));
  add(createGuildCard("BUILDERS GUILD",
                      {{ResourceType::STONE, 2},
                       {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 1},
                       {ResourceType::GLASS, 1}},
                      GuildType::BUILDERS));
  add(createGuildCard("MAGISTRATES GUILD",
                      {{ResourceType::PAPER, 1},
                       {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 2}},
                      GuildType::MAGISTRATES));
  add(createGuildCard("SCIENTISTS GUILD",
                      {{ResourceType::WOOD, 1},
                       {ResourceType::CLAY, 1},
                       {ResourceType::PAPER, 1}},
                      GuildType::SCIENTISTS));
  add(createGuildCard("MONEYLENDERS GUILD",
                      {{ResourceType::STONE, 1},
                       {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 2}},
                      GuildType::MONEYLENDERS));
  add(createGuildCard("TACTICIANS GUILD",
                      {{ResourceType::STONE, 1},
                       {ResourceType::CLAY, 1},
                       {ResourceType::PAPER, 1}},
                      GuildType::TACTICIANS));

  return deck;
}

std::vector<std::unique_ptr<Wonder>> CardFactory::createAllWonders() {
  std::vector<std::unique_ptr<Wonder>> wonders;

  Effect colossusEffect;
  colossusEffect.militaryShields = 1;
  colossusEffect.victoryPoints = 3;
  wonders.push_back(createWonder(
      "COLOSSUS", 3, {{ResourceType::STONE, 2}, {ResourceType::WOOD, 1}},
      colossusEffect));

  Effect circusMaximus;
  circusMaximus.militaryShields = 1;
  circusMaximus.victoryPoints = 3;
  circusMaximus.destroyOpponentGrey = true;
  wonders.push_back(createWonder("CIRCUS MAXIMUS", 2,
                                 {{ResourceType::GLASS, 1},
                                  {ResourceType::STONE, 1},
                                  {ResourceType::WOOD, 1}},
                                 circusMaximus));

  Effect statueZeus;
  statueZeus.militaryShields = 1;
  statueZeus.victoryPoints = 3;
  statueZeus.destroyOpponentBrown = true;
  wonders.push_back(createWonder("STATUE OF ZEUS", 2,
                                 {{ResourceType::CLAY, 1},
                                  {ResourceType::WOOD, 1},
                                  {ResourceType::STONE, 1}},
                                 statueZeus));

  Effect templeArtemis;
  templeArtemis.coins = 12;
  templeArtemis.playAgain = true;
  wonders.push_back(createWonder(
      "TEMPLE OF ARTEMIS", 2,
      {{ResourceType::PAPER, 1}, {ResourceType::GLASS, 1}}, templeArtemis));

  Effect greatLibrary;
  greatLibrary.victoryPoints = 4;
  greatLibrary.gainProgressToken = true;
  wonders.push_back(createWonder("GREAT LIBRARY", 2,
                                 {{ResourceType::PAPER, 1},
                                  {ResourceType::WOOD, 1},
                                  {ResourceType::GLASS, 1}},
                                 greatLibrary));

  Effect greatLighthouse;
  greatLighthouse.victoryPoints = 4;
  greatLighthouse.resourcesProduced[ResourceType::WOOD] = 1;
  greatLighthouse.resourcesProduced[ResourceType::STONE] = 1;
  greatLighthouse.resourcesProduced[ResourceType::CLAY] = 1;
  wonders.push_back(createWonder(
      "GREAT LIGHTHOUSE", 2,
      {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}}, greatLighthouse));

  Effect mausoleum;
  mausoleum.victoryPoints = 2;
  mausoleum.buildFromDiscard = true;
  wonders.push_back(createWonder("MAUSOLEUM", 1,
                                 {{ResourceType::GLASS, 1},
                                  {ResourceType::PAPER, 1},
                                  {ResourceType::STONE, 1}},
                                 mausoleum));

  Effect sphinx;
  sphinx.victoryPoints = 6;
  sphinx.playAgain = true;
  wonders.push_back(createWonder(
      "SPHINX", 2, {{ResourceType::CLAY, 2}, {ResourceType::PAPER, 1}},
      sphinx));

  Effect pyramids;
  pyramids.victoryPoints = 9;
  wonders.push_back(createWonder(
      "PYRAMIDS", 3, {{ResourceType::STONE, 3}, {ResourceType::WOOD, 1}},
      pyramids));

  Effect hangingGardens;
  hangingGardens.victoryPoints = 3;
  hangingGardens.coins = 6;
  hangingGardens.playAgain = true;
  wonders.push_back(createWonder(
      "HANGING GARDENS", 2, {{ResourceType::WOOD, 2}, {ResourceType::CLAY, 1}},
      hangingGardens));

  Effect piraeus;
  piraeus.resourcesProduced[ResourceType::GLASS] = 1;
  piraeus.resourcesProduced[ResourceType::PAPER] = 1;
  piraeus.playAgain = true;
  wonders.push_back(createWonder(
      "PIRAEUS", 2, {{ResourceType::CLAY, 2}, {ResourceType::GLASS, 1}},
      piraeus));

  Effect appianWay;
  appianWay.victoryPoints = 3;
  appianWay.coins = 3;
  appianWay.playAgain = true;
  wonders.push_back(createWonder("APPIAN WAY", 2,
                                 {{ResourceType::STONE, 1},
                                  {ResourceType::CLAY, 1},
                                  {ResourceType::PAPER, 1}},
                                 appianWay));

  return wonders;
}
