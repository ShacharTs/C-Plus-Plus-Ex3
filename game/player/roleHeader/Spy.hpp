#pragma once
#include "../Player.hpp"
#include "../../Game.hpp"

class Spy final : public Player {
public:
    explicit  Spy(const std::string& name);

    Player* clone() const override;

    void useAbility(coup::Game& game) override;
    std::string getCoinReport(const coup::Game& game);
    void listOptions() const override;
};
