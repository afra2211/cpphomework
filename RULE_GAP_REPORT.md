# 7 Wonders Duel Rule Coverage Gaps

This document highlights rule areas from the official rulebook (https://cdn.svc.asmodee.net/production-rprod/storage/downloads/games/7wonders-duel/en/7du-rules-us-15990558193s5I6.pdf) that still need implementation or correction in the current codebase.

## Setup and Player Model
- `Game::init` still tries to instantiate `Player` directly with AI flags, but `Player` is abstract and its constructor accepts only a name. The game has no concrete `HumanPlayer`/`AIPlayer` wiring and cannot set up players, coins, or turn order reliably. 【F:src/Game.cpp†L14-L24】【F:include/Player.h†L13-L53】
- Player state tracks coins, resources, shields, VP, and science symbols, but there is no storage for progress tokens, discard pile access, or trade discounts that are required for science progress and commercial effects. 【F:include/Player.h†L45-L75】

## Wonders
- The wonder selection flow only places the chosen wonders onto the board without enforcing the 7-wonder global limit or removing the 8th unbuilt wonder. Wonders taken during play are not tracked as “built” vs. “available,” and their special effects (play again, shields, coin/VP, destroy card) are not wired into turn resolution. 【F:src/Game.cpp†L26-L118】【F:src/Board.cpp†L121-L151】

## Age Decks and Layout
- Age decks in `Deck::getAge1Deck` are partially hard-coded and contain inconsistent variable names (e.g., `palisadeEffect` vs. `effect`), suggesting the lists are incomplete and may not match the official 23/20/17 card compositions, random removal of 3 cards per Age, or Age III guild insertion. 【F:src/Deck.cpp†L30-L151】
- While `Board::setupAge` builds pyramid layouts and reveals accessible cards, there is no Age transition driver in `Game` that removes 3 cards unseen, inserts guilds, or resets the pyramid at Age end. 【F:src/Board.cpp†L15-L104】

## Turn Actions
- Only card construction is partially covered. There is no discard action that pays `2 + yellow-card count` and stores the discarded card, and wonder construction does not consume an Age card or apply wonder effects. Play-again sequencing is absent. 【F:src/Player.cpp†L54-L120】【F:src/Game.cpp†L10-L118】

## Cost & Trading Rules
- `Player::calculateCost` handles base trading and chaining but leaves yellow-card discounts as TODOs, and it counts all of the player’s own resources as reusable without differentiating fixed/choice production or wonder/ yellow exclusions from opponent cost calculation. Payments are coin-only with no resource availability check. 【F:src/Player.cpp†L54-L120】

## Progress Tokens & Science
- `Board::setupProgressTokens` initializes tokens, but there is no call in `Game::init` or Age setup to place the five available tokens. The science pair handler only increments counts and contains comments for external triggers; no progress token is awarded nor is science victory enforced. 【F:src/Board.cpp†L153-L204】

## Military Conflict
- `Board::moveMilitary` updates the pawn position but does not clamp to capitals or trigger military token penalties/rewards. `Game::handleMilitaryConflict` is a stub (not shown in code paths) and victory detection is not connected to turn flow. 【F:src/Board.cpp†L105-L120】【F:src/Game.cpp†L10-L24】

## Scoring and End Conditions
- There is no end-of-age handoff or final scoring that sums building VP, wonder VP, progress token VP, military track points, or coin conversion. Science and military instant victories are not checked immediately after builds. 【F:src/Game.cpp†L10-L118】

## AI Integration
- `AIPlayer` still calls the removed `Player` constructor with an AI flag and does not return a decision compatible with `Game::playTurn`, leaving AI turns unplayable. 【F:src/AIPlayer.cpp†L9-L65】【F:include/Player.h†L19-L53】

## Required Next Steps
1. Reintroduce concrete `HumanPlayer`/`AIPlayer` classes with proper construction in `Game::init`, including setup of coins, progress tokens, and wonder limits.
2. Complete Age deck definitions per rulebook, implement random removal and guild insertion, and drive Age transitions that rebuild the pyramid and reveal cards.
3. Implement all turn actions: discard with coin gain and discard pile tracking, wonder construction consuming an Age card, application of wonder effects, and play-again sequencing.
4. Finish trading and chaining: apply yellow-card discounts, wonder/progress token cost reductions, and opponent brown/grey production pricing while ensuring resources are considered reusable.
5. Wire progress token acquisition on science pairs, include Law as a symbol for scientific victory, and enforce immediate science/military victory checks.
6. Add military token thresholds, clamped pawn movement, coin penalties, and final scoring that covers all VP sources and tie-breakers.
