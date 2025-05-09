#include "../roleHeader/General.hpp"

#include <Game.hpp>
#include <iostream>

General::General(const std::string& name) :Player(name) {
    role = Role::General;
}

void General::useAbility(coup::Game &game) {
    const std::string coupShield = "Active coup shield";
    size_t playerIndex = game.getTurn();
    playerIndex = game.choosePlayer(this, coupShield);
    removeCoins(5);
    game.getPlayers().at(playerIndex)->coupShield = true;
    playerUsedTurn();

}






