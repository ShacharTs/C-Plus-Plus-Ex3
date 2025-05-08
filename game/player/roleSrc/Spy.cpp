#include "../roleHeader/Spy.hpp"

#include <Game.hpp>
#include <iostream>

Spy::Spy(const std::string &name) : Player(name) {
    role = Role::Spy;
}

/**
 * allow spy to block another player from using arrest for one turn
 */
void Spy::blockArrest(coup::Game &game) {
    const std::string blockArrest = "Block Arrest";
    size_t playerIndex = game.choosePlayer(game.players,this,blockArrest);
    game.players.at(playerIndex)->canArrest = false;
    addExtraTurn();
}

/**
 * allow spy to see all players coins
 @param players player list
 */
void Spy::watchCoins(const std::vector<Player*> &players) {
    std::cout << "\n=== Players coins ===" << std::endl;
    for (size_t i = 0; i < players.size();i++) {
        if (players.at(i)->getName() != this->getName()) {
            std::cout << players.at(i)->getName() << " coins: " << players.at(i)->getCoins() << std::endl;
        }
    }
    std::cout << "=== Players coins ===\n" << std::endl;
    addExtraTurn();
}

void Spy::useAbility(coup::Game &game) {
        std::cout << "Please choose ability:\n"
                    << "1. Watch players coins\n"
                    << "2. Block player from using arrest" <<std::endl;
        size_t input = 0;
        std::cin >> input;
        if (input == 1) {
            watchCoins(game.players);
        }
        else if (input == 2) {
            blockArrest(game);
        }else {
            std::__throw_logic_error("Invalid input please try again");
        }
}

