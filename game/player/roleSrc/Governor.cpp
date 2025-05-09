#include "../roleHeader/Governor.hpp"

#include <Game.hpp>
#include <iostream>
using namespace std;

Governor::Governor(const std::string& name) :Player(name) {
    role = Role::Governor;
}


void Governor::useAbility(coup::Game &game) {
    const std::string blockTax = "Block tax";
    const size_t playerIndex = game.choosePlayer(this, blockTax);
    if (playerIndex == game.getTurn()) {
        throw std::logic_error("You can not block yourself");
    }
    game.getPlayers().at(playerIndex)->canTax = false;
    playerUsedTurn();
}

void Governor::tax() {
    addCoins(3);
    playerUsedTurn();
}