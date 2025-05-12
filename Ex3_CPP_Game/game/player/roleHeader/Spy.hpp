#pragma once
#include <vector>

#include "../Player.hpp"

class Spy final : public Player {
public:
    explicit  Spy(const std::string& name);
    void useAbility(coup::Game& game) override;
    std::string getCoinReport(const coup::Game& game);
    void listOptions() const override;
};
