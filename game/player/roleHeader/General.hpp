#pragma once
#include "../Player.hpp"




class General final : public virtual Player {
public:
    explicit General(const std::string& name);

    void passiveAbility() override;

    void listOptions() const override;
    Player* clone() const override;
    void passiveAbility(Player* target) override;

    void useAbility(coup::Game &game) override;
};