#include "Wonder.h"

Wonder::Wonder(std::string name, Cost cost, Effect effect)
    : name(name), cost(cost), effect(effect), built(false) {}

std::string Wonder::getName() const {
    return name;
}

const Cost& Wonder::getCost() const {
    return cost;
}

const Effect& Wonder::getEffect() const {
    return effect;
}

bool Wonder::isBuilt() const {
    return built;
}

void Wonder::build() {
    built = true;
}
