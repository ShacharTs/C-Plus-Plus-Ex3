#include "../roleHeader/Baron.hpp"
#include <iostream>

Baron::Baron(const std::string &name) : Player(name) {
    role = Role::Baron;
}


void Baron::useAbility(coup::Game &game) {
    try {
        removeCoins(3);
        addCoins(6);
        playerUsedTurn();
    } catch (std::exception &e) {
        throw std::runtime_error(e.what());
    }
}

void Baron::passiveAbility() {
    addCoins(1);
}



void Baron::listOptions() const {
    printPlayerStats();
    std::cout << "Choose an action:\n"
            << "1. Gather [Free: Gain 1 coin]\n"
            << "2. Tax [Free: Gain 2 coins]\n"
            << "3. Bribe [4 coins: Gain extra turn this turn]\n"
            << "4. Arrest [Free: Steal a coin from another player]\n"
            << "5. Sanction [3 Coins: Choose a player to block using Tax or Gather for one turn]\n"
            << "6. Coup [7 coins: Kick a player form the game]\n"
            << "7. Use Ability [3 coins: Gain 6 coins]\n"
            << "0. Skip Turn\n"
            << "Enter choice: ";
}
