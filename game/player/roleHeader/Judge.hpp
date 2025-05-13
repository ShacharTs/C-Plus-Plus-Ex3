#pragma once
#include "../Player.hpp"
#include "../../Game.hpp"

class Judge final : public Player {
public:
    explicit Judge(const std::string& name);
    void passiveAbility(Player *target) override;
    void listOptions() const override;
    void useAbility(coup::Game& game) override;
};