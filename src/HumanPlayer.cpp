#include "HumanPlayer.h"
#include "Board.h"
#include "ConsoleView.h"
#include "Game.h"
#include <iostream>

HumanPlayer::HumanPlayer(std::string name) : Player(std::move(name), false) {}

HumanPlayer::~HumanPlayer() = default;

Decision HumanPlayer::makeDecision(const Game &game) {
  Decision decision;

  const Board &board = game.getBoard();
  const Player *selfPtr = this;
  const Player &opponent =
      (&game.getPlayer1() == selfPtr) ? game.getPlayer2() : game.getPlayer1();
  const auto &pyramid = board.getPyramid();

  std::vector<int> accessibleIndices;
  for (size_t i = 0; i < pyramid.size(); ++i) {
    if (board.isCardAccessible(i) && pyramid.at(i).isFaceUp) {
      accessibleIndices.push_back(static_cast<int>(i));
    }
  }

  if (accessibleIndices.empty()) {
    ConsoleView::printError("No accessible cards available.");
    decision.action = DecisionAction::EXIT;
    return decision;
  }

  bool validCardSelected = false;
  while (!validCardSelected) {
    std::cout << "Enter card number to take (or type 'exit' to quit): ";

    std::string input;
    std::getline(std::cin, input);

    if (input == "exit" || input == "EXIT" || input == "Exit") {
      std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                << std::endl;
      exit(0);
    }

    try {
      int selection = std::stoi(input);
      if (!board.isCardAccessible(selection) || selection < 0 ||
          selection >= (int)pyramid.size() || !pyramid.at(selection).isFaceUp) {
        std::cout << "Invalid card! Choose an accessible card." << std::endl;
      } else {
        decision.cardIndex = selection;
        validCardSelected = true;
      }
    } catch (const std::invalid_argument &) {
      std::cout << "Invalid input. Please enter a card number, or type 'exit' "
                   "to quit."
                << std::endl;
    } catch (const std::out_of_range &) {
      std::cout << "Number out of range. Please enter a valid card number."
                << std::endl;
    }
  }

  const Card &previewCard = pyramid.at(decision.cardIndex).card;
  const Cost &cost = previewCard.getCost();

  std::cout << "\n========================================" << std::endl;
  std::cout << "[" << decision.cardIndex << "] " << previewCard.getName()
            << " (Cost: " << cost.coins;
  if (!cost.resources.empty()) {
    std::cout << " + resources";
  }
  std::cout << ")" << std::endl;
  std::cout << "========================================" << std::endl;

  bool canBuildCard = canAfford(cost, opponent, previewCard.getChainTarget());
  bool canBuildWonder = false;
  const auto &availableWonders = board.getAvailableWonders();
  for (size_t i = 0; i < availableWonders.size(); ++i) {
    if (availableWonders[i] != nullptr && !availableWonders[i]->isBuilt() &&
        canAfford(availableWonders[i]->getCost(), opponent)) {
      canBuildWonder = true;
      break;
    }
  }

  std::cout << "\nChoose your action:\n";
  std::cout << "1. Build the card";
  if (!canBuildCard) {
    std::cout << " (Cannot afford)";
  }
  std::cout << "\n2. Discard the card for coins\n";
  std::cout << "3. Build a wonder with the card";
  if (!canBuildWonder) {
    std::cout << " (No affordable wonders)";
  }
  std::cout << "\n4. Exit game\n";

  bool validChoice = false;
  while (!validChoice) {
    std::cout << "Enter your choice (1-4): ";
    std::string actionInput;
    std::getline(std::cin, actionInput);

    if (actionInput == "exit" || actionInput == "EXIT" ||
        actionInput == "Exit") {
      std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                << std::endl;
      exit(0);
    }

    try {
      int action = std::stoi(actionInput);
      if (action == 1) {
        if (canBuildCard) {
          decision.action = DecisionAction::BUILD_CARD;
          validChoice = true;
        } else {
          std::cout << "ERROR: You cannot afford to build this card!"
                    << std::endl;
        }
      } else if (action == 2) {
        decision.action = DecisionAction::DISCARD;
        validChoice = true;
      } else if (action == 3) {
        if (!canBuildWonder) {
          std::cout << "ERROR: No affordable wonders available!" << std::endl;
          continue;
        }

        std::cout << "\nAvailable Wonders:" << std::endl;
        for (size_t i = 0; i < availableWonders.size(); ++i) {
          if (availableWonders[i] != nullptr &&
              !availableWonders[i]->isBuilt()) {
            const Cost &wonderCost = availableWonders[i]->getCost();
            bool affordable = canAfford(wonderCost, opponent);
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

        std::cout << "Enter wonder number to build (or type 'exit' to quit): ";
        std::string wonderInput;
        std::getline(std::cin, wonderInput);

        if (wonderInput == "exit" || wonderInput == "EXIT" ||
            wonderInput == "Exit") {
          std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                    << std::endl;
          exit(0);
        }

        try {
          int wonderChoice = std::stoi(wonderInput);
          if (wonderChoice >= 0 &&
              wonderChoice < static_cast<int>(availableWonders.size()) &&
              availableWonders[wonderChoice] != nullptr &&
              !availableWonders[wonderChoice]->isBuilt() &&
              canAfford(availableWonders[wonderChoice]->getCost(), opponent)) {
            decision.action = DecisionAction::BUILD_WONDER;
            decision.wonderIndex = wonderChoice;
            validChoice = true;
          } else {
            std::cout << "ERROR: Invalid wonder selection!" << std::endl;
          }
        } catch (const std::exception &) {
          std::cout << "Invalid input. Please enter a wonder number."
                    << std::endl;
        }
      } else if (action == 4) {
        std::cout << "\nPlayer has chosen to exit the game. Goodbye!"
                  << std::endl;
        exit(0);
      } else {
        std::cout << "Invalid choice! Please enter 1, 2, 3, or 4." << std::endl;
      }
    } catch (const std::invalid_argument &) {
      std::cout << "Invalid input. Please enter a number between 1 and 4, or "
                   "type 'exit' to quit."
                << std::endl;
    } catch (const std::out_of_range &) {
      std::cout << "Number out of range. Please enter a valid choice."
                << std::endl;
    }
  }

  return decision;
}
