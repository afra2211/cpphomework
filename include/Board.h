#ifndef BOARD_H
#define BOARD_H

#include "Card.h"
#include "Wonder.h"
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

class Player;

struct MilitaryToken {
    int threshold;          // 触发位置（距离对手首都的格数）
    int coinPenalty;        // 罚款/奖励金币（对手扣除）
    int victoryPoints = 0;  // 触发方获得的胜利点
    int extraShields = 0;   // 额外盾牌（继续沿当前方向移动）
    bool removed{false};
};

// 卡牌槽位结构体：表示金字塔布局中的每个卡牌位置
struct CardSlot {
  Card card;                  // 卡牌对象
  bool isFaceUp;              // 是否正面朝上（可见）
  bool isTaken;               // 是否已被玩家拿走
  std::vector<int> coveredBy; // 覆盖此卡牌的槽位索引（依赖关系）
};

// === 新增：进步标记类型枚举 ===
// 进步标记是游戏中的重要元素，通过科学符号配对获得，提供特殊能力
enum class ProgressTokenType {
  AGRICULTURE,  // 农业：立即获得6金币，终局+4胜利点
  ARCHITECTURE, // 建筑学：未来建造奇迹时资源费用减少2
  ECONOMY,      // 经济学：对手购买资源时，你获得等额金币
  LAW,          // 法学：提供独特的第7种科学符号，用于科技胜利
  MASONRY,      // 砌筑术：未来建造蓝色建筑时资源费用减少2
  MATHEMATICS,  // 数学：终局时每个进步标记+3胜利点
  PHILOSOPHY,   // 哲学：终局时+7胜利点
  STRATEGY,     // 战略：未来建造红色建筑时额外获得+1盾牌
  THEOLOGY,     // 神学：未来建造的奇迹自动获得"再来一回合"效果
  URBANISM      // 城市规划：立即获得6金币，每次连锁建造获得4金币
};

// === 新增：进步标记类 ===
// 代表游戏中的进步标记，每个标记有独特的效果
class ProgressToken {
public:
  // 构造函数：创建指定类型的进步标记
  ProgressToken(ProgressTokenType type = ProgressTokenType::AGRICULTURE);

  // 获取标记类型
  ProgressTokenType getType() const;
  // 获取标记名称（英文）
  std::string getName() const;
  // 获取标记效果描述
  std::string getDescription() const;
  // 获取基础胜利点数（部分标记有固定分数）
  int getBaseVictoryPoints() const;

  // 应用标记的立即效果到玩家
  void applyEffect(class Player *player);

private:
  ProgressTokenType type; // 标记类型
};

// 游戏板类：管理卡牌布局、军事状态、奇迹和进步标记
class Board {
public:
  static constexpr int MILITARY_START_POSITION = 0;

  // 构造函数：初始化游戏板状态
  Board();

  // === 核心游戏功能 ===

  /**
   * @brief 设置指定时代的卡牌布局
   * @param age 时代编号（1, 2, 3）
   * @param deck 该时代的卡牌堆
   * @note 已实现：R2.1 Age卡牌布局功能
   * @note 时代1和3使用正金字塔布局，时代2使用倒金字塔布局
   */
  void setupAge(int age, std::vector<Card> deck,
                const std::vector<Card> &removed = {});

  /**
   * @brief 获取当前金字塔布局的所有卡牌槽位
   * @return 所有卡牌槽位的只读引用
   */
  const std::vector<CardSlot> &getPyramid() const;

  /**
   * @brief 检查指定索引的卡牌是否可访问
   * @param index 卡牌在金字塔中的索引
   * @return true如果卡牌未被覆盖且未被拿走，否则false
   * @note 已实现：R2.2 可访问卡牌检查
   * @note 检查卡牌是否被其他卡牌覆盖以及是否已被拿走
   */
  bool isCardAccessible(int index) const;

  /**
   * @brief 玩家拿走指定卡牌
   * @param index 要拿走的卡牌索引
   * @return 被拿走的卡牌对象
   * @throws std::runtime_error 如果卡牌不可访问
   * @note 拿走后会自动触发revealAvailableCards翻开下层卡牌
   */
  Card takeCard(int index);

  /**
   * @brief 翻开可用的卡牌（当覆盖卡被移除时）
   * @note 已实现：R2.3 翻牌机制
   * @note 遍历所有卡牌，检查是否因上层卡牌被拿走而变得可访问
   */
  void revealAvailableCards();

  // === 军事系统功能 ===

  /**
   * @brief 获取军事标记当前位置
   * @return 军事位置（-9到9，0为中央，负值偏向P1，正值偏向P2）
   * @note 军事位置影响终局军事分数计算
   */
  int getMilitaryPosition() const;

  /**
   * @brief 移动军事标记
   * @param amount 移动距离（正数向P2移动，负数向P1移动）
   * @note 军事移动由红色卡牌和奇迹的盾牌数量决定
   */
  void moveMilitary(int amount, Player *mover = nullptr,
                    Player *opponent = nullptr);

  const std::vector<MilitaryToken> &getMilitaryTokens() const;

  const std::vector<MilitaryToken> &getMilitaryTokens() const;

  // === 奇迹系统功能 ===

  /**
   * @brief 获取当前可用的奇迹列表
   * @return 可用奇迹的只读引用
   */
  const std::vector<Wonder *> &getAvailableWonders() const;

  /**
   * @brief 设置可用的奇迹列表
   * @param wonders 奇迹指针向量
   */
  void setAvailableWonders(const std::vector<Wonder *> &wonders);

  /**
   * @brief 玩家拿走指定奇迹
   * @param index 奇迹索引
   * @return 被拿走的奇迹指针，如果索引无效返回nullptr
   */
  Wonder *takeWonder(int index);

  /**
   * @brief 当第七座奇观建成后移除剩余的一座未建奇观
   */
  void removeFirstUnbuiltWonder();

  // === 新增：进步标记系统接口 ===

  /**
   * @brief 设置进步标记：从10个标记中随机选择5个放置到游戏板
   * @note 符合规则：游戏开始时从10个进步标记中随机抽取5个放置
   * @note 标记放置后可供玩家通过科学符号配对获取
   */
  void setupProgressTokens();

  /**
   * @brief 获取当前可用的进步标记列表
   * @return 可用进步标记列表的只读引用
   * @note 返回的是游戏板上实际放置的5个进步标记
   */
  const std::vector<ProgressToken> &getAvailableProgressTokens() const;

  /**
   * @brief 玩家拿走指定进步标记
   * @param index 标记索引（0-4对应游戏板上的5个位置）
   * @return 被拿走的进步标记对象
   * @throws std::runtime_error 如果索引无效或标记不可用
   * @note 拿走后会从可用标记列表中移除该标记
   */
  ProgressToken takeProgressToken(int index);

  /**
   * @brief 检查玩家是否可以获取进步标记
   * @param symbol 科学符号类型
   * @return true如果满足获取条件（拥有2个相同的科学符号），否则false
   * @note 获取条件：收集到2个相同的科学符号且游戏板上有可用标记
   */
  bool canTakeProgressToken(ScienceSymbol symbol) const;

  /**
   * @brief 科学符号配对事件处理接口
   * @param player 触发配对的玩家
   * @return 当玩家收集到6种不同科学符号时返回true
   * @note 会在玩家拥有2张相同科学符号时让其从公共5枚中拿取一枚进步标记
   */
  bool onSciencePair(Player &player);

private:
  // === 核心游戏数据成员 ===
  std::vector<CardSlot> pyramid;          // 卡牌金字塔布局（时代1-3的不同结构）
  int militaryPosition;                   // 军事标记位置（-9到9，0为中央）
  std::vector<MilitaryToken> militaryTokens; // 军事区段 token 配置
  std::vector<Wonder *> availableWonders; // 当前可用的奇迹列表

  // === 新增：进步标记系统数据成员 ===
  std::vector<ProgressToken>
      availableProgressTokens; // 游戏板上可用的进步标记（5个）
  std::map<ScienceSymbol, int>
      scienceSymbolCounts;                      // 科学符号计数（用于配对检查）
  std::vector<ProgressToken> allProgressTokens; // 所有进步标记池（10个）
  std::vector<ProgressToken> removedProgressTokens; // 被移除的进步标记

  // === 时代布局设置私有方法 ===

  /**
   * @brief 设置时代1的卡牌布局（正金字塔）
   * @param deck 时代1的卡牌堆
   * @note 布局结构：2-3-4-5-6行的正金字塔
   */
  void setupAge1(std::vector<Card> &deck);

  /**
   * @brief 设置时代2的卡牌布局（倒金字塔）
   * @param deck 时代2的卡牌堆
   * @note 布局结构：6-5-4-3-2行的倒金字塔
   */
  void setupAge2(std::vector<Card> &deck);

  /**
   * @brief 设置时代3的卡牌布局（同时代1）
   * @param deck 时代3的卡牌堆
   * @note 布局结构：与时代1相同的正金字塔
   */
  void setupAge3(std::vector<Card> &deck);

  // === 新增：进步标记系统辅助方法 ===

  /**
   * @brief 初始化所有10种进步标记到标记池
   * @note 游戏开始时调用，创建完整的进步标记集合
   */
  void initializeAllProgressTokens();

  /**
   * @brief 随机打乱进步标记顺序
   * @note 使用梅森旋转算法确保随机性，用于setupProgressTokens
   */
  void shuffleProgressTokens();
};

#endif // BOARD_H