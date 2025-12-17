#include "Deck.h"
#include <algorithm>   
#include <chrono>      // 用于获取时间种子
#include <random>      // 随机数生成

// 辅助函数：简化卡牌创建（构建器模式）
// 参数说明：名称、类型、金币成本、资源成本、产出资源、胜利点数、盾牌、科学符号、金币
Card createCard(std::string name, CardType type, int costCoins,
                std::map<ResourceType, int> costRes,
                std::map<ResourceType, int> prodRes, int vp, int shields,
                std::vector<ScienceSymbol> sci, int coins) {
  Cost c;
  c.coins = costCoins;         // 设置金币成本
  c.resources = costRes;       // 设置资源成本
  
  Effect e;
  e.resourcesProduced = prodRes; // 设置产出资源
  e.victoryPoints = vp;         // 设置胜利点数
  e.militaryShields = shields;  // 设置军事盾牌
  e.scienceSymbols = sci;       // 设置科学符号
  e.coins = coins;              // 设置金币产出

  //12.14 新增playAgain 和 resourceRebate 参数
  e.playAgain = playAgain;
  e.resourceRebate = resourceRebate;

  return Card(name, type, c, e); // 创建并返回卡牌
}

// 时代1卡池
std::vector<Card*> Deck::getAge1Deck() {
  std::vector<Card*> age1Cards;

  // 1. 棕色 - 原材料卡（6张）
  // 伐木场：产出木头，无成本
  age1Cards.push_back(CardFactory::createRawMaterialCard(
      "LUMBER YARD",
      CardColor::Brown,
      {}, // 建造成本：无
      Resource::Wood, // 产出资源
      0 // 胜利分数
  ));
  // 伐木营地：产出木头，无成本
  age1Cards.push_back(CardFactory::createRawMaterialCard(
      "LOGGING CAMP",
      CardColor::Brown,
      {},
      Resource::Wood,
      0
  ));
  // 粘土池：产出粘土，无成本
  age1Cards.push_back(CardFactory::createRawMaterialCard(
      "CLAY POOL",
      CardColor::Brown,
      {},
      Resource::Clay,
      0
  ));
  // 粘土矿：产出粘土，无成本
  age1Cards.push_back(CardFactory::createRawMaterialCard(
      "CLAY PIT",
      CardColor::Brown,
      {},
      Resource::Clay,
      0
  ));
  // 采石场：产出石头，无成本
  age1Cards.push_back(CardFactory::createRawMaterialCard(
      "QUARRY",
      CardColor::Brown,
      {},
      Resource::Stone,
      0
  ));
  // 石矿：产出石头，无成本
  age1Cards.push_back(CardFactory::createRawMaterialCard(
      "STONE PIT",
      CardColor::Brown,
      {},
      Resource::Stone,
      0
  ));

  // 2. 灰色 - 制成品卡（2张）
  // 玻璃厂：产出玻璃，成本1木头+1粘土
  age1Cards.push_back(CardFactory::createManufacturedGoodCard(
      "GLASSWORKS",
      CardColor::Gray,
      {Resource::Wood, Resource::Clay}, // 建造成本
      Resource::Glass, // 产出资源
      1 // 胜利分数
  ));
  // 印刷厂：产出莎草纸，成本1木头+1石头
  age1Cards.push_back(CardFactory::createManufacturedGoodCard(
      "PRESS",
      CardColor::Gray,
      {Resource::Wood, Resource::Stone}, // 建造成本
      Resource::Papyrus, // 产出资源
      1 // 胜利分数
  ));

  // 3. 红色 - 军事建筑卡（4张）
  // 守卫塔：1个军事盾牌，成本1木头+1石头
  age1Cards.push_back(CardFactory::createMilitaryBuildingCard(
      "GUARD TOWER",
      CardColor::Red,
      {Resource::Wood, Resource::Stone}, // 建造成本
      [](Game* game, Player* player) {
          player->addShield(1); // 效果：+1盾牌
      },
      1 // 胜利分数
  ));
  // 马厩：军事效果（骑兵标识），成本1木头+1粘土
  age1Cards.push_back(CardFactory::createMilitaryBuildingCard(
      "STABLE",
      CardColor::Red,
      {Resource::Wood, Resource::Clay}, // 建造成本
      [](Game* game, Player* player) {
          player->addCavalryToken(1); // 效果：+1骑兵标识（需Player类支持）
          player->addShield(1); // 附加+1盾牌
      },
      1 // 胜利分数
  ));
  // 军营：军事效果（剑形标识），成本2石头+1粘土
  age1Cards.push_back(CardFactory::createMilitaryBuildingCard(
      "GARRISON",
      CardColor::Red,
      {Resource::Stone, Resource::Stone, Resource::Clay}, // 建造成本
      [](Game* game, Player* player) {
          player->addSwordToken(1); // 效果：+1剑形标识（需Player类支持）
          player->addShield(1); // 附加+1盾牌
      },
      2 // 胜利分数
  ));
  // 栅栏：军事效果（盾牌标识），成本3木头
  age1Cards.push_back(CardFactory::createMilitaryBuildingCard(
      "PALISADE",
      CardColor::Red,
      {Resource::Wood, Resource::Wood, Resource::Wood}, // 建造成本
      [](Game* game, Player* player) {
          player->addShieldToken(1); // 效果：+1盾牌标识（需Player类支持）
          player->addShield(1); // 附加+1盾牌
      },
      1 // 胜利分数
  ));

  // 4. 绿色 - 科学卡（4张）：调用Board::onSciencePair接口
  // 工坊：三角科学符号，成本1木头+1粘土
  age1Cards.push_back(CardFactory::createScienceCard(
      "WORKSHOP",
      CardColor::Green,
      {Resource::Wood, Resource::Clay}, // 建造成本
      ScienceSymbol::Triangle, // 科学符号类型
      [](Game* game, Player* player) {
          // 调用B定义的Board接口，触发科学符号配对
          Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
          player->addScienceSymbol(ScienceSymbol::Triangle, 1); // 玩家添加该符号
      },
      0 // 胜利分数
  ));
  // 药剂师：烧瓶科学符号，成本1粘土+1石头
  age1Cards.push_back(CardFactory::createScienceCard(
      "APOTHECARY",
      CardColor::Green,
      {Resource::Clay, Resource::Stone}, // 建造成本
      ScienceSymbol::Flask, // 科学符号类型
      [](Game* game, Player* player) {
          Board::getInstance()->onSciencePair(ScienceSymbol::Flask);
          player->addScienceSymbol(ScienceSymbol::Flask, 1);
      },
      0 // 胜利分数
  ));
  // 文书院：羽毛笔科学符号，成本1木头+1莎草纸（时代1合理成本调整）
  age1Cards.push_back(CardFactory::createScienceCard(
      "SCRIPTORIUM",
      CardColor::Green,
      {Resource::Wood, Resource::Papyrus}, // 建造成本
      ScienceSymbol::Quill, // 科学符号类型
      [](Game* game, Player* player) {
          Board::getInstance()->onSciencePair(ScienceSymbol::Quill);
          player->addScienceSymbol(ScienceSymbol::Quill, 1);
      },
      0 // 胜利分数
  ));
  // 药剂师（带齿轮联动）：烧瓶科学符号，成本1玻璃+1粘土
  age1Cards.push_back(CardFactory::createScienceCard(
      "PHARMACIST",
      CardColor::Green,
      {Resource::Glass, Resource::Clay}, // 建造成本
      ScienceSymbol::Flask, // 科学符号类型
      [](Game* game, Player* player) {
          Board::getInstance()->onSciencePair(ScienceSymbol::Flask);
          player->addScienceSymbol(ScienceSymbol::Flask, 1);
          player->triggerGearEffect(); // 齿轮联动效果（需Player类支持）
      },
      0 // 胜利分数
  ));

  // 5. 黄色 - 商业建筑卡（4张）
  // 石储备库：石头资源+1/折扣，成本1石头
  age1Cards.push_back(CardFactory::createCommercialBuildingCard(
      "STONE RESERVE",
      CardColor::Yellow,
      {Resource::Stone}, // 建造成本
      [](Game* game, Player* player) {
          player->addPerTurnResource(Resource::Stone, 1); // 每回合+1石头
          player->setResourceDiscount(Resource::Stone, 1); // 石头成本-1
      },
      0 // 胜利分数
  ));
  // 粘土储备库：粘土资源+1/折扣，成本1粘土
  age1Cards.push_back(CardFactory::createCommercialBuildingCard(
      "CLAY RESERVE",
      CardColor::Yellow,
      {Resource::Clay}, // 建造成本
      [](Game* game, Player* player) {
          player->addPerTurnResource(Resource::Clay, 1); // 每回合+1粘土
          player->setResourceDiscount(Resource::Clay, 1); // 粘土成本-1
      },
      0 // 胜利分数
  ));
  // 木材储备库：木头资源+1/折扣，成本1木头
  age1Cards.push_back(CardFactory::createCommercialBuildingCard(
      "WOOD RESERVE",
      CardColor::Yellow,
      {Resource::Wood}, // 建造成本
      [](Game* game, Player* player) {
          player->addPerTurnResource(Resource::Wood, 1); // 每回合+1木头
          player->setResourceDiscount(Resource::Wood, 1); // 木头成本-1
      },
      0 // 胜利分数
  ));
  // 酒馆：产出4金币，成本1木头+1粘土
  age1Cards.push_back(CardFactory::createCommercialBuildingCard(
      "TAVERN",
      CardColor::Yellow,
      {Resource::Wood, Resource::Clay}, // 建造成本
      [](Game* game, Player* player) {
          player->addGold(4); // 效果：获得4金币
      },
      1 // 胜利分数
  ));

  // 6. 蓝色 - 民用建筑卡（3张）
  // 剧院：胜利分3，成本2石头+1粘土
  age1Cards.push_back(CardFactory::createCivilianBuildingCard(
      "THEATER",
      CardColor::Blue,
      {Resource::Stone, Resource::Stone, Resource::Clay}, // 建造成本
      [](Game* game, Player* player) {
          // 无额外效果，仅基础胜利分
      },
      3 // 胜利分数（月亮标识）
  ));
  // 祭坛：胜利分3，成本2粘土+1石头
  age1Cards.push_back(CardFactory::createCivilianBuildingCard(
      "ALTAR",
      CardColor::Blue,
      {Resource::Clay, Resource::Clay, Resource::Stone}, // 建造成本
      [](Game* game, Player* player) {
          // 无额外效果，仅基础胜利分
      },
      3 // 胜利分数（星星标识）
  ));
  // 浴场：胜利分3，成本1木头+1石头+1粘土
  age1Cards.push_back(CardFactory::createCivilianBuildingCard(
      "BATHS",
      CardColor::Blue,
      {Resource::Wood, Resource::Stone, Resource::Clay}, // 建造成本
      [](Game* game, Player* player) {
          // 无额外效果，仅基础胜利分
      },
      3 // 胜利分数（水滴标识）
  ));

  // 验证：确保总数量为23张
  assert(age1Cards.size() == 23 && "Age 1 deck must have exactly 23 cards");

  // 随机抽取3张舍弃，剩余20张作为可用卡组
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(age1Cards.begin(), age1Cards.end(), g);

  // 移除最后3张（舍弃）
  age1Cards.erase(age1Cards.end() - 3, age1Cards.end());

  return age1Cards;
}


// 时代2卡池：23张基础卡，抽3张舍弃，返回20张可用卡组
std::vector<Card*> Deck::getAge2Deck() {
    std::vector<Card*> age2Cards;

    // 1. 棕色 - 原材料卡（3张，带联动标识）
    // 锯木厂：产出木头+联动效果，成本1木头+1粘土
    age2Cards.push_back(CardFactory::createRawMaterialCard(
        "SAWMILL",
        CardColor::Brown,
        {Resource::Wood, Resource::Clay}, // 建造成本
        Resource::Wood, // 产出资源
        0, // 胜利分数
        true // 联动标识（第5参数，需CardFactory支持）
    ));
    // 砖厂：产出粘土+联动效果，成本1粘土+1石头
    age2Cards.push_back(CardFactory::createRawMaterialCard(
        "BRICKYARD",
        CardColor::Brown,
        {Resource::Clay, Resource::Stone}, // 建造成本
        Resource::Clay, // 产出资源
        0,
        true // 联动标识
    ));
    // 架式采石场：产出石头+联动效果，成本1石头+1木头
    age2Cards.push_back(CardFactory::createRawMaterialCard(
        "SHELF QUARRY",
        CardColor::Brown,
        {Resource::Stone, Resource::Wood}, // 建造成本
        Resource::Stone, // 产出资源
        0,
        true // 联动标识
    ));

    // 2. 灰色 - 制成品卡（2张）
    // 玻璃吹制厂：产出玻璃，成本1木头+1玻璃（时代2进阶成本）
    age2Cards.push_back(CardFactory::createManufacturedGoodCard(
        "GLASS-BLOWER",
        CardColor::Gray,
        {Resource::Wood, Resource::Glass}, // 建造成本
        Resource::Glass, // 产出资源
        2 // 胜利分数
    ));
    // 干燥房：产出莎草纸，成本1粘土+1莎草纸（时代2进阶成本）
    age2Cards.push_back(CardFactory::createManufacturedGoodCard(
        "DRYING ROOM",
        CardColor::Gray,
        {Resource::Clay, Resource::Papyrus}, // 建造成本
        Resource::Papyrus, // 产出资源
        2 // 胜利分数
    ));

    // 3. 红色 - 军事建筑卡（5张）
    // 城墙：防御效果（盾牌标识），成本2石头+1粘土
    age2Cards.push_back(CardFactory::createMilitaryBuildingCard(
        "WALLS",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Clay}, // 建造成本
        [](Game* game, Player* player) {
            player->addShield(2); // 效果：+2盾牌（防御强化）
            player->addShieldToken(1); // 盾牌标识（需Player类支持）
        },
        2 // 胜利分数
    ));
    // 育马场：骑兵效果（骑兵标识），成本2木头+1粘土
    age2Cards.push_back(CardFactory::createMilitaryBuildingCard(
        "HORSE BREEDERS",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Clay}, // 建造成本
        [](Game* game, Player* player) {
            player->addShield(1);
            player->addCavalryToken(2); // +2骑兵标识（时代2强化）
        },
        2 // 胜利分数
    ));
    // 兵营：剑形效果（剑形标识），成本2石头+1木头+1粘土
    age2Cards.push_back(CardFactory::createMilitaryBuildingCard(
        "BARRACKS",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Wood, Resource::Clay}, // 建造成本
        [](Game* game, Player* player) {
            player->addShield(2);
            player->addSwordToken(2); // +2剑形标识（时代2强化）
        },
        3 // 胜利分数
    ));
    // 箭术场：远程效果（弓箭标识），成本1木头+1石头+1玻璃
    age2Cards.push_back(CardFactory::createMilitaryBuildingCard(
        "ARCHERY RANGE",
        CardColor::Red,
        {Resource::Wood, Resource::Stone, Resource::Glass}, // 建造成本
        [](Game* game, Player* player) {
            player->addShield(1);
            player->addBowToken(2); // +2弓箭标识（需Player类支持）
        },
        2 // 胜利分数
    ));
    // 阅兵场：军事联动效果（齿轮标识），成本1木头+1粘土+1莎草纸
    age2Cards.push_back(CardFactory::createMilitaryBuildingCard(
        "PARADE GROUND",
        CardColor::Red,
        {Resource::Wood, Resource::Clay, Resource::Papyrus}, // 建造成本
        [](Game* game, Player* player) {
            player->addShield(1);
            player->triggerGearEffect(); // 齿轮联动（触发额外军事加成）
            // 联动效果：复制1个已有的军事标识
            player->copyRandomMilitaryToken(); // 需Player类支持
        },
        3 // 胜利分数
    ));

    // 4. 绿色 - 科学卡（4张，调用Board接口）
    // 图书馆：2个羽毛笔符号+齿轮联动，成本1木头+1玻璃+1莎草纸
    age2Cards.push_back(CardFactory::createScienceCard(
        "LIBRARY",
        CardColor::Green,
        {Resource::Wood, Resource::Glass, Resource::Papyrus}, // 建造成本
        ScienceSymbol::Quill, // 科学符号类型
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Quill); // 调用B的接口
            player->addScienceSymbol(ScienceSymbol::Quill, 2); // +2羽毛笔
            player->triggerGearEffect(); // 齿轮联动（额外科学加成）
        },
        0 // 胜利分数
    ));
    // 药房：2个烧瓶符号+齿轮联动，成本1粘土+1玻璃+1莎草纸
    age2Cards.push_back(CardFactory::createScienceCard(
        "DISPENSARY",
        CardColor::Green,
        {Resource::Clay, Resource::Glass, Resource::Papyrus}, // 建造成本
        ScienceSymbol::Flask, // 科学符号类型
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Flask);
            player->addScienceSymbol(ScienceSymbol::Flask, 2); // +2烧瓶
            player->triggerGearEffect(); // 齿轮联动
        },
        0 // 胜利分数
    ));
    // 学校：1个三角符号+三叉戟标识，成本1石头+1玻璃+1莎草纸
    age2Cards.push_back(CardFactory::createScienceCard(
        "SCHOOL",
        CardColor::Green,
        {Resource::Stone, Resource::Glass, Resource::Papyrus}, // 建造成本
        ScienceSymbol::Triangle, // 科学符号类型
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
            player->addScienceSymbol(ScienceSymbol::Triangle, 1); // +1三角
            player->addTridentToken(1); // 三叉戟标识（需Player类支持）
        },
        1 // 胜利分数
    ));
    // 实验室：1个三角符号+三角联动，成本2玻璃+1莎草纸
    age2Cards.push_back(CardFactory::createScienceCard(
        "LABORATORY",
        CardColor::Green,
        {Resource::Glass, Resource::Glass, Resource::Papyrus}, // 建造成本
        ScienceSymbol::Triangle, // 科学符号类型
        [](Game* game, Player* player) {
            Board::getInstance()->onSciencePair(ScienceSymbol::Triangle);
            player->addScienceSymbol(ScienceSymbol::Triangle, 1); // +1三角
            player->triggerTriangleLinkEffect(); // 三角联动效果（需Player类支持）
        },
        1 // 胜利分数
    ));

    // 5. 黄色 - 商业建筑卡（4张）
    // 商队旅馆：金币/贸易效果，成本1木头+1粘土+1金币
    age2Cards.push_back(CardFactory::createCommercialBuildingCard(
        "CARAVANSERY",
        CardColor::Yellow,
        {Resource::Wood, Resource::Clay, Resource::Gold}, // 建造成本
        [](Game* game, Player* player) {
            player->addPerTurnGold(2); // 每回合+2金币
            player->addTradeBonus(1); // 贸易加成+1（需Player类支持）
        },
        1 // 胜利分数
    ));
    // 海关：商业贸易效果，成本1石头+1玻璃+1金币
    age2Cards.push_back(CardFactory::createCommercialBuildingCard(
        "CUSTOMS HOUSE",
        CardColor::Yellow,
        {Resource::Stone, Resource::Glass, Resource::Gold}, // 建造成本
        [](Game* game, Player* player) {
            // 效果：每拥有1个贸易标识，获得2金币
            int tradeCount = player->getTradeTokenCount();
            player->addGold(tradeCount * 2);
            player->addTradeToken(1); // +1贸易标识
        },
        2 // 胜利分数
    ));
    // 酒馆（时代2）：产出4金币，成本1木头+1莎草纸
    age2Cards.push_back(CardFactory::createCommercialBuildingCard(
        "TAVERN",
        CardColor::Yellow,
        {Resource::Wood, Resource::Papyrus}, // 建造成本（时代2进阶）
        [](Game* game, Player* player) {
            player->addGold(4); // 效果：获得4金币
        },
        1 // 胜利分数
    ));
    // 酿酒厂：产出6金币，成本1粘土+1玻璃+1金币
    age2Cards.push_back(CardFactory::createCommercialBuildingCard(
        "BREWERY",
        CardColor::Yellow,
        {Resource::Clay, Resource::Glass, Resource::Gold}, // 建造成本
        [](Game* game, Player* player) {
            player->addGold(6); // 效果：获得6金币
        },
        2 // 胜利分数
    ));

    // 6. 蓝色 - 民用建筑卡（5张）
    // 法院：胜利分5，成本2石头+1玻璃+1莎草纸
    age2Cards.push_back(CardFactory::createCivilianBuildingCard(
        "COURTHOUSE",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Papyrus}, // 建造成本
        [](Game* game, Player* player) {
            // 无额外效果，基础胜利分+水滴标识加成
        },
        5 // 胜利分数（水滴标识）
    ));
    // 雕像：胜利分4，成本1石头+1粘土+1玻璃
    age2Cards.push_back(CardFactory::createCivilianBuildingCard(
        "STATUE",
        CardColor::Blue,
        {Resource::Stone, Resource::Clay, Resource::Glass}, // 建造成本
        [](Game* game, Player* player) {
            // 飞鸟标识加成
        },
        4 // 胜利分数（飞鸟标识）
    ));
    // 神庙：胜利分4，成本1木头+1粘土+1莎草纸
    age2Cards.push_back(CardFactory::createCivilianBuildingCard(
        "TEMPLE",
        CardColor::Blue,
        {Resource::Wood, Resource::Clay, Resource::Papyrus}, // 建造成本
        [](Game* game, Player* player) {
            // 月亮标识加成
        },
        4 // 胜利分数（月亮标识）
    ));
    // 渡槽：胜利分5，成本2木头+1石头+1玻璃
    age2Cards.push_back(CardFactory::createCivilianBuildingCard(
        "AQUEDUCT",
        CardColor::Blue,
        {Resource::Wood, Resource::Wood, Resource::Stone, Resource::Glass}, // 建造成本
        [](Game* game, Player* player) {
            // 水滴标识加成
        },
        5 // 胜利分数（水滴标识）
    ));
    // 演讲台：胜利分4，成本1木头+1石头+1莎草纸
    age2Cards.push_back(CardFactory::createCivilianBuildingCard(
        "ROSTRUM",
        CardColor::Blue,
        {Resource::Wood, Resource::Stone, Resource::Papyrus}, // 建造成本
        [](Game* game, Player* player) {
            // 喇叭标识加成
        },
        4 // 胜利分数（喇叭标识）
    ));

    // 验证：确保总数量为23张
    assert(age2Cards.size() == 23 && "Age 2 deck must have exactly 23 cards");

    // 随机抽取3张舍弃，剩余20张作为可用卡组
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age2Cards.begin(), age2Cards.end(), g);

    // 移除最后3张（舍弃）
    age2Cards.erase(age2Cards.end() - 3, age2Cards.end());

    return age2Cards;
}

// 时代3完整卡组：20张基础卡抽3弃3（剩17张）+ 3张公会卡，最终20张
std::vector<Card*> Deck::getAge3Deck() {
    std::vector<Card*> age3BaseCards;

    // -------------------------- 第一步：构建20张时代3基础卡 --------------------------
    // 1. 黄色 - 商业建筑卡（5张）
    age3BaseCards.push_back(CardFactory::createCommercialBuildingCard(
        "LIGHTHOUSE",
        CardColor::Yellow,
        {Resource::Glass, Resource::Glass, Resource::Papyrus, Resource::Gold},
        [](Game* game, Player* player) { player->addGold(3); },
        2
    ));
    age3BaseCards.push_back(CardFactory::createCommercialBuildingCard(
        "ARENA",
        CardColor::Yellow,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) { 
            player->addGold(3); 
            player->addTriangleToken(1); 
        },
        2
    ));
    age3BaseCards.push_back(CardFactory::createCommercialBuildingCard(
        "CHAMBER OF COMMERCE",
        CardColor::Yellow,
        {Resource::Glass, Resource::Papyrus, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) { 
            int tradeCount = player->getTradeTokenCount();
            player->addGold(tradeCount * 3); 
        },
        3
    ));
    age3BaseCards.push_back(CardFactory::createCommercialBuildingCard(
        "PORT",
        CardColor::Yellow,
        {Resource::Wood, Resource::Glass, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) { 
            player->setTradeGoldBonus(2);
            player->addTradeToken(1); 
        },
        3
    ));
    age3BaseCards.push_back(CardFactory::createCommercialBuildingCard(
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
    age3BaseCards.push_back(CardFactory::createCivilianBuildingCard(
        "PALACE",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Glass, Resource::Glass, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) {},
        7
    ));
    age3BaseCards.push_back(CardFactory::createCivilianBuildingCard(
        "TOWN HALL",
        CardColor::Blue,
        {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Papyrus, Resource::Papyrus, Resource::Gold, Resource::Gold},
        [](Game* game, Player* player) {},
        7
    ));
    age3BaseCards.push_back(CardFactory::createCivilianBuildingCard(
        "OBELISK",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) {},
        5
    ));
    age3BaseCards.push_back(CardFactory::createCivilianBuildingCard(
        "GARDENS",
        CardColor::Blue,
        {Resource::Wood, Resource::Wood, Resource::Clay, Resource::Clay, Resource::Gold},
        [](Game* game, Player* player) {},
        6
    ));
    age3BaseCards.push_back(CardFactory::createCivilianBuildingCard(
        "PANTHEON",
        CardColor::Blue,
        {Resource::Stone, Resource::Stone, Resource::Papyrus, Resource::Papyrus, Resource::Gold},
        [](Game* game, Player* player) {},
        6
    ));
    age3BaseCards.push_back(CardFactory::createCivilianBuildingCard(
        "SENATE",
        CardColor::Blue,
        {Resource::Wood, Resource::Stone, Resource::Glass, Resource::Gold},
        [](Game* game, Player* player) {},
        5
    ));

    // 3. 红色 - 军事建筑卡（5张）
    age3BaseCards.push_back(CardFactory::createMilitaryBuildingCard(
        "ARSENAL",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Glass, Resource::Papyrus},
        [](Game* game, Player* player) { player->addShield(3); },
        4
    ));
    age3BaseCards.push_back(CardFactory::createMilitaryBuildingCard(
        "PRETORIUM",
        CardColor::Red,
        {Resource::Stone, Resource::Stone, Resource::Clay, Resource::Clay, Resource::Glass},
        [](Game* game, Player* player) { 
            player->addShield(3); 
            player->addCommandToken(1); 
        },
        4
    ));
    age3BaseCards.push_back(CardFactory::createMilitaryBuildingCard(
        "FORTIFICATIONS",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Stone, Resource::Stone, Resource::Gold},
        [](Game* game, Player* player) { 
            player->addShield(4); 
            player->addShieldToken(2); 
        },
        5
    ));
    age3BaseCards.push_back(CardFactory::createMilitaryBuildingCard(
        "SIEGE WORKSHOP",
        CardColor::Red,
        {Resource::Wood, Resource::Wood, Resource::Glass, Resource::Glass, Resource::Papyrus},
        [](Game* game, Player* player) { 
            player->addShield(3); 
            player->addBowToken(3); 
        },
        4
    ));
    age3BaseCards.push_back(CardFactory::createMilitaryBuildingCard(
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
    age3BaseCards.push_back(CardFactory::createScienceCard(
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
    age3BaseCards.push_back(CardFactory::createScienceCard(
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
    age3BaseCards.push_back(CardFactory::createScienceCard(
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
    age3BaseCards.push_back(CardFactory::createScienceCard(
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

    // 验证：基础卡数量必须为20张
    assert(age3BaseCards.size() == 20 && "Age 3 base deck must have exactly 20 cards");

    // -------------------------- 第二步：20张基础卡抽3弃3（剩17张） --------------------------
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age3BaseCards.begin(), age3BaseCards.end(), g); // 随机打乱
    age3BaseCards.erase(age3BaseCards.end() - 3, age3BaseCards.end()); // 移除最后3张（舍弃）
    assert(age3BaseCards.size() == 17 && "Age 3 base deck must have 17 cards after discarding 3");

    // -------------------------- 第三步：抽取3张公会卡 --------------------------
    // 定义公会卡池（假设公会卡池共有6张，抽3张；若公会卡池仅3张则直接全取）
    std::vector<Card*> guildCardPool = createGuildCardPool();
    std::shuffle(guildCardPool.begin(), guildCardPool.end(), g); // 公会卡池打乱
    std::vector<Card*> selectedGuildCards(guildCardPool.begin(), guildCardPool.begin() + 3); // 抽前3张

    // -------------------------- 第四步：合并17张基础卡+3张公会卡（最终20张） --------------------------
    age3BaseCards.insert(age3BaseCards.end(), selectedGuildCards.begin(), selectedGuildCards.end());
    assert(age3BaseCards.size() == 20 && "Age 3 complete deck must have exactly 20 cards");

    return age3BaseCards;
}

// 创建公会卡池（7张完整公会卡，供抽取3张）
std::vector<Card*> Deck::createGuildCardPool() {
  std::vector<Card*> guildPool;

  // 1. 商人公会（Merchants Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Merchants Guild",
      CardColor::Purple, // 公会卡专属紫色
      {Resource::Wood, Resource::Stone, Resource::Glass, Resource::Papyrus}, // 费用：1木材+1石材+1玻璃+1纸莎草
      [](Game* game, Player* player) {
          // 建造时：每拥有1张黄色商业卡获得1枚硬币
          int commercialCount = player->getCommercialBuildingCount();
          player->addGold(commercialCount * 1);
          // 终局时：每张黄色商业卡计1分（通过标记标识，终局结算时统计）
          player->addGuildVictoryPointBonus(GuildBonusType::CommercialPerCard, 1);
      },
      0, // 基础胜利分0，终局按效果结算
      {EffectIcon::YellowSquare, EffectIcon::Coin} // 效果图标：黄色方块+1硬币
  ));

  // 2. 船主公会（Shipowners Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Shipowners Guild",
      CardColor::Purple,
      {Resource::Wood, Resource::Stone, Resource::Glass, Resource::Papyrus}, // 费用：1木材+1石材+1玻璃+1纸莎草
      [](Game* game, Player* player) {
          // 建造时：每拥有1张灰色制成品卡获得1枚硬币
          int manufacturedCount = player->getManufacturedGoodBuildingCount();
          player->addGold(manufacturedCount * 1);
          // 终局时：每张灰色制成品卡计1分
          player->addGuildVictoryPointBonus(GuildBonusType::ManufacturedPerCard, 1);
      },
      0,
      {EffectIcon::GrayManufactured, EffectIcon::Coin} // 效果图标：灰色制成品卡+1硬币
  ));

  // 3. 建造者公会（Builders Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Builders Guild",
      CardColor::Purple,
      {Resource::Stone, Resource::Clay, Resource::Wood, Resource::Glass}, // 费用：1石材+1砖（粘土）+1木材+1玻璃
      [](Game* game, Player* player) {
          // 终局时：每完成1个奇迹阶段获得2分
          player->addGuildVictoryPointBonus(GuildBonusType::WonderStagePer2Points, 2);
      },
      0,
      {EffectIcon::Pyramid, EffectIcon::TwoPoints} // 效果图标：金字塔（奇迹）+2分
  ));

  // 4. 行政官公会（Magistrates Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Magistrates Guild",
      CardColor::Purple,
      {Resource::Wood, Resource::Clay, Resource::Papyrus, Resource::Cloth}, // 费用：1木材+1砖（粘土）+1纸莎草+1布料
      [](Game* game, Player* player) {
          // 建造时：每拥有1张蓝色民用卡获得1枚硬币
          int civilianCount = player->getCivilianBuildingCount();
          player->addGold(civilianCount * 1);
          // 终局时：每张蓝色民用卡计1分
          player->addGuildVictoryPointBonus(GuildBonusType::CivilianPerCard, 1);
      },
      0,
      {EffectIcon::BlueCivilian, EffectIcon::Coin} // 效果图标：蓝色民用卡+1硬币
  ));

  // 5. 科学家公会（Scientists Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Scientists Guild",
      CardColor::Purple,
      {Resource::Wood, Resource::Stone, Resource::Papyrus, Resource::Cloth}, // 费用：1木材+1石材+1纸莎草+1布料
      [](Game* game, Player* player) {
          // 建造时：每拥有1张绿色科学卡获得1枚硬币
          int scienceCount = player->getScienceBuildingCount();
          player->addGold(scienceCount * 1);
          // 终局时：每张绿色科学卡计1分
          player->addGuildVictoryPointBonus(GuildBonusType::SciencePerCard, 1);
      },
      0,
      {EffectIcon::GreenScience, EffectIcon::Coin} // 效果图标：绿色科学卡+1硬币
  ));

  // 6. 放债人公会（Moneylenders Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Moneylenders Guild",
      CardColor::Purple,
      {Resource::Stone, Resource::Clay, Resource::Papyrus, Resource::Cloth}, // 费用：1石材+1砖（粘土）+1纸莎草+1布料
      [](Game* game, Player* player) {
          // 每回合开始时获得1枚硬币
          player->addPerTurnGold(1);
          // 终局时：剩余的每枚硬币计1分
          player->addGuildVictoryPointBonus(GuildBonusType::GoldPer1Point, 1);
      },
      0,
      {EffectIcon::GoldCoin, EffectIcon::Coin} // 效果图标：金币+1硬币
  ));

  // 7. 战术家公会（Tacticians Guild）
  guildPool.push_back(CardFactory::createGuildCard(
      "Tacticians Guild",
      CardColor::Purple,
      {Resource::Wood, Resource::Clay, Resource::Glass, Resource::Cloth}, // 费用：1木材+1砖（粘土）+1玻璃+1布料
      [](Game* game, Player* player) {
          // 建造时：每拥有1张红色军事卡获得1枚硬币
          int militaryCount = player->getMilitaryBuildingCount();
          player->addGold(militaryCount * 1);
          // 终局时：每张红色军事卡计1分
          player->addGuildVictoryPointBonus(GuildBonusType::MilitaryPerCard, 1);
      },
      0,
      {EffectIcon::RedMilitary, EffectIcon::Coin} // 效果图标：红色军事卡+1硬币
  ));

  return guildPool;
}
//12.15 奇迹列表
std::vector<Card*> Deck::getWonders() {
  std::vector<Card*> wonders;

  // 1. 亚壁古道
  wonders.push_back(CardFactory::createWonderCard(
      "The Appian Way",
      {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Gold, Resource::Gold, Resource::Gold}, // 3石头+3金币
      [](Game* game, Player* player) {
          // 效果：获得3金币
          player->addGold(3);
          // 对手失去3金币
          for (auto& opp : game->getOpponents(player)) {
              opp->subtractGold(3);
          }
          // 立即再执行一个回合
          game->setPlayAgain(true);
      },
      3 // 胜利分数
  ));

  // 2. 大竞技场
  wonders.push_back(CardFactory::createWonderCard(
      "Circus Maximus",
      {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Clay, Resource::Clay, Resource::Clay}, // 3石头+3黏土
      [](Game* game, Player* player) {
          // 效果：弃置对手一张灰色卡牌（制成品）
          game->promptAbandonOpponentCard(player, CardType::ManufacturedGood);
          // 提供1个盾牌
          player->addShield(1);
      },
      3 // 胜利分数
  ));

  // 3. 巨像
  wonders.push_back(CardFactory::createWonderCard(
      "The Colossus",
      {Resource::Clay, Resource::Clay, Resource::Clay, Resource::Clay, Resource::Clay, Resource::Glass}, // 5黏土+1玻璃
      [](Game* game, Player* player) {
          // 效果：提供2个盾牌
          player->addShield(2);
      },
      3 // 胜利分数
  ));

  // 4. 大图书馆
  wonders.push_back(CardFactory::createWonderCard(
      "The Great Library",
      {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Glass, Resource::Glass, Resource::Glass,
       Resource::Papyrus, Resource::Papyrus, Resource::Papyrus}, // 3石头+3玻璃+3莎草纸
      [](Game* game, Player* player) {
          // 效果：从初始移除的5个进程标志中抽3选1生效
          game->triggerGreatLibraryEffect(player);
      },
      4 // 胜利分数
  ));

  // 5. 大灯塔
  wonders.push_back(CardFactory::createWonderCard(
      "The Great Lighthouse",
      {Resource::Clay, Resource::Clay, Resource::Clay, Resource::Wood, Resource::Wood, Resource::Wood}, // 3黏土+3木头
      [](Game* game, Player* player) {
          // 效果：每回合任选1个基础资源（石头/黏土/木头）生产
          player->addPerTurnResourceChoice({Resource::Stone, Resource::Clay, Resource::Wood});
      },
      4 // 胜利分数
  ));

  // 6. 空中花园
  wonders.push_back(CardFactory::createWonderCard(
      "The Hanging Gardens",
      {Resource::Clay, Resource::Clay, Resource::Clay, Resource::Wood, Resource::Wood, Resource::Wood,
       Resource::Gold, Resource::Gold, Resource::Gold}, // 3黏土+3木头+3金币
      [](Game* game, Player* player) {
          // 效果：获得6金币
          player->addGold(6);
          // 立即再执行一个回合
          game->setPlayAgain(true);
      },
      3 // 胜利分数
  ));

  // 7. 摩索拉斯陵墓
  wonders.push_back(CardFactory::createWonderCard(
      "The Mausoleum",
      {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Papyrus, Resource::Papyrus, Resource::Papyrus,
       Resource::Gold, Resource::Gold, Resource::Gold}, // 3石头+3莎草纸+3金币
      [](Game* game, Player* player) {
          // 效果：回收弃牌堆选1张免费建造
          game->promptChooseDiscardedCardToBuildFree(player);
      },
      2 // 胜利分数
  ));

  // 8. 比雷埃夫斯港
  wonders.push_back(CardFactory::createWonderCard(
      "Piraeus",
      {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Glass, Resource::Glass, Resource::Glass}, // 3木头+3玻璃
      [](Game* game, Player* player) {
          // 效果：每回合任选1个高级资源（玻璃/莎草纸）生产
          player->addPerTurnResourceChoice({Resource::Glass, Resource::Papyrus});
          // 立即再执行一个回合
          game->setPlayAgain(true);
      },
      2 // 胜利分数
  ));

  // 9. 金字塔
  wonders.push_back(CardFactory::createWonderCard(
      "The Pyramids",
      {Resource::Stone, Resource::Stone, Resource::Stone, Resource::Stone, Resource::Stone,
       Resource::Wood, Resource::Wood, Resource::Wood, Resource::Wood, Resource::Wood}, // 5石头+5木头
      [](Game* game, Player* player) {
          // 无特殊效果
      },
      9 // 胜利分数
  ));

  // 10. 狮身人面像
  wonders.push_back(CardFactory::createWonderCard(
      "The Sphinx",
      {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Papyrus, Resource::Papyrus, Resource::Papyrus}, // 3木头+3莎草纸
      [](Game* game, Player* player) {
          // 效果：立即再执行一个回合
          game->setPlayAgain(true);
      },
      6 // 胜利分数
  ));

  // 11. 宙斯神像
  wonders.push_back(CardFactory::createWonderCard(
      "The Statue of Zeus",
      {Resource::Wood, Resource::Wood, Resource::Wood, Resource::Stone, Resource::Stone, Resource::Stone}, // 3木头+3石头
      [](Game* game, Player* player) {
          // 效果：弃置对手一张棕色卡牌（原材料）
          game->promptAbandonOpponentCard(player, CardType::RawMaterial);
          // 提供1个盾牌
          player->addShield(1);
      },
      3 // 胜利分数
  ));

  // 12. 阿尔忒弥斯神庙
  wonders.push_back(CardFactory::createWonderCard(
      "The Temple of Artemis",
      {Resource::Clay, Resource::Clay, Resource::Clay, Resource::Papyrus, Resource::Papyrus, Resource::Papyrus}, // 3黏土+3莎草纸
      [](Game* game, Player* player) {
          // 效果：获得12金币
          player->addGold(12);
          // 立即再执行一个回合
          game->setPlayAgain(true);
      },
      0 // 胜利分数（规则补充）
  ));

  return wonders;
}