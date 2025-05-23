#pragma once
#include "../Player.hpp"

class Merchant final :public virtual Player {
public:
    explicit Merchant(const std::string& name);

    void passiveAbility() override;
    void passiveAbility(Player* target) override;

    void useAbility(coup::Game &game) override;

    void listOptions() const override;
    Player* clone() const override;

};