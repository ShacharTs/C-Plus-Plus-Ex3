#pragma once
#include "Player.hpp"

class Judge final : public Player {
public:
    explicit Judge(std::string name);
    void undoBribe(Player* player);
    void passiveAbility();
};