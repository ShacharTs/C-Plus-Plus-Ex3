#pragma once

#include "Player.hpp"

class Baron final : public Player {
public:
    explicit Baron(std::string name);
    void legitInvestment();

    void passiveAbility();
};
