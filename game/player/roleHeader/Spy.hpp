#pragma once
#include <vector>

#include "../Player.hpp"

class Spy final : public Player {
public:
    explicit  Spy(const std::string& name);
    void watchCoins(const std::vector<Player *> &players);

    void blockArrest(coup::Game &game);

    void useAbility(coup::Game &game) override;
};
