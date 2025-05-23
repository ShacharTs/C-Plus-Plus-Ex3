#pragma once

#include "../Player.hpp"
#include "../../Game.hpp"
class Baron final : public virtual Player {
public:
    explicit Baron(const std::string& name);

    void useAbility(coup::Game &game) override;
    void passiveAbility() override;
    void listOptions() const override;
    Player* clone() const override;

};
