#ifndef RESOURCES_H
#define RESOURCES_H

#include <map>
#include <string>
#include <vector>

enum class ResourceType { WOOD, CLAY, STONE, GLASS, PAPER, NONE };
// 11
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

enum class GuildType {
  NONE,
  SHIPOWNERS,  // 1 VP per Brown/Grey card (Highest city)
  SCIENTISTS,  // 1 VP per Green card (Highest city)
  TRADERS,     // 1 VP per Yellow card (Highest city)
  MAGISTRATES, // 1 VP per Blue card (Highest city)
  TACTICIANS,  // 1 VP per Red card (Highest city)
  BUILDERS,    // 2 VP per Wonder (Highest city)
  MONEYLENDERS // 1 VP per 3 coins (Highest city)
};

struct Cost {
  int coins = 0;
  std::map<ResourceType, int> resources;
};

// 表示商业折扣：
// - priceToOne: 将对应资源的单价固定为 1 金币
// - coinDiscount: 在常规单价基础上给予的金币减免（下限为 0）
struct TradeDiscount {
  bool priceToOne = false;
  int coinDiscount = 0;
};

struct Effect {
  std::map<ResourceType, int> resourcesProduced;
  std::vector<ResourceType> productionChoice; // Produces 1 of these types
  int victoryPoints = 0;
  int militaryShields = 0;
  std::vector<ScienceSymbol> scienceSymbols;
  int coins = 0;
  // Commercial effects like trading benefits can be added here later

  // 12.14 修改增添内容：
  bool playAgain = false;            // 是否允许额外回合
  int resourceRebate = 0;            // 资源折扣（如5表示5%，或固定值）
  bool destroyOpponentCard = false;  // 摧毁对手任意一张建筑
  bool destroyOpponentBrown = false; // 摧毁对手一张棕色牌
  bool destroyOpponentGrey = false;  // 摧毁对手一张灰色牌
  bool buildFromDiscard = false;     // 从弃牌堆免费建造一张牌
  bool gainProgressToken = false;    // 获得一枚进步标记(从面板)
  bool gainProgressTokenFromBox =
      false;                 // 从盒子中获得的进步标记(Great Library)
  int opponentCoinsLoss = 0; // 对手失去金币

  // Commercial / Wonders trade effects
  std::map<ResourceType, TradeDiscount> tradeDiscounts;

  GuildType guildType = GuildType::NONE;
};

#endif // RESOURCES_H