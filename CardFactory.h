#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "Card.h"
#include "Resources.h"
#include "Wonder.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <functional> // 新增补充函数对象头文件

class Game;
class Player;
class Board; // 声明前置类，避免头文件依赖

class CardFactory {
public:
  // 单例模式访问点
  static CardFactory &getInstance();

  // 各类卡牌的工厂方法
  // 修正原材料卡参数：增加CardColor和hasChain参数
  std::unique_ptr<Card>
  createRawMaterialCard(const std::string &name, CardColor color,
                       const std::map<ResourceType, int> &costRes,
                       Resource prodRes, int coins = 0, bool hasChain = false);

  // 修正制造品卡参数：增加CardColor参数
  std::unique_ptr<Card>
  createManufacturedGoodCard(const std::string &name, CardColor color,
                             const std::map<ResourceType, int> &costRes,
                             Resource prodRes, int coins = 0);

  // 新增军事建筑卡声明（匹配cpp实现）
  std::unique_ptr<Card>
  createMilitaryBuildingCard(const std::string &name, CardColor color,
                             const std::map<ResourceType, int> &costRes,
                             std::function<void(Game*, Player*)> effect,
                             int shields);

  // 新增科学卡声明（匹配cpp实现，注意函数名和参数）
  std::unique_ptr<Card>
  createScienceCard(const std::string &name, CardColor color,
                    const std::map<ResourceType, int> &costRes,
                    ScienceSymbol symbol,
                    std::function<void(Game*, Player*)> effect, int vp);

  // 新增商业建筑卡声明（匹配cpp实现）
  std::unique_ptr<Card>
  createCommercialBuildingCard(const std::string &name, CardColor color,
                               const std::map<ResourceType, int> &costRes,
                               std::function<void(Game*, Player*)> effect,
                               int vp);

  // 新增平民建筑卡声明（匹配cpp实现）
  std::unique_ptr<Card>
  createCivilianBuildingCard(const std::string &name, CardColor color,
                             const std::map<ResourceType, int> &costRes,
                             std::function<void(Game*, Player*)> effect,
                             int vp);

  // 公会卡保持原有声明
  std::unique_ptr<Card>
  createGuildCard(const std::string &name,
                  const std::map<ResourceType, int> &costRes,
                  int vpConditionType);

  // 奇迹创建接口
  std::unique_ptr<Wonder>
  createWonder(const std::string &name, int costCoins,
               const std::map<ResourceType, int> &costRes,
               const Effect &effect);

  // 修正牌堆生成函数返回值：从unique_ptr向量改为原始指针向量
  std::vector<Card*> createAge1Deck();
  std::vector<Card*> createAge2Deck();
  std::vector<Card*> createAge3Deck();

  // 移除类内createAllWonders声明（改为全局函数）

private:
  // 私有构造函数（单例模式）
  CardFactory() = default;
  
  std::vector<Card*> createGuildCardPool(); // 新增公会卡池创建函数声明
  // 禁止拷贝和赋值
  CardFactory(const CardFactory &) = delete;
  CardFactory &operator=(const CardFactory &) = delete;

  // 通用内部辅助函数
  std::unique_ptr<Card> createCardInternal(const std::string &name,
                                           CardType type, Cost cost,
                                           Effect effect,
                                           const std::string &chainSymbol,
                                           const std::string &chainTarget);
};

// 声明全局函数createAllWonders（匹配cpp实现）
std::vector<std::unique_ptr<Wonder>> createAllWonders();

#endif // CARDFACTORY_H