#pragma once

#include "../Player.hpp"

class Spy final : public Player {
public:
    explicit  Spy(const std::string& name);

    void useAbility(coup::Game &game) override;

    void listOptions() const override;
};
