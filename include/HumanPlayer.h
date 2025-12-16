#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"

class HumanPlayer : public Player {
public:
  HumanPlayer(std::string name);
  virtual ~HumanPlayer() override;
};

#endif // HUMANPLAYER_H