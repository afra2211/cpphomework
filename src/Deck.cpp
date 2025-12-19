#include "Deck.h"
#include "CardFactory.h"
#include <algorithm>   
#include <chrono>      // 用于获取时间种子
#include <random>      // 随机数生成

// 辅助函数：简化卡牌创建（构建器模式）
// 参数说明：名称、类型、金币成本、资源成本、产出资源、胜利点数、盾牌、科学符号、金币
Card createCard(std::string name, CardType type, int costCoins,
                std::map<ResourceType, int> costRes,
                std::map<ResourceType, int> prodRes, int vp, int shields,
                std::vector<ScienceSymbol> sci, int coins, bool playAgain = false, bool resourceRebate = false) {
  Cost c;
  c.coins = costCoins;         // 设置金币成本
  c.resources = costRes;       // 设置资源成本
  
  Effect e;
  e.resourcesProduced = prodRes; // 设置产出资源
  e.victoryPoints = vp;         // 设置胜利点数
  e.militaryShields = shields;  // 设置军事盾牌
  e.scienceSymbols = sci;       // 设置科学符号
  e.coins = coins;              // 设置金币产出

  e.playAgain = playAgain;
  e.resourceRebate = resourceRebate;

  return Card(name, type, c, e); // 创建并返回卡牌
}

// 时代1卡池
std::vector<Card*> Deck::getAge1Deck() {
  std::vector<Card*> age1Cards;

  // 1. 棕色 - 原材料卡（6张）
  // 伐木场：产出木头，无成本
  age1Cards.push_back(CardFactory::getInstance().createRawMaterialCard(
      "LUMBER YARD",
      0,
      {},
      {{ResourceType::WOOD, 1}},
      0
  ).release());
  // 伐木营地：产出木头，无成本
  age1Cards.push_back(CardFactory::getInstance().createRawMaterialCard(
      "LOGGING CAMP",
      0,
      {},
      {{ResourceType::WOOD, 1}},
      0
  ).release());
  // 粘土池：产出粘土，无成本
  age1Cards.push_back(CardFactory::getInstance().createRawMaterialCard(
      "CLAY POOL",
      0,
      {},
      {{ResourceType::CLAY, 1}},
      0
  ).release());
  // 粘土矿：产出粘土，无成本
  age1Cards.push_back(CardFactory::getInstance().createRawMaterialCard(
      "CLAY PIT",
      0,
      {},
      {{ResourceType::CLAY, 1}},
      0
  ).release());
  // 采石场：产出石头，无成本
  age1Cards.push_back(CardFactory::getInstance().createRawMaterialCard(
      "QUARRY",
      0,
      {},
      {{ResourceType::STONE, 1}},
      0
  ).release());
  // 石矿：产出石头，无成本
  age1Cards.push_back(CardFactory::getInstance().createRawMaterialCard(
      "STONE PIT",
      0,
      {},
      {{ResourceType::STONE, 1}},
      0
  ).release());

  // 2. 灰色 - 制成品卡（2张）
  // 玻璃厂：产出玻璃，成本1木头+1粘土
  age1Cards.push_back(CardFactory::getInstance().createManufacturedGoodCard(
      "GLASSWORKS",
      0,
      {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}},
      {{ResourceType::GLASS, 1}},
      1
  ).release());
  // 印刷厂：产出莎草纸，成本1木头+1石头
  age1Cards.push_back(CardFactory::getInstance().createManufacturedGoodCard(
      "PRESS",
      0,
      {{ResourceType::WOOD, 1}, {ResourceType::STONE, 1}},
      {{ResourceType::PAPER, 1}},
      1
  ).release());

  // 3. 红色 - 军事建筑卡（4张）
  // 守卫塔：1个军事盾牌，成本1木头+1石头
  Effect guardTowerEffect;
  guardTowerEffect.militaryShields = 1;
  guardTowerEffect.victoryPoints = 1;
  age1Cards.push_back(new Card(
      "GUARD TOWER",
      CardType::MILITARY,
      {0, {{ResourceType::WOOD, 1}, {ResourceType::STONE, 1}}},
      guardTowerEffect,
      "",
      ""
  ));
  // 马厩：军事效果（骑兵标识），成本1木头+1粘土
  Effect stableEffect;
  stableEffect.militaryShields = 1;
  stableEffect.victoryPoints = 1;
  age1Cards.push_back(new Card(
      "STABLE",
      CardType::MILITARY,
      {0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}},
      stableEffect,
      "",
      ""
  ));
  // 军营：军事效果（剑形标识），成本2石头+1粘土
  Effect garrisonEffect;
  garrisonEffect.militaryShields = 1;
  garrisonEffect.victoryPoints = 2;
  age1Cards.push_back(new Card(
      "GARRISON",
      CardType::MILITARY,
      {0, {{ResourceType::STONE, 2}, {ResourceType::CLAY, 1}}},
      garrisonEffect,
      "",
      ""
  ));
  // 栅栏：军事效果（盾牌标识），成本3木头
  Effect palisadeEffect;
  effect.militaryShields = 1;
  effect.victoryPoints = 1;
  age1Cards.push_back(new Card(
      "PALISADE",
      CardType::MILITARY,
      {0, {{ResourceType::WOOD, 3}}},
      palisadeEffect,
      "",
      ""
  ));

  // 4. 绿色 - 科学卡（4张）
  // 工坊：三角科学符号，成本1木头+1粘土
  Effect workshopEffect;
  workshopEffect.scienceSymbols = {ScienceSymbol::TRIANGLE};
  age1Cards.push_back(new Card(
      "WORKSHOP",
      CardType::SCIENTIFIC,
      {0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}},
      workshopEffect,
      "",
      ""
  ));
  // 药剂师：烧瓶科学符号，成本1粘土+1石头
  Effect apothecaryEffect;
  apothecaryEffect.scienceSymbols = {ScienceSymbol::FLASK};
  age1Cards.push_back(new Card(
      "APOTHECARY",
      CardType::SCIENTIFIC,
      {0, {{ResourceType::CLAY, 1}, {ResourceType::STONE, 1}}},
      apothecaryEffect,
      "",
      ""
  ));
  // 文书院：羽毛笔科学符号，成本1木头+1莎草纸
  Effect scriptoriumEffect;
  scriptoriumEffect.scienceSymbols = {ScienceSymbol::QUILL};
  age1Cards.push_back(new Card(
      "SCRIPTORIUM",
      CardType::SCIENTIFIC,
      {0, {{ResourceType::WOOD, 1}, {ResourceType::PAPER, 1}}},
      scriptoriumEffect,
      "",
      ""
  ));
  // 药剂师（带齿轮联动）：烧瓶科学符号，成本1玻璃+1粘土
  Effect pharmacistEffect;
  pharmacistEffect.scienceSymbols = {ScienceSymbol::FLASK};
  age1Cards.push_back(new Card(
      "PHARMACIST",
      CardType::SCIENTIFIC,
      {0, {{ResourceType::GLASS, 1}, {ResourceType::CLAY, 1}}},
      pharmacistEffect,
      "",
      ""
  ));

  // 5. 黄色 - 商业建筑卡（4张）
  // 石储备库：石头资源+1/折扣，成本1石头
  Effect stoneReserveEffect;
  stoneReserveEffect.resourcesProduced = {{ResourceType::STONE, 1}};
  age1Cards.push_back(new Card(
      "STONE RESERVE",
      CardType::COMMERCIAL,
      {0, {{ResourceType::STONE, 1}}},
      stoneReserveEffect,
      "",
      ""
  ));
  // 粘土储备库：粘土资源+1/折扣，成本1粘土
  Effect clayReserveEffect;
  clayReserveEffect.resourcesProduced = {{ResourceType::CLAY, 1}};
  age1Cards.push_back(new Card(
      "CLAY RESERVE",
      CardType::COMMERCIAL,
      {0, {{ResourceType::CLAY, 1}}},
      clayReserveEffect,
      "",
      ""
  ));
  // 木材储备库：木头资源+1/折扣，成本1木头
  Effect woodReserveEffect;
  woodReserveEffect.resourcesProduced = {{ResourceType::WOOD, 1}};
  age1Cards.push_back(new Card(
      "WOOD RESERVE",
      CardType::COMMERCIAL,
      {0, {{ResourceType::WOOD, 1}}},
      woodReserveEffect,
      "",
      ""
  ));
  // 酒馆：产出4金币，成本1木头+1粘土
  Effect tavernEffect;
  tavernEffect.coins = 4;
  tavernEffect.victoryPoints = 1;
  age1Cards.push_back(new Card(
      "TAVERN",
      CardType::COMMERCIAL,
      {0, {{ResourceType::WOOD, 1}, {ResourceType::CLAY, 1}}},
      tavernEffect,
      "",
      ""
  ));

  // 6. 蓝色 - 民用建筑卡（3张）
  // 剧院：胜利分3，成本2石头+1粘土
  Effect theaterEffect;
  theaterEffect.victoryPoints = 3;
  age1Cards.push_back(new Card(
      "THEATER",
      CardType::CIVILIAN,
      {0, {{ResourceType::STONE, 2}, {ResourceType::CLAY, 1}}},
      theaterEffect,
      "",
      ""
  ));
  // 祭坛：胜利分3，成本2粘土+1石头
  Effect altarEffect;
  altarEffect.victoryPoints = 3;
  age1Cards.push_back(new Card(
      "ALTAR",
      CardType::CIVILIAN,
      {0, {{ResourceType::CLAY, 2}, {ResourceType::STONE, 1}}},
      altarEffect,
      "",
      ""
  ));
  // 浴场：胜利分3，成本1木头+1石头+1粘土
  Effect bathsEffect;
  bathsEffect.victoryPoints = 3;
  age1Cards.push_back(new Card(
      "BATHS",
      CardType::CIVILIAN,
      {0, {{ResourceType::WOOD, 1}, {ResourceType::STONE, 1}, {ResourceType::CLAY, 1}}},
      bathsEffect,
      "",
      ""
  ));

  assert(age1Cards.size() == 23 && "Age 1 deck must have exactly 23 cards");
  return age1Cards;
}

std::vector<Wonder> Deck::createWonders() {
    std::vector<Wonder> wonders;
    auto uniqueWonders = CardFactory::getInstance().createAllWonders();
    for (auto& wonder : uniqueWonders) {
        wonders.push_back(*wonder);
    }
    return wonders;
}
