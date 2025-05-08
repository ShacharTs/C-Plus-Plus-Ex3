#pragma once
#include "../Player.hpp"

class Merchant final :public Player {
public:
    explicit Merchant(const std::string& name);

    void passiveAbility();
};