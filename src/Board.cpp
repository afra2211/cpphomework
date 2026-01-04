#include "Board.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>

/**
 * @brief Board类构造函数
 * @note 初始化军事位置为0（中央），并初始化所有进步标记
 */
Board::Board() : militaryPosition(MILITARY_START_POSITION) {
  // 初始化军事区段 token 配置（两侧对称），默认未触发
  militaryTokens = {{3, 2, 0, 0, false},  {6, 5, 0, 0, false},
                    {9, 10, 0, 0, false}, {-3, 2, 0, 0, false},
                    {-6, 5, 0, 0, false}, {-9, 10, 0, 0, false}};

  // === 新增：初始化进步标记系统 ===
  initializeAllProgressTokens();
}

/**
 * @brief 设置指定时代的卡牌布局
 * @param age 时代编号（1、2、3）
 * @param deck 该时代的卡牌堆（应包含20张卡牌）
 * @note 根据时代不同采用不同的金字塔布局结构
 */
void Board::setupAge(int age, std::vector<Card> deck,
                     const std::vector<Card> &removed) {
  pyramid.clear(); // 清空现有金字塔布局

  // 预留处理：如果需要展示/记录被移除的卡牌，可在此使用 removed
  (void)removed;

  // 根据时代选择对应的布局设置函数
  if (age == 1)
    setupAge1(deck); // 时代1：正金字塔布局（2-3-4-5-6行）
  else if (age == 2)
    setupAge2(deck); // 时代2：倒金字塔布局（6-5-4-3-2行）
  else if (age == 3)
    setupAge3(deck); // 时代3：与时代1相同的正金字塔布局

  // 设置完成后翻开初始可访问的卡牌
  revealAvailableCards();
}

/**
 * @brief 获取当前金字塔布局的所有卡牌槽位
 * @return 卡牌槽位向量的只读引用
 */
const std::map<int, CardSlot> &Board::getPyramid() const { return pyramid; }

/**
 * @brief 检查指定卡牌是否可被玩家访问
 * @param index 卡牌在金字塔中的索引
 * @return true如果卡牌可访问，否则false
 * @note 可访问条件：1.索引有效 2.卡牌未被拿走 3.所有覆盖此卡的卡牌已被拿走
 */
bool Board::isCardAccessible(int index) const {
  auto it = pyramid.find(index);
  if (it == pyramid.end() || it->second.isTaken)
    return false;

  for (int coverIndex : it->second.coveredBy) {
    auto coverIt = pyramid.find(coverIndex);
    if (coverIt != pyramid.end() && !coverIt->second.isTaken) {
      return false;
    }
  }
  return true;
}

/**
 * @brief 玩家拿走指定卡牌
 * @param index 要拿走的卡牌索引
 * @return 被拿走的卡牌对象
 * @throws std::runtime_error 如果卡牌不可访问
 * @note 拿走后会自动翻开因此变得可访问的下层卡牌
 */
Card Board::takeCard(int index) {
  // 安全检查：确保卡牌可访问
  if (!isCardAccessible(index)) {
    throw std::runtime_error("Card is not accessible");
  }

  // 标记卡牌为已拿走状态
  pyramid[index].isTaken = true;

  // 自动翻开因这张卡被拿走而变得可访问的下层卡牌
  revealAvailableCards();

  // 返回被拿走的卡牌对象
  return pyramid[index].card;
}

/**
 * @brief 翻开所有可访问的卡牌
 * @note 遍历金字塔中的所有卡牌，将可访问的背面卡牌翻转为正面
 */
void Board::revealAvailableCards() {
  for (size_t i = 0; i < pyramid.size(); ++i) {
    // 检查条件：卡牌未被拿走且当前是背面朝上
    if (!pyramid[i].isTaken && !pyramid[i].isFaceUp) {
      // 如果卡牌变得可访问，则将其翻开（正面朝上）
      if (isCardAccessible(i)) {
        pyramid[i].isFaceUp = true;
      }
    }
  }
}

/**
 * @brief 获取军事标记当前位置
 * @return 军事位置值（-9到9，0为中央，负值偏向玩家1，正值偏向玩家2）
 */
int Board::getMilitaryPosition() const { return militaryPosition; }

/**
 * @brief 移动军事标记
 * @param amount 移动距离（正数向玩家2移动，负数向玩家1移动）
 * @note 军事移动由红色卡牌和奇迹的盾牌数量决定，边界检查和胜利判定由Game类处理
 */
void Board::moveMilitary(int amount, Player *mover, Player *opponent) {
  if (amount == 0) {
    return;
  }

  int original = militaryPosition;
  int target = std::clamp(militaryPosition + amount, -9, 9);
  int direction = (target > original) ? 1 : -1;
  int extraMovement = 0;

  for (auto &token : militaryTokens) {
    if (token.removed) {
      continue;
    }

    bool crossed = false;
    if (direction > 0) {
      crossed = token.threshold > original && token.threshold <= target;
    } else {
      crossed = token.threshold < original && token.threshold >= target;
    }

    if (crossed) {
      if (opponent && token.coinPenalty > 0) {
        opponent->removeCoins(token.coinPenalty);
      }
      if (mover && token.victoryPoints > 0) {
        mover->addVictoryPoints(token.victoryPoints);
      }
      if (token.extraShields != 0) {
        extraMovement += token.extraShields * direction;
      }
      token.removed = true;
    }
  }

  militaryPosition = target;

  if (extraMovement != 0) {
    moveMilitary(extraMovement, mover, opponent);
  }
}

const std::vector<MilitaryToken> &Board::getMilitaryTokens() const {
  return militaryTokens;
}

/**
 * @brief 获取当前可用的奇迹列表
 * @return 可用奇迹指针向量的只读引用
 */
const std::vector<Wonder *> &Board::getAvailableWonders() const {
  return availableWonders;
}

/**
 * @brief 设置可用的奇迹列表
 * @param wonders 奇迹指针向量
 */
void Board::setAvailableWonders(const std::vector<Wonder *> &wonders) {
  availableWonders = wonders;
}

/**
 * @brief 玩家拿走指定奇迹
 * @param index 奇迹在可用列表中的索引
 * @return 被拿走的奇迹指针，如果索引无效返回nullptr
 */
Wonder *Board::takeWonder(int index) {
  // 检查索引有效性
  if (index < 0 || index >= static_cast<int>(availableWonders.size()))
    return nullptr;

  // 获取奇迹指针并从可用列表中移除
  Wonder *w = availableWonders[index];
  availableWonders.erase(availableWonders.begin() + index);
  return w;
}

void Board::removeFirstUnbuiltWonder() {
  for (auto it = availableWonders.begin(); it != availableWonders.end(); ++it) {
    if (*it != nullptr && !(*it)->isBuilt()) {
      availableWonders.erase(it);
      break;
    }
  }
}

// === 新增：进步标记系统实现 ===

/**
 * @brief 设置游戏板上的进步标记
 * @note 从10个进步标记中随机选择5个放置到游戏板，供玩家通过科学符号配对获取
 */
void Board::setupProgressTokens() {
  /**
   * 实现进步标记的随机放置逻辑：
   * 1. 清空现有标记
   * 2. 随机打乱所有标记
   * 3. 选择前5个作为可用标记
   * 4. 重置科学符号计数
   */
  availableProgressTokens.clear();

  // 随机打乱进步标记顺序，确保游戏随机性
  shuffleProgressTokens();

  // 从打乱后的标记池中选择前5个放置到游戏板
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(allProgressTokens.begin(), allProgressTokens.end(), g);

  // 4. 将前5个标记放入游戏板
  availableProgressTokens.clear();
  removedProgressTokens.clear(); // Clear previously removed tokens
  for (int i = 0; i < 5 && i < (int)allProgressTokens.size(); ++i) {
    availableProgressTokens.push_back(allProgressTokens[i]);
  }

  // The rest go to removed tokens
  for (size_t i = 5; i < allProgressTokens.size(); ++i) {
    removedProgressTokens.push_back(allProgressTokens[i]);
  }

  // 重置科学符号计数（新游戏开始）
  scienceSymbolCounts.clear();
}

/**
 * @brief 获取游戏板上剩余可用的进步标记
 * @return 进步标记向量的只读引用
 */
const std::vector<ProgressToken> &Board::getAvailableProgressTokens() const {
  return availableProgressTokens;
}

const std::vector<ProgressToken> &Board::getRemovedProgressTokens() const {
  return removedTokens;
}

/**
 * @brief 玩家拿走指定进步标记
 * @param index 标记在可用列表中的索引（0-4）
 * @return 被拿走的进步标记对象
 * @throws std::runtime_error 如果索引无效
 */
ProgressToken Board::takeProgressToken(int index) {
  if (index < 0 || index >= (int)availableProgressTokens.size()) {
    throw std::runtime_error("Invalid token index");
  }

  ProgressToken token = availableProgressTokens[index];
  availableProgressTokens.erase(availableProgressTokens.begin() + index);
  return token;
}

ProgressToken Board::takeRemovedProgressToken(int index) {
  if (index < 0 || index >= (int)removedProgressTokens.size()) {
    throw std::runtime_error("Invalid removed token index");
  }

  ProgressToken token = removedProgressTokens[index];
  removedProgressTokens.erase(removedProgressTokens.begin() + index);
  return token;
}

/**
 * @brief 检查玩家是否可以获取进步标记
 * @param symbol 科学符号类型
 * @return true如果满足获取条件，否则false
 * @note 获取条件：1.游戏板上有可用标记 2.该科学符号已收集2个（配对成功）
 */
bool Board::canTakeProgressToken(ScienceSymbol symbol) const {
  // 检查游戏板上是否有可用的进步标记
  if (availableProgressTokens.empty()) {
    return false;
  }

  // 检查该科学符号是否达到配对条件（收集到2个相同的符号）
  auto it = scienceSymbolCounts.find(symbol);
  if (it != scienceSymbolCounts.end() && it->second >= 2) {
    return true; // 满足获取条件
  }

  return false; // 不满足获取条件
}

/**
 * @brief 科学符号配对事件处理
 * @param symbol 触发的科学符号类型
 * @note
 * 当玩家建造绿色科学建筑获得科学符号时调用，检查进步标记获取和科技胜利条件
 */
bool Board::onSciencePair(Player &player) {
  // 使用玩家自身的符号计数检查配对情况
  const auto counts = player.getScienceSymbolCounts();

  for (const auto &[symbol, count] : counts) {
    if (symbol == ScienceSymbol::NONE) {
      continue;
    }
    if (count >= 2 && !player.hasClaimedSciencePair(symbol) &&
        !availableProgressTokens.empty()) {
      ProgressToken token = takeProgressToken(0);
      player.addProgressToken(token);
      player.markSciencePairClaimed(symbol);
    }
  }

  int uniqueSymbols = 0;
  for (const auto &[symbol, count] : counts) {
    if (count > 0) {
      uniqueSymbols++;
    }
  }

  return uniqueSymbols >= 6;
}

/**
 * @brief 初始化所有10种进步标记到标记池
 * @note 游戏开始时调用，创建完整的进步标记集合
 */
void Board::initializeAllProgressTokens() {
  /**
   * 初始化所有10种进步标记：
   * 1. 清空现有标记池
   * 2. 创建所有类型的进步标记
   */
  allProgressTokens.clear();

  // 创建所有10种进步标记并添加到标记池
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::AGRICULTURE));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::ARCHITECTURE));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::ECONOMY));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::LAW));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::MASONRY));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::MATHEMATICS));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::PHILOSOPHY));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::STRATEGY));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::THEOLOGY));
  allProgressTokens.push_back(ProgressToken(ProgressTokenType::URBANISM));
}

/**
 * @brief 随机打乱进步标记顺序
 * @note 使用梅森旋转算法确保高质量的随机性，避免可预测的模式
 */
void Board::shuffleProgressTokens() {
  /**
   * 随机打乱进步标记顺序：
   * 1. 使用硬件随机设备作为种子源
   * 2. 使用梅森旋转算法生成随机数
   * 3. 打乱标记顺序确保游戏随机性
   */
  static std::random_device rd; // 硬件随机设备
  static std::mt19937 g(rd());  // 梅森旋转算法生成器
  std::shuffle(allProgressTokens.begin(), allProgressTokens.end(), g);
}

// === 新增：ProgressToken类的内联实现 ===

/**
 * @brief ProgressToken构造函数
 * @param type 进步标记类型
 */
ProgressToken::ProgressToken(ProgressTokenType type) : type(type) {}

/**
 * @brief 获取进步标记类型
 * @return 进步标记类型枚举值
 */
ProgressTokenType ProgressToken::getType() const { return type; }

/**
 * @brief 获取进步标记名称（英文）
 * @return 标记名称字符串
 */
std::string ProgressToken::getName() const {
  switch (type) {
  case ProgressTokenType::AGRICULTURE:
    return "Agriculture";
  case ProgressTokenType::ARCHITECTURE:
    return "Architecture";
  case ProgressTokenType::ECONOMY:
    return "Economy";
  case ProgressTokenType::LAW:
    return "Law";
  case ProgressTokenType::MASONRY:
    return "Masonry";
  case ProgressTokenType::MATHEMATICS:
    return "Mathematics";
  case ProgressTokenType::PHILOSOPHY:
    return "Philosophy";
  case ProgressTokenType::STRATEGY:
    return "Strategy";
  case ProgressTokenType::THEOLOGY:
    return "Theology";
  case ProgressTokenType::URBANISM:
    return "Urbanism";
  default:
    return "Unknown";
  }
}

/**
 * @brief 获取进步标记效果描述
 * @return 效果描述字符串
 */
std::string ProgressToken::getDescription() const {
  switch (type) {
  case ProgressTokenType::AGRICULTURE:
    return "Gain 6 coins immediately, +4 VP at game end";
  case ProgressTokenType::ARCHITECTURE:
    return "Future wonders cost 2 less resources";
  case ProgressTokenType::ECONOMY:
    return "Gain coins when opponent buys resources";
  case ProgressTokenType::LAW:
    return "Provides a unique science symbol for scientific victory";
  case ProgressTokenType::MASONRY:
    return "Future blue buildings cost 2 less resources";
  case ProgressTokenType::MATHEMATICS:
    return "+3 VP per progress token at game end";
  case ProgressTokenType::PHILOSOPHY:
    return "+7 VP at game end";
  case ProgressTokenType::STRATEGY:
    return "Future red buildings provide +1 shield";
  case ProgressTokenType::THEOLOGY:
    return "Future wonders gain Play Again effect";
  case ProgressTokenType::URBANISM:
    return "Gain 6 coins immediately, +4 coins per chain build";
  default:
    return "Unknown effect";
  }
}

/**
 * @brief 获取基础胜利点数
 * @return 基础胜利点数（部分标记有固定分数）
 * @note 大多数标记的分数通过效果计算，只有少数有固定基础分
 */
int ProgressToken::getBaseVictoryPoints() const {
  switch (type) {
  case ProgressTokenType::PHILOSOPHY:
    return 7; // 哲学标记固定7分
  default:
    return 0; // 其他标记基础分为0，按效果计算
  }
}

/**
 * @brief 应用标记的立即效果到玩家
 * @param player 要应用效果的玩家对象
 * @note 部分标记有立即效果（如获得金币），部分标记有持续效果
 */
void ProgressToken::applyEffect(Player *player) {
  if (!player)
    return; // 安全检查

  // 根据标记类型应用不同的立即效果
  switch (type) {
  case ProgressTokenType::AGRICULTURE:
  case ProgressTokenType::URBANISM:
    player->addCoins(6); // 农业和城市规划标记立即获得6金币
    break;
  // 其他标记的效果需要在玩家回合中持续生效，不在此处处理
  default:
    break;
  }
}

// === 原有的辅助函数和时代设置函数 ===

/**
 * @brief 辅助函数：添加卡牌槽位到金字塔
 * @param p 金字塔向量引用
 * @param c 要添加的卡牌
 * @param faceUp 是否正面朝上
 * @param coveredBy 覆盖此卡牌的槽位索引列表
 */
void addSlot(std::map<int, CardSlot> &p, int index, const Card &c, bool faceUp,
             const std::vector<int> &coveredBy) {
  p[index] = {c, faceUp, false, coveredBy};
}

/**
 * @brief 设置时代1的卡牌布局（正金字塔）
 * @param deck 时代1的卡牌堆
 * @note 布局结构：2-3-4-5-6行的正金字塔，顶层2张卡覆盖第二层
 */
void Board::setupAge1(std::vector<Card> &deck) {
  // 检查卡牌数量是否足够
  if (deck.size() < 20)
    return;

  int cardIdx = 0;
  // Row 1 (Top, 2 cards) - Indices 0-1
  addSlot(pyramid, 0, deck[cardIdx++], true, {2, 3});
  addSlot(pyramid, 1, deck[cardIdx++], true, {3, 4});

  // Row 2 (3 cards) - Indices 2-4
  addSlot(pyramid, 2, deck[cardIdx++], false, {5, 6});
  addSlot(pyramid, 3, deck[cardIdx++], false, {6, 7});
  addSlot(pyramid, 4, deck[cardIdx++], false, {7, 8});

  // Row 3 (4 cards) - Indices 5-8
  addSlot(pyramid, 5, deck[cardIdx++], true, {9, 10});
  addSlot(pyramid, 6, deck[cardIdx++], true, {10, 11});
  addSlot(pyramid, 7, deck[cardIdx++], true, {11, 12});
  addSlot(pyramid, 8, deck[cardIdx++], true, {12, 13});

  // Row 4 (5 cards) - Indices 9-13
  addSlot(pyramid, 9, deck[cardIdx++], false, {14, 15});
  addSlot(pyramid, 10, deck[cardIdx++], false, {15, 16});
  addSlot(pyramid, 11, deck[cardIdx++], false, {16, 17});
  addSlot(pyramid, 12, deck[cardIdx++], false, {17, 18});
  addSlot(pyramid, 13, deck[cardIdx++], false, {18, 19});

  // Row 5 (Bottom, 6 cards) - Indices 14-19
  for (int i = 14; i <= 19; ++i) {
    addSlot(pyramid, i, deck[cardIdx++], true, {});
  }
}

/**
 * @brief 设置时代2的卡牌布局（倒金字塔）
 * @param deck 时代2的卡牌堆
 * @note 布局结构：6-5-4-3-2行的倒金字塔，底层6张卡被上层覆盖
 */
void Board::setupAge2(std::vector<Card> &deck) {
  if (deck.size() < 20)
    return;
  pyramid.clear();
  int cardIdx = 0;

  // Inverted Pyramid structure for Age 2 (approximate for Duel)
  // Actually Age 2 is also mostly pyramid but inverted visibility or structure?
  // Let's stick to the indices provided in original code but use addSlot
  // correctly.

  // Row 1 (6 cards) 0-5
  addSlot(pyramid, 0, deck[cardIdx++], true, {6});
  addSlot(pyramid, 1, deck[cardIdx++], true, {6, 7});
  addSlot(pyramid, 2, deck[cardIdx++], true, {7, 8});
  addSlot(pyramid, 3, deck[cardIdx++], true, {8, 9});
  addSlot(pyramid, 4, deck[cardIdx++], true, {9, 10});
  addSlot(pyramid, 5, deck[cardIdx++], true, {10});

  // Row 2 (5 cards) 6-10
  addSlot(pyramid, 6, deck[cardIdx++], false, {11});
  addSlot(pyramid, 7, deck[cardIdx++], false, {11, 12});
  addSlot(pyramid, 8, deck[cardIdx++], false, {12, 13});
  addSlot(pyramid, 9, deck[cardIdx++], false, {13, 14});
  addSlot(pyramid, 10, deck[cardIdx++], false, {14});

  // Row 3 (4 cards) 11-14
  addSlot(pyramid, 11, deck[cardIdx++], true, {15});
  addSlot(pyramid, 12, deck[cardIdx++], true, {15, 16});
  addSlot(pyramid, 13, deck[cardIdx++], true, {16, 17});
  addSlot(pyramid, 14, deck[cardIdx++], true, {17});

  // Row 4 (3 cards) 15-17
  addSlot(pyramid, 15, deck[cardIdx++], false, {18});
  addSlot(pyramid, 16, deck[cardIdx++], false, {18, 19});
  addSlot(pyramid, 17, deck[cardIdx++], false, {19});

  // Row 5 (2 cards) 18-19
  addSlot(pyramid, 18, deck[cardIdx++], true, {});
  addSlot(pyramid, 19, deck[cardIdx++], true, {});
}

/**
 * @brief 设置时代3的卡牌布局（与时代1相同）
 * @param deck 时代3的卡牌堆
 * @note 布局结构：与时代1相同的正金字塔布局
 */
void Board::setupAge3(std::vector<Card> &deck) {
  if (deck.size() < 20)
    return;
  pyramid.clear();
  int cardIdx = 0;

  // Age 3 "Snake" or "Ring" Layout
  // Structure (Rows 1 to 7): 2 - 3 - 4 - 2 - 4 - 3 - 2
  // Card Index Distribution:
  // Row 1 (Top): 0, 1
  // Row 2: 2, 3, 4
  // Row 3: 5, 6, 7, 8
  // Row 4: 9, 10
  // Row 5: 11, 12, 13, 14
  // Row 6: 15, 16, 17
  // Row 7 (Bottom): 18, 19

  // Row 1 (Top, 2 cards, Face Up) - Indices 0-1
  addSlot(pyramid, 0, deck[cardIdx++], true, {2, 3});
  addSlot(pyramid, 1, deck[cardIdx++], true, {3, 4});

  // Row 2 (3 cards, Face Down) - Indices 2-4
  addSlot(pyramid, 2, deck[cardIdx++], false, {5, 6});
  addSlot(pyramid, 3, deck[cardIdx++], false, {6, 7});
  addSlot(pyramid, 4, deck[cardIdx++], false, {7, 8});

  // Row 3 (4 cards, Face Up) - Indices 5-8
  addSlot(pyramid, 5, deck[cardIdx++], true, {9});
  addSlot(pyramid, 6, deck[cardIdx++], true, {9});
  addSlot(pyramid, 7, deck[cardIdx++], true, {10});
  addSlot(pyramid, 8, deck[cardIdx++], true, {10});

  // Row 4 (2 cards, Face Down) - Indices 9-10
  addSlot(pyramid, 9, deck[cardIdx++], false, {11, 12});
  addSlot(pyramid, 10, deck[cardIdx++], false, {13, 14});

  // Row 5 (4 cards, Face Up) - Indices 11-14
  addSlot(pyramid, 11, deck[cardIdx++], true, {15});
  addSlot(pyramid, 12, deck[cardIdx++], true, {15, 16});
  addSlot(pyramid, 13, deck[cardIdx++], true, {16, 17});
  addSlot(pyramid, 14, deck[cardIdx++], true, {17});

  // Row 6 (3 cards, Face Down) - Indices 15-17
  addSlot(pyramid, 15, deck[cardIdx++], false, {18});
  addSlot(pyramid, 16, deck[cardIdx++], false, {18, 19});
  addSlot(pyramid, 17, deck[cardIdx++], false, {19});

  // Row 7 (Bottom, 2 cards, Face Up) - Indices 18-19
  // Bottom cards are accessible immediately (coveredBy = {})
  addSlot(pyramid, 18, deck[cardIdx++], true, {});
  addSlot(pyramid, 19, deck[cardIdx++], true, {});
}