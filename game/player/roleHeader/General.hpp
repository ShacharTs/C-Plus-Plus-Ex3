#pragma once
#include "../Player.hpp"




class General final : public Player {
public:
    explicit General(const std::string& name);
    void shield(Player* target);
    void useAbility(Player* target) override;
};