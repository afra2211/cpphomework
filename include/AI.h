#ifndef AI_H
#define AI_H

#include "Game.h"
#include "Player.h"

class AI {
public:
    static int getBestMove(const Game &game, const Player &player);
    static int getWonderChoice(const std::vector<Wonder*>& wonders);
};

#endif // AI_H
