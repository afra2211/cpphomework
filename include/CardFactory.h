#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "Card.h"
#include "Resources.h"
#include "Wonder.h"
#include <map>
#include <memory> // 必须包含，用于 std::unique_ptr
#include <string>
#include <vector>

class CardFactory {
public:
  // === 1. 单例模式访问点 ===
  // 确保全局只有一个工厂实例，符合设计模式要求
  static CardFactory &getInstance();

  // === 2. 各类卡牌的工厂方法 (Factory Methods) ===
  // 返回 std::unique_ptr，表示将对象的所有权移交给调用者

  // 棕色卡牌 (原料)
  std::unique_ptr<Card>
  createRawMaterialCard(const std::string &name, int costCoins,
                        const std::map<ResourceType, int> &costRes,
                        const std::map<ResourceType, int> &prodRes,
                        int coins = 0);

  // 灰色卡牌 (制造品)
  std::unique_ptr<Card>
  createManufacturedGoodCard(const std::string &name, int costCoins,
                             const std::map<ResourceType, int> &costRes,
                             const std::map<ResourceType, int> &prodRes,
                             int coins = 0);

  // 蓝色卡牌 (平民建筑) - 重点是 VP
  std::unique_ptr<Card>
  createCivilianCard(const std::string &name, int costCoins,
                     const std::map<ResourceType, int> &costRes, int vp,
                     const std::string &chainSymbol = "",
                     const std::string &chainTarget = "");

  // 红色卡牌 (军事建筑) - 重点是 shields
  std::unique_ptr<Card>
  createMilitaryCard(const std::string &name, int costCoins,
                     const std::map<ResourceType, int> &costRes, int shields,
                     const std::string &chainSymbol = "",
                     const std::string &chainTarget = "");

  // 绿色卡牌 (科学建筑) - 重点是 science symbol
  std::unique_ptr<Card> createScientificCard(
      const std::string &name, int costCoins,
      const std::map<ResourceType, int> &costRes, int vp, ScienceSymbol symbol,
      const std::string &chainSymbol = "", const std::string &chainTarget = "");

  // 黄色卡牌 (商业建筑) - 重点是 coins, vp, 和特殊效果 (trading)
  std::unique_ptr<Card> createCommercialCard(
      const std::string &name, int costCoins,
      const std::map<ResourceType, int> &costRes, int vp, int immediateCoins,
      const std::map<ResourceType, int> &prodRes = {},
      const std::string &chainSymbol = "", const std::string &chainTarget = "");

  // 紫色卡牌 (公会) - 重点是复杂的 VP 计算 (这里简化为固定VP，后续可扩展 Effect
  // 结构支持策略)
  std::unique_ptr<Card>
  createGuildCard(const std::string &name,
                  const std::map<ResourceType, int> &costRes,
                  int vpConditionType); // 示例：用 int 代表不同的公会得分策略

  // === 3. 奇迹创建接口 ===
  // 将 Wonder 的创建也封装在这里
  std::unique_ptr<Wonder>
  createWonder(const std::string &name, int costCoins,
               const std::map<ResourceType, int> &costRes,
               const Effect &effect);

  // === 4. 牌堆生成接口 (Deck Generation) ===
  // 返回智能指针的向量，这是现代 C++ 处理多态对象集合的标准做法
  std::vector<std::unique_ptr<Card>> createAge1Deck();
  std::vector<std::unique_ptr<Card>> createAge2Deck();
  std::vector<std::unique_ptr<Card>> createAge3Deck();

  // 生成所有 12 个奇迹
  std::vector<std::unique_ptr<Wonder>> createAllWonders();

private:
  // 私有构造函数（单例模式）
  CardFactory() = default;

  // 禁止拷贝和赋值（防止意外复制单例）
  CardFactory(const CardFactory &) = delete;
  CardFactory &operator=(const CardFactory &) = delete;

  // 通用内部辅助函数：负责组装 Cost 和 Effect 结构体并 new 对象
  std::unique_ptr<Card> createCardInternal(const std::string &name,
                                           CardType type, Cost cost,
                                           Effect effect,
                                           const std::string &chainSymbol,
                                           const std::string &chainTarget);
};

#endif // CARDFACTORY_H