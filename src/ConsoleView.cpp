// ConsoleView.cpp - 成员D (HY) 完成任务2.3：视图实现
#include "ConsoleView.h"
#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "Card.h"
#include "Wonder.h"
#include "Resources.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

// === 内部辅助函数 ===

// 获取资源类型名称
static std::string getResourceTypeName(ResourceType type) {
    switch (type) {
    case ResourceType::WOOD: return "Wood";
    case ResourceType::CLAY: return "Clay";
    case ResourceType::STONE: return "Stone";
    case ResourceType::GLASS: return "Glass";
    case ResourceType::PAPER: return "Paper";
    case ResourceType::NONE: return "None";
    default: return "Unknown";
    }
}

// 获取科学符号名称
static std::string getScienceSymbolName(ScienceSymbol symbol) {
    switch (symbol) {
    case ScienceSymbol::GLOBE: return "Globe";
    case ScienceSymbol::TABLET: return "Tablet";
    case ScienceSymbol::GEAR: return "Gear";
    case ScienceSymbol::COMPASS: return "Compass";
    case ScienceSymbol::WHEEL: return "Wheel";
    case ScienceSymbol::MORTAR: return "Mortar";
    case ScienceSymbol::NONE: return "None";
    default: return "Unknown";
    }
}

// 获取卡牌类型名称
static std::string getCardTypeName(CardType type) {
    switch (type) {
    case CardType::RAW_MATERIAL: return "Brown";
    case CardType::MANUFACTURED_GOOD: return "Grey";
    case CardType::CIVILIAN: return "Blue";
    case CardType::SCIENTIFIC: return "Green";
    case CardType::COMMERCIAL: return "Yellow";
    case CardType::MILITARY: return "Red";
    case CardType::GUILD: return "Purple";
    default: return "Unknown";
    }
}

// === 公开接口实现 ===

// 1. 打印完整游戏状态
void ConsoleView::printGameState(const Game &game) {
    std::cout << "\n=== FULL GAME STATE ===" << std::endl;
    
    // 当前玩家信息
    const Player& currentPlayer = (game.getCurrentPlayerIndex() == 0) ? 
                                  game.getPlayer1() : game.getPlayer2();
    const Player& otherPlayer = (game.getCurrentPlayerIndex() == 0) ? 
                                game.getPlayer2() : game.getPlayer1();
    
    ConsoleView::printPlayerStatus(currentPlayer, true);
    ConsoleView::printPlayerStatus(otherPlayer, false);
    
    // 军事状态
    ConsoleView::printMilitaryStatus(game.getBoard().getMilitaryPosition());
    
    // 游戏信息
    std::cout << "Current Age: " << game.getCurrentAge() << std::endl;
    std::cout << "Current Player: " << currentPlayer.getName() << std::endl;
    
    // 显示可用卡牌
    ConsoleView::printAvailableCards(game.getBoard());
    
    std::cout << "======================\n" << std::endl;
}

// 2. 打印游戏板（简化版）
void ConsoleView::printBoard(const Game &game) {
    const Board& board = game.getBoard();
    
    std::cout << "\n====== GAME BOARD ======" << std::endl;
    std::cout << "Age: " << game.getCurrentAge() 
              << " | Military: " << board.getMilitaryPosition() << std::endl;
    std::cout << game.getPlayer1().getName() << " (Coins: " << game.getPlayer1().getCoins() << ") vs "
              << game.getPlayer2().getName() << " (Coins: " << game.getPlayer2().getCoins() << ")"
              << std::endl;
    
    // 调用金字塔显示
    ConsoleView::printPyramid(board);
    
    std::cout << "\n* = Accessible card\n" << std::endl;
    std::cout << "========================\n" << std::endl;
}

// 3. 金字塔布局显示（核心功能）
void ConsoleView::printPyramid(const Board &board) {
    std::cout << "\nPYRAMID LAYOUT:" << std::endl;
    
    const auto& pyramid = board.getPyramid();
    
    // 假设是Age 1/3布局（2-3-4-5-6）
    // 实际应该根据游戏时代调整，这里简化处理
    std::vector<int> rowSizes = {2, 3, 4, 5, 6};
    
    int cardIndex = 0;
    for (size_t row = 0; row < rowSizes.size(); ++row) {
        int rowSize = rowSizes[row];
        
        // 计算缩进（居中对齐）
        int indent = (6 - rowSize) * 4;
        std::cout << std::string(indent, ' ');
        
        // 打印这一行的卡牌
        for (int col = 0; col < rowSize; ++col) {
            if (cardIndex >= pyramid.size()) break;
            
            const auto& slot = pyramid[cardIndex];
            bool accessible = board.isCardAccessible(cardIndex);
            
            // 格式: [*ID] 或 [ ID]
            std::string marker = accessible ? "*" : " ";
            std::cout << "[" << marker << std::setw(2) << cardIndex << "]";
            
            cardIndex++;
        }
        std::cout << std::endl;
    }
    
    // 显示卡牌详细信息
    std::cout << "\nCard Details:" << std::endl;
    cardIndex = 0;
    for (const auto& slot : pyramid) {
        if (!slot.isTaken && slot.isFaceUp) {
            bool accessible = board.isCardAccessible(cardIndex);
            std::string marker = accessible ? "*" : " ";
            std::cout << "[" << marker << std::setw(2) << cardIndex << "] "
                      << slot.card.getName() 
                      << " (" << getCardTypeName(slot.card.getType()) << ")" 
                      << std::endl;
        }
        cardIndex++;
    }
}

// 4. 玩家状态显示
void ConsoleView::printPlayerStatus(const Player &player, bool isCurrentPlayer) {
    std::cout << "\n" << std::string(40, '=') << std::endl;
    
    if (isCurrentPlayer) {
        std::cout << ">>> CURRENT PLAYER: " << player.getName() << " <<<" << std::endl;
    } else {
        std::cout << ">>> OPPONENT: " << player.getName() << " <<<" << std::endl;
    }
    std::cout << std::string(40, '-') << std::endl;
    
    // 基本信息
    std::cout << "Coins: " << player.getCoins()
              << " | Military: " << player.getMilitaryPower()
              << " | VP: " << player.getVictoryPoints() << std::endl;
    
    // 资源信息
    std::cout << "Resources: ";
    const auto& resources = player.getResources();
    bool first = true;
    for (const auto& [type, amount] : resources) {
        if (amount > 0) {
            if (!first) std::cout << ", ";
            std::cout << getResourceTypeName(type) << ": " << amount;
            first = false;
        }
    }
    if (first) std::cout << "None";
    std::cout << std::endl;
    
    // 科学符号
    const auto& scienceSymbols = player.getScienceSymbols();
    if (!scienceSymbols.empty()) {
        std::map<ScienceSymbol, int> counts;
        for (const auto& symbol : scienceSymbols) {
            if (symbol != ScienceSymbol::NONE) {
                counts[symbol]++;
            }
        }
        
        if (!counts.empty()) {
            std::cout << "Science: ";
            first = true;
            for (const auto& [symbol, count] : counts) {
                if (!first) std::cout << ", ";
                std::cout << getScienceSymbolName(symbol) << "(" << count << ")";
                first = false;
            }
            std::cout << std::endl;
        }
    }
    
    // 建造的卡牌（简要）
    const auto& builtCards = player.getBuiltCards();
    if (!builtCards.empty()) {
        std::cout << "Built Cards: " << builtCards.size() << " cards";
        if (builtCards.size() <= 5) {
            std::cout << " [";
            for (size_t i = 0; i < builtCards.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << builtCards[i].getName();
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
}

// 5. 军事状态显示
void ConsoleView::printMilitaryStatus(int position) {
    std::cout << "\n=== MILITARY CONFLICT ===" << std::endl;
    
    // 绘制简单的军事轨道
    std::cout << "P1 ";
    for (int i = -9; i <= 9; ++i) {
        if (i == position) {
            std::cout << "[X]";
        } else if (i == 0) {
            std::cout << "[0]";
        } else if (i % 3 == 0) {
            std::cout << "[" << std::setw(2) << i << "]";
        } else {
            std::cout << "[  ]";
        }
    }
    std::cout << " P2" << std::endl;
    
    // 解释位置
    if (position <= -9) {
        std::cout << " Player 1 WINS by Military Victory!" << std::endl;
    } else if (position >= 9) {
        std::cout << " Player 2 WINS by Military Victory!" << std::endl;
    } else if (position < 0) {
        std::cout << "Player 1 leads by " << -position << " steps" << std::endl;
    } else if (position > 0) {
        std::cout << "Player 2 leads by " << position << " steps" << std::endl;
    } else {
        std::cout << "Military is balanced" << std::endl;
    }
    
    std::cout << "=========================\n" << std::endl;
}

// 6. 可用卡牌列表
void ConsoleView::printAvailableCards(const Board &board) {
    const auto& pyramid = board.getPyramid();
    
    std::cout << "\n=== AVAILABLE CARDS ===" << std::endl;
    bool hasAvailable = false;
    
    for (size_t i = 0; i < pyramid.size(); ++i) {
        if (board.isCardAccessible(i) && !pyramid[i].isTaken) {
            hasAvailable = true;
            std::cout << "[" << i << "] ";
            if (pyramid[i].isFaceUp) {
                const Card& card = pyramid[i].card;
                std::cout << card.getName() 
                          << " (" << getCardTypeName(card.getType()) << ")";
                
                // 显示简要成本
                const Cost& cost = card.getCost();
                if (cost.coins > 0) {
                    std::cout << " - Cost: " << cost.coins << " coins";
                }
            } else {
                std::cout << "Face Down (Unknown)";
            }
            std::cout << std::endl;
        }
    }
    
    if (!hasAvailable) {
        std::cout << "No available cards" << std::endl;
    }
    
    std::cout << "======================\n" << std::endl;
}

// 7. 卡牌详细信息
void ConsoleView::printCardDetails(const Card& card) {
    std::cout << "\n" << std::string(40, '=') << std::endl;
    std::cout << "CARD DETAILS" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    std::cout << "Name: " << card.getName() << std::endl;
    std::cout << "Type: " << getCardTypeName(card.getType()) << std::endl;
    
    const Cost& cost = card.getCost();
    std::cout << "Cost: " << cost.coins << " coins";
    if (!cost.resources.empty()) {
        std::cout << " + ";
        bool first = true;
        for (const auto& [type, amount] : cost.resources) {
            if (!first) std::cout << ", ";
            std::cout << amount << " " << getResourceTypeName(type);
            first = false;
        }
    }
    std::cout << std::endl;
    
    const Effect& effect = card.getEffect();
    if (effect.victoryPoints > 0)
        std::cout << "Victory Points: " << effect.victoryPoints << std::endl;
    if (effect.militaryShields > 0)
        std::cout << "Military Shields: " << effect.militaryShields << std::endl;
    if (effect.coins > 0)
        std::cout << "Immediate Coins: +" << effect.coins << std::endl;
    
    if (!effect.resourcesProduced.empty()) {
        std::cout << "Produces: ";
        bool first = true;
        for (const auto& [type, amount] : effect.resourcesProduced) {
            if (!first) std::cout << ", ";
            std::cout << amount << " " << getResourceTypeName(type) << "/turn";
            first = false;
        }
        std::cout << std::endl;
    }
    
    if (!effect.scienceSymbols.empty()) {
        std::cout << "Science: ";
        for (size_t i = 0; i < effect.scienceSymbols.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << getScienceSymbolName(effect.scienceSymbols[i]);
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
}

// 8. 行动菜单
void ConsoleView::printActionMenu(bool canBuildCard, bool canBuildWonder, int discardCoins) {
    std::cout << "\n" << std::string(40, '=') << std::endl;
    std::cout << "ACTION MENU" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    std::cout << "1. Build this card as a Building";
    if (!canBuildCard) {
        std::cout << " [CANNOT AFFORD]";
    }
    std::cout << std::endl;
    
    std::cout << "2. Discard for " << discardCoins << " coins" << std::endl;
    
    std::cout << "3. Use this card to construct a Wonder";
    if (!canBuildWonder) {
        std::cout << " [NO AFFORDABLE WONDERS]";
    }
    std::cout << std::endl;
    
    std::cout << "4. Exit Game" << std::endl;
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Enter choice (1-4): ";
}

// 9. 奇迹显示
void ConsoleView::printWonders(const std::vector<Wonder*> &wonders, bool showAvailability) {
    std::cout << "\n=== AVAILABLE WONDERS ===" << std::endl;
    
    for (size_t i = 0; i < wonders.size(); ++i) {
        if (wonders[i] != nullptr) {
            std::cout << "[" << i << "] " << wonders[i]->getName();
            
            if (showAvailability) {
                if (wonders[i]->isBuilt()) {
                    std::cout << " [BUILT]";
                } else {
                    std::cout << " [AVAILABLE]";
                }
            }
            
            std::cout << std::endl;
        }
    }
    
    std::cout << "=======================\n" << std::endl;
}

// 10. 复杂玩家状态面板（对应Game.cpp中的长函数）
void ConsoleView::printPlayerStatePanel(const Player *currentPlayer, const Player *otherPlayer) {
    std::cout << "\n";
    std::cout << "================================================================" << std::endl;
    std::cout << "                     PLAYER STATUS PANEL" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    // 当前玩家信息
    std::cout << ">>> " << currentPlayer->getName() << " (YOU) <<<" << std::endl;
    std::cout << "Coins: " << currentPlayer->getCoins()
              << " | Military: " << currentPlayer->getMilitaryPower()
              << " | VP: " << currentPlayer->getVictoryPoints() << std::endl;
    
    // 对手信息
    std::cout << ">>> " << otherPlayer->getName() << " (OPPONENT) <<<" << std::endl;
    std::cout << "Coins: " << otherPlayer->getCoins()
              << " | Military: " << otherPlayer->getMilitaryPower()
              << " | VP: " << otherPlayer->getVictoryPoints() << std::endl;
    
    std::cout << "================================================================\n" << std::endl;
}

// 11. 完整游戏板显示（替代Game::printBoard）
void ConsoleView::printGameBoard(const Game &game) {
    // 直接调用我们实现的printBoard
    ConsoleView::printBoard(game);
}

// === 消息输出函数 ===

void ConsoleView::printError(const std::string &message) {
    std::cout << "\nERROR: " << message << std::endl;
}

void ConsoleView::printInfo(const std::string &message) {
    std::cout << "\nINFO: " << message << std::endl;
}

void ConsoleView::printSuccess(const std::string &message) {
    std::cout << "\nSUCCESS: " << message << std::endl;
}

// === Wonder selection helpers ===
void ConsoleView::printSectionHeader(const std::string &title) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "        " << title << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void ConsoleView::printSubHeader(const std::string &title) {
    std::cout << "\n=== " << title << " ===" << std::endl;
}

void ConsoleView::printPlain(const std::string &message) {
    std::cout << message << std::endl;
}

void ConsoleView::printPrompt(const std::string &prompt) {
    std::cout << prompt;
}

void ConsoleView::printNumberedWonderList(const std::vector<Wonder *> &wonders,
                                          const std::string &title) {
    if (!title.empty()) {
        std::cout << title << std::endl;
    }

    for (size_t i = 0; i < wonders.size(); ++i) {
        std::cout << i + 1 << ". " << wonders[i]->getName() << std::endl;
    }
    std::cout << std::endl;
}

void ConsoleView::printPlayerWonderCollection(const Player &player,
                                              const std::vector<Wonder *> &wonders) {
    std::cout << "========================================" << std::endl;
    std::cout << player.getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < wonders.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << wonders[i]->getName() << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}