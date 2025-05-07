#pragma once
#include "../Player.hpp"

class Judge final : public Player {
public:
    explicit Judge(const std::string& name);
    void cancelBribe(Player* target);
    void useAbility(Player* target) override;
};