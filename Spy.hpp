#pragma once
#include "Player.hpp"

class Spy final : public Player {
public:
    explicit  Spy(std::string name);
    void watchCoins(Player *target);

    void blockArrest(Player *target);
};
