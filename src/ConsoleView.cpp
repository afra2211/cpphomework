// ConsoleView.cpp - 成员D (HY) 完成任务2.3：视图实现
#include "ConsoleView.h"
#include "Board.h"
#include "Card.h"
#include "Game.h"
#include "Player.h"
#include "Resources.h"
#include "Wonder.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// === 内部辅助函数 ===

// 获取资源类型名称
static std::string getResourceTypeName(ResourceType type) {
  switch (type) {
  case ResourceType::WOOD:
    return "Wood";
  case ResourceType::CLAY:
    return "Clay";
  case ResourceType::STONE:
    return "Stone";
  case ResourceType::GLASS:
    return "Glass";
  case ResourceType::PAPER:
    return "Paper";
  case ResourceType::NONE:
    return "None";
  default:
    return "Unknown";
  }
}

// 获取科学符号名称
static std::string getScienceSymbolName(ScienceSymbol symbol) {
  switch (symbol) {
  case ScienceSymbol::GLOBE:
    return "Globe";
  case ScienceSymbol::TABLET:
    return "Tablet";
  case ScienceSymbol::GEAR:
    return "Gear";
  case ScienceSymbol::COMPASS:
    return "Compass";
  case ScienceSymbol::WHEEL:
    return "Wheel";
  case ScienceSymbol::MORTAR:
    return "Mortar";
  case ScienceSymbol::NONE:
    return "None";
  default:
    return "Unknown";
  }
}

// 获取卡牌类型名称
static std::string getCardTypeName(CardType type) {
  switch (type) {
  case CardType::RAW_MATERIAL:
    return "Brown";
  case CardType::MANUFACTURED_GOOD:
    return "Grey";
  case CardType::CIVILIAN:
    return "Blue";
  case CardType::SCIENTIFIC:
    return "Green";
  case CardType::COMMERCIAL:
    return "Yellow";
  case CardType::MILITARY:
    return "Red";
  case CardType::GUILD:
    return "Purple";
  default:
    return "Unknown";
  }
}

static std::string getCostString(const Cost &cost, int maxLength = 15) {
  std::string result;

  if (cost.coins > 0) {
    result += std::to_string(cost.coins) + "C ";
  }

  for (const auto &[type, amount] : cost.resources) {
    if (amount > 0) {
      std::string resourceAbbr;
      switch (type) {
      case ResourceType::WOOD:
        resourceAbbr = "W";
        break;
      case ResourceType::CLAY:
        resourceAbbr = "C";
        break;
      case ResourceType::STONE:
        resourceAbbr = "S";
        break;
      case ResourceType::GLASS:
        resourceAbbr = "G";
        break;
      case ResourceType::PAPER:
        resourceAbbr = "P";
        break;
      default:
        resourceAbbr = "?";
      }
      result += std::to_string(amount) + resourceAbbr + " ";
    }
  }

  // If string is empty, it's free
  if (result.empty()) {
    result = "FREE";
  }

  // Truncate to max length
  if (result.length() > maxLength) {
    result = result.substr(0, maxLength - 3) + "...";
  }

  return result;
}

// === 辅助函数：获取卡牌简写信息 ===
static std::string getCardShortInfo(const Card &card, bool showCost = true) {
  std::string result = card.getName();

  // Truncate long names
  if (result.length() > 10) {
    result = result.substr(0, 10);
  }

  // Add type abbreviation
  std::string typeAbbr;
  switch (card.getType()) {
  case CardType::RAW_MATERIAL:
    typeAbbr = "[Br]";
    break;
  case CardType::MANUFACTURED_GOOD:
    typeAbbr = "[Gr]";
    break;
  case CardType::CIVILIAN:
    typeAbbr = "[Bl]";
    break;
  case CardType::SCIENTIFIC:
    typeAbbr = "[Gn]";
    break;
  case CardType::COMMERCIAL:
    typeAbbr = "[Yl]";
    break;
  case CardType::MILITARY:
    typeAbbr = "[Rd]";
    break;
  case CardType::GUILD:
    typeAbbr = "[Pu]";
    break;
  default:
    typeAbbr = "[?]";
  }

  // Add cost information
  if (showCost) {
    std::string costStr = getCostString(card.getCost(), 10);
    return result + typeAbbr + "(" + costStr + ")";
  }

  return result + typeAbbr;
}

// === 公开接口实现 ===

// 1. 打印完整游戏状态
void ConsoleView::printGameState(const Game &game) {
  std::cout << "\n=== FULL GAME STATE ===" << std::endl;

  // 当前玩家信息
  const Player &currentPlayer = (game.getCurrentPlayerIndex() == 0)
                                    ? game.getPlayer1()
                                    : game.getPlayer2();
  const Player &otherPlayer = (game.getCurrentPlayerIndex() == 0)
                                  ? game.getPlayer2()
                                  : game.getPlayer1();

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
  const Board &board = game.getBoard();

  std::cout << "\n====== GAME BOARD ======" << std::endl;
  std::cout << "Age: " << game.getCurrentAge()
            << " | Military: " << board.getMilitaryPosition() << std::endl;
  std::cout << game.getPlayer1().getName()
            << " (Coins: " << game.getPlayer1().getCoins() << ") vs "
            << game.getPlayer2().getName()
            << " (Coins: " << game.getPlayer2().getCoins() << ")" << std::endl;

  // 调用金字塔显示
  ConsoleView::printPyramid(board, game.getCurrentAge());

  std::cout << "\n* = Accessible card\n" << std::endl;
  std::cout << "========================\n" << std::endl;
}

// 3. 金字塔布局显示（核心功能）
static void printSingleCardSlot(const Board &board,
                                const std::map<int, CardSlot> &pyramid,
                                int cardIndex) {
  if (pyramid.find(cardIndex) == pyramid.end()) {
    return;
  }
  const auto &slot = pyramid.at(cardIndex);
  bool accessible = board.isCardAccessible(cardIndex);
  bool isTaken = slot.isTaken;

  // Accessibility marker
  std::string marker = accessible ? "*" : " ";
  if (isTaken)
    marker = "X";

  std::cout << "[" << marker << std::setw(2) << cardIndex << "]:";

  // Print cards in this row
  if (isTaken) {
    std::cout << "TAKEN     ";
  } else if (!slot.isFaceUp) {
    std::cout << "Face Down ";
  } else {
    // Show card brief info
    std::string cardInfo = getCardShortInfo(slot.card);
    std::cout << std::setw(25) << std::left << cardInfo << std::right;
  }
}

// 3. 金字塔布局显示（核心功能） - Updated to support Age 2 inversion
void ConsoleView::printPyramid(const Board &board, int age) {
  std::cout << "\nPYRAMID LAYOUT (Age " << age << "):" << std::endl;
  std::cout << "Legend: *=accessible, [Br]=Brown, [Gr]=Grey, [Bl]=Blue,"
            << std::endl;
  std::cout << "        [Gn]=Green, [Yl]=Yellow, [Rd]=Red, [Pu]=Purple"
            << std::endl;
  std::cout << "Resources: W=Wood, C=Clay, S=Stone, G=Glass, P=Paper"
            << std::endl;
  std::cout << "Effects: VP=Victory Points, Sh=Shields, C=Coins" << std::endl;
  std::cout << std::string(70, '=') << std::endl;
  const auto &pyramid = board.getPyramid();

  // Define pyramid structure based on age
  std::vector<int> rowSizes;
  if (age == 1) {
    rowSizes = {2, 3, 4, 5, 6}; // Age 1: Standard
  } else if (age == 2) {
    rowSizes = {6, 5, 4, 3, 2}; // Age 2: Inverted
  } else if (age == 3) {
    // Age 3: Special Split/Merge Layout
    // Row 1-7: 2, 3, 4, 2, 4, 3, 2
    rowSizes = {2, 3, 4, 2, 4, 3, 2};
  }

  int cardIndex = 0;
  for (size_t row = 0; row < rowSizes.size(); ++row) {
    int rowSize = rowSizes[row];
    std::cout << "Row " << (row + 1) << ": ";

    if (age == 3) {
      if (row == 2) { // Row 3: 4 cards split (2 -- 2)
        for (int col = 0; col < rowSize; ++col) {
          if (cardIndex < static_cast<int>(pyramid.size()))
            printSingleCardSlot(board, pyramid, cardIndex++);
          if (col == 1)
            std::cout << "          "; // Gap between groups
        }
      } else if (row == 3) { // Row 4: 2 cards split (1 -- 1)
        for (int col = 0; col < rowSize; ++col) {
          if (col == 0)
            std::cout << "     ";
          if (cardIndex < static_cast<int>(pyramid.size()))
            printSingleCardSlot(board, pyramid, cardIndex++);
          if (col == 0)
            std::cout << "                    "; // Wide gap
        }
      } else if (row == 4) { // Row 5: 4 cards split (2 -- 2)
        for (int col = 0; col < rowSize; ++col) {
          if (cardIndex < static_cast<int>(pyramid.size()))
            printSingleCardSlot(board, pyramid, cardIndex++);
          if (col == 1)
            std::cout << "          "; // Gap between groups
        }
      } else {
        // Standard centering
        int spaces = (6 - rowSize) * 4;
        if (spaces > 0)
          std::cout << std::string(spaces, ' ');

        for (int col = 0; col < rowSize; ++col) {
          if (cardIndex < static_cast<int>(pyramid.size())) {
            printSingleCardSlot(board, pyramid, cardIndex++);
          }
        }
      }
    } else {
      // Ages 1 & 2
      int maxRow = 6;
      int spaces = (maxRow - rowSize) * 4;
      if (spaces > 0)
        std::cout << std::string(spaces, ' ');

      for (int col = 0; col < rowSize; ++col) {
        if (cardIndex < static_cast<int>(pyramid.size())) {
          printSingleCardSlot(board, pyramid, cardIndex++);
        }
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // Print taken cards list
  std::cout << std::string(70, '-') << std::endl;
  std::cout << "TAKEN CARDS: ";
  bool hasTaken = false;
  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (pyramid.at(i).isTaken) {
      if (hasTaken)
        std::cout << ", ";
      std::cout << "[" << i << "]" << pyramid.at(i).card.getName();
      hasTaken = true;
    }
  }
  if (!hasTaken)
    std::cout << "None";
  std::cout << std::endl;

  // Print accessible cards list
  std::cout << "ACCESSIBLE CARDS: ";
  bool hasAccessible = false;
  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (board.isCardAccessible(i) && !pyramid.at(i).isTaken) {
      if (hasAccessible)
        std::cout << ", ";
      std::cout << "[" << i << "]";
      if (pyramid.at(i).isFaceUp) {
        std::cout << pyramid.at(i).card.getName() << "("
                  << getCostString(pyramid.at(i).card.getCost(), 12) << ")";
      } else {
        std::cout << "Face Down";
      }
      hasAccessible = true;
    }
  }
  if (!hasAccessible)
    std::cout << "None";
  std::cout << std::endl << std::string(70, '=') << std::endl;
}

// 4. 玩家状态显示
void ConsoleView::printPlayerStatus(const Player &player,
                                    bool isCurrentPlayer) {
  std::cout << "\n" << std::string(40, '=') << std::endl;

  if (isCurrentPlayer) {
    std::cout << ">>> CURRENT PLAYER: " << player.getName() << " <<<"
              << std::endl;
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
  const auto &resources = player.getResources();
  bool first = true;
  for (const auto &[type, amount] : resources) {
    if (amount > 0) {
      if (!first)
        std::cout << ", ";
      std::cout << getResourceTypeName(type) << ": " << amount;
      first = false;
    }
  }
  if (first)
    std::cout << "None";
  std::cout << std::endl;

  // 科学符号
  const auto &scienceSymbols = player.getScienceSymbols();
  if (!scienceSymbols.empty()) {
    std::map<ScienceSymbol, int> counts;
    for (const auto &symbol : scienceSymbols) {
      if (symbol != ScienceSymbol::NONE) {
        counts[symbol]++;
      }
    }

    if (!counts.empty()) {
      std::cout << "Science: ";
      first = true;
      for (const auto &[symbol, count] : counts) {
        if (!first)
          std::cout << ", ";
        std::cout << getScienceSymbolName(symbol) << "(" << count << ")";
        first = false;
      }
      std::cout << std::endl;
    }
  }

  // 建造的卡牌（简要）
  const auto &builtCards = player.getBuiltCards();
  if (!builtCards.empty()) {
    std::cout << "Built Cards: " << builtCards.size() << " cards";
    if (builtCards.size() <= 5) {
      std::cout << " [";
      for (size_t i = 0; i < builtCards.size(); ++i) {
        if (i > 0)
          std::cout << ", ";
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
  const auto &pyramid = board.getPyramid();

  std::cout << "\n=== AVAILABLE CARDS ===" << std::endl;
  bool hasAvailable = false;

  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (board.isCardAccessible(i) && !pyramid.at(i).isTaken) {
      hasAvailable = true;
      std::cout << "[" << i << "] ";
      if (pyramid.at(i).isFaceUp) {
        const Card &card = pyramid.at(i).card;
        std::cout << card.getName() << " (" << getCardTypeName(card.getType())
                  << ")";

        // 显示简要成本
        const Cost &cost = card.getCost();
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
void ConsoleView::printCardDetails(const Card &card) {
  std::cout << "\n" << std::string(40, '=') << std::endl;
  std::cout << "CARD DETAILS" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::cout << "Name: " << card.getName() << std::endl;
  std::cout << "Type: " << getCardTypeName(card.getType()) << std::endl;

  const Cost &cost = card.getCost();
  std::cout << "Cost: " << cost.coins << " coins";
  if (!cost.resources.empty()) {
    std::cout << " + ";
    bool first = true;
    for (const auto &[type, amount] : cost.resources) {
      if (!first)
        std::cout << ", ";
      std::cout << amount << " " << getResourceTypeName(type);
      first = false;
    }
  }
  std::cout << std::endl;

  const Effect &effect = card.getEffect();
  if (effect.victoryPoints > 0)
    std::cout << "Victory Points: " << effect.victoryPoints << std::endl;
  if (effect.militaryShields > 0)
    std::cout << "Military Shields: " << effect.militaryShields << std::endl;
  if (effect.coins > 0)
    std::cout << "Immediate Coins: +" << effect.coins << std::endl;

  if (!effect.resourcesProduced.empty()) {
    std::cout << "Produces: ";
    bool first = true;
    for (const auto &[type, amount] : effect.resourcesProduced) {
      if (!first)
        std::cout << ", ";
      std::cout << amount << " " << getResourceTypeName(type) << "/turn";
      first = false;
    }
    std::cout << std::endl;
  }

  if (!effect.scienceSymbols.empty()) {
    std::cout << "Science: ";
    for (size_t i = 0; i < effect.scienceSymbols.size(); ++i) {
      if (i > 0)
        std::cout << ", ";
      std::cout << getScienceSymbolName(effect.scienceSymbols[i]);
    }
    std::cout << std::endl;
  }

  std::cout << std::string(40, '=') << std::endl;
}

// 8. 行动菜单
void ConsoleView::printActionMenu(bool canBuildCard, bool canBuildWonder,
                                  int discardCoins) {
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
void ConsoleView::printWonders(const std::vector<Wonder *> &wonders,
                               bool showAvailability) {
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

// 10. 复杂玩家状态面板（对应Game.cpp中的长函数） - Now fully implemented
void ConsoleView::printPlayerStatePanel(const Player *currentPlayer,
                                        const Player *otherPlayer) {
  std::cout << "\n";
  std::cout << "==============================================================="
               "=================\n";
  std::cout << "                              PLAYER STATUS PANEL              "
               "                 \n";
  std::cout << "==============================================================="
               "=================\n";

  // 当前玩家的信息
  std::cout << ">>> " << currentPlayer->getName() << "'s Resources <<<\n";

  // 获取卡牌类型统计
  std::map<CardType, int> cardsByType = currentPlayer->getCardsByType();

  // 获取总资源产量
  std::map<ResourceType, int> totalResources =
      currentPlayer->getTotalResourcesFromCards();

  // 一横排显示七个颜色资源的获得情况
  std::cout << "Brown: ";
  if (cardsByType[CardType::RAW_MATERIAL] > 0) {
    // 显示资源类型和数量
    bool firstResource = true;
    if (totalResources[ResourceType::WOOD] > 0) {
      std::cout << "wood " << totalResources[ResourceType::WOOD];
      firstResource = false;
    }
    if (totalResources[ResourceType::CLAY] > 0) {
      if (!firstResource)
        std::cout << ", ";
      std::cout << "clay " << totalResources[ResourceType::CLAY];
      firstResource = false;
    }
    if (totalResources[ResourceType::STONE] > 0) {
      if (!firstResource)
        std::cout << ", ";
      std::cout << "stone " << totalResources[ResourceType::STONE];
      firstResource = false;
    }
    if (firstResource) {
      std::cout << "none";
    }
  } else {
    std::cout << "none";
  }
  std::cout << " | ";

  std::cout << "Grey: ";
  if (cardsByType[CardType::MANUFACTURED_GOOD] > 0) {
    bool firstResource = true;
    if (totalResources[ResourceType::GLASS] > 0) {
      std::cout << "glass " << totalResources[ResourceType::GLASS];
      firstResource = false;
    }
    if (totalResources[ResourceType::PAPER] > 0) {
      if (!firstResource)
        std::cout << ", ";
      std::cout << "paper " << totalResources[ResourceType::PAPER];
      firstResource = false;
    }
    if (firstResource) {
      std::cout << "none";
    }
  } else {
    std::cout << "none";
  }
  std::cout << " | ";

  std::cout << "Blue: ";
  std::cout << cardsByType[CardType::CIVILIAN] << " cards";
  std::cout << " | ";

  std::cout << "Green: ";
  const auto &scienceSymbols = currentPlayer->getScienceSymbols();
  if (!scienceSymbols.empty()) {
    // 统计每个ScienceSymbol的数量
    std::map<ScienceSymbol, int> symbolCounts;
    for (const auto &symbol : scienceSymbols) {
      if (symbol != ScienceSymbol::NONE) {
        symbolCounts[symbol]++;
      }
    }

    if (!symbolCounts.empty()) {
      bool firstSymbol = true;
      for (const auto &[symbol, count] : symbolCounts) {
        if (!firstSymbol)
          std::cout << ", ";

        switch (symbol) {
        case ScienceSymbol::GLOBE:
          std::cout << "Globe";
          break;
        case ScienceSymbol::TABLET:
          std::cout << "Tablet";
          break;
        case ScienceSymbol::GEAR:
          std::cout << "Gear";
          break;
        case ScienceSymbol::COMPASS:
          std::cout << "Compass";
          break;
        case ScienceSymbol::WHEEL:
          std::cout << "Wheel";
          break;
        case ScienceSymbol::MORTAR:
          std::cout << "Mortar";
          break;
        default:
          std::cout << "Unknown";
        }

        if (count > 1) {
          std::cout << "×" << count;
        }
        firstSymbol = false;
      }
    } else {
      std::cout << "no symbols";
    }
  } else {
    std::cout << "no symbols";
  }
  std::cout << " | ";

  std::cout << "Yellow: ";
  std::cout << cardsByType[CardType::COMMERCIAL] << " cards";
  std::cout << " | ";

  std::cout << "Red: ";
  int militaryShields = currentPlayer->getMilitaryPower();
  std::cout << "shields " << militaryShields;
  std::cout << " | ";

  std::cout << "Purple: ";
  std::cout << cardsByType[CardType::GUILD] << " cards";
  std::cout << "\n";

  // 显示其他信息
  std::cout << "\nCoins: " << currentPlayer->getCoins();
  std::cout << " | Military Power: " << currentPlayer->getMilitaryPower();
  std::cout << " | Victory Points: " << currentPlayer->getVictoryPoints();

  // 显示简要的对手信息
  std::cout << "\n\n>>> Opponent (" << otherPlayer->getName() << ") <<<";
  std::cout << " Coins: " << otherPlayer->getCoins();
  std::cout << " | VP: " << otherPlayer->getVictoryPoints();
  std::cout
      << "\n==============================================================="
         "=================\n";
}

// 11. 完整游戏板显示（替代Game::printBoard）
void ConsoleView::printGameBoard(const Game &game) {
  const Board &board = game.getBoard();

  std::cout << "\n====== GAME STATE ======" << std::endl;
  std::cout << "Age: " << game.getCurrentAge()
            << " | Military: " << board.getMilitaryPosition() << std::endl;
  std::cout << game.getPlayer1().getName()
            << " (Coins: " << game.getPlayer1().getCoins() << ") vs "
            << game.getPlayer2().getName()
            << " (Coins: " << game.getPlayer2().getCoins() << ")" << std::endl;
  std::cout << "\nPYRAMID:" << std::endl;

  // 调用增强版printPyramid，传入age
  ConsoleView::printPyramid(board, game.getCurrentAge());

  std::cout << "\n* = Accessible (can be picked)" << std::endl;
  std::cout << "========================\n" << std::endl;
}

// === Input Methods ===
std::string ConsoleView::getLineInput() {
  std::string input;
  std::getline(std::cin, input);
  return input;
}

// === Implement Notifications ===
void ConsoleView::notifyAgeStart(int age) {
  std::cout << "\n========================================" << std::endl;
  std::cout << "          AGE " << age << " BEGINS!" << std::endl;
  std::cout << "========================================\n" << std::endl;
}

void ConsoleView::notifyTurnStart(const std::string &playerName) {
  std::cout << "\n--- " << playerName << "'s Turn ---" << std::endl;
}

void ConsoleView::notifyInvalidDecision() {
  std::cout << "Invalid decision. Skipping turn." << std::endl;
}

void ConsoleView::notifyBuildSuccess(const std::string &playerName,
                                     const std::string &cardName, int cost,
                                     bool extraTurn) {
  std::cout << playerName << " built " << cardName << " for " << cost
            << " coins.";
  if (extraTurn) {
    std::cout << " (Takes another turn)";
  }
  std::cout << std::endl;
}

void ConsoleView::notifyDiscard(const std::string &playerName,
                                const std::string &cardName, int coins) {
  std::cout << playerName << " discarded " << cardName << " for " << coins
            << " coins." << std::endl;
}

void ConsoleView::notifyWonderBuilt(const std::string &playerName,
                                    const std::string &wonderName,
                                    const std::string &effectDesc,
                                    bool extraTurn) {
  std::cout << "\n>>> " << playerName << " built wonder " << wonderName << "!";
  if (extraTurn) {
    std::cout << " (Takes another turn)";
  }
  std::cout << std::endl;
  std::cout << "    Effect: " << effectDesc << std::endl;
}

void ConsoleView::notifyScientificVictory(const std::string &playerName) {
  std::cout << "\n*** " << playerName << " wins by Scientific Victory! ***"
            << std::endl;
}

void ConsoleView::notifyMilitaryVictory(const std::string &playerName) {
  std::cout << "\n*** " << playerName << " wins by Military Victory! ***"
            << std::endl;
}

void ConsoleView::notifyInsufficientFunds(const std::string &playerName,
                                          const std::string &itemName,
                                          int coinsGained) {
  std::cout << playerName << " could not afford " << itemName
            << " and discarded it for " << coinsGained << " coins."
            << std::endl;
}

void ConsoleView::notifyExtraTurn(const std::string &playerName) {
  std::cout << "Extra turn granted! " << playerName << " will play again."
            << std::endl;
}

void ConsoleView::notifyGameEnd(const std::string &winner,
                                const std::string &reason) {
  std::cout << "\nGAME OVER: " << winner << " wins by " << reason << "!"
            << std::endl;
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

void ConsoleView::printPlayerWonderCollection(
    const Player &player, const std::vector<Wonder *> &wonders) {
  std::cout << "========================================" << std::endl;
  std::cout << player.getName() << "'s Wonders:" << std::endl;
  for (size_t i = 0; i < wonders.size(); ++i) {
    std::cout << "  " << i + 1 << ". " << wonders[i]->getName() << std::endl;
  }
  std::cout << "========================================\n" << std::endl;
}

// === Progress Token Helpers ===
void ConsoleView::printProgressTokens(
    const std::vector<ProgressToken> &tokens) {
  std::cout << "\n=== AVAILABLE PROGRESS TOKENS ===" << std::endl;
  for (size_t i = 0; i < tokens.size(); ++i) {
    std::cout << i << ". " << tokens[i].getDescription()
              << " (Base VP: " << tokens[i].getBaseVictoryPoints() << ")"
              << std::endl;
  }
  std::cout << "=================================\n" << std::endl;
}

int ConsoleView::promptProgressTokenSelection(
    const std::vector<ProgressToken> &tokens) {
  printProgressTokens(tokens);
  int choice = -1;
  while (true) {
    std::cout << "Select a Progress Token (0-" << tokens.size() - 1 << "): ";
    if (std::cin >> choice) {
      if (choice >= 0 && choice < (int)tokens.size()) {
        break;
      }
    } else {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
    }
  }
  return choice;
}