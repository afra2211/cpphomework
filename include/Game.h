#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Deck.h"
#include "Player.h"
#include <deque>
#include <memory>
#include <string>
#include <vector>

class Game {
public:
  Game();

  void init(std::string p1Name, bool p1IsAI, std::string p2Name, bool p2IsAI);
  void start(); // Console game loop

  // Public for testing/AI access
  const Board &getBoard() const;
  const Player &getPlayer1() const;
  const Player &getPlayer2() const;
  int getCurrentPlayerIndex() const;
  int getCurrentAge() const;
  bool isGameOver() const;

  // API Methods
  bool playMove(int cardIndex);
  // We will use a helper to construct JSON in main.cpp or here.
  // Let's keep it simple and just expose necessary getters.

private:
    Player *p1;
    Player *p2;
    Board board;
    std::vector<Card> discardPile;     // 弃牌堆
    int bankCoins;                     // 银行金币（抽象为充足的金库）
    std::vector<ProgressToken>
        removedProgressTokens; // 被移除的进步标记池
    std::vector<ProgressToken>
        claimedProgressTokens; // 已被玩家获得的进步标记记录
    int builtWonderCount;       // 已建奇观数量（全局计数）
    bool eighthWonderRemoved;   // 是否已移除第8座奇观
    bool extraTurnPending;      // 当前玩家是否拥有额外回合
    int currentPlayerIndex; // 0 or 1
    int currentAge;
    bool gameOver;

    // 奇迹相关的成员变量
    std::vector<Wonder> allWonders; // 所有12个奇迹
    std::vector<Wonder*> player1Wonders; // 玩家1的奇迹
    std::vector<Wonder*> player2Wonders;


    void playAge(int age);   // Setup age
    void playTurn();         // Play one turn (console input or AI)
    void printBoard() const; // Display ASCII board
    void switchTurn();
    bool checkVictory();
    void handleMilitaryConflict();

    void setupWonders();
    void selectWondersRound(std::vector<Wonder*>& availableWonders, int round);
    int getPlayerWonderChoice(int min, int max, const std::string& prompt, Player* player);

  // 辅助函数
    void shuffleWonders(std::vector<Wonder*>& wonders);
    void printPlayerStatePanel(Player* currentPlayer, Player* otherPlayer);
  // Helper to get input (from console or AI)
  // void processPlayerMove(Player &player); // Removed
};

#endif // GAME_H
