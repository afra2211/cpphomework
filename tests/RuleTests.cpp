#include "Board.h"
#include "Card.h"
#include "Player.h"
#include "Resources.h"
#include "Wonder.h"
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class TestPlayer : public Player {
public:
  using Player::Player;
  using Player::addProgressToken;
  using Player::addResource;
  using Player::addScienceSymbol;
  using Player::buildCard;
  using Player::buildWonder;
  using Player::calculateCost;
  using Player::discardForCoins;
  using Player::getBuiltCards;
  using Player::getCoins;
  using Player::getProgressTokens;
  using Player::getScienceSymbolCounts;
  using Player::markSciencePairClaimed;
  using Player::removeCoins;
  using Player::removeResource;

  void setTradeDiscount(ResourceType type, const TradeDiscount &discount) {
    tradeDiscounts[type] = discount;
  }

  void setCoins(int value) { coins = value; }

  void setYellowCardCount(int value) { yellowCardCount = value; }
};

Card makeResourceCard(const std::string &name, CardType type,
                      ResourceType resource, int amount = 1) {
  Effect effect;
  effect.resourcesProduced[resource] = amount;
  return Card{name, type, Cost{}, effect};
}

Card makeChainProvider(const std::string &symbol) {
  return Card{"Provider", CardType::CIVILIAN, Cost{}, Effect{}, symbol};
}

Card makeChainTarget(const std::string &target) {
  Cost cost;
  cost.resources[ResourceType::WOOD] = 1;
  return Card{"Target", CardType::CIVILIAN, cost, Effect{}, "", target};
}

void testTradingCostWithOpponentProduction() {
  TestPlayer player{"Alice"};
  TestPlayer opponent{"Bob"};

  opponent.buildCard(makeResourceCard("Lumber Yard", CardType::RAW_MATERIAL,
                                      ResourceType::WOOD, 1));

  Cost cost;
  cost.resources[ResourceType::WOOD] = 2;

  int computed = player.calculateCost(cost, opponent);
  assert(computed == 6 && "Trading cost should account for opponent production");
}

void testTradingCostWithDiscount() {
  TestPlayer player{"Alice"};
  TestPlayer opponent{"Bob"};

  TradeDiscount discount;
  discount.priceToOne = true;
  player.setTradeDiscount(ResourceType::STONE, discount);

  Cost cost;
  cost.resources[ResourceType::STONE] = 3;

  int computed = player.calculateCost(cost, opponent);
  assert(computed == 3 && "Discount should force unit price to 1");
}

void testChainFreeConstruction() {
  TestPlayer player{"Alice"};
  TestPlayer opponent{"Bob"};

  player.buildCard(makeChainProvider("BOOK"));
  Cost expensive;
  expensive.resources[ResourceType::CLAY] = 2;

  int computed = player.calculateCost(expensive, opponent, "BOOK");
  assert(computed == 0 && "Chain symbol should grant free construction");
}

void testDiscardCoinsFormula() {
  TestPlayer player{"Alice"};
  player.removeCoins(100); // reset to 0

  Card yellow{"Market", CardType::COMMERCIAL, Cost{}, Effect{}};
  player.buildCard(yellow);
  player.buildCard(yellow);

  int gained = player.discardForCoins();
  assert(gained == 4 && player.getCoins() == 4 &&
         "Discard gains 2 plus yellow card count");
}

void testSeventhWonderRemoval() {
  Board board;
  Cost cost;
  Wonder w1{"Wonder1", cost, Effect{}};
  Wonder w2{"Wonder2", cost, Effect{}};
  Wonder w3{"Wonder3", cost, Effect{}};

  std::vector<Wonder *> wonders{&w1, &w2, &w3};
  board.setAvailableWonders(wonders);

  board.removeFirstUnbuiltWonder();
  assert(board.getAvailableWonders().size() == 2 &&
         "One unbuilt wonder should be removed");
}

void testMilitaryTokenTrigger() {
  Board board;
  TestPlayer attacker{"Alice"};
  TestPlayer defender{"Bob"};
  defender.setCoins(5);

  board.moveMilitary(4, &attacker, &defender);
  assert(board.getMilitaryPosition() == 4 && defender.getCoins() == 3 &&
         "Crossing first token should move pawn and deduct coins");
}

void testSciencePairAwardsToken() {
  Board board;
  board.setupProgressTokens();

  TestPlayer scientist{"Alice"};
  scientist.addScienceSymbol(ScienceSymbol::GLOBE);
  scientist.addScienceSymbol(ScienceSymbol::GLOBE);

  bool win = board.onSciencePair(scientist);
  assert(!win && scientist.getProgressTokens().size() == 1 &&
         board.getAvailableProgressTokens().size() == 4 &&
         "Pairing should grant one progress token without triggering victory");
}

int main() {
  testTradingCostWithOpponentProduction();
  testTradingCostWithDiscount();
  testChainFreeConstruction();
  testDiscardCoinsFormula();
  testSeventhWonderRemoval();
  testMilitaryTokenTrigger();
  testSciencePairAwardsToken();

  std::cout << "All tests passed\n";
  return 0;
}
