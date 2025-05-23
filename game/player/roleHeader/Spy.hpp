#pragma once
#include "../Player.hpp"
#include "../../Game.hpp"

class Spy : public virtual Player {
public:
    explicit  Spy(const std::string& name);

    Player* clone() const override;
    void passiveAbility(Player* target) override;
    void useAbility(coup::Game& game) override;
    std::string getCoinReport(const coup::Game& game);

    void passiveAbility() override;


    void listOptions() const override;
};
