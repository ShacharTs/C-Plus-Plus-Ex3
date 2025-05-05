#pragma once
#include "Player.hpp"

class General final : public Player {
public:
    explicit  General(std::string name);
    void shield(Player* target);
    void passiveAbility();
};