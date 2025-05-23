#pragma once

#include "../Player.hpp"

class Governor final : public virtual Player {
public:
    explicit Governor(const std::string& name);
    void tax() override;
    void passiveAbility(Player* target) override;

    void useAbility(coup::Game &game) override;

    void passiveAbility() override;

    void listOptions() const override;
    Player* clone() const override;

};
