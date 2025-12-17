// AIPlayer.cpp - 成员D (HY) 完成
#include "AIPlayer.h"
#include "Game.h"
#include "Board.h"
#include "Card.h"
#include "ConsoleView.h" // 使用你定义的视图接口来输出
#include <vector>
#include <cstdlib> // 用于 rand()
#include <ctime>   // 用于 srand()

AIPlayer::AIPlayer(std::string name) : Player(name, true) // 调用基类构造函数，标记为AI
{
    // 初始化随机数种子，用于随机选择
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

AIPlayer::~AIPlayer()
{
    // 没有动态分配的资源，留空
}

// 核心AI决策函数 - 完整实现
void AIPlayer::makeDecision(const Game& game)
{
    // 使用ConsoleView输出，而不是直接cout，符合架构
    ConsoleView::printInfo(getName() + " (AI) 开始思考...");

    // 1. 获取游戏状态
    const Board& board = game.getBoard();
    const Player& opponent = (&game.getPlayer1() == this) ? game.getPlayer2() : game.getPlayer1();

    // 2. 获取所有可访问的卡牌索引
    std::vector<int> accessibleIndices;
    const auto& pyramid = board.getPyramid();
    for (size_t i = 0; i < pyramid.size(); ++i) {
        if (board.isCardAccessible(i) && !pyramid[i].isTaken) {
            accessibleIndices.push_back(static_cast<int>(i));
        }
    }

    if (accessibleIndices.empty()) {
        ConsoleView::printError("AI错误：没有可访问的卡牌。");
        return;
    }

    // 3. 评估每张可访问的卡牌，并找出最佳可购买卡牌
    int bestCardIndex = -1;
    int bestCardScore = -1000; // 初始一个很低的分数

    for (int cardIndex : accessibleIndices) {
        const Card& card = pyramid[cardIndex].card;

        // 3.1 检查是否买得起（使用基类Player的canAfford方法）
        if (this->canAfford(card.getCost(), opponent)) {
            // 3.2 如果买得起，计算这张卡的吸引力得分
            int score = evaluateCard(card, game);

            ConsoleView::printInfo("  评估卡牌 [" + std::to_string(cardIndex) + "]: " + card.getName() + "，得分: " + std::to_string(score));

            // 3.3 更新最佳卡牌
            if (score > bestCardScore) {
                bestCardScore = score;
                bestCardIndex = cardIndex;
            }
        } else {
            ConsoleView::printInfo("  卡牌 [" + std::to_string(cardIndex) + "]: " + card.getName() + " - 买不起，跳过。");
        }
    }

    // 4. 做出最终决策
    if (bestCardIndex != -1 && bestCardScore > 0) {
        // 情况1：有买得起且得分正的卡牌，选择建造它
        const Card& chosenCard = pyramid[bestCardIndex].card;
        ConsoleView::printInfo("*** 决策：建造卡牌 [" + std::to_string(bestCardIndex) + "] " + chosenCard.getName() + " (得分: " + std::to_string(bestCardScore) + ")");
        // 注意：这里应该触发游戏逻辑真正建造卡牌。由于Game类流程控制，AI通常只返回选择。
        // 具体建造动作可能在Game::playTurn中根据AI返回的索引执行。
    } else {
        // 情况2：没有买得起的好牌，选择弃牌换钱（策略：弃置最便宜或最没用的牌）
        // 这里简化：随机弃置一张可访问的牌
        int randomIndex = accessibleIndices[std::rand() % accessibleIndices.size()];
        const Card& cardToDiscard = pyramid[randomIndex].card;
        ConsoleView::printInfo("*** 决策：没有可建造的好牌，弃置卡牌 [" + std::to_string(randomIndex) + "] " + cardToDiscard.getName() + " 来换取金币。");
        // 弃牌动作同样应由Game类执行。
    }

    ConsoleView::printInfo(getName() + " (AI) 思考结束。\n");
}

// 卡牌评估函数 - 私有逻辑，决定AI的喜好
int AIPlayer::evaluateCard(const Card& card, const Game& game) const
{
    int score = 0;
    const Effect& effect = card.getEffect();

    // 计分规则 (你可以调整这些权重来改变AI性格)
    const int VP_WEIGHT = 3;      // 胜利点权重
    const int MILITARY_WEIGHT = 6; // 军事权重 (AI偏好军事)
    const int SCIENCE_WEIGHT = 5;  // 科技符号权重
    const int RESOURCE_WEIGHT = 2; // 资源生产力权重
    const int COIN_WEIGHT = 1;     // 直接金币权重

    // 1. 基础胜利点
    score += effect.victoryPoints * VP_WEIGHT;

    // 2. 军事力量（非常重要）
    score += effect.militaryShields * MILITARY_WEIGHT;

    // 3. 科学符号（为科技胜利做准备）
    if (!effect.scienceSymbols.empty()) {
        score += SCIENCE_WEIGHT;
        // 如果这是AI缺少的符号，可以额外加分（进阶逻辑）
    }

    // 4. 资源生产（增强未来购买力）
    for (const auto& [resource, amount] : effect.resourcesProduced) {
        score += amount * RESOURCE_WEIGHT;
    }

    // 5. 直接金币奖励
    score += effect.coins * COIN_WEIGHT;

    // 6. 根据游戏阶段微调（简化版）
    // 如果当前是Age 1，更看重军事和资源；Age 3更看重分数和科技
    if (game.getCurrentAge() == 1) {
        score += effect.militaryShields * 2; // 时代1额外重视军事
    } else if (game.getCurrentAge() == 3) {
        score += effect.victoryPoints * 2;   // 时代3额外重视分数
    }

    // 7. 成本惩罚（让AI倾向于买便宜的卡，即使买得起）
    // 公式：扣分 = 卡牌金币成本 * 0.5（权重可调）
    score -= static_cast<int>(card.getCost().coins * 0.5);

    return score;
}