#pragma once
#include "../Player.hpp"

class Spy final : public Player {
public:
    explicit  Spy(const std::string& name);
    void watchCoins(Player *target);

    void blockArrest(Player *target);

    void useAbility(Player *target) override;
    void useAbility() override;
};
