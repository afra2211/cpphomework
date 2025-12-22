# Choice Resource Modeling Proposal

## 1. Problem With Current Modeling
The game currently records player resources as `std::map<ResourceType, int>`, incremented directly by `Player::addResource`. This collapses **choice resources** (e.g., "1 Wood **or** 1 Clay") into permanent counts of a single type. Once such a card is played, the player forever owns the chosen resource, so `canAfford` incorrectly sees either:
- A free permanent resource that should remain conditional, or
- No added resource at all, forcing gold payments even when a free choice should be allowed.

This breaks the Seven Wonders Duel rule that each choice can be reassigned every time the player builds a card/wonder.

## 2. Data Structures
Represent fixed and optional production separately so choices remain flexible per purchase.

```cpp
// Core types
using ResourceCounts = std::map<ResourceType, int>;

struct ChoicePack {
    // One use per build action: pick ONE option from options
    std::vector<ResourceCounts> options; // e.g., {{Wood:1}, {Clay:1}}
};

class Player {
    ResourceCounts fixed;                 // Permanent production (brown/gray cards, wonders)
    std::vector<ChoicePack> flexible;     // Yellow cards, wonders with OR production
    // ...
};
```

Notes:
- `fixed` holds additive production that is always available.
- Each `ChoicePack` corresponds to a single card/wonder that grants "pick one" production per build.
- `ChoicePack::options` can model multi-choice (e.g., Forum: `{Glass:1}`, `{Papyrus:1}`, `{Silk:1}`).
- For more complex cards that offer *two independent picks* (e.g., Port: wood or clay **plus** papyrus or glass), model this as two `ChoicePack` entries so each pick is handled separately.

### Adding Resources
- `addFixedResource(ResourceType t, int n)` adjusts `fixed[t] += n`.
- `addChoicePack(const ChoicePack& pack)` pushes into `flexible` without altering `fixed`.
- Existing loaders should translate card metadata into either fixed increments or choice packs.

## 3. `canAfford` Calculation Outline
Goal: Given a `cost` (map of required counts) and current gold, decide if the player can pay with resources (fixed + any assignment of choice packs), optionally buying missing units with gold prices provided by the board state.

### Steps
1. **Start with fixed production**
   ```cpp
   ResourceCounts remaining = cost;
   for (auto [type, have] : fixed) {
       remaining[type] = std::max(0, remaining[type] - have);
   }
   ```

2. **Search over choice assignments**
   Use depth-first search/backtracking over `flexible` to see how many missing units can be covered. Each `ChoicePack` contributes at most one of its options.
   ```cpp
   bool dfs(int idx, ResourceCounts rem) {
       if (idx == flexible.size()) return rem.emptyOrZero();
       for (auto& opt : flexible[idx].options) {
           ResourceCounts next = rem;
           for (auto [t, n] : opt) {
               next[t] = std::max(0, next[t] - n);
           }
           // Remove zero entries for easier termination checks
           pruneZeros(next);
           if (dfs(idx + 1, next)) return true;
       }
       // Also allow skipping packs that are irrelevant to the current cost
       if (dfs(idx + 1, rem)) return true;
       return false;
   }
   ```

3. **Gold fallback**
   If DFS ends with residual `rem` not fully zero, compute gold cost for the remaining quantity using existing market rules (opponent brown/gray counts, trading post discounts, etc.). If the required gold `<= playerGold`, the purchase is affordable.
   ```cpp
   int computeGoldCost(const ResourceCounts& rem, const Board& board);
   bool affordable = dfs(0, remaining) || computeGoldCost(remainingAfterBestChoices) <= gold;
   ```
   A small optimization is to track the minimal residual across DFS branches and compute the minimal gold spend.

### Complexity Considerations
- Number of choice packs per player is small in Duel, so simple DFS/backtracking is practical.
- To avoid recomputation, memoize `(idx, remKey)` where `remKey` can be a compact tuple of remaining counts sorted by `ResourceType`.

## 4. API Sketch

```cpp
class Player {
public:
    void addFixedResource(ResourceType type, int qty = 1);
    void addChoicePack(ChoicePack pack);

    bool canAfford(const ResourceCounts& cost, const Board& board) const {
        ResourceCounts rem = subtractFixed(cost);
        int bestGold = dfsMinGold(0, rem, board);
        return bestGold <= gold;
    }

private:
    ResourceCounts subtractFixed(const ResourceCounts& cost) const;
    int dfsMinGold(size_t idx, const ResourceCounts& rem, const Board& board) const;
};
```

`dfsMinGold` should return the minimal gold required after optimally assigning the remaining choice packs (return 0 if fully covered by resources). This keeps `canAfford` clean and allows reuse in UI warnings or AI evaluation.

---
This separation preserves the per-build flexibility of choice resources, keeps `Player` responsible for resource accounting, and isolates the search logic needed to honor Seven Wonders Duel's OR-production rules.
