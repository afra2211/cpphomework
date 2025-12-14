#include "Deck.h"
#include <algorithm>
#include <chrono>
#include <random>

// Helper to create cards easily
Card createCard(std::string name, CardType type, int costCoins,
                std::map<ResourceType, int> costRes,
                std::map<ResourceType, int> prodRes, int vp, int shields,
                std::vector<ScienceSymbol> sci, int coins,
                std::string chainSymbol = "", std::string chainTarget = "") {
  Cost c;
  c.coins = costCoins;
  c.resources = costRes;

  Effect e;
  e.resourcesProduced = prodRes;
  e.victoryPoints = vp;
  e.militaryShields = shields;
  e.scienceSymbols = sci;
  e.coins = coins;

  return Card(name, type, c, e, chainSymbol, chainTarget);
}

std::vector<Card> Deck::getAge1Deck() {
  std::vector<Card> deck;
  // Raw Materials (Brown)
  deck.push_back(createCard("Lumber Yard", CardType::RAW_MATERIAL, 0, {},
                            {{ResourceType::WOOD, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Logging Camp", CardType::RAW_MATERIAL, 1, {},
                            {{ResourceType::WOOD, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Clay Pool", CardType::RAW_MATERIAL, 0, {},
                            {{ResourceType::CLAY, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Clay Pit", CardType::RAW_MATERIAL, 1, {},
                            {{ResourceType::CLAY, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Quarry", CardType::RAW_MATERIAL, 0, {},
                            {{ResourceType::STONE, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Stone Pit", CardType::RAW_MATERIAL, 1, {},
                            {{ResourceType::STONE, 1}}, 0, 0, {}, 0));

  // Manufactured Goods (Grey)
  deck.push_back(createCard("Glassworks", CardType::MANUFACTURED_GOOD, 1, {},
                            {{ResourceType::GLASS, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Press", CardType::MANUFACTURED_GOOD, 1, {},
                            {{ResourceType::PAPER, 1}}, 0, 0, {}, 0));

  // Civilian (Blue)
  deck.push_back(createCard("Theater", CardType::CIVILIAN, 0, {}, {}, 3, 0, {},
                            0, "mask"));
  deck.push_back(
      createCard("Altar", CardType::CIVILIAN, 0, {}, {}, 3, 0, {}, 0, "moon"));
  deck.push_back(createCard("Baths", CardType::CIVILIAN, 0,
                            {{ResourceType::STONE, 1}}, {}, 3, 0, {}, 0,
                            "drop"));

  // Scientific (Green)
  deck.push_back(createCard("Apothecary", CardType::SCIENTIFIC, 2, {}, {}, 1, 0,
                            {ScienceSymbol::WHEEL}, 0, "wheel"));
  deck.push_back(createCard("Workshop", CardType::SCIENTIFIC, 1, {}, {}, 1, 0,
                            {ScienceSymbol::COMPASS}, 0, "lamp"));
  deck.push_back(createCard("Scriptorium", CardType::SCIENTIFIC, 2, {}, {}, 1,
                            0, {ScienceSymbol::TABLET}, 0, "book"));
  deck.push_back(createCard("Pharmacist", CardType::SCIENTIFIC, 2, {}, {}, 1, 0,
                            {ScienceSymbol::GEAR}, 0, "pestle"));

  // Commercial (Yellow)
  deck.push_back(
      createCard("Tavern", CardType::COMMERCIAL, 0, {}, {}, 0, 0, {},
                 4)); // Produces coins? No, gives 4 coins immediately?
  // Actually Tavern usually produces coins or gives immediate coins. In Duel it
  // gives 4 coins.

  deck.push_back(createCard("Wood Reserve", CardType::COMMERCIAL, 3, {}, {}, 0,
                            0, {},
                            0)); // Trading bonus logic not fully implemented
  deck.push_back(
      createCard("Clay Reserve", CardType::COMMERCIAL, 3, {}, {}, 0, 0, {}, 0));

  // Military (Red)
  deck.push_back(
      createCard("Guard Tower", CardType::MILITARY, 0, {}, {}, 0, 1, {}, 0));
  deck.push_back(createCard("Stable", CardType::MILITARY, 0,
                            {{ResourceType::WOOD, 1}}, {}, 0, 1, {}, 0));
  deck.push_back(createCard("Garrison", CardType::MILITARY, 0,
                            {{ResourceType::CLAY, 1}}, {}, 0, 1, {}, 0));
  deck.push_back(
      createCard("Palisade", CardType::MILITARY, 2, {}, {}, 0, 1, {}, 0));

  // Fill to 23 cards (3 removed in setup usually, but we need enough for
  // pyramid) Pyramid Age 1: 20 cards. We have ~19 above. Let's add a few more
  // to be safe.
  deck.push_back(createCard("Lumber Yard 2", CardType::RAW_MATERIAL, 0, {},
                            {{ResourceType::WOOD, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Clay Pool 2", CardType::RAW_MATERIAL, 0, {},
                            {{ResourceType::CLAY, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Quarry 2", CardType::RAW_MATERIAL, 0, {},
                            {{ResourceType::STONE, 1}}, 0, 0, {}, 0));

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

  // Return exactly 20 cards
  if (deck.size() > 20)
    deck.resize(20);
  return deck;
}

std::vector<Card> Deck::getAge2Deck() {
  std::vector<Card> deck;
  // Add Age 2 cards... simplified for now, reusing some logic or making
  // stronger ones
  deck.push_back(createCard("Sawmill", CardType::RAW_MATERIAL, 2, {},
                            {{ResourceType::WOOD, 2}}, 0, 0, {}, 0));
  deck.push_back(createCard("Brickyard", CardType::RAW_MATERIAL, 2, {},
                            {{ResourceType::CLAY, 2}}, 0, 0, {}, 0));
  deck.push_back(createCard("Shelf Quarry", CardType::RAW_MATERIAL, 2, {},
                            {{ResourceType::STONE, 2}}, 0, 0, {}, 0));
  deck.push_back(createCard("Glassblower", CardType::MANUFACTURED_GOOD, 0, {},
                            {{ResourceType::GLASS, 1}}, 0, 0, {}, 0));
  deck.push_back(createCard("Drying Room", CardType::MANUFACTURED_GOOD, 0, {},
                            {{ResourceType::PAPER, 1}}, 0, 0, {}, 0));

  deck.push_back(createCard("Walls", CardType::MILITARY, 0,
                            {{ResourceType::STONE, 2}}, {}, 0, 2, {}, 0));
  deck.push_back(createCard("Horse Breeders", CardType::MILITARY, 0,
                            {{ResourceType::CLAY, 1}, {ResourceType::WOOD, 1}},
                            {}, 0, 2, {}, 0));
  deck.push_back(
      createCard("Barracks", CardType::MILITARY, 3, {}, {}, 0, 2, {}, 0));
  deck.push_back(createCard("Archery Range", CardType::MILITARY, 0,
                            {{ResourceType::WOOD, 2}, {ResourceType::GLASS, 1}},
                            {}, 0, 2, {}, 0));

  deck.push_back(createCard("Library", CardType::SCIENTIFIC, 0,
                            {{ResourceType::STONE, 1},
                             {ResourceType::WOOD, 1},
                             {ResourceType::GLASS, 1}},
                            {}, 2, 0, {ScienceSymbol::TABLET}, 0, "book2",
                            "book"));
  deck.push_back(createCard("Dispensary", CardType::SCIENTIFIC, 0,
                            {{ResourceType::CLAY, 2}, {ResourceType::STONE, 1}},
                            {}, 2, 0, {ScienceSymbol::MORTAR}, 0, "",
                            "pestle"));
  deck.push_back(createCard("School", CardType::SCIENTIFIC, 0,
                            {{ResourceType::WOOD, 1}, {ResourceType::PAPER, 1}},
                            {}, 2, 0, {ScienceSymbol::WHEEL}, 0, "wheel2",
                            "wheel"));
  deck.push_back(createCard("Laboratory", CardType::SCIENTIFIC, 0,
                            {{ResourceType::WOOD, 1},
                             {ResourceType::GLASS, 1},
                             {ResourceType::PAPER, 1}},
                            {}, 2, 0, {ScienceSymbol::GEAR}, 0, "", "lamp"));

  deck.push_back(createCard("Statue", CardType::CIVILIAN, 0,
                            {{ResourceType::CLAY, 2}}, {}, 4, 0, {}, 0, "",
                            "mask"));
  deck.push_back(createCard("Temple", CardType::CIVILIAN, 0,
                            {{ResourceType::WOOD, 1}, {ResourceType::PAPER, 1}},
                            {}, 4, 0, {}, 0, "sun", "moon"));
  deck.push_back(createCard("Aqueduct", CardType::CIVILIAN, 0,
                            {{ResourceType::STONE, 3}}, {}, 5, 0, {}, 0, "",
                            "drop"));
  deck.push_back(createCard("Rostrum", CardType::CIVILIAN, 0,
                            {{ResourceType::STONE, 1}, {ResourceType::WOOD, 1}},
                            {}, 4, 0, {}, 0));

  deck.push_back(
      createCard("Brewery", CardType::COMMERCIAL, 0, {}, {}, 0, 0, {}, 6));

  // Fill to exactly 20 cards BEFORE shuffling
  while (deck.size() < 20) {
    deck.push_back(createCard("Generic Age 2", CardType::CIVILIAN, 0, {}, {}, 3,
                              0, {}, 0));
  }

  // Shuffle and ensure exactly 20 cards
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

  // Resize to exactly 20 (in case we have more)
  deck.resize(20);

  return deck;
}

std::vector<Card> Deck::getAge3Deck() {
  std::vector<Card> deck;
  // Age 3 - High costs, high rewards, Guilds
  deck.push_back(createCard("Arsenal", CardType::MILITARY, 0,
                            {{ResourceType::CLAY, 3}, {ResourceType::WOOD, 2}},
                            {}, 0, 3, {}, 0));
  deck.push_back(createCard("Courthouse", CardType::CIVILIAN, 0,
                            {{ResourceType::CLAY, 2}, {ResourceType::PAPER, 1}},
                            {}, 5, 0, {}, 0));
  deck.push_back(createCard("Academy", CardType::SCIENTIFIC, 0,
                            {{ResourceType::STONE, 1},
                             {ResourceType::WOOD, 1},
                             {ResourceType::GLASS, 1}},
                            {}, 3, 0, {ScienceSymbol::GLOBE}, 0));
  deck.push_back(createCard("Study", CardType::SCIENTIFIC, 0,
                            {{ResourceType::WOOD, 1},
                             {ResourceType::PAPER, 1},
                             {ResourceType::GLASS, 1}},
                            {}, 3, 0, {ScienceSymbol::GLOBE}, 0));
  deck.push_back(createCard("Chamber of Commerce", CardType::COMMERCIAL, 0,
                            {{ResourceType::PAPER, 2}}, {}, 3, 0, {},
                            3)); // 3 VP + 3 coins
  deck.push_back(createCard("Port", CardType::COMMERCIAL, 0,
                            {{ResourceType::WOOD, 1},
                             {ResourceType::GLASS, 1},
                             {ResourceType::PAPER, 1}},
                            {}, 3, 0, {}, 0));
  deck.push_back(createCard("Armory", CardType::MILITARY, 0,
                            {{ResourceType::STONE, 2}, {ResourceType::WOOD, 1}},
                            {}, 0, 3, {}, 0));
  deck.push_back(createCard("Palace", CardType::CIVILIAN, 0,
                            {{ResourceType::STONE, 1},
                             {ResourceType::WOOD, 1},
                             {ResourceType::GLASS, 1},
                             {ResourceType::CLAY, 1}},
                            {}, 7, 0, {}, 0));
  deck.push_back(createCard("Town Hall", CardType::CIVILIAN, 0,
                            {{ResourceType::STONE, 2}, {ResourceType::WOOD, 1}},
                            {}, 6, 0, {}, 0));
  deck.push_back(createCard(
      "Obelisk", CardType::CIVILIAN, 0,
      {{ResourceType::STONE, 2}, {ResourceType::GLASS, 1}}, {}, 5, 0, {}, 0));
  deck.push_back(createCard("Fortifications", CardType::MILITARY, 0,
                            {{ResourceType::STONE, 2},
                             {ResourceType::CLAY, 1},
                             {ResourceType::PAPER, 1}},
                            {}, 0, 3, {}, 0));
  deck.push_back(createCard("Siege Workshop", CardType::MILITARY, 0,
                            {{ResourceType::WOOD, 3}, {ResourceType::GLASS, 1}},
                            {}, 0, 3, {}, 0));
  deck.push_back(createCard("Circus", CardType::MILITARY, 0,
                            {{ResourceType::STONE, 2}, {ResourceType::CLAY, 2}},
                            {}, 0, 3, {}, 0));

  // Guilds
  deck.push_back(createCard("Merchants Guild", CardType::GUILD, 0,
                            {{ResourceType::CLAY, 1},
                             {ResourceType::WOOD, 1},
                             {ResourceType::GLASS, 1},
                             {ResourceType::PAPER, 1}},
                            {}, 10, 0, {}, 0)); // Simplified VP
  deck.push_back(createCard("Shipowners Guild", CardType::GUILD, 0,
                            {{ResourceType::CLAY, 1},
                             {ResourceType::STONE, 1},
                             {ResourceType::GLASS, 1},
                             {ResourceType::PAPER, 1}},
                            {}, 10, 0, {}, 0));
  deck.push_back(createCard("Builders Guild", CardType::GUILD, 0,
                            {{ResourceType::STONE, 2},
                             {ResourceType::CLAY, 2},
                             {ResourceType::GLASS, 1}},
                            {}, 10, 0, {}, 0));
  deck.push_back(createCard("Magistrates Guild", CardType::GUILD, 0,
                            {{ResourceType::WOOD, 2},
                             {ResourceType::CLAY, 1},
                             {ResourceType::PAPER, 1}},
                            {}, 10, 0, {}, 0));
  deck.push_back(createCard("Scientists Guild", CardType::GUILD, 0,
                            {{ResourceType::WOOD, 2}, {ResourceType::STONE, 2}},
                            {}, 10, 0, {}, 0));
  deck.push_back(createCard("Moneylenders Guild", CardType::GUILD, 0,
                            {{ResourceType::STONE, 2}, {ResourceType::WOOD, 2}},
                            {}, 10, 0, {}, 0));
  deck.push_back(createCard("Tacticians Guild", CardType::GUILD, 0,
                            {{ResourceType::STONE, 2},
                             {ResourceType::CLAY, 2},
                             {ResourceType::PAPER, 1}},
                            {}, 10, 0, {}, 0));

  // Fill to 20
  while (deck.size() < 20) {
    deck.push_back(createCard("Generic Age 3", CardType::CIVILIAN, 0, {}, {}, 5,
                              0, {}, 0));
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
  if (deck.size() > 20)
    deck.resize(20);
  return deck;
}

std::vector<Wonder> Deck::getWonders() {
  std::vector<Wonder> wonders;

  Cost c1;
  c1.resources = {{ResourceType::STONE, 3}, {ResourceType::CLAY, 1}};
  Effect e1;
  e1.victoryPoints = 3;
  e1.coins = 3; // The Pyramids
  wonders.push_back(Wonder("The Pyramids", c1, e1));

  Cost c2;
  c2.resources = {{ResourceType::WOOD, 2}, {ResourceType::STONE, 2}};
  Effect e2;
  e2.militaryShields = 2; // The Great Wall (Simplified)
  wonders.push_back(Wonder("The Great Wall", c2, e2));

  Cost c3;
  c3.resources = {{ResourceType::CLAY, 3}, {ResourceType::GLASS, 1}};
  Effect e3;
  e3.victoryPoints = 4; // The Hanging Gardens
  wonders.push_back(Wonder("The Hanging Gardens", c3, e3));

  Cost c4;
  c4.resources = {{ResourceType::PAPER, 1},
                  {ResourceType::GLASS, 1},
                  {ResourceType::WOOD, 1}};
  Effect e4;
  e4.scienceSymbols = {ScienceSymbol::GLOBE}; // The Great Library
  wonders.push_back(Wonder("The Great Library", c4, e4));

  Cost c5;
  c5.resources = {{ResourceType::STONE, 4}};
  Effect e5;
  e5.victoryPoints = 6; // The Sphinx
  wonders.push_back(Wonder("The Sphinx", c5, e5));

  Cost c6;
  c6.resources = {{ResourceType::WOOD, 3}, {ResourceType::PAPER, 2}};
  Effect e6;
  e6.militaryShields = 1;
  e6.victoryPoints = 2; // The Statue of Zeus
  wonders.push_back(Wonder("The Statue of Zeus", c6, e6));

  Cost c7;
  c7.resources = {{ResourceType::CLAY, 2},
                  {ResourceType::GLASS, 1},
                  {ResourceType::PAPER, 1}};
  Effect e7;
  e7.coins = 6;
  e7.victoryPoints = 3; // The Temple of Artemis
  wonders.push_back(Wonder("The Temple of Artemis", c7, e7));

  Cost c8;
  c8.resources = {{ResourceType::STONE, 2},
                  {ResourceType::WOOD, 2},
                  {ResourceType::CLAY, 1}};
  Effect e8;
  e8.militaryShields = 1;
  e8.victoryPoints = 3; // The Colossus
  wonders.push_back(Wonder("The Colossus", c8, e8));

  return wonders;
}
