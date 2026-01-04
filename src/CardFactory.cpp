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
// 参数说明：
// name: 卡牌名称 | costCoins: 金币消耗 | costRes: 资源消耗 | prodRes: 产出资源 | coins: 金币产出
std::unique_ptr<Card>
CardFactory::createRawMaterialCard(const std::string &name, int costCoins,
                                   const std::map<ResourceType, int> &costRes,
                                   const std::map<ResourceType, int> &prodRes,
                                   int coins) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.resourcesProduced = prodRes; // 设置产出资源
  effect.coins = coins;              // 设置金币产出

  return createCardInternal(name, CardType::RAW_MATERIAL, cost, effect, "", "");
}

// 制造品卡创建函数（参数含义同原材料卡）
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
// 新增参数：vp=胜利点 | chainSymbol=连锁符号 | chainTarget=连锁目标卡牌
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
// 新增参数：shields=军事盾数量
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
// 新增参数：symbol=科学符号（GEAR/COMPASS/TABLET等）
std::unique_ptr<Card> CardFactory::createScientificCard(
    const std::string &name, int costCoins,
    const std::map<ResourceType, int> &costRes, int vp, ScienceSymbol symbol,
    const std::string &chainSymbol, const std::string &chainTarget) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.victoryPoints = vp;               // 胜利点
  effect.scienceSymbols = {symbol};        // 科学符号

  return createCardInternal(name, CardType::SCIENTIFIC, cost, effect,
                            chainSymbol, chainTarget);
}

// 商业建筑卡创建函数
// 新增参数：immediateCoins=即时金币 | prodRes=产出资源
std::unique_ptr<Card> CardFactory::createCommercialCard(
    const std::string &name, int costCoins,
    const std::map<ResourceType, int> &costRes, int vp, int immediateCoins,
    const std::map<ResourceType, int> &prodRes, const std::string &chainSymbol,
    const std::string &chainTarget) {

  Cost cost{costCoins, costRes};
  Effect effect;
  effect.victoryPoints = vp;             // 胜利点
  effect.coins = immediateCoins;         // 即时金币
  effect.resourcesProduced = prodRes;    // 产出资源

  return createCardInternal(name, CardType::COMMERCIAL, cost, effect,
                            chainSymbol, chainTarget);
}

// 公会卡创建函数（最终生效版本）
// 参数说明：costRes=资源消耗 | immediateCoins=即时金币 | victoryPoints=胜利点 | effectType=效果类型（对应不同卡牌加成）
std::unique_ptr<Card>
CardFactory::createGuildCard(const std::string &name,
                             const std::map<ResourceType, int> &costRes,
                             int immediateCoins,
                             int victoryPoints,
                             const std::string &effectType) {

  Cost cost{0, costRes};  // 公会卡固定金币消耗为0
  Effect effect;
  effect.coins = immediateCoins;       // 即时金币
  effect.victoryPoints = victoryPoints;// 胜利点
  effect.effectType = effectType;      // 效果类型（如黄色商业卡加成、红色军事卡加成等）

  return createCardInternal(name, CardType::GUILD, cost, effect, "", "");
}

// 奇迹创建函数
// 参数：name=奇迹名 | costCoins=金币消耗 | costRes=资源消耗 | effect=奇迹效果（复合属性）
std::unique_ptr<Wonder>
CardFactory::createWonder(const std::string &name, int costCoins,
                          const std::map<ResourceType, int> &costRes,
                          const Effect &effect) {

  Cost cost{costCoins, costRes};
  return std::make_unique<Wonder>(name, cost, effect);
}

// 内部卡牌创建辅助函数：封装所有卡牌的通用创建逻辑
std::unique_ptr<Card> CardFactory::createCardInternal(
    const std::string &name, CardType type, Cost cost, Effect effect,
    const std::string &chainSymbol, const std::string &chainTarget) {

  return std::make_unique<Card>(name, type, cost, effect, chainSymbol,
                                chainTarget);
}

// 时代1卡牌组创建（23张基础牌）
std::vector<Card> CardFactory::createAge1Deck() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // ---------------------- 棕色：原材料卡（6张） ----------------------
  // 伐木场：0金币/0资源消耗 | 产出1木材 | 0金币
  add(createRawMaterialCard("LUMBER YARD", 0, {}, {{ResourceType::WOOD, 1}}, 0));
  // 伐木营地：1金币/0资源消耗 | 产出1木材 | 0金币
  add(createRawMaterialCard("LOGGING CAMP", 1, {}, {{ResourceType::WOOD, 1}}, 0));
  // 粘土池：0金币/0资源消耗 | 产出1粘土 | 0金币
  add(createRawMaterialCard("CLAY POOL", 0, {}, {{ResourceType::CLAY, 1}}, 0));
  // 粘土坑：1金币/0资源消耗 | 产出1粘土 | 0金币
  add(createRawMaterialCard("CLAY PIT", 1, {}, {{ResourceType::CLAY, 1}}, 0));
  // 采石场：0金币/0资源消耗 | 产出1石头 | 0金币
  add(createRawMaterialCard("STONE PIT", 0, {}, {{ResourceType::STONE, 1}}, 0));
  // 采石坊：0金币/0资源消耗 | 产出1石头 | 0金币
  add(createRawMaterialCard("QUARRY", 0, {}, {{ResourceType::STONE, 1}}, 0));

  // ---------------------- 灰色：制造品卡（2张） ----------------------
  // 玻璃厂：0金币/0资源消耗 | 产出1玻璃 | 0金币
  add(createManufacturedGoodCard("GLASSWORKS", 0, {}, {{ResourceType::GLASS, 1}}, 0));
  // 造纸厂：0金币/0资源消耗 | 产出1纸 | 0金币
  add(createManufacturedGoodCard("PRESS", 0, {}, {{ResourceType::PAPER, 1}}, 0));

  // ---------------------- 红色：军事卡（4张） ----------------------
  // 岗楼：0金币 | 消耗1粘土 | 产出1军事盾
  add(createMilitaryCard("GUARD TOWER", 0, {{ResourceType::CLAY, 1}}, 1));
  // 卫戍部队：0金币 | 消耗1石头 | 产出1军事盾
  add(createMilitaryCard("GARRISON", 0, {{ResourceType::STONE, 1}}, 1));
  // 马厩：0金币 | 消耗1木材+1粘土 | 产出1军事盾
  add(createMilitaryCard("STABLE", 0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}, 1));
  // 木栅栏（修改版）：2金币 | 消耗1木材 | 产出1军事盾（原版消耗/金币有调整）
  add(createMilitaryCard("PALISADE", 2, {{ResourceType::WOOD, 1}}, 1));

  // ---------------------- 绿色：科学卡（4张） ----------------------
  // 工坊：0金币 | 消耗1粘土 | 0胜利点 | 科学符号：齿轮
  add(createScientificCard("WORKSHOP", 0, {{ResourceType::CLAY, 1}}, 0, ScienceSymbol::GEAR));
  // 药剂师：0金币 | 消耗1木材 | 0胜利点 | 科学符号：指南针
  add(createScientificCard("APOTHECARY", 0, {{ResourceType::WOOD, 1}}, 0, ScienceSymbol::COMPASS));
  // 文书院：0金币 | 消耗1纸 | 0胜利点 | 科学符号：石板
  add(createScientificCard("SCRIPTORIUM", 0, {{ResourceType::PAPER, 1}}, 0, ScienceSymbol::TABLET));
  // 药剂房：0金币 | 消耗1玻璃 | 0胜利点 | 科学符号：研钵
  add(createScientificCard("PHARMACIST", 0, {{ResourceType::GLASS, 1}}, 0, ScienceSymbol::MORTAR));

  // ---------------------- 黄色：商业卡（4张） ----------------------
  // 酒馆：0金币/0资源消耗 | 0胜利点 | 即时产出4金币
  add(createCommercialCard("TAVERN", 0, {}, 0, 4, {}, "", ""));
  // 石头储备（修改版）：3金币/0资源消耗 | 0胜利点 | 0即时金币 | 无产出
  add(createCommercialCard("STONE RESERVE", 3, {}, 0, 0, {}, "", ""));
  // 粘土储备（修改版）：3金币/0资源消耗 | 0胜利点 | 0即时金币 | 无产出
  add(createCommercialCard("CLAY RESERVE", 3, {}, 0, 0, {}, "", ""));
  // 木材储备（修改版）：3金币/0资源消耗 | 0胜利点 | 0即时金币 | 无产出
  add(createCommercialCard("WOOD RESERVE", 3, {}, 0, 0, {}, "", ""));

  // ---------------------- 蓝色：平民卡（3张） ----------------------
  // 浴场：0金币 | 消耗1石头 | 3胜利点 | 连锁目标：渡槽
  add(createCivilianCard("BATHS", 0, {{ResourceType::STONE, 1}}, 3, "", "AQUEDUCT"));
  // 祭坛：0金币/0资源消耗 | 2胜利点 | 连锁目标：神庙
  add(createCivilianCard("ALTAR", 0, {}, 2, "", "TEMPLE"));
  // 剧院：0金币/0资源消耗 | 3胜利点 | 连锁符号：面具 | 连锁目标：雕像
  add(createCivilianCard("THEATER", 0, {}, 3, "MASK", "STATUE"));
  // 注：典当行卡牌被移除，原代码注释标记

  return deck;
}

// 时代2卡牌组创建（23张基础牌）
std::vector<Card> CardFactory::createAge2Deck() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // ---------------------- 棕色：原材料卡（3张） ----------------------
  // 锯木厂：2金币/0资源消耗 | 产出2木材 | 0金币
  add(createRawMaterialCard("SAWMILL", 2, {}, {{ResourceType::WOOD, 2}}, 0));
  // 砖厂：2金币/0资源消耗 | 产出2粘土 | 0金币
  add(createRawMaterialCard("BRICKYARD", 2, {}, {{ResourceType::CLAY, 2}}, 0));
  // 分层采石场：2金币/0资源消耗 | 产出2石头 | 0金币
  add(createRawMaterialCard("SHELF QUARRY", 2, {}, {{ResourceType::STONE, 2}}, 0));

  // ---------------------- 灰色：制造品卡（2张） ----------------------
  // 吹玻璃工坊：0金币/0资源消耗 | 产出1玻璃 | 0金币
  add(createManufacturedGoodCard("GLASSBLOWER", 0, {}, {{ResourceType::GLASS, 1}}, 0));
  // 干燥室：0金币/0资源消耗 | 产出1纸 | 0金币
  add(createManufacturedGoodCard("DRYING ROOM", 0, {}, {{ResourceType::PAPER, 1}}, 0));

  // ---------------------- 红色：军事卡（5张） ----------------------
  // 城墙：0金币 | 消耗2石头 | 2军事盾 | 连锁目标：防御工事
  add(createMilitaryCard("WALLS", 0, {{ResourceType::STONE, 2}}, 2, "", "FORTIFICATIONS"));
  // 育马场：0金币 | 消耗1木材+1粘土 | 1军事盾 | 连锁目标：竞技场
  add(createMilitaryCard("HORSE BREEDERS", 0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}, 1, "", "CIRCUS"));
  // 兵营：3金币/0资源消耗 | 1军事盾
  add(createMilitaryCard("BARRACKS", 3, {}, 1));
  // 箭术靶场：0金币 | 消耗1石头+1木材+1纸 | 2军事盾
  add(createMilitaryCard("ARCHERY RANGE", 0,{{ResourceType::STONE, 1},{ResourceType::WOOD, 1}, {ResourceType::PAPER, 1}}, 2));
  // 阅兵场：0金币 | 消耗2粘土+1玻璃 | 2军事盾
  add(createMilitaryCard("PARADE GROUND", 0, {{ResourceType::CLAY, 2} ,{ResourceType::GLASS, 1}}, 2));

  // ---------------------- 绿色：科学卡（4张） ----------------------
  // 图书馆：0金币 | 消耗1石头+1木材+1玻璃 | 2胜利点 | 科学符号：羽毛笔
  add(createScientificCard("LIBRARY", 0, {{ResourceType::STONE, 1}, {ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}}, 2, ScienceSymbol::QUILL, "", ""));
  // 药房：0金币 | 消耗2粘土+1石头 | 2胜利点 | 科学符号：研钵
  add(createScientificCard("DISPENSARY", 0, {{ResourceType::CLAY, 2}, {ResourceType::STONE, 1}}, 2, ScienceSymbol::MORTAR, "", ""));
  // 学校：0金币 | 消耗1木材+2纸 | 1胜利点 | 科学符号：齿轮
  add(createScientificCard("SCHOOL", 0, {{ResourceType::WOOD, 1}, {ResourceType::PAPER, 2}}, 1, ScienceSymbol::GEAR));
  // 实验室：0金币 | 消耗1木材+2玻璃 | 1胜利点 | 科学符号：烧瓶
  add(createScientificCard("LABORATORY", 0, {{ResourceType::WOOD, 1}, {ResourceType::GLASS, 2}}, 1, ScienceSymbol::FLASK, "", ""));

  // ---------------------- 蓝色：平民卡（5张） ----------------------
  // 雕像：0金币 | 消耗2石头 | 4胜利点 | 连锁符号：面具 | 连锁目标：花园
  add(createCivilianCard("STATUE", 0, {{ResourceType::STONE, 2}}, 4, "MASK", "GARDENS"));
  // 神庙：0金币 | 消耗1木材+1粘土 | 4胜利点 | 连锁符号：祭坛 | 连锁目标：万神殿
  add(createCivilianCard("TEMPLE", 0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}, 4, "ALTAR", "PANTHEON"));
  // 渡槽：0金币 | 消耗3石头 | 5胜利点 | 连锁符号：浴场
  add(createCivilianCard("AQUEDUCT", 0, {{ResourceType::STONE, 3}}, 5, "BATHS"));
  // 法院：0金币 | 消耗2木材+1玻璃 | 5胜利点
  add(createCivilianCard("COURTHOUSE", 0, {{ResourceType::WOOD, 2}, {ResourceType::GLASS, 1}}, 5, "", ""));
  // 演讲台：0金币 | 消耗1石头+1木材 | 4胜利点
  add(createCivilianCard("ROSTRUM", 0, {{ResourceType::STONE, 1}, {ResourceType::WOOD, 1}}, 4, "", ""));

  // ---------------------- 黄色：商业卡（4张） ----------------------
  // 论坛：3金币 | 消耗1粘土 | 0胜利点 | 0即时金币 | 产出1玻璃+1纸
  add(createCommercialCard("FORUM", 3, {{ResourceType::CLAY, 1}}, 0, 0, {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}}, "", ""));
  // 商队旅馆：2金币 | 消耗1玻璃+1纸 | 0胜利点 | 0即时金币 | 产出1木材+1粘土+1石头
  add(createCommercialCard("CARAVANSERY", 2, {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}}, 0, 0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}, {ResourceType::STONE, 1}}, "", ""));
  // 海关大楼：4金币/0资源消耗 | 0胜利点 | 0即时金币 | 产出1纸+1玻璃
  add(createCommercialCard("CUSTOMS HOUSE", 4, {}, 0, 0, {{ResourceType::PAPER, 1}, {ResourceType::GLASS, 1}}, "", ""));
  // 酿酒厂：0金币/0资源消耗 | 0胜利点 | 即时产出6金币（特殊：资源类型标注为GOLD）
  add(createCommercialCard("BREWERY", 0, {}, 0, 6, {{ResourceType::GOLD, 6}}, "", ""));

  return deck;
}

// 时代3卡牌组创建（20张基础牌）
std::vector<Card> CardFactory::createAge3Deck() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // ---------------------- 红色：军事卡（5张） ----------------------
  // 防御工事：0金币 | 消耗2石头+1粘土+1纸 | 2军事盾
  add(createMilitaryCard("FORTIFICATIONS", 0, {{ResourceType::STONE, 2}, {ResourceType::CLAY, 1}, {ResourceType::PAPER, 1}}, 2));
  // 攻城工坊：0金币 | 消耗3木材+1玻璃 | 2军事盾
  add(createMilitaryCard("SIEGE WORKSHOP", 0, {{ResourceType::WOOD, 3}, {ResourceType::GLASS, 1}}, 2));
  // 竞技场：0金币 | 消耗3粘土+1石头 | 2军事盾
  add(createMilitaryCard("CIRCUS", 0, {{ResourceType::CLAY, 3}, {ResourceType::STONE, 1}}, 2));
  // 兵工厂：0金币 | 消耗3粘土+2木材 | 3军事盾
  add(createMilitaryCard("ARSENAL", 0, {{ResourceType::CLAY, 3}, {ResourceType::WOOD, 2}}, 3));
  // 执政官府：8金币/0资源消耗 | 3军事盾
  add(createMilitaryCard("PRETORIUM", 8, {}, 3));

  // ---------------------- 绿色：科学卡（4张） ----------------------
  // 学院：0金币 | 消耗1石头+1木材+2玻璃 | 3胜利点 | 科学符号：石板
  add(createScientificCard("ACADEMY", 0, {{ResourceType::STONE, 1}, {ResourceType::WOOD, 1}, {ResourceType::GLASS, 2}}, 3, ScienceSymbol::TABLET, "", ""));
  // 研习所：0金币 | 消耗2木材+1玻璃+1纸 | 3胜利点 | 科学符号：石板
  add(createScientificCard("STUDY", 0, {{ResourceType::WOOD, 2}, {ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}}, 3, ScienceSymbol::TABLET, "", ""));
  // 大学：0金币 | 消耗1粘土+1玻璃+1纸 | 2胜利点 | 科学符号：指南针
  add(createScientificCard("UNIVERSITY", 0, {{ResourceType::CLAY, 1}, {ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}}, 2, ScienceSymbol::COMPASS, "", ""));
  // 天文台：0金币 | 消耗1石头+2纸 | 2胜利点 | 科学符号：指南针
  add(createScientificCard("OBSERVATORY", 0, {{ResourceType::STONE, 1}, {ResourceType::PAPER, 2}}, 2, ScienceSymbol::COMPASS, "", ""));

  // ---------------------- 蓝色：平民卡（6张） ----------------------
  // 宫殿：0金币 | 消耗1粘土+1石头+1木材+2玻璃 | 7胜利点
  add(createCivilianCard("PALACE", 0, {{ResourceType::CLAY, 1}, {ResourceType::STONE, 1}, {ResourceType::WOOD, 1}, {ResourceType::GLASS, 2}}, 7, "", ""));
  // 市政厅：0金币 | 消耗3石头+2木材 | 7胜利点
  add(createCivilianCard("TOWN HALL", 0, {{ResourceType::STONE, 3}, {ResourceType::WOOD, 2}}, 7, "", ""));
  // 方尖碑：0金币 | 消耗2石头+1玻璃 | 5胜利点
  add(createCivilianCard("OBELISK", 0, {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}}, 5, "", ""));
  // 花园：0金币 | 消耗2粘土+2木材 | 6胜利点
  add(createCivilianCard("GARDENS", 0, {{ResourceType::CLAY, 2}, {ResourceType::WOOD, 2}}, 6, "", ""));
  // 万神殿：0金币 | 消耗1玻璃+1粘土+2纸 | 6胜利点
  add(createCivilianCard("PANTHEON", 0, {{ResourceType::GLASS, 1}, {ResourceType::CLAY, 1}, {ResourceType::PAPER, 2}}, 6, "", ""));
  // 元老院：0金币 | 消耗2粘土+1石头+1纸 | 5胜利点
  add(createCivilianCard("SENATE", 0, {{ResourceType::CLAY, 2}, {ResourceType::STONE, 1}, {ResourceType::PAPER, 1}}, 5, "", ""));

  // ---------------------- 黄色：商业卡（5张） ----------------------
  // 商会：0金币 | 消耗2纸 | 3胜利点 | 即时3金币 | 无产出
  add(createCommercialCard("CHAMBER OF COMMERCE", 0, {{ResourceType::PAPER, 2}}, 3, 3, {}, "", ""));
  // 港口：0金币 | 消耗1木材+1玻璃+1纸 | 3胜利点 | 即时2金币 | 无产出
  add(createCommercialCard("PORT", 0, {{ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}}, 3, 2, {}, "", ""));
  // 军械库：0金币 | 消耗2石头+1玻璃 | 3胜利点 | 即时1金币 | 无产出
  add(createCommercialCard("ARMORY", 0, {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}}, 3, 1, {}, "", ""));
  // 灯塔：0金币 | 消耗2粘土+1玻璃 | 3胜利点 | 即时1金币 | 无产出
  add(createCommercialCard("LIGHTHOUSE", 0, {{ResourceType::CLAY, 2}, {ResourceType::GLASS, 1}}, 3, 1, {}, "", ""));
  // 竞技场：0金币 | 消耗1粘土+1石头+1木材 | 3胜利点 | 即时2金币 | 无产出
  add(createCommercialCard("ARENA", 0, {{ResourceType::CLAY, 1}, {ResourceType::STONE, 1}, {ResourceType::WOOD, 1}}, 3, 2, {}, "", ""));

  return deck;
}

// 公会卡创建（废弃版本，仅保留注释说明）
std::vector<Card> CardFactory::createGuildCards() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };
  // 注：此版本为废弃版本，实际生效为下方重载版本
  add(createGuildCard("MERCHANTS' GUILD", {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}}, 0));
  add(createGuildCard("SHIPOWNERS' GUILD", {{ResourceType::WOOD, 2}, {ResourceType::GLASS, 1}}, 0));
  add(createGuildCard("BUILDERS' GUILD", {{ResourceType::STONE, 2}, {ResourceType::PAPER, 1}}, 0));
  add(createGuildCard("MAGISTRATES' GUILD", {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}}, 0));
  add(createGuildCard("TACTICIANS' GUILD", {{ResourceType::CLAY, 2}, {ResourceType::GLASS, 1}}, 0));
  add(createGuildCard("SCIENTISTS' GUILD", {{ResourceType::WOOD, 2}, {ResourceType::PAPER, 1}}, 0));
  add(createGuildCard("MONEY LENDERS' GUILD", {{ResourceType::STONE, 1}, {ResourceType::CLAY, 1}, {ResourceType::WOOD, 1}}, 0));
  return deck;
}

// 公会卡创建（最终生效版本，7张）
std::vector<Card> CardFactory::createGuildCards() {
  std::vector<Card> deck;
  auto add = [&deck](std::unique_ptr<Card> card) { deck.push_back(*card); };

  // 1. 商人公会
  // 消耗：1纸+1粘土+1木材+1玻璃 | 即时1金币 | 1胜利点 | 效果类型：黄色商业卡加成
  add(createGuildCard("MERCHANTS GUILD",
                      {{ResourceType::PAPER, 1}, {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}},
                      1, 1, "YELLOW_COMMERCIAL_CARD"));

  // 2. 船主公会
  // 消耗：1纸+1粘土+1木材+1玻璃 | 即时1金币 | 1胜利点 | 效果类型：棕/灰色贸易卡加成
  add(createGuildCard("SHIPOWNERS GUILD",
                      {{ResourceType::PAPER, 1}, {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}},
                      1, 1, "BROWN_GRAY_TRADE_CARD"));

  // 3. 建筑商公会
  // 消耗：2石头+1粘土+1木材+1玻璃 | 即时2金币 | 2胜利点 | 效果类型：金字塔建筑卡加成
  add(createGuildCard("BUILDERS GUILD",
                      {{ResourceType::STONE, 2}, {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}},
                      2, 2, "PYRAMID_BUILDING_CARD"));

  // 4. 地方官公会
  // 消耗：1纸+1粘土+2木材 | 即时1金币 | 1胜利点 | 效果类型：蓝色平民卡加成
  add(createGuildCard("MAGISTRATES GUILD",
                      {{ResourceType::PAPER, 1}, {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 2}},
                      1, 1, "BLUE_CIVILIAN_CARD"));

  // 5. 科学家公会
  // 消耗：1木材+1粘土+1纸 | 即时1金币 | 1胜利点 | 效果类型：绿色科学卡加成
  add(createGuildCard("SCIENTISTS GUILD",
                      {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1},
                       {ResourceType::PAPER, 1}},
                      1, 1, "GREEN_SCIENTIFIC_CARD"));

  // 6. 放债人公会
  // 消耗：1石头+1粘土+2木材 | 即时3金币 | 1胜利点 | 效果类型：金币奖励加成
  add(createGuildCard("MONEYLENDERS GUILD",
                      {{ResourceType::STONE, 1}, {ResourceType::CLAY, 1},
                       {ResourceType::WOOD, 2}},
                      3, 1, "GOLD_COIN_REWARD"));

  // 7. 战术家公会
  // 消耗：1石头+1粘土+1纸 | 即时1金币 | 1胜利点 | 效果类型：红色军事卡加成
  add(createGuildCard("TACTICIANS GUILD",
                      {{ResourceType::STONE, 1}, {ResourceType::CLAY, 1},
                       {ResourceType::PAPER, 1}},
                      1, 1, "RED_MILITARY_CARD"));

  return deck;
}

// 所有奇迹创建（12张）
std::vector<std::unique_ptr<Wonder>> CardFactory::createAllWonders() {
  std::vector<std::unique_ptr<Wonder>> wonders;

  // 1. 罗德岛巨像
  // 消耗：3金币 + 2石头+1木材 | 效果：1军事盾 + 3胜利点
  Effect colossusEffect;
  colossusEffect.militaryShields = 1;
  colossusEffect.victoryPoints = 3;
  wonders.push_back(createWonder("COLOSSUS", 3, {{ResourceType::STONE, 2}, {ResourceType::WOOD, 1}}, colossusEffect));

  // 2. 马克西姆斯竞技场
  // 消耗：2金币 + 1玻璃+1石头+1木材 | 效果：1军事盾 + 3胜利点 + 摧毁对手灰色卡
  Effect circusMaximus;
  circusMaximus.militaryShields = 1;
  circusMaximus.victoryPoints = 3;
  circusMaximus.destroyOpponentGrey = true;
  wonders.push_back(createWonder("CIRCUS MAXIMUS", 2, {{ResourceType::GLASS, 1}, {ResourceType::STONE, 1}, {ResourceType::WOOD, 1}}, circusMaximus));

  // 3. 宙斯神像
  // 消耗：2金币 + 1粘土+1木材+1石头 | 效果：1军事盾 + 3胜利点 + 摧毁对手棕色卡
  Effect statueZeus;
  statueZeus.militaryShields = 1;
  statueZeus.victoryPoints = 3;
  statueZeus.destroyOpponentBrown = true;
  wonders.push_back(createWonder("STATUE OF ZEUS", 2, {{ResourceType::CLAY, 1}, {ResourceType::WOOD, 1}, {ResourceType::STONE, 1}}, statueZeus));

  // 4. 阿耳忒弥斯神庙
  // 消耗：2金币 + 1纸+1玻璃 | 效果：12金币 + 可再次行动
  Effect templeArtemis;
  templeArtemis.coins = 12;
  templeArtemis.playAgain = true;
  wonders.push_back(createWonder("TEMPLE OF ARTEMIS", 2, {{ResourceType::PAPER, 1}, {ResourceType::GLASS, 1}}, templeArtemis));

  // 5. 大图书馆
  // 消耗：2金币 + 1纸+1木材+1玻璃 | 效果：4胜利点 + 获得进度代币
  Effect greatLibrary;
  greatLibrary.victoryPoints = 4;
  greatLibrary.gainProgressToken = true;
  wonders.push_back(createWonder("GREAT LIBRARY", 2, {{ResourceType::PAPER, 1}, {ResourceType::WOOD, 1}, {ResourceType::GLASS, 1}}, greatLibrary));

  // 6. 亚历山大灯塔
  // 消耗：2金币 + 2石头+1玻璃 | 效果：4胜利点 + 产出1木材+1石头+1粘土
  Effect greatLighthouse;
  greatLighthouse.victoryPoints = 4;
  greatLighthouse.resourcesProduced[ResourceType::WOOD] = 1;
  greatLighthouse.resourcesProduced[ResourceType::STONE] = 1;
  greatLighthouse.resourcesProduced[ResourceType::CLAY] = 1;
  wonders.push_back(createWonder("GREAT LIGHTHOUSE", 2, {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}}, greatLighthouse));

  // 7. 摩索拉斯陵墓
  // 消耗：1金币 + 1玻璃+1纸+1石头 | 效果：2胜利点 + 可从弃牌堆建造
  Effect mausoleum;
  mausoleum.victoryPoints = 2;
  mausoleum.buildFromDiscard = true;
  wonders.push_back(createWonder("MAUSOLEUM", 1, {{ResourceType::GLASS, 1}, {ResourceType::PAPER, 1}, {ResourceType::STONE, 1}}, mausoleum));

  // 8. 斯芬克斯
  // 消耗：2金币 + 2粘土+1纸 | 效果：6胜利点 + 可再次行动
  Effect sphinx;
  sphinx.victoryPoints = 6;
  sphinx.playAgain = true;
  wonders.push_back(createWonder("SPHINX", 2, {{ResourceType::CLAY, 2}, {ResourceType::PAPER, 1}}, sphinx));

  // 9. 金字塔
  // 消耗：3金币 + 3石头+1木材 | 效果：9胜利点（无额外效果）
  Effect pyramids;
  pyramids.victoryPoints = 9;
  wonders.push_back(createWonder("PYRAMIDS", 3, {{ResourceType::STONE, 3}, {ResourceType::WOOD, 1}}, pyramids));

  // 10. 空中花园
  // 消耗：2金币 + 2木材+1粘土 | 效果：3胜利点 + 6金币 + 可再次行动
  Effect hangingGardens;
  hangingGardens.victoryPoints = 3;
  hangingGardens.coins = 6;
  hangingGardens.playAgain = true;
  wonders.push_back(createWonder("HANGING GARDENS", 2, {{ResourceType::WOOD, 2}, {ResourceType::CLAY, 1}}, hangingGardens));

  // 11. 比雷埃夫斯港
  // 消耗：2金币 + 2粘土+1玻璃 | 效果：产出1玻璃+1纸 + 可再次行动
  Effect piraeus;
  piraeus.resourcesProduced[ResourceType::GLASS] = 1;
  piraeus.resourcesProduced[ResourceType::PAPER] = 1;
  piraeus.playAgain = true;
  wonders.push_back(createWonder("PIRAEUS", 2, {{ResourceType::CLAY, 2}, {ResourceType::GLASS, 1}}, piraeus));

  // 12. 阿庇亚大道
  // 消耗：2金币 + 1石头+1粘土+1纸 | 效果：3胜利点 + 3金币 + 可再次行动
  Effect appianWay;
  appianWay.victoryPoints = 3;
  appianWay.coins = 3;
  appianWay.playAgain = true;
  wonders.push_back(createWonder("APPIAN WAY", 2, {{ResourceType::STONE, 1}, {ResourceType::CLAY, 1}, {ResourceType::PAPER, 1}}, appianWay));

  return wonders;
}
