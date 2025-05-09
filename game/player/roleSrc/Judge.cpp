#include "../roleHeader/Judge.hpp"
#include <Game.hpp>
#include <iostream>

Judge::Judge(const std::string &name) : Player(name) {
    role = Role::Judge;
}



void Judge::useAbility(coup::Game &game) {
    const std::string blockBribe = "Block a player from using bribe";
    size_t playerIndex = game.getTurn();
    playerIndex = game.choosePlayer(this, blockBribe);
    game.getPlayers().at(playerIndex)->canBribe = false;
    playerUsedTurn();
}


