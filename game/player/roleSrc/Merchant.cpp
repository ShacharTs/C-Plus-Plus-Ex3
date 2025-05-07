#include  "../roleHeader/Merchant.hpp"
#include <iostream>

Merchant::Merchant(const std::string &name) : Player(getName()) {
    role = Role::Merchant;
}
