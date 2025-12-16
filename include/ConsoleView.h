#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <map>
#include <string>
#include <vector>

class Game;
class Player;
class Card;
class Wonder;
enum class ScienceSymbol;

class ConsoleView {
public:
  static void printBoard(const Game &game);
  static void printPlayerStatus(const Player &player, bool isCurrentPlayer);
  static void printMilitaryStatus(int position);
  static void printActionMenu(bool canBuildCard, bool canBuildWonder,
                              int discardCoins);
  static void printError(const std::string &message);
  static void printInfo(const std::string &message);
};

#endif // CONSOLEVIEW_H