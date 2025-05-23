#pragma once
#include "../Player.hpp"

class Judge final : public Player {
public:
    explicit Judge(const std::string& name);
    void useAbility(coup::Game &game) override;
    void passiveAbility(Player *target) override;
    void listOptions() const override;
};