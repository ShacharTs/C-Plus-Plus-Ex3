#include "../roleHeader/Spy.hpp"
#include <Game.hpp>
#include <iostream>

Spy::Spy(const std::string &name) : Player(name) {
    role = Role::Spy;
}

/**
 * allow spy to block another player from using arrest for one turn
 * for some reasons, this is a free ablilty (Gods know why)
 */
void Spy::blockArrest(coup::Game &game) {
    const std::string blockArrest = "Block Arrest";
    const size_t playerIndex = game.choosePlayer(this, blockArrest);
    if (playerIndex == game.getTurn()) {
        throw std::logic_error("You can not block yourself");
    }
    game.getPlayers().at(playerIndex)->canArrest = false;
    addExtraTurn();
}

/**
 * allow spy to see all players coins
 @param players player list
 */
void Spy::watchCoins(const std::vector<Player *> &players) {
    std::cout << "\n=== Players coins ===" << std::endl;
    for (size_t i = 0; i < players.size(); i++) {
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
            << "2. Block player from using arrest" << std::endl;
    size_t input = 0;
    std::cin >> input;
    if (input == 1) {
        watchCoins(game.getPlayers());
    } else if (input == 2) {
        blockArrest(game);
    } else {
        std::__throw_logic_error("Invalid input please try again");
    }
}

void Spy::listOptions() const {
    printPlayerStats();
    std::cout << "Choose an action:\n"
            << "1. Gather [Free: Gain 1 coin]\n"
            << "2. Tax [Free: Gain 2 coins]\n"
            << "3. Bribe [4 coins: Gain extra turn this turn]\n"
            << "4. Arrest [Free: Steal a coin from another player]\n"
            << "5. Sanction [3 Coins: Choose a player to block using Tax or Gather for one turn]\n"
            << "6. Coup [7 coins: Kick a player form the game]\n"
            << "7. Use Ability [Free: Watch all players's coin, Free: Choose a player to block Arrest for one turn]\n"
            << "0. Skip Turn\n"
            << "Enter choice: ";
}
