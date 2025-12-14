#ifndef RESOURCES_H
#define RESOURCES_H

#include <map>
#include <string>
#include <vector>


enum class ResourceType { WOOD, CLAY, STONE, GLASS, PAPER, NONE };

enum class CardType {
  RAW_MATERIAL,      // Brown
  MANUFACTURED_GOOD, // Grey
  CIVILIAN,          // Blue
  SCIENTIFIC,        // Green
  COMMERCIAL,        // Yellow
  MILITARY,          // Red
  GUILD              // Purple
};

enum class ScienceSymbol { GLOBE, TABLET, GEAR, COMPASS, WHEEL, MORTAR, NONE };

struct Cost {
  int coins = 0;
  std::map<ResourceType, int> resources;
};

struct Effect {
  std::map<ResourceType, int> resourcesProduced;
  int victoryPoints = 0;
  int militaryShields = 0;
  std::vector<ScienceSymbol> scienceSymbols;
  int coins = 0;
  // Commercial effects like trading benefits can be added here later
};

#endif // RESOURCES_H
