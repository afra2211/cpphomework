#include "Board.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

/**
 * @brief Board类构造函数
 * @note 初始化军事位置为0（中央），并初始化所有进步标记
 */
Board::Board() : militaryPosition(0) {
  // === 新增：初始化进步标记系统 ===
  initializeAllProgressTokens();
}

/**
 * @brief 设置指定时代的卡牌布局
 * @param age 时代编号（1、2、3）
 * @param deck 该时代的卡牌堆（应包含20张卡牌）
 * @note 根据时代不同采用不同的金字塔布局结构
 */
void Board::setupAge(int age, std::vector<Card> deck) {
  pyramid.clear(); // 清空现有金字塔布局

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
const std::vector<CardSlot> &Board::getPyramid() const { return pyramid; }

/**
 * @brief 检查指定卡牌是否可被玩家访问
 * @param index 卡牌在金字塔中的索引
 * @return true如果卡牌可访问，否则false
 * @note 可访问条件：1.索引有效 2.卡牌未被拿走 3.所有覆盖此卡的卡牌已被拿走
 */
bool Board::isCardAccessible(int index) const {
  // 检查索引有效性
  if (index < 0 || index >= pyramid.size())
    return false;

  // 检查卡牌是否已被玩家拿走
  if (pyramid[index].isTaken)
    return false;

  // 检查是否被其他卡牌覆盖（所有覆盖卡必须已被拿走）
  for (int coverIndex : pyramid[index].coveredBy) {
    if (!pyramid[coverIndex].isTaken) {
      return false; // 存在未被拿走的覆盖卡，当前卡不可访问
    }
  }
  return true; // 所有条件满足，卡牌可访问
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
void Board::moveMilitary(int amount) {
  militaryPosition += amount;
  // 注意：边界限制（-9到9）和军事胜利判定应由Game类处理
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
  if (index < 0 || index >= availableWonders.size())
    return nullptr;

  // 获取奇迹指针并从可用列表中移除
  Wonder *w = availableWonders[index];
  availableWonders.erase(availableWonders.begin() + index);
  return w;
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
  int tokensToPlace = std::min(5, static_cast<int>(allProgressTokens.size()));
  for (int i = 0; i < tokensToPlace; ++i) {
    availableProgressTokens.push_back(allProgressTokens[i]);
  }

  // 重置科学符号计数（新游戏开始）
  scienceSymbolCounts.clear();
}

/**
 * @brief 获取当前可用的进步标记列表
 * @return 可用进步标记列表的只读引用
 */
const std::vector<ProgressToken> &Board::getAvailableProgressTokens() const {
  return availableProgressTokens;
}

/**
 * @brief 玩家拿走指定进步标记
 * @param index 标记在可用列表中的索引（0-4）
 * @return 被拿走的进步标记对象
 * @throws std::runtime_error 如果索引无效
 */
ProgressToken Board::takeProgressToken(int index) {
  // 异常处理：检查索引有效性
  if (index < 0 || index >= availableProgressTokens.size()) {
    throw std::runtime_error("Invalid progress token index");
  }

  // 获取标记对象
  ProgressToken token = availableProgressTokens[index];

  // 从可用列表中移除该标记（每个标记只能被获取一次）
  availableProgressTokens.erase(availableProgressTokens.begin() + index);

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
void Board::onSciencePair(ScienceSymbol symbol) {
  /**
   * 科学符号配对事件处理：
   * 1. 增加对应符号计数
   * 2. 检查是否满足进步标记获取条件（2个相同符号）
   * 3. 检查是否达成科技胜利（6种不同符号）
   */

  // 增加对应科学符号的计数
  scienceSymbolCounts[symbol]++;

  // 检查进步标记获取条件：达到2个相同符号且游戏板上有可用标记
  if (scienceSymbolCounts[symbol] >= 2 && !availableProgressTokens.empty()) {
    // 触发进步标记获取事件（可由Game类处理玩家选择）
    // 例如：game->onProgressTokenAvailable(currentPlayer);
  }

  // 检查科技胜利条件：集齐6种不同的科学符号
  int uniqueSymbols = 0;
  for (const auto &pair : scienceSymbolCounts) {
    if (pair.second > 0) {
      uniqueSymbols++; // 统计不同符号的数量
    }
  }

  // 如果收集到6种不同的科学符号，触发科技胜利
  if (uniqueSymbols >= 6) {
    // 触发科技胜利（可由Game类处理）
    // 例如：game->triggerScientificVictory(currentPlayer);
  }
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
void addSlot(std::vector<CardSlot> &p, const Card &c, bool faceUp,
             const std::vector<int> &coveredBy) {
  p.push_back({c, faceUp, false, coveredBy});
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

  int cardIdx = 0; // 卡牌索引计数器

  // 第0行（顶层）：2张卡牌，每张覆盖第1行的2张卡牌
  addSlot(pyramid, deck[cardIdx++], true, {2, 3}); // 卡牌0，覆盖2和3
  addSlot(pyramid, deck[cardIdx++], true, {3, 4}); // 卡牌1，覆盖3和4

  // 第1行：3张卡牌，每张覆盖第2行的2张卡牌
  addSlot(pyramid, deck[cardIdx++], false, {5, 6}); // 卡牌2，覆盖5和6
  addSlot(pyramid, deck[cardIdx++], false, {6, 7}); // 卡牌3，覆盖6和7
  addSlot(pyramid, deck[cardIdx++], false, {7, 8}); // 卡牌4，覆盖7和8

  // 第2行：4张卡牌，每张覆盖第3行的2张卡牌
  addSlot(pyramid, deck[cardIdx++], true, {9, 10});  // 卡牌5，覆盖9和10
  addSlot(pyramid, deck[cardIdx++], true, {10, 11}); // 卡牌6，覆盖10和11
  addSlot(pyramid, deck[cardIdx++], true, {11, 12}); // 卡牌7，覆盖11和12
  addSlot(pyramid, deck[cardIdx++], true, {12, 13}); // 卡牌8，覆盖12和13

  // 第3行：5张卡牌，每张覆盖第4行的2张卡牌
  addSlot(pyramid, deck[cardIdx++], false, {14, 15}); // 卡牌9，覆盖14和15
  addSlot(pyramid, deck[cardIdx++], false, {15, 16}); // 卡牌10，覆盖15和16
  addSlot(pyramid, deck[cardIdx++], false, {16, 17}); // 卡牌11，覆盖16和17
  addSlot(pyramid, deck[cardIdx++], false, {17, 18}); // 卡牌12，覆盖17和18
  addSlot(pyramid, deck[cardIdx++], false, {18, 19}); // 卡牌13，覆盖18和19

  // 第4行（底层）：6张卡牌，无覆盖（初始即可访问）
  for (int i = 0; i < 6; ++i) {
    addSlot(pyramid, deck[cardIdx++], true, {}); // 卡牌14-19，无覆盖
  }
}

/**
 * @brief 设置时代2的卡牌布局（倒金字塔）
 * @param deck 时代2的卡牌堆
 * @note 布局结构：6-5-4-3-2行的倒金字塔，底层6张卡被上层覆盖
 */
void Board::setupAge2(std::vector<Card> &deck) {
  // 检查卡牌数量是否足够
  if (deck.size() < 20)
    return;

  int cardIdx = 0; // 卡牌索引计数器

  // 第0行（顶层）：6张卡牌，被第1行覆盖
  addSlot(pyramid, deck[cardIdx++], true, {6});     // 卡牌0，被6覆盖
  addSlot(pyramid, deck[cardIdx++], true, {6, 7});  // 卡牌1，被6和7覆盖
  addSlot(pyramid, deck[cardIdx++], true, {7, 8});  // 卡牌2，被7和8覆盖
  addSlot(pyramid, deck[cardIdx++], true, {8, 9});  // 卡牌3，被8和9覆盖
  addSlot(pyramid, deck[cardIdx++], true, {9, 10}); // 卡牌4，被9和10覆盖
  addSlot(pyramid, deck[cardIdx++], true, {10});    // 卡牌5，被10覆盖

  // 第1行：5张卡牌，被第2行覆盖
  addSlot(pyramid, deck[cardIdx++], false, {11});     // 卡牌6，被11覆盖
  addSlot(pyramid, deck[cardIdx++], false, {11, 12}); // 卡牌7，被11和12覆盖
  addSlot(pyramid, deck[cardIdx++], false, {12, 13}); // 卡牌8，被12和13覆盖
  addSlot(pyramid, deck[cardIdx++], false, {13, 14}); // 卡牌9，被13和14覆盖
  addSlot(pyramid, deck[cardIdx++], false, {14});     // 卡牌10，被14覆盖

  // 第2行：4张卡牌，被第3行覆盖
  addSlot(pyramid, deck[cardIdx++], true, {15});     // 卡牌11，被15覆盖
  addSlot(pyramid, deck[cardIdx++], true, {15, 16}); // 卡牌12，被15和16覆盖
  addSlot(pyramid, deck[cardIdx++], true, {16, 17}); // 卡牌13，被16和17覆盖
  addSlot(pyramid, deck[cardIdx++], true, {17});     // 卡牌14，被17覆盖

  // 第3行：3张卡牌，被第4行覆盖
  addSlot(pyramid, deck[cardIdx++], false, {18});     // 卡牌15，被18覆盖
  addSlot(pyramid, deck[cardIdx++], false, {18, 19}); // 卡牌16，被18和19覆盖
  addSlot(pyramid, deck[cardIdx++], false, {19});     // 卡牌17，被19覆盖

  // 第4行（底层）：2张卡牌，无覆盖（初始即可访问）
  addSlot(pyramid, deck[cardIdx++], true, {}); // 卡牌18，无覆盖
  addSlot(pyramid, deck[cardIdx++], true, {}); // 卡牌19，无覆盖
}

/**
 * @brief 设置时代3的卡牌布局（与时代1相同）
 * @param deck 时代3的卡牌堆
 * @note 布局结构：与时代1相同的正金字塔布局
 */
void Board::setupAge3(std::vector<Card> &deck) {
  // 重用时代1的布局设置函数
  setupAge1(deck);
}