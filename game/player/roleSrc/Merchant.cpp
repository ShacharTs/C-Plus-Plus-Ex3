#include  "../roleHeader/Merchant.hpp"
#include <iostream>

Merchant::Merchant(const std::string &name) : Player(name) {
    role = Role::Merchant;
}

void Merchant::passiveAbility() {
        addCoins(1);
}
