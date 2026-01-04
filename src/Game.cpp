#include "Game.h"
#include "AIPlayer.h"
#include "ConsoleView.h"
#include "HumanPlayer.h"
#include <algorithm>
#include <chrono> // Added for system_clock
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

// Note: iostream and iomanip removed to enforce MVC separation

Game::Game()
    : p1(nullptr), p2(nullptr), bankCoins(0), builtWonderCount(0),
      eighthWonderRemoved(false), extraTurnPending(false),
      currentPlayerIndex(0), lastActionPlayerIndex(0), currentAge(1),
      gameOver(false) {}

Game::~Game() {
  delete p1;
  delete p2;
}

void Game::init(std::string p1Name, bool p1IsAI, std::string p2Name,
                bool p2IsAI) {
  p1 = p1IsAI ? static_cast<Player *>(new AIPlayer(p1Name))
              : static_cast<Player *>(new HumanPlayer(p1Name));
  p2 = p2IsAI ? static_cast<Player *>(new AIPlayer(p2Name))
              : static_cast<Player *>(new HumanPlayer(p2Name));
  board = Board();
  discardPile.clear();
  bankCoins = 0; // 抽象银行：默认不封顶
  removedProgressTokens.clear();
  claimedProgressTokens.clear();
  builtWonderCount = 0;
  eighthWonderRemoved = false;
  extraTurnPending = false;
  currentPlayerIndex = 0;
  lastActionPlayerIndex = 0;
  currentAge = 1;
  gameOver = false;

  board.setupProgressTokens();
  // 冲突棋子重置到起始位置
  board.moveMilitary(Board::MILITARY_START_POSITION -
                     board.getMilitaryPosition());

  // Military Victory Setup
  // Scientific Victory Setup
  p1->addScienceSymbol(ScienceSymbol::GLOBE);
  p1->addScienceSymbol(ScienceSymbol::TABLET);
  p1->addScienceSymbol(ScienceSymbol::GEAR);
  p1->addScienceSymbol(ScienceSymbol::COMPASS);
  p1->addScienceSymbol(ScienceSymbol::WHEEL);
  // Just need 1 more for victory (MORTAR)
  // p1->addCoins(50);
  // p2->addCoins(50);

  setupWonders();
}

void Game::setupWonders() {
  ConsoleView::printSectionHeader("WONDER SELECTION PHASE");

  // 获取所有12个奇迹
  allWonders = Deck::getWonders();

  // 创建指针向量以便随机选择
  std::vector<Wonder *> allWonderPtrs;
  for (auto &wonder : allWonders) {
    allWonderPtrs.push_back(&wonder);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(allWonderPtrs.begin(), allWonderPtrs.end(),
               std::default_random_engine(seed));

  // 第一次选择：从12个中随机选4个
  ConsoleView::printSubHeader("ROUND 1: Selecting 4 wonders from 12");
  std::vector<Wonder *> round1Wonders;
  for (int i = 0; i < 4; ++i) {
    round1Wonders.push_back(allWonderPtrs[i]);
  }

  // 随机决定第一轮玩家顺序
  currentPlayerIndex = rand() % 2;                // 0或1
  int firstRoundFirstPlayer = currentPlayerIndex; // 保存第一轮先手玩家

  Player *firstPlayerRound1 = (currentPlayerIndex == 0) ? p1 : p2;
  // Player *secondPlayerRound1 = (currentPlayerIndex == 0) ? p2 : p1; // Unused
  // variable

  ConsoleView::printPlain("\nRandomly deciding player order...");
  ConsoleView::printPlain(firstPlayerRound1->getName() +
                          " will choose first in Round 1!\n");

  // 第一轮奇迹选择
  selectWondersRound(round1Wonders, 1);

  // 从剩下的8个中移除已选择的4个
  allWonderPtrs.erase(allWonderPtrs.begin(), allWonderPtrs.begin() + 4);

  // 再次随机打乱剩下的8个
  std::shuffle(allWonderPtrs.begin(), allWonderPtrs.end(),
               std::default_random_engine(seed + 1));

  // 第二次选择：从剩下的8个中随机选4个
  ConsoleView::printSubHeader(
      "ROUND 2: Selecting 4 more wonders from remaining 8");
  std::vector<Wonder *> round2Wonders;
  for (int i = 0; i < 4; ++i) {
    round2Wonders.push_back(allWonderPtrs[i]);
  }

  // 第二轮交换玩家顺序：第一轮先手玩家变成后手，后手玩家变成先手
  ConsoleView::printPlain("\nSwapping player order for Round 2...");
  // Force Player 1 start
  currentPlayerIndex = 0; // 交换0和1

  Player *firstPlayerRound2 = (currentPlayerIndex == 0) ? p1 : p2;
  ConsoleView::printPlain(firstPlayerRound2->getName() +
                          " will choose first in Round 2!\n");

  // 第二轮奇迹选择（现在玩家顺序已经交换）
  selectWondersRound(round2Wonders, 2);

  // 设置玩家的奇迹
  std::vector<Wonder *> boardWonders;
  for (auto wonderPtr : player1Wonders) {
    boardWonders.push_back(wonderPtr);
  }
  for (auto wonderPtr : player2Wonders) {
    boardWonders.push_back(wonderPtr);
  }
  board.setAvailableWonders(boardWonders);

  // 显示最终结果
  ConsoleView::printSectionHeader("WONDER SELECTION COMPLETE");
  ConsoleView::printPlayerWonderCollection(*p1, player1Wonders);
  ConsoleView::printPlayerWonderCollection(*p2, player2Wonders);

  // 根据7 Wonders Duel规则，奇迹选择阶段第一轮的先手玩家在游戏开始时先手
  currentPlayerIndex = firstRoundFirstPlayer;
  ConsoleView::printPlain(
      "\nGame will start with " +
      (currentPlayerIndex == 0 ? p1->getName() : p2->getName()) +
      " (first to choose in Round 1)!\n");

  ConsoleView::printPrompt(
      "Press Enter to continue to the game (or type 'exit' to quit)...");

  std::string continueInput = ConsoleView::getLineInput();

  // 检查是否输入了exit
  if (continueInput == "exit" || continueInput == "EXIT" ||
      continueInput == "Exit") {
    ConsoleView::printPlain("\nPlayer has chosen to exit the game. Goodbye!");
    exit(0);
  }
}

void Game::selectWondersRound(std::vector<Wonder *> &availableWonders,
                              int round) {
  ConsoleView::printSectionHeader("WONDER SELECTION - ROUND " +
                                  std::to_string(round));

  // 显示所有可用的奇迹
  ConsoleView::printNumberedWonderList(availableWonders,
                                       "Available wonders:\n");

  // 确定当前玩家和对手
  Player *firstPlayer = (currentPlayerIndex == 0) ? p1 : p2;
  Player *secondPlayer = (currentPlayerIndex == 0) ? p2 : p1;
  std::vector<Wonder *> &firstPlayerWonders =
      (currentPlayerIndex == 0) ? player1Wonders : player2Wonders;
  std::vector<Wonder *> &secondPlayerWonders =
      (currentPlayerIndex == 0) ? player2Wonders : player1Wonders;

  // 根据先手玩家类型采取不同的选择流程
  if (firstPlayer->isAIPlayer()) {
    // 情况：先手玩家是AI

    // 1. AI先选择一张奇迹
    ConsoleView::printPlain(">>> " + firstPlayer->getName() +
                            " (AI) chooses first <<<");
    int aiChoice =
        getPlayerWonderChoice(1, availableWonders.size(), "", firstPlayer);
    ConsoleView::printPlain(firstPlayer->getName() + " (AI) selected: " +
                            availableWonders[aiChoice - 1]->getName() + "\n");

    firstPlayerWonders.push_back(availableWonders[aiChoice - 1]);
    availableWonders.erase(availableWonders.begin() + aiChoice - 1);

    // 显示AI的奇迹区
    ConsoleView::printPlayerWonderCollection(*firstPlayer, firstPlayerWonders);

    // 2. 显示剩下的三张奇迹给人类玩家选择
    ConsoleView::printNumberedWonderList(
        availableWonders,
        "Remaining wonders for " + secondPlayer->getName() + " (Human):\n");

    ConsoleView::printPlain(">>> " + secondPlayer->getName() +
                            "'s turn (Human) <<<");
    ConsoleView::printPlain("Please choose 2 wonders from the remaining 3.");

    // 人类玩家选择第一张奇迹
    int humanChoice1 = getPlayerWonderChoice(
        1, availableWonders.size(),
        "Enter the number of your first wonder: ", secondPlayer);
    ConsoleView::printPlain(
        "\n" + secondPlayer->getName() +
        " selected: " + availableWonders[humanChoice1 - 1]->getName() + "\n");

    secondPlayerWonders.push_back(availableWonders[humanChoice1 - 1]);
    availableWonders.erase(availableWonders.begin() + humanChoice1 - 1);

    // 显示人类玩家的奇迹区
    ConsoleView::printPlayerWonderCollection(*secondPlayer,
                                             secondPlayerWonders);

    // 显示剩余的奇迹
    ConsoleView::printNumberedWonderList(availableWonders,
                                         "Remaining wonders:\n");

    // 人类玩家选择第二张奇迹
    int humanChoice2 = getPlayerWonderChoice(
        1, availableWonders.size(),
        "Enter the number of your second wonder: ", secondPlayer);
    ConsoleView::printPlain(
        "\n" + secondPlayer->getName() +
        " selected: " + availableWonders[humanChoice2 - 1]->getName() + "\n");

    secondPlayerWonders.push_back(availableWonders[humanChoice2 - 1]);
    availableWonders.erase(availableWonders.begin() + humanChoice2 - 1);

    // 显示人类玩家的最终奇迹区
    ConsoleView::printPlayerWonderCollection(*secondPlayer,
                                             secondPlayerWonders);

    // 3. 最后一张奇迹自动归AI
    ConsoleView::printPlain("The last wonder automatically goes to " +
                            firstPlayer->getName() + " (AI):");
    ConsoleView::printPlain(firstPlayer->getName() + " receives: " +
                            availableWonders[0]->getName() + "\n");

    firstPlayerWonders.push_back(availableWonders[0]);

    // 显示AI的最终奇迹区
    ConsoleView::printPlayerWonderCollection(*firstPlayer, firstPlayerWonders);

  } else {
    // 情况：先手玩家是人类

    // 1. 人类玩家先选择一张奇迹
    ConsoleView::printPlain(">>> " + firstPlayer->getName() +
                            "'s turn (Human) <<<");
    ConsoleView::printPrompt(
        "Please enter the number (1-4) to select your wonder: ");

    int humanChoice = getPlayerWonderChoice(
        1, availableWonders.size(),
        "Enter the number of the wonder you want: ", firstPlayer);

    ConsoleView::printPlain(
        "\n" + firstPlayer->getName() +
        " selected: " + availableWonders[humanChoice - 1]->getName() + "\n");

    firstPlayerWonders.push_back(availableWonders[humanChoice - 1]);
    availableWonders.erase(availableWonders.begin() + humanChoice - 1);

    // 显示人类玩家的奇迹区
    ConsoleView::printPlayerWonderCollection(*firstPlayer, firstPlayerWonders);

    // 2. 显示剩余的奇迹
    ConsoleView::printNumberedWonderList(availableWonders,
                                         "Remaining wonders:\n");

    std::string turnPrefix = ">>> " + secondPlayer->getName() + "'s turn";
    if (secondPlayer->isAIPlayer()) {
      ConsoleView::printPlain(turnPrefix + " (AI) <<<");
      ConsoleView::printPlain(
          "AI will choose 2 wonders from the remaining 3.\n");
    } else {
      ConsoleView::printPlain(turnPrefix + " (Human) <<<");
      ConsoleView::printPlain("Please choose 2 wonders from the remaining 3.");
    }

    // 后手玩家选择第一张奇迹
    int secondChoice1;
    if (secondPlayer->isAIPlayer()) {
      secondChoice1 =
          getPlayerWonderChoice(1, availableWonders.size(), "", secondPlayer);
    } else {
      secondChoice1 = getPlayerWonderChoice(
          1, availableWonders.size(),
          "Enter the number of your first wonder: ", secondPlayer);
    }

    ConsoleView::printPlain(
        secondPlayer->getName() +
        " selected: " + availableWonders[secondChoice1 - 1]->getName() + "\n");

    secondPlayerWonders.push_back(availableWonders[secondChoice1 - 1]);
    availableWonders.erase(availableWonders.begin() + secondChoice1 - 1);

    // 显示后手玩家的奇迹区
    ConsoleView::printPlayerWonderCollection(*secondPlayer,
                                             secondPlayerWonders);

    // 显示剩余的奇迹
    ConsoleView::printNumberedWonderList(availableWonders,
                                         "Remaining wonders:\n");

    // 后手玩家选择第二张奇迹
    int secondChoice2;
    if (secondPlayer->isAIPlayer()) {
      secondChoice2 =
          getPlayerWonderChoice(1, availableWonders.size(), "", secondPlayer);
    } else {
      secondChoice2 = getPlayerWonderChoice(
          1, availableWonders.size(),
          "Enter the number of your second wonder: ", secondPlayer);
    }

    ConsoleView::printPlain(
        secondPlayer->getName() +
        " selected: " + availableWonders[secondChoice2 - 1]->getName() + "\n");

    secondPlayerWonders.push_back(availableWonders[secondChoice2 - 1]);
    availableWonders.erase(availableWonders.begin() + secondChoice2 - 1);

    // 显示后手玩家的最终奇迹区
    ConsoleView::printPlayerWonderCollection(*secondPlayer,
                                             secondPlayerWonders);

    // 3. 最后一张奇迹自动归先手玩家
    ConsoleView::printPlain("The last wonder automatically goes to " +
                            firstPlayer->getName() + ":");
    ConsoleView::printPlain(firstPlayer->getName() + " receives: " +
                            availableWonders[0]->getName() + "\n");

    firstPlayerWonders.push_back(availableWonders[0]);

    // 显示先手玩家的最终奇迹区
    ConsoleView::printPlayerWonderCollection(*firstPlayer, firstPlayerWonders);
  }

  // 显示本轮选择后的结果总结
  ConsoleView::printSubHeader("ROUND " + std::to_string(round) +
                              " SELECTION COMPLETE");
  ConsoleView::printPlain(p1->getName() + " now has " +
                          std::to_string(player1Wonders.size()) + " wonders.");
  ConsoleView::printPlain(p2->getName() + " now has " +
                          std::to_string(player2Wonders.size()) +
                          " wonders.\n");
}

int Game::getPlayerWonderChoice(int min, int max, const std::string &prompt,
                                Player *player) {
  int choice = 0;

  // 如果是AI玩家，使用AI逻辑选择
  if (player->isAIPlayer()) {
    // 随机选择
    choice = (rand() % max) + min;
    return choice;
  }

  // 人类玩家选择
  while (true) {
    if (!prompt.empty()) {
      ConsoleView::printPrompt(prompt);
    }

    std::string input = ConsoleView::getLineInput();

    if (input == "exit" || input == "EXIT" || input == "Exit") {
      ConsoleView::printPlain("\nPlayer has chosen to exit the game. Goodbye!");
      exit(0); // 退出程序
    }

    try {
      choice = std::stoi(input);

      if (choice < min || choice > max) {
        ConsoleView::printPlain(
            "Invalid choice. Please enter a number between " +
            std::to_string(min) + " and " + std::to_string(max) + ".");
      } else {
        return choice;
      }
    } catch (const std::invalid_argument &e) {
      ConsoleView::printPlain("Invalid input. Please enter a number between " +
                              std::to_string(min) + " and " +
                              std::to_string(max) +
                              ", or type 'exit' to quit.");
    } catch (const std::out_of_range &e) {
      ConsoleView::printPlain(
          "Number out of range. Please enter a number between " +
          std::to_string(min) + " and " + std::to_string(max) + ".");
    }
  }
}

void Game::start() {
  // Main game loop
  int age = 1;
  while (age <= 3 && !gameOver) {
    playAge(age);

    bool ageComplete = false;
    while (!gameOver) {
      // Display board
      printBoard();

      // Check if pyramid is empty
      bool empty = true;
      for (const auto &pair : board.getPyramid()) {
        const auto &slot = pair.second;
        if (!slot.isTaken) {
          empty = false;
          break;
        }
      }

      if (empty) {
        ageComplete = true;
        break; // Age complete
      }

      // Play turn
      playTurn();
    }

    if (gameOver) {
      break;
    }

    if (!ageComplete) {
      // 游戏提前结束或异常直接退出循环
      break;
    }

    if (age == 3) {
      break; // 所有时代结束
    }

    // 决定下一时代先手
    int militaryPos = board.getMilitaryPosition();
    if (militaryPos > 0) {
      // 冲突棋子靠近玩家2一侧 -> 玩家2决定先手，这里直接让玩家2先手
      currentPlayerIndex = 1;
    } else if (militaryPos < 0) {
      // 冲突棋子靠近玩家1一侧 -> 玩家1决定先手
      currentPlayerIndex = 0;
    } else {
      // 冲突棋子在中立格，上一回合行动玩家决定
      currentPlayerIndex = lastActionPlayerIndex;
    }

    age += 1;
  }

  // Final scoring
  if (!gameOver) {
    checkVictory();
  }
}

void Game::playAge(int age) {
  currentAge = age;
  ConsoleView::notifyAgeStart(age);

  std::vector<Card> deck;
  std::vector<Card> removed;
  if (age == 1)
    deck = Deck::getAge1Deck(false, &removed);
  else if (age == 2)
    deck = Deck::getAge2Deck(false, &removed);
  else
    deck = Deck::getAge3Deck(false, &removed);

  board.setupAge(age, deck, removed);
}

void Game::playTurn() {
  Player *currentPlayer = (currentPlayerIndex == 0) ? p1 : p2;
  Player *otherPlayer = (currentPlayerIndex == 0) ? p2 : p1;
  bool grantExtraTurn = false;
  int actingPlayerIndex = currentPlayerIndex;

  // Use ConsoleView for the player panel
  ConsoleView::printPlayerStatePanel(currentPlayer, otherPlayer);

  ConsoleView::notifyTurnStart(currentPlayer->getName());

  Decision decision = currentPlayer->makeDecision(*this);

  const auto &pyramid = board.getPyramid();

  if (decision.action == DecisionAction::EXIT ||
      pyramid.find(decision.cardIndex) == pyramid.end() ||
      !board.isCardAccessible(decision.cardIndex) ||
      pyramid.at(decision.cardIndex).isTaken) {
    ConsoleView::notifyInvalidDecision();
    switchTurn();
    return;
  }
  Card card = board.takeCard(decision.cardIndex);

  if (decision.action == DecisionAction::BUILD_CARD) {
    bool canBuild =
        currentPlayer->canAfford(card.getCost(), *otherPlayer,
                                 card.getChainTarget(), card.getType(), false);
    if (!canBuild) {
      int coinsGained = currentPlayer->discardForCoins();
      discardPile.push_back(card);
      ConsoleView::notifyInsufficientFunds(currentPlayer->getName(),
                                           card.getName(), coinsGained);
    } else {
      int actualCost = currentPlayer->calculateCost(
          card.getCost(), *otherPlayer, card.getChainTarget(), card.getType(),
          false);

      // Calculate trade cost for Economy token
      int tradeCost = currentPlayer->calculateTradeCost(
          actualCost, card.getCost(), *otherPlayer, card.getChainTarget(),
          card.getType(), false);

      currentPlayer->payCost(actualCost);
      // Removed duplicate payCost call

      // Economy Effect: Opponent gains trade money
      if (tradeCost > 0 &&
          otherPlayer->hasProgressToken(ProgressTokenType::ECONOMY)) {
        otherPlayer->addCoins(tradeCost);
        ConsoleView::printPlain(otherPlayer->getName() + " gained " +
                                std::to_string(tradeCost) +
                                " coins due to Economy.");
      }

      // Urbanism Effect: Gain 4 coins if constructed via chain
      // Check if chain was actually used (cost 0 and valid chain match)
      // Note: calculateCost returns 0 if chain matched.
      if (actualCost == 0 && !card.getChainTarget().empty()) {
        // Verify player has the symbol (calculateCost already did, but we need
        // to confirm for Urbanism trigger)
        bool hasSymbol = false;
        for (const auto &built : currentPlayer->getBuiltCards()) {
          if (built.getChainSymbol() == card.getChainTarget()) {
            hasSymbol = true;
            break;
          }
        }
        if (hasSymbol &&
            currentPlayer->hasProgressToken(ProgressTokenType::URBANISM)) {
          currentPlayer->addCoins(4);
          ConsoleView::printPlain(
              currentPlayer->getName() +
              " gained 4 coins from Urbanism (Chain Build).");
        }
      }

      currentPlayer->buildCard(card);

      const Effect &e = card.getEffect();
      grantExtraTurn = e.playAgain;
      ConsoleView::notifyBuildSuccess(currentPlayer->getName(), card.getName(),
                                      actualCost, grantExtraTurn);
      if (e.militaryShields > 0) {
        if (currentPlayer == p1) {
          board.moveMilitary(e.militaryShields, currentPlayer, otherPlayer);
        } else {
          board.moveMilitary(-e.militaryShields, currentPlayer, otherPlayer);
        }
        handleMilitaryConflict();
        if (gameOver) {
          return;
        }
      }

      if (board.onSciencePair(*currentPlayer)) {
        ConsoleView::notifyScientificVictory(currentPlayer->getName());
        gameOver = true;
        return;
      }
    }
  } else if (decision.action == DecisionAction::DISCARD) {
    int coinsGained = currentPlayer->discardForCoins();
    discardPile.push_back(card);
    ConsoleView::notifyDiscard(currentPlayer->getName(), card.getName(),
                               coinsGained);
  } else if (decision.action == DecisionAction::BUILD_WONDER) {
    const auto &availableWonders = board.getAvailableWonders();
    if (decision.wonderIndex >= 0 &&
        decision.wonderIndex < (int)availableWonders.size() &&
        availableWonders[decision.wonderIndex] != nullptr &&
        !availableWonders[decision.wonderIndex]->isBuilt()) {
      Wonder *targetWonder = availableWonders[decision.wonderIndex];

      // === Validation: Ensure player owns this wonder ===
      bool ownsWonder = false;
      const std::vector<Wonder *> &playerWonders =
          (currentPlayer == p1) ? player1Wonders : player2Wonders;
      for (const auto *w : playerWonders) {
        if (w == targetWonder) {
          ownsWonder = true;
          break;
        }
      }

      if (!ownsWonder) {
        int coinsGained = currentPlayer->discardForCoins();
        discardPile.push_back(card);
        ConsoleView::printPlain("Invalid Move: You do not own this wonder!");
        ConsoleView::notifyInsufficientFunds(currentPlayer->getName(),
                                             "Wonder (Not Owned - Discarded)",
                                             coinsGained);
      } else if (currentPlayer->canAfford(targetWonder->getCost(), *otherPlayer,
                                          "", CardType::CIVILIAN,
                                          true)) { // isWonder=true
        int actualCost =
            currentPlayer->calculateCost(targetWonder->getCost(), *otherPlayer,
                                         "", CardType::CIVILIAN, true);

        // Calculate trade cost for Economy
        int tradeCost = currentPlayer->calculateTradeCost(
            actualCost, targetWonder->getCost(), *otherPlayer, "",
            CardType::CIVILIAN, true);

        currentPlayer->payCost(actualCost);

        if (tradeCost > 0 &&
            otherPlayer->hasProgressToken(ProgressTokenType::ECONOMY)) {
          otherPlayer->addCoins(tradeCost);
          ConsoleView::printPlain(otherPlayer->getName() + " gained " +
                                  std::to_string(tradeCost) +
                                  " coins due to Economy.");
        }

        Effect wonderEffect = currentPlayer->buildWonder(*targetWonder);

        applyWonderEffect(wonderEffect, currentPlayer, otherPlayer);
        handleSeventhWonderBuilt();
        grantExtraTurn = grantExtraTurn || wonderEffect.playAgain;
        ConsoleView::notifyWonderBuilt(currentPlayer->getName(),
                                       targetWonder->getName(), card.getName(),
                                       grantExtraTurn);

        if (board.onSciencePair(*currentPlayer)) {
          ConsoleView::notifyScientificVictory(currentPlayer->getName());
          gameOver = true;
          return;
        }
      } else {
        int coinsGained = currentPlayer->discardForCoins();
        discardPile.push_back(card);
        ConsoleView::notifyInsufficientFunds(currentPlayer->getName(), "wonder",
                                             coinsGained);
      }
    }
  }

  // 记录本回合的行动玩家以用于下一时代先手判定
  lastActionPlayerIndex = actingPlayerIndex;

  // Check victory
  if (gameOver) {
    return;
  }
  if (checkVictory()) {
    gameOver = true;
    return;
  }

  extraTurnPending = grantExtraTurn;
  if (extraTurnPending) {
    ConsoleView::notifyExtraTurn(currentPlayer->getName());
    extraTurnPending = false; // 消耗本次额外回合
  } else {
    switchTurn();
  }
}

void Game::printBoard() const { ConsoleView::printGameBoard(*this); }

// printPlayerStatePanel implementation removed (Moved to ConsoleView)
void Game::printPlayerStatePanel(Player *currentPlayer, Player *otherPlayer) {
  ConsoleView::printPlayerStatePanel(currentPlayer, otherPlayer);
}

// printPlayerStatePanel implementation removed (Moved to ConsoleView)

const Board &Game::getBoard() const { return board; }
const Player &Game::getPlayer1() const { return *p1; }
const Player &Game::getPlayer2() const { return *p2; }
int Game::getCurrentPlayerIndex() const { return currentPlayerIndex; }
int Game::getCurrentAge() const { return currentAge; }
bool Game::isGameOver() const { return gameOver; }

void Game::switchTurn() { currentPlayerIndex = 1 - currentPlayerIndex; }

bool Game::checkVictory() {
  // Military victory
  int milPos = board.getMilitaryPosition();
  if (milPos >= 9) {
    ConsoleView::notifyMilitaryVictory(p1->getName());
    gameOver = true;
    return true;
  }
  if (milPos <= -9) {
    ConsoleView::notifyMilitaryVictory(p2->getName());
    gameOver = true;
    return true;
  }

  // Scientific victory - check if player has 6 different science symbols
  // Player 1 check
  std::map<ScienceSymbol, int> p1ScienceCounts = p1->getScienceSymbolCounts();
  int p1UniqueSymbols = 0;
  for (const auto &[symbol, count] : p1ScienceCounts) {
    if (count > 0) {
      p1UniqueSymbols++;
    }
  }
  if (p1UniqueSymbols >= 6) {
    ConsoleView::notifyScientificVictory(p1->getName());
    gameOver = true;
    return true;
  }

  // Player 2 check
  std::map<ScienceSymbol, int> p2ScienceCounts = p2->getScienceSymbolCounts();
  int p2UniqueSymbols = 0;
  for (const auto &[symbol, count] : p2ScienceCounts) {
    if (count > 0) {
      p2UniqueSymbols++;
    }
  }
  if (p2UniqueSymbols >= 6) {
    ConsoleView::notifyScientificVictory(p2->getName());
    gameOver = true;
    return true;
  }

  // Final civilian scoring only triggers when Age III pyramid is empty
  if (currentAge < 3 || !isPyramidEmpty()) {
    return false;
  }

  ScoreBreakdown p1Score = calculateFinalScore(*p1, *p2);
  ScoreBreakdown p2Score = calculateFinalScore(*p2, *p1);

  // Note: Detailed score printing should theoretically move to View,
  // but for now we replace std::cout to maintain compilation.
  // Ideally ConsoleView should have printScoreBreakdown.
  // We'll use a simple notification for the winner to avoid complex refactoring
  // mid-execution.

  std::string winnerName = "Draw";
  std::string reason = "Civilian Points";

  int totalP1 = p1Score.total();
  int totalP2 = p2Score.total();

  if (totalP1 > totalP2) {
    winnerName = p1->getName();
    reason = "Civilian Victory (" + std::to_string(totalP1) + " vs " +
             std::to_string(totalP2) + ")";
  } else if (totalP2 > totalP1) {
    winnerName = p2->getName();
    reason = "Civilian Victory (" + std::to_string(totalP2) + " vs " +
             std::to_string(totalP1) + ")";
  } else {
    // Tie-breaker: compare blue card victory points
    if (p1Score.blue > p2Score.blue) {
      winnerName = p1->getName();
      reason = "Civilian Victory (Blue tie-breaker)";
    } else if (p2Score.blue > p1Score.blue) {
      winnerName = p2->getName();
      reason = "Civilian Victory (Blue tie-breaker)";
    } else {
      winnerName = "Draw";
      reason = "Tie";
    }
  }

  ConsoleView::notifyGameEnd(winnerName, reason);

  gameOver = true;
  return true;
}

void Game::handleMilitaryConflict() {
  int pos = board.getMilitaryPosition();
  ConsoleView::printMilitaryStatus(pos);

  if (pos >= 9) {
    gameOver = true;
    ConsoleView::notifyMilitaryVictory(p1->getName());
  } else if (pos <= -9) {
    gameOver = true;
    ConsoleView::notifyMilitaryVictory(p2->getName());
  }
}

void Game::applyWonderEffect(const Effect &effect, Player *owner,
                             Player *opponent) {
  if (!owner) {
    return;
  }

  owner->addVictoryPoints(effect.victoryPoints);
  owner->addCoins(effect.coins);

  // 军事移动
  if (effect.militaryShields > 0) {
    if (owner == p1) {
      board.moveMilitary(effect.militaryShields, owner, opponent);
    } else {
      board.moveMilitary(-effect.militaryShields, owner, opponent);
    }
    handleMilitaryConflict();
    if (gameOver) {
      return;
    }
  }

  for (const auto &[type, amount] : effect.resourcesProduced) {
    owner->addResource(type, amount);
  }

  for (const auto &symbol : effect.scienceSymbols) {
    owner->addScienceSymbol(symbol);
  }

  if (effect.destroyOpponentCard && opponent) {
    opponent->removeLastBuiltCard();
  }

  if (effect.destroyOpponentBrown && opponent) {
    opponent->removeBuiltCardByType(CardType::RAW_MATERIAL);
  }

  if (effect.destroyOpponentGrey && opponent) {
    opponent->removeBuiltCardByType(CardType::MANUFACTURED_GOOD);
  }

  if (effect.buildFromDiscard && !discardPile.empty()) {
    // Mausoleum: Allow player to choose from discard pile
    if (owner->isAIPlayer()) {
      // AI simplified logic: Pick the last one (simplest) or highest VP
      Card cardToBuild = discardPile.back();
      // Ideally AI should evaluate, but for now simple fallback
      // We need to find the card in discardPile and remove it
      // The original logic just popped back, which assumes taking the top.
      // But for Mausoleum, you fetch the whole pile.
      // Let's stick to simple "take last" for AI for now.
      discardPile.pop_back();
      owner->buildCard(cardToBuild);
    } else {
      // Human: Show UI
      Card chosenCard = chooseCardFromDiscard(owner);
      // chooseCardFromDiscard manages removing from discardPile inside
      owner->buildCard(chosenCard);
    }
  }

  if (effect.opponentCoinsLoss > 0 && opponent) {
    int loss = effect.opponentCoinsLoss;
    int removed = opponent->getCoins() < loss ? opponent->getCoins() : loss;
    opponent->removeCoins(removed);
    ConsoleView::printPlain(opponent->getName() + " lost " +
                            std::to_string(removed) +
                            " coins due to wonder effect.");
  }

  if (effect.gainProgressToken) {
    const auto &tokens = board.getAvailableProgressTokens();

    // Check for Great Library "box" effect first
    if (effect.gainProgressTokenFromBox) {
      if (owner->isAIPlayer()) {
        // AI: Pick random if available
        const auto &removed = board.getRemovedProgressTokens();
        if (!removed.empty()) {
          ProgressToken t = board.takeRemovedProgressToken(0);
          owner->addProgressToken(t);
        }
      } else {
        // Human: Pick from box
        ProgressToken t = chooseProgressTokenFromBox(owner);
        // Logic inside checks valid token
        if (t.getType() != ProgressTokenType::AGRICULTURE ||
            t.getDescription() != "None") { // check valid
          owner->addProgressToken(t);
        }
      }
    } else {
      // Law Token or similar (pick from board)
      if (!tokens.empty()) {
        // Should be interactive ideally, but existing logic was auto-take
        // first. Let's keep it simple or make it interactive if we want. For
        // now, existing logic was "take 0".
        // TODO: If this is "Law", it should be interactive.
        // But let's verify if gainProgressToken is used for Law.
        // Update: We are implementing Great Library now.
        ProgressToken token = board.takeProgressToken(0);
        owner->addProgressToken(token);
      }
    }
  }

  builtWonderCount++;
  if (effect.playAgain) {
    extraTurnPending = true;
  }
}

bool Game::isPyramidEmpty() const {
  for (const auto &pair : board.getPyramid()) {
    const auto &slot = pair.second;
    if (!slot.isTaken) {
      return false;
    }
  }
  return true;
}

int Game::calculateColorVictoryPoints(const Player &player,
                                      CardType type) const {
  int points = 0;
  for (const auto &card : player.getBuiltCards()) {
    if (card.getType() == type) {
      points += card.getEffect().victoryPoints;
    }
  }
  return points;
}

int Game::calculateWonderVictoryPoints(const Player &player) const {
  int points = 0;
  for (const auto *wonder : player.getBuiltWonders()) {
    if (wonder) {
      points += wonder->getEffect().victoryPoints;
    }
  }
  return points;
}

int Game::calculateProgressTokenPoints(const Player &player) const {
  int points = 0;
  bool hasMathematics = false;
  const auto &tokens = player.getProgressTokens();

  for (const auto &token : tokens) {
    switch (token.getType()) {
    case ProgressTokenType::AGRICULTURE:
      points += 4;
      break;
    case ProgressTokenType::PHILOSOPHY:
      points += 7;
      break;
    case ProgressTokenType::MATHEMATICS:
      hasMathematics = true;
      break;
    default:
      break;
    }
  }

  if (hasMathematics) {
    points += 3 * static_cast<int>(tokens.size());
  }

  return points;
}

int Game::calculateMilitaryVictoryPoints(bool isPlayerOne) const {
  int pos = board.getMilitaryPosition();
  int distance = isPlayerOne ? pos : -pos;
  if (distance <= 0) {
    return 0;
  }

  if (distance >= 9) {
    return 10;
  }
  if (distance >= 6) {
    return 5;
  }
  if (distance >= 3) {
    return 2;
  }
  return 0;
}

ScoreBreakdown Game::calculateFinalScore(const Player &player,
                                         const Player &opponent) const {
  bool isPlayerOne = (&player == p1);
  ScoreBreakdown score;
  score.military = calculateMilitaryVictoryPoints(isPlayerOne);
  score.blue = calculateColorVictoryPoints(player, CardType::CIVILIAN);
  score.green = calculateColorVictoryPoints(player, CardType::SCIENTIFIC);
  score.yellow = calculateColorVictoryPoints(player, CardType::COMMERCIAL);
  score.purple = calculateGuildVictoryPoints(player, opponent);
  score.wonder = calculateWonderVictoryPoints(player);
  score.progress = calculateProgressTokenPoints(player);
  score.coins = player.getCoins() / 3;

  return score;
}

int Game::calculateGuildVictoryPoints(const Player &player,
                                      const Player &opponent) const {
  int points = 0;
  for (const auto &card : player.getBuiltCards()) {
    if (card.getType() != CardType::GUILD)
      continue;

    // Most guilds reward 1 point per specific card type in the city with the
    // MOST of that type (Player's city OR Opponent's city)
    GuildType type = card.getEffect().guildType;
    if (type == GuildType::NONE)
      continue;

    // Helper lambda to max count
    auto getMaxCount = [&](int pCount, int oCount) {
      return std::max(pCount, oCount);
    };

    switch (type) {
    case GuildType::SHIPOWNERS: // Brown + Grey
    {
      auto pCounts = player.getCardsByType();
      auto oCounts = opponent.getCardsByType();
      int pTotal = pCounts[CardType::RAW_MATERIAL] +
                   pCounts[CardType::MANUFACTURED_GOOD];
      int oTotal = oCounts[CardType::RAW_MATERIAL] +
                   oCounts[CardType::MANUFACTURED_GOOD];
      points += getMaxCount(pTotal, oTotal);
    } break;
    case GuildType::SCIENTISTS: // Green (1 VP)
    {
      int pCount = player.getCardsByType().at(CardType::SCIENTIFIC);
      int oCount = opponent.getCardsByType().at(CardType::SCIENTIFIC);
      points += getMaxCount(pCount, oCount);
    } break;
    case GuildType::TRADERS: // Yellow (1 VP)
    {
      int pCount = player.getCardsByType().at(CardType::COMMERCIAL);
      int oCount = opponent.getCardsByType().at(CardType::COMMERCIAL);
      points += getMaxCount(pCount, oCount);
    } break;
    case GuildType::MAGISTRATES: // Blue (1 VP)
    {
      int pCount = player.getCardsByType().at(CardType::CIVILIAN);
      int oCount = opponent.getCardsByType().at(CardType::CIVILIAN);
      points += getMaxCount(pCount, oCount);
    } break;
    case GuildType::TACTICIANS: // Red (1 VP)
    {
      int pCount = player.getCardsByType().at(CardType::MILITARY);
      int oCount = opponent.getCardsByType().at(CardType::MILITARY);
      points += getMaxCount(pCount, oCount);
    } break;
    case GuildType::BUILDERS: // Wonders (2 VP)
    {
      int pCount = (int)player.getBuiltWonders().size();
      int oCount = (int)opponent.getBuiltWonders().size();
      points += getMaxCount(pCount, oCount) * 2;
    } break;
    case GuildType::MONEYLENDERS: // Coins (1 VP per 3 coins)
    {
      int pVP = player.getCoins() / 3;
      int oVP = opponent.getCoins() / 3;
      points += getMaxCount(pVP, oVP);
    } break;
    default:
      break;
    }
  }
  return points;
}

void Game::handleSeventhWonderBuilt() {
  if (builtWonderCount >= 7 && !eighthWonderRemoved) {
    // 1. Remove from Board's available list (logical removal)
    board.removeFirstUnbuiltWonder();

    // 2. Remove from Player lists (UI/Selection removal)
    // We need to find which player holds the unbuilt wonder and remove it.
    auto removeUnbuilt = [](std::vector<Wonder *> &wonders) {
      for (auto it = wonders.begin(); it != wonders.end(); ++it) {
        if (*it != nullptr && !(*it)->isBuilt()) {
          wonders.erase(it);
          return true; // Found and removed
        }
      }
      return false;
    };

    if (!removeUnbuilt(player1Wonders)) {
      removeUnbuilt(player2Wonders);
    }

    eighthWonderRemoved = true;
    ConsoleView::printPlain(">>> The 7th Wonder has been built! The 8th Wonder "
                            "is immediately removed from the game. <<<");
  }
}
// Helper to interactively choose a card from discard
Card Game::chooseCardFromDiscard(Player * /*player*/) {
  if (discardPile.empty()) {
    // Should not happen if checked before calling
    return Card();
  }

  // We need to show list to user
  // Since we don't have a direct "Select from List" in ConsoleView, we'll
  // emulate it
  ConsoleView::printPlain("=== Choose a card from Discard Pile ===");
  for (size_t i = 0; i < discardPile.size(); ++i) {
    ConsoleView::printPlain(std::to_string(i) + ": " +
                            discardPile[i].getName());
  }

  int choice = -1;
  // Input loop
  while (true) {
    std::cout << "Enter index (0-" << discardPile.size() - 1 << "): ";
    if (std::cin >> choice) {
      if (choice >= 0 && choice < (int)discardPile.size()) {
        break;
      }
    } else {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
    }
  }

  Card chosen = discardPile[choice];
  // Remove from discard
  discardPile.erase(discardPile.begin() + choice);
  return chosen;
}

// Great Library: Choose from removed tokens
ProgressToken Game::chooseProgressTokenFromBox(Player * /*player*/) {
  const auto &removed = board.getRemovedProgressTokens();
  if (removed.empty()) {
    ConsoleView::printPlain("No removed progress tokens available.");
    // Return dummy
    return ProgressToken(ProgressTokenType::AGRICULTURE);
  }

  // Pick 3 random distinct indices
  std::vector<int> indices;
  if (removed.size() <= 3) {
    for (size_t i = 0; i < removed.size(); ++i)
      indices.push_back(i);
  } else {
    std::vector<int> allIndices(removed.size());
    std::iota(allIndices.begin(), allIndices.end(), 0);
    std::shuffle(allIndices.begin(), allIndices.end(),
                 std::mt19937(std::random_device{}()));
    for (int i = 0; i < 3; ++i)
      indices.push_back(allIndices[i]);
  }

  ConsoleView::printPlain("=== Choose a Progress Token from the Box ===");
  for (size_t i = 0; i < indices.size(); ++i) {
    int realIndex = indices[i];
    ConsoleView::printPlain(std::to_string(i) + ": " +
                            removed[realIndex].getDescription());
  }

  int choice = -1;
  while (true) {
    std::cout << "Enter choice (0-" << indices.size() - 1 << "): ";
    if (std::cin >> choice) {
      if (choice >= 0 && choice < (int)indices.size()) {
        break;
      }
    } else {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
    }
  }

  int actualIndex = indices[choice];
  return board.takeRemovedProgressToken(actualIndex);
}
