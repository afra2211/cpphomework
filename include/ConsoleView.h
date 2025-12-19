#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <map>
#include <string>
#include <vector>

// 前向声明（告诉编译器这些是类，但不用包含完整文件）
class Game;
class Board;
class Player;
class Card;

class ConsoleView {
public:
  // === 核心显示函数 ===
  
  // 1. 完整游戏状态显示
  static void printGameState(const Game &game);
  
  // 2. 游戏板显示（金字塔布局）
  static void printBoard(const Game &game);
  
  // 3. 金字塔布局显示（独立的金字塔渲染）
  static void printPyramid(const Board &board);
  
  // 4. 玩家状态显示
  static void printPlayerStatus(const Player &player, bool isCurrentPlayer);
  
  // 5. 军事状态显示
  static void printMilitaryStatus(int position);
  
  // 6. 可用卡牌列表
  static void printAvailableCards(const Board &board);
  
  // 7. 卡牌详细信息
  static void printCardDetails(const Card &card);
  
  // 8. 行动菜单
  static void printActionMenu(bool canBuildCard, bool canBuildWonder,
                              int discardCoins);
  
  // === 消息输出函数 ===
  static void printError(const std::string &message);
  static void printInfo(const std::string &message);
  static void printSuccess(const std::string &message);
  
  // === 辅助显示函数 ===
  static void printPlayerStatePanel(const Player *currentPlayer, const Player *otherPlayer);
  
  // === 新增：奇迹显示 ===
  static void printWonders(const std::vector<class Wonder*> &wonders, bool showAvailability = true);
};

#endif // CONSOLEVIEW_H