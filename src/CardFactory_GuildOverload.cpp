
// Implements the 5-arg createGuildCard overloaded method
std::unique_ptr<Card> CardFactory::createGuildCard(
    const std::string &name, const std::map<ResourceType, int> &costRes,
    int immediateCoins, int vp, const std::string &effectStrategy) {

  Effect effect;
  effect.victoryPoints = vp;
  effect.coins = immediateCoins;
  // Note: effectStrategy string is passed to help AI or UI logic if needed,
  // typically Guild effects are complex. For now providing basic VP/Coins.
  // The 'effectStrategy' might need to be parsed to set GuildType or
  // specialized flags. Based on the strings passed ("YELLOW_COMMERCIAL_CARD"),
  // we can map to GuildType.

  if (effectStrategy == "YELLOW_COMMERCIAL_CARD")
    effect.guildType = GuildType::TRADERS;
  else if (effectStrategy == "BROWN_GRAY_TRADE_CARD")
    effect.guildType = GuildType::SHIPOWNERS;
  else if (effectStrategy == "PYRAMID_BUILDING_CARD")
    effect.guildType = GuildType::BUILDERS;
  else if (effectStrategy == "BLUE_CIVILIAN_CARD")
    effect.guildType = GuildType::MAGISTRATES;
  else if (effectStrategy == "GREEN_SCIENTIFIC_CARD")
    effect.guildType = GuildType::SCIENTISTS;
  else if (effectStrategy == "GOLD_COIN_REWARD")
    effect.guildType = GuildType::MONEYLENDERS;
  else if (effectStrategy == "RED_MILITARY_CARD")
    effect.guildType = GuildType::TACTICIANS;
  else
    effect.guildType = GuildType::NONE;

  Cost cost;
  cost.resources = costRes;
  cost.coins = 0;

  return createCardInternal(name, CardType::GUILD, cost, effect, "", "");
}
