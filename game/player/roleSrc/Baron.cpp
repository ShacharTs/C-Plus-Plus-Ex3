#include "../roleHeader/Baron.hpp"
#include <iostream>

Baron::Baron(const std::string& name) : Player(name) {
    role = Role::Baron;
}


void Baron::useAbility(coup::Game &game) {
    try {
        removeCoins(3);
        addCoins(6);
        playerUsedTurn();
    }catch (std::exception &e) {
        throw std::logic_error(e.what());
    }



}