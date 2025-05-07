#include  "../roleHeader/Merchant.hpp"
#include <iostream>

Merchant::Merchant(const std::string &name) : Player(name) {
    role = Role::Merchant;
}
