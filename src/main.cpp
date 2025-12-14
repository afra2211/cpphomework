#include "Game.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <cstdlib>

void clearInput() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::cout << "==================================" << std::endl;
    std::cout << "   7 WONDERS DUEL - CONSOLE GAME" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;

  // Game setup
  std::string p1Name, p2Name;
  int mode;

  std::cout << "Enter Player 1 name: ";
  std::getline(std::cin, p1Name);

  std::cout << "Game Mode:\n";
  std::cout << "1. Player vs Player\n";
  std::cout << "2. Player vs AI\n";
  std::cout << "Choose mode (1 or 2): ";
  std::cin >> mode;
  clearInput();

  bool p1IsAI = false;
  bool p2IsAI = false;

  if (mode == 2) {
    p2Name = "AI Bot";
    p2IsAI = true;
  } else {
    std::cout << "Enter Player 2 name: ";
    std::getline(std::cin, p2Name);
  }

  // Initialize and start game
  Game game;
  game.init(p1Name, p1IsAI, p2Name, p2IsAI);
  game.start();

  std::cout << "\nGame Over! Thanks for playing!" << std::endl;
  return 0;
}
