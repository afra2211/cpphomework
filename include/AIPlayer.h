#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"

class AIPlayer : public Player {
public:
  AIPlayer(std::string name);
  virtual ~AIPlayer() override;
};

#endif // AIPLAYER_H