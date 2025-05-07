#pragma once

#include "../Player.hpp"

class Governor final : public Player {
public:
    explicit Governor(const std::string& name);
    void blockTax(Player *target);

    void useAbility(Player *target) override;
};
