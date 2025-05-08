#include "../roleHeader/Governor.hpp"
#include <iostream>
using namespace std;

Governor::Governor(const std::string& name) :Player(name) {
    role = Role::Governor;
}

void Governor::useAbility(coup::Game &game) {
    Player::useAbility(game);
}

void Governor::tax() {
    addCoins(3);
}
