#include "Game.h"
#include "AI.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>

Game::Game()
    : p1(nullptr), p2(nullptr), currentPlayerIndex(0), currentAge(1),
      gameOver(false) {}

void Game::init(std::string p1Name, bool p1IsAI, std::string p2Name,
                bool p2IsAI) {
  p1 = new Player(p1Name, p1IsAI);
  p2 = new Player(p2Name, p2IsAI);
  board = Board();
  currentPlayerIndex = 0;
  currentAge = 1;
  gameOver = false;

  setupWonders();
}

void Game::setupWonders() {
  std::cout << "\n========================================" << std::endl;
  std::cout << "        WONDER SELECTION PHASE" << std::endl;
  std::cout << "========================================\n" << std::endl;

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
  std::cout << "=== ROUND 1: Selecting 4 wonders from 12 ===" << std::endl;
  std::vector<Wonder *> round1Wonders;
  for (int i = 0; i < 4; ++i) {
    round1Wonders.push_back(allWonderPtrs[i]);
  }

  // 随机决定第一轮玩家顺序
  currentPlayerIndex = rand() % 2;                // 0或1
  int firstRoundFirstPlayer = currentPlayerIndex; // 保存第一轮先手玩家

  Player *firstPlayerRound1 = (currentPlayerIndex == 0) ? p1 : p2;
  Player *secondPlayerRound1 = (currentPlayerIndex == 0) ? p2 : p1;

  std::cout << "\nRandomly deciding player order..." << std::endl;
  std::cout << firstPlayerRound1->getName()
            << " will choose first in Round 1!\n"
            << std::endl;

  // 第一轮奇迹选择
  selectWondersRound(round1Wonders, 1);

  // 从剩下的8个中移除已选择的4个
  allWonderPtrs.erase(allWonderPtrs.begin(), allWonderPtrs.begin() + 4);

  // 再次随机打乱剩下的8个
  std::shuffle(allWonderPtrs.begin(), allWonderPtrs.end(),
               std::default_random_engine(seed + 1));

  // 第二次选择：从剩下的8个中随机选4个
  std::cout << "\n=== ROUND 2: Selecting 4 more wonders from remaining 8 ==="
            << std::endl;
  std::vector<Wonder *> round2Wonders;
  for (int i = 0; i < 4; ++i) {
    round2Wonders.push_back(allWonderPtrs[i]);
  }

  // 第二轮交换玩家顺序：第一轮先手玩家变成后手，后手玩家变成先手
  std::cout << "\nSwapping player order for Round 2..." << std::endl;
  currentPlayerIndex = 1 - currentPlayerIndex; // 交换0和1

  Player *firstPlayerRound2 = (currentPlayerIndex == 0) ? p1 : p2;
  std::cout << firstPlayerRound2->getName()
            << " will choose first in Round 2!\n"
            << std::endl;

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
  std::cout << "\n========================================" << std::endl;
  std::cout << "        WONDER SELECTION COMPLETE" << std::endl;
  std::cout << "========================================\n" << std::endl;

  std::cout << p1->getName() << "'s wonders:" << std::endl;
  for (int i = 0; i < player1Wonders.size(); ++i) {
    std::cout << "  " << i + 1 << ". " << player1Wonders[i]->getName()
              << std::endl;
  }

  std::cout << "\n" << p2->getName() << "'s wonders:" << std::endl;
  for (int i = 0; i < player2Wonders.size(); ++i) {
    std::cout << "  " << i + 1 << ". " << player2Wonders[i]->getName()
              << std::endl;
  }

  // 根据7 Wonders Duel规则，奇迹选择阶段第一轮的先手玩家在游戏开始时先手
  currentPlayerIndex = firstRoundFirstPlayer;
  std::cout << "\nGame will start with "
            << (currentPlayerIndex == 0 ? p1->getName() : p2->getName())
            << " (first to choose in Round 1)!\n"
            << std::endl;

  std::cout
      << "Press Enter to continue to the game (or type 'exit' to quit)...";

  std::string continueInput;
  std::getline(std::cin, continueInput);

  // 检查是否输入了exit
  if (continueInput == "exit" || continueInput == "EXIT" ||
      continueInput == "Exit") {
    std::cout << "\nPlayer has chosen to exit the game. Goodbye!" << std::endl;
    exit(0);
  }
}

void Game::selectWondersRound(std::vector<Wonder *> &availableWonders,
                              int round) {
  std::cout << "\n========================================" << std::endl;
  std::cout << "        WONDER SELECTION - ROUND " << round << std::endl;
  std::cout << "========================================\n" << std::endl;

  // 显示所有可用的奇迹
  std::cout << "Available wonders:\n" << std::endl;
  for (int i = 0; i < availableWonders.size(); ++i) {
    std::cout << i + 1 << ". " << availableWonders[i]->getName() << std::endl;
  }
  std::cout << std::endl;

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
    std::cout << ">>> " << firstPlayer->getName() << " (AI) chooses first <<<"
              << std::endl;
    int aiChoice =
        getPlayerWonderChoice(1, availableWonders.size(), "", firstPlayer);
    std::cout << firstPlayer->getName()
              << " (AI) selected: " << availableWonders[aiChoice - 1]->getName()
              << "\n"
              << std::endl;

    firstPlayerWonders.push_back(availableWonders[aiChoice - 1]);
    availableWonders.erase(availableWonders.begin() + aiChoice - 1);

    // 显示AI的奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << firstPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < firstPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << firstPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // 2. 显示剩下的三张奇迹给人类玩家选择
    std::cout << "Remaining wonders for " << secondPlayer->getName()
              << " (Human):\n"
              << std::endl;
    for (int i = 0; i < availableWonders.size(); ++i) {
      std::cout << i + 1 << ". " << availableWonders[i]->getName() << std::endl;
    }
    std::cout << std::endl;

    std::cout << ">>> " << secondPlayer->getName() << "'s turn (Human) <<<"
              << std::endl;
    std::cout << "Please choose 2 wonders from the remaining 3." << std::endl;

    // 人类玩家选择第一张奇迹
    int humanChoice1 = getPlayerWonderChoice(
        1, availableWonders.size(),
        "Enter the number of your first wonder: ", secondPlayer);
    std::cout << "\n"
              << secondPlayer->getName()
              << " selected: " << availableWonders[humanChoice1 - 1]->getName()
              << "\n"
              << std::endl;

    secondPlayerWonders.push_back(availableWonders[humanChoice1 - 1]);
    availableWonders.erase(availableWonders.begin() + humanChoice1 - 1);

    // 显示人类玩家的奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << secondPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < secondPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << secondPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // 显示剩余的奇迹
    std::cout << "Remaining wonders:\n" << std::endl;
    for (int i = 0; i < availableWonders.size(); ++i) {
      std::cout << i + 1 << ". " << availableWonders[i]->getName() << std::endl;
    }
    std::cout << std::endl;

    // 人类玩家选择第二张奇迹
    int humanChoice2 = getPlayerWonderChoice(
        1, availableWonders.size(),
        "Enter the number of your second wonder: ", secondPlayer);
    std::cout << "\n"
              << secondPlayer->getName()
              << " selected: " << availableWonders[humanChoice2 - 1]->getName()
              << "\n"
              << std::endl;

    secondPlayerWonders.push_back(availableWonders[humanChoice2 - 1]);
    availableWonders.erase(availableWonders.begin() + humanChoice2 - 1);

    // 显示人类玩家的最终奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << secondPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < secondPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << secondPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // 3. 最后一张奇迹自动归AI
    std::cout << "The last wonder automatically goes to "
              << firstPlayer->getName() << " (AI):" << std::endl;
    std::cout << firstPlayer->getName()
              << " receives: " << availableWonders[0]->getName() << "\n"
              << std::endl;

    firstPlayerWonders.push_back(availableWonders[0]);

    // 显示AI的最终奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << firstPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < firstPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << firstPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

  } else {
    // 情况：先手玩家是人类

    // 1. 人类玩家先选择一张奇迹
    std::cout << ">>> " << firstPlayer->getName() << "'s turn (Human) <<<"
              << std::endl;
    std::cout << "Please enter the number (1-4) to select your wonder: ";

    int humanChoice = getPlayerWonderChoice(
        1, availableWonders.size(),
        "Enter the number of the wonder you want: ", firstPlayer);

    std::cout << "\n"
              << firstPlayer->getName()
              << " selected: " << availableWonders[humanChoice - 1]->getName()
              << "\n"
              << std::endl;

    firstPlayerWonders.push_back(availableWonders[humanChoice - 1]);
    availableWonders.erase(availableWonders.begin() + humanChoice - 1);

    // 显示人类玩家的奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << firstPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < firstPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << firstPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // 2. 显示剩余的奇迹
    std::cout << "Remaining wonders:\n" << std::endl;
    for (int i = 0; i < availableWonders.size(); ++i) {
      std::cout << i + 1 << ". " << availableWonders[i]->getName() << std::endl;
    }
    std::cout << std::endl;

    std::cout << ">>> " << secondPlayer->getName() << "'s turn";
    if (secondPlayer->isAIPlayer()) {
      std::cout << " (AI) <<<" << std::endl;
      std::cout << "AI will choose 2 wonders from the remaining 3.\n"
                << std::endl;
    } else {
      std::cout << " (Human) <<<" << std::endl;
      std::cout << "Please choose 2 wonders from the remaining 3." << std::endl;
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

    std::cout << secondPlayer->getName()
              << " selected: " << availableWonders[secondChoice1 - 1]->getName()
              << "\n"
              << std::endl;

    secondPlayerWonders.push_back(availableWonders[secondChoice1 - 1]);
    availableWonders.erase(availableWonders.begin() + secondChoice1 - 1);

    // 显示后手玩家的奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << secondPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < secondPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << secondPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // 显示剩余的奇迹
    std::cout << "Remaining wonders:\n" << std::endl;
    for (int i = 0; i < availableWonders.size(); ++i) {
      std::cout << i + 1 << ". " << availableWonders[i]->getName() << std::endl;
    }
    std::cout << std::endl;

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

    std::cout << secondPlayer->getName()
              << " selected: " << availableWonders[secondChoice2 - 1]->getName()
              << "\n"
              << std::endl;

    secondPlayerWonders.push_back(availableWonders[secondChoice2 - 1]);
    availableWonders.erase(availableWonders.begin() + secondChoice2 - 1);

    // 显示后手玩家的最终奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << secondPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < secondPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << secondPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // 3. 最后一张奇迹自动归先手玩家
    std::cout << "The last wonder automatically goes to "
              << firstPlayer->getName() << ":" << std::endl;
    std::cout << firstPlayer->getName()
              << " receives: " << availableWonders[0]->getName() << "\n"
              << std::endl;

    firstPlayerWonders.push_back(availableWonders[0]);

    // 显示先手玩家的最终奇迹区
    std::cout << "========================================" << std::endl;
    std::cout << firstPlayer->getName() << "'s Wonders:" << std::endl;
    for (size_t i = 0; i < firstPlayerWonders.size(); ++i) {
      std::cout << "  " << i + 1 << ". " << firstPlayerWonders[i]->getName()
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
  }

  // 显示本轮选择后的结果总结
  std::cout << "\n=== ROUND " << round
            << " SELECTION COMPLETE ===" << std::endl;
  std::cout << p1->getName() << " now has " << player1Wonders.size()
            << " wonders." << std::endl;
  std::cout << p2->getName() << " now has " << player2Wonders.size()
            << " wonders.\n"
            << std::endl;
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
      std::cout << prompt;
    }

    std::string input;
    std::getline(std::cin, input);

    if (input == "exit" || input == "EXIT" || input == "Exit") {
      std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                << std::endl;
      exit(0); // 退出程序
    }

    try {
      choice = std::stoi(input);

      if (choice < min || choice > max) {
        std::cout << "Invalid choice. Please enter a number between " << min
                  << " and " << max << "." << std::endl;
      } else {
        return choice;
      }
    } catch (const std::invalid_argument &e) {
      std::cout << "Invalid input. Please enter a number between " << min
                << " and " << max << ", or type 'exit' to quit." << std::endl;
    } catch (const std::out_of_range &e) {
      std::cout << "Number out of range. Please enter a number between " << min
                << " and " << max << "." << std::endl;
    }
  }
}

void Game::start() {
  // Main game loop
  for (int age = 1; age <= 3 && !gameOver; ++age) {
    playAge(age);

    while (!gameOver) {
      // Display board
      printBoard();

      // Check if pyramid is empty
      bool empty = true;
      for (const auto &slot : board.getPyramid()) {
        if (!slot.isTaken) {
          empty = false;
          break;
        }
      }

      if (empty)
        break; // Age complete

      // Play turn
      playTurn();
    }
  }

  // Final scoring
  if (!gameOver) {
    checkVictory();
  }
}

void Game::playAge(int age) {
  currentAge = age;
  std::cout << "\n========================================" << std::endl;
  std::cout << "          AGE " << age << " BEGINS!" << std::endl;
  std::cout << "========================================\n" << std::endl;

  std::vector<Card> deck;
  if (age == 1)
    deck = Deck::getAge1Deck();
  else if (age == 2)
    deck = Deck::getAge2Deck();
  else
    deck = Deck::getAge3Deck();

  board.setupAge(age, deck);
}

void Game::playTurn() {
  Player *currentPlayer = (currentPlayerIndex == 0) ? p1 : p2;
  Player *otherPlayer = (currentPlayerIndex == 0) ? p2 : p1;

  printPlayerStatePanel(currentPlayer, otherPlayer);

  std::cout << "\n--- " << currentPlayer->getName() << "'s Turn ---"
            << std::endl;

  int cardIndex = -1;

  if (currentPlayer->isAIPlayer()) {
    // AI move - simplified for now, just builds the card
    cardIndex = AI::getBestMove(*this, *currentPlayer);
    std::cout << "AI chooses card " << cardIndex << std::endl;
    Card card = board.takeCard(cardIndex);
    std::cout << "AI took card: " << card.getName() << std::endl;
    currentPlayer->buildCard(card);

    // Apply military effect
    const Effect &e = card.getEffect();
    if (e.militaryShields > 0) {
      if (currentPlayer == p1) {
        board.moveMilitary(e.militaryShields);
      } else {
        board.moveMilitary(-e.militaryShields);
      }
      handleMilitaryConflict();
    }
  } else {
    // Human input - select card first
    bool validCardSelected = false;
    while (!validCardSelected) {
      std::cout << "Enter card number to take (or type 'exit' to quit): ";

      std::string input;
      std::getline(std::cin, input);

      // 检查是否输入了exit
      if (input == "exit" || input == "EXIT" || input == "Exit") {
        std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                  << std::endl;
        exit(0); // 退出程序
      }

      // 尝试将输入转换为整数
      try {
        cardIndex = std::stoi(input);

        // Validate card selection
        if (!board.isCardAccessible(cardIndex) || cardIndex < 0 ||
            cardIndex >= (int)board.getPyramid().size() ||
            board.getPyramid()[cardIndex].isTaken) {
          std::cout << "Invalid card! Choose an accessible card." << std::endl;
        } else {
          validCardSelected = true;
        }
      } catch (const std::invalid_argument &e) {
        std::cout << "Invalid input. Please enter a card number, or type "
                     "'exit' to quit."
                  << std::endl;
      } catch (const std::out_of_range &e) {
        std::cout << "Number out of range. Please enter a valid card number."
                  << std::endl;
      }
    }

    // Take the card and show it to the player
    Card card = board.takeCard(cardIndex);
    const Cost &cost = card.getCost();

    std::cout << "\n========================================" << std::endl;
    std::cout << "You selected: " << card.getName() << " (Cost: " << cost.coins
              << " coins";
    if (!cost.resources.empty()) {
      std::cout << " + resources";
    }
    std::cout << ")" << std::endl;
    std::cout << "========================================" << std::endl;

    // Check affordability for each action
    bool canBuildCard =
        currentPlayer->canAfford(cost, *otherPlayer, card.getChainTarget());
    bool canDiscardForCoins = true; // Always available
    bool canBuildWonder = false;
    int wonderIndex = -1;

    // Check if there are any wonders available to build
    const auto &availableWonders = board.getAvailableWonders();
    if (!availableWonders.empty()) {
      // Check if any wonder is affordable
      for (size_t i = 0; i < availableWonders.size(); ++i) {
        if (availableWonders[i] != nullptr && !availableWonders[i]->isBuilt() &&
            currentPlayer->canAfford(availableWonders[i]->getCost(),
                                     *otherPlayer)) {
          canBuildWonder = true;
          break;
        }
      }
    }

    // Display action menu
    std::cout << "\nChoose action:" << std::endl;
    std::cout << "1. Build Building";
    if (!canBuildCard) {
      std::cout << " [DISABLED - Cannot afford]";
    }
    std::cout << std::endl;

    std::cout << "2. Discard for Coins (Gain " << (cost.coins + 2) << " coins)"
              << std::endl;

    std::cout << "3. Construct Wonder";
    if (!canBuildWonder) {
      std::cout << " [DISABLED - No affordable wonders available]";
    }
    std::cout << std::endl;
    std::cout << "4. Exit Game" << std::endl;

    // Get player choice
    int action = 0;
    bool validChoice = false;
    while (!validChoice) {
      std::cout << "\nEnter your choice (1-4, or type 'exit'): ";

      std::string actionInput;
      std::getline(std::cin, actionInput);

      // 检查是否输入了exit
      if (actionInput == "exit" || actionInput == "EXIT" ||
          actionInput == "Exit") {
        std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                  << std::endl;
        exit(0); // 退出程序
      }

      // 尝试将输入转换为整数
      try {
        action = std::stoi(actionInput);

        if (action < 1 || action > 4) {
          std::cout << "Invalid choice! Please enter a number between 1 and 4."
                    << std::endl;
          continue;
        }

        if (action == 4) {
          std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                    << std::endl;
          exit(0);
        }

        if (action == 1) {
          if (canBuildCard) {
            // Build the card
            int actualCost = currentPlayer->calculateCost(
                cost, *otherPlayer, card.getChainTarget());
            currentPlayer->payCost(actualCost);
            currentPlayer->buildCard(card);
            std::cout << "\n"
                      << currentPlayer->getName()
                      << " built: " << card.getName() << " (Paid " << actualCost
                      << " coins)" << std::endl;

            // Apply military effect
            const Effect &e = card.getEffect();
            if (e.militaryShields > 0) {
              if (currentPlayer == p1) {
                board.moveMilitary(e.militaryShields);
              } else {
                board.moveMilitary(-e.militaryShields);
              }
              handleMilitaryConflict();
            }
            validChoice = true;
          } else {
            std::cout << "ERROR: You cannot afford to build this card!"
                      << std::endl;
          }
        } else if (action == 2) {
          // Discard for coins
          // R3.2: 2 coins + 1 coin for every yellow card (Commercial) built
          int yellowCards =
              currentPlayer->getCardsByType()[CardType::COMMERCIAL];
          int coinsGained = 2 + yellowCards;
          currentPlayer->addCoins(coinsGained);
          std::cout << "\n"
                    << currentPlayer->getName() << " discarded "
                    << card.getName() << " for " << coinsGained
                    << " coins (2 + " << yellowCards << " yellow cards)."
                    << std::endl;
          validChoice = true;
        } else if (action == 3) {
          if (canBuildWonder) {
            // Show available wonders
            std::cout << "\nAvailable Wonders:" << std::endl;
            for (size_t i = 0; i < availableWonders.size(); ++i) {
              if (availableWonders[i] != nullptr &&
                  !availableWonders[i]->isBuilt()) {
                const Cost &wonderCost = availableWonders[i]->getCost();
                bool affordable =
                    currentPlayer->canAfford(wonderCost, *otherPlayer);
                std::cout << i << ". " << availableWonders[i]->getName()
                          << " (Cost: " << wonderCost.coins << " coins";
                if (!wonderCost.resources.empty()) {
                  std::cout << " + resources";
                }
                std::cout << ")";
                if (!affordable) {
                  std::cout << " [Cannot afford]";
                }
                std::cout << std::endl;
              }
            }

            // Select wonder
            bool validWonderSelected = false;
            while (!validWonderSelected) {
              std::cout
                  << "Enter wonder number to build (or type 'exit' to quit): ";

              std::string wonderInput;
              std::getline(std::cin, wonderInput);

              // 检查是否输入了exit
              if (wonderInput == "exit" || wonderInput == "EXIT" ||
                  wonderInput == "Exit") {
                std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                          << std::endl;
                exit(0); // 退出程序
              }

              // 尝试将输入转换为整数
              try {
                wonderIndex = std::stoi(wonderInput);

                // Validate wonder selection
                if (wonderIndex >= 0 &&
                    wonderIndex < (int)availableWonders.size() &&
                    availableWonders[wonderIndex] != nullptr &&
                    !availableWonders[wonderIndex]->isBuilt()) {

                  if (currentPlayer->canAfford(
                          availableWonders[wonderIndex]->getCost(),
                          *otherPlayer)) {
                    // Build the wonder
                    int actualCost = currentPlayer->calculateCost(
                        availableWonders[wonderIndex]->getCost(), *otherPlayer);
                    currentPlayer->payCost(actualCost);
                    currentPlayer->buildWonder(*availableWonders[wonderIndex]);
                    std::cout << "\n"
                              << currentPlayer->getName()
                              << " constructed wonder: "
                              << availableWonders[wonderIndex]->getName()
                              << " using card " << card.getName() << std::endl;
                    validChoice = true;
                    validWonderSelected = true;
                  } else {
                    std::cout << "ERROR: You cannot afford this wonder!"
                              << std::endl;
                  }
                } else {
                  std::cout << "ERROR: Invalid wonder selection!" << std::endl;
                }
              } catch (const std::invalid_argument &e) {
                std::cout << "Invalid input. Please enter a wonder number, or "
                             "type 'exit' to quit."
                          << std::endl;
              } catch (const std::out_of_range &e) {
                std::cout << "Number out of range. Please enter a valid wonder "
                             "number."
                          << std::endl;
              }
            }
          } else {
            std::cout << "ERROR: No affordable wonders available!" << std::endl;
          }
        } else {
          std::cout << "Invalid choice! Please enter 1, 2, 3, or 4."
                    << std::endl;
        }
      } catch (const std::invalid_argument &e) {
        std::cout << "Invalid input. Please enter a number between 1 and 4, or "
                     "type 'exit' to quit."
                  << std::endl;
      } catch (const std::out_of_range &e) {
        std::cout << "Number out of range. Please enter a valid choice."
                  << std::endl;
      }
    }
  }

  // Check victory
  if (checkVictory()) {
    gameOver = true;
    return;
  }

  switchTurn();
}

void Game::printBoard() const {
  std::cout << "\n====== GAME STATE ======" << std::endl;
  std::cout << "Age: " << currentAge
            << " | Military: " << board.getMilitaryPosition() << std::endl;
  std::cout << p1->getName() << " (Coins: " << p1->getCoins() << ") vs "
            << p2->getName() << " (Coins: " << p2->getCoins() << ")"
            << std::endl;
  std::cout << "\nPYRAMID:" << std::endl;

  const auto &pyramid = board.getPyramid();

  // Define pyramid structure based on age
  std::vector<int> rowSizes;
  if (currentAge == 1 || currentAge == 3) {
    // Age 1/3: Top to bottom - 2, 3, 4, 5, 6 cards
    rowSizes = {2, 3, 4, 5, 6};
  } else {
    // Age 2: Inverted pyramid - 6, 5, 4, 3, 2 cards
    rowSizes = {6, 5, 4, 3, 2};
  }

  int cardIndex = 0;
  for (size_t row = 0; row < rowSizes.size(); ++row) {
    int rowSize = rowSizes[row];

    // Calculate indentation (spaces to center the row)
    int indent = (6 - rowSize) * 7; // Approximate centering
    std::cout << std::string(indent, ' ');

    // Print cards in this row
    for (int col = 0; col < rowSize; ++col) {
      if (cardIndex >= pyramid.size())
        break;

      const auto &slot = pyramid[cardIndex];
      bool accessible = board.isCardAccessible(cardIndex);

      // Format: [*ID: Name] or [ ID: ????]
      std::string marker = accessible ? "*" : " ";
      std::cout << "[" << marker << std::setw(2) << cardIndex << ":";

      if (slot.isTaken) {
        std::cout << "TAKEN ";
      } else if (!slot.isFaceUp) {
        std::cout << " ??? ";
      } else {
        // Truncate long names
        std::string name = slot.card.getName();
        if (name.length() > 7)
          name = name.substr(0, 7);
        std::cout << std::setw(7) << std::left << name << std::right;
      }

      std::cout << "] ";
      cardIndex++;
    }
    std::cout << std::endl;
  }

  std::cout << "\n* = Accessible (can be picked)" << std::endl;
  std::cout << "========================\n" << std::endl;
}

void Game::printPlayerStatePanel(Player *currentPlayer, Player *otherPlayer) {
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

  // 显示科学符号
  if (!scienceSymbols.empty()) {
    // 重新统计用于详细显示
    std::map<ScienceSymbol, int> symbolCounts;
    for (const auto &symbol : scienceSymbols) {
      if (symbol != ScienceSymbol::NONE) {
        symbolCounts[symbol]++;
      }
    }

    if (!symbolCounts.empty()) {
      std::cout << "\nScience Symbols: ";
      bool first = true;
      int totalSymbols = 0;
      for (const auto &[symbol, count] : symbolCounts) {
        if (!first)
          std::cout << ", ";

        std::string symbolName;
        switch (symbol) {
        case ScienceSymbol::GLOBE:
          symbolName = "Globe";
          break;
        case ScienceSymbol::TABLET:
          symbolName = "Tablet";
          break;
        case ScienceSymbol::GEAR:
          symbolName = "Gear";
          break;
        case ScienceSymbol::COMPASS:
          symbolName = "Compass";
          break;
        case ScienceSymbol::WHEEL:
          symbolName = "Wheel";
          break;
        case ScienceSymbol::MORTAR:
          symbolName = "Mortar";
          break;
        default:
          symbolName = "Unknown";
        }

        std::cout << symbolName << "(" << count << ")";
        totalSymbols += count;
        first = false;
      }
      std::cout << " [Total: " << totalSymbols << " symbols]";
    }
  }

  // 显示建造的奇迹
  const auto &builtWonders = currentPlayer->getBuiltWonders();
  if (!builtWonders.empty()) {
    std::cout << "\nWonders: ";
    for (size_t i = 0; i < builtWonders.size(); ++i) {
      if (i > 0)
        std::cout << ", ";
      if (builtWonders[i]) {
        std::cout << builtWonders[i]->getName();
      }
    }
  }

  // 显示已建造的卡牌（简要）
  const auto &builtCards = currentPlayer->getBuiltCards();
  if (!builtCards.empty() &&
      builtCards.size() <= 10) { // 如果卡牌太多，就不显示
    std::cout << "\nBuilt Cards: ";
    for (size_t i = 0; i < builtCards.size(); ++i) {
      if (i > 0)
        std::cout << ", ";
      std::cout << builtCards[i].getName();
    }
  } else if (!builtCards.empty()) {
    std::cout << "\nBuilt Cards: " << builtCards.size() << " cards";
  }

  std::cout << "\n";

  // 对手的简要信息
  std::cout << "\n--- " << otherPlayer->getName() << " (Opponent) ---\n";
  std::cout << "Coins: " << otherPlayer->getCoins();
  int opponentMilitaryShields = otherPlayer->getMilitaryPower();
  std::cout << " | Military: " << opponentMilitaryShields;

  std::cout << " | VP: " << otherPlayer->getVictoryPoints();

  std::map<CardType, int> opponentCards = otherPlayer->getCardsByType();
  std::cout << " | Cards: ";
  std::cout << "B" << opponentCards[CardType::RAW_MATERIAL];
  std::cout << "/G" << opponentCards[CardType::MANUFACTURED_GOOD];
  std::cout << "/Bl" << opponentCards[CardType::CIVILIAN];
  std::cout << "/Gr" << opponentCards[CardType::SCIENTIFIC];
  std::cout << "/Y" << opponentCards[CardType::COMMERCIAL];
  std::cout << "/R" << opponentMilitaryShields;
  std::cout << "/P" << opponentCards[CardType::GUILD];

  std::cout << "\n";
  std::cout << "==============================================================="
               "=================\n";
}

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
    std::cout << "\n*** " << p1->getName() << " wins by Military Victory! ***"
              << std::endl;
    return true;
  }
  if (milPos <= -9) {
    std::cout << "\n*** " << p2->getName() << " wins by Military Victory! ***"
              << std::endl;
    return true;
  }

  // Scientific victory - check if player has 6 different science symbols
  // Player 1 check
  std::map<ScienceSymbol, int> p1ScienceCounts = p1->getScienceSymbolCounts();
  int p1UniqueSymbols = 0;
  for (const auto &[symbol, count] : p1ScienceCounts) {
    if (symbol != ScienceSymbol::NONE && count > 0) {
      p1UniqueSymbols++;
    }
  }
  if (p1UniqueSymbols >= 6) {
    std::cout << "\n*** " << p1->getName() << " wins by Scientific Victory! ***"
              << std::endl;
    std::cout << "Collected 6 different science symbols!" << std::endl;
    return true;
  }

  // Player 2 check
  std::map<ScienceSymbol, int> p2ScienceCounts = p2->getScienceSymbolCounts();
  int p2UniqueSymbols = 0;
  for (const auto &[symbol, count] : p2ScienceCounts) {
    if (symbol != ScienceSymbol::NONE && count > 0) {
      p2UniqueSymbols++;
    }
  }
  if (p2UniqueSymbols >= 6) {
    std::cout << "\n*** " << p2->getName() << " wins by Scientific Victory! ***"
              << std::endl;
    std::cout << "Collected 6 different science symbols!" << std::endl;
    return true;
  }

  // Civilian victory (end of age 3)
  if (currentAge == 3) {
    // Calculate total points (simplified)
    int p1Score = p1->getCoins() + milPos * 2;
    int p2Score = p2->getCoins() - milPos * 2;

    std::cout << "\n=== FINAL SCORES ===" << std::endl;
    std::cout << p1->getName() << ": " << p1Score << std::endl;
    std::cout << p2->getName() << ": " << p2Score << std::endl;

    if (p1Score > p2Score) {
      std::cout << "\n*** " << p1->getName() << " wins by Civilian Victory! ***"
                << std::endl;
    } else {
      std::cout << "\n*** " << p2->getName() << " wins by Civilian Victory! ***"
                << std::endl;
    }
    return true;
  }

  return false;
}

void Game::handleMilitaryConflict() {
  // Simplified - just print current position
  int pos = board.getMilitaryPosition();
  std::cout << "Military position: " << pos << std::endl;
}
