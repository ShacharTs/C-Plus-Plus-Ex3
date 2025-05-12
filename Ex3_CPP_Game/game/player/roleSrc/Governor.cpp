#include "../roleHeader/Governor.hpp"
#include <../../game/Game.hpp>
#include <iostream>
using namespace std;

Governor::Governor(const std::string& name) :Player(name) {
    role = Role::Governor;
}


void Governor::tax() {
    addCoins(3);
    playerUsedTurn();
}

void Governor::listOptions() const {
    printPlayerStats();
    std::cout << "Choose an action:\n"
        << "1. Gather [Free: Gain 1 coin]\n"
        << "2. Tax [Free: Gain 3 coins]\n"
        << "3. Bribe [4 coins: Gain extra turn this turn]\n"
        << "4. Arrest [Free: Steal a coin from another player]\n"
        << "5. Sanction [3 Coins: Choose a player to block using Tax or Gather for one turn]\n"
        << "6. Coup [7 coins: Kick a player form the game]\n"
        << "7. Use Ability [Free: Choose a player to block using Tax for one turn]\n"
        << "0. Skip Turn\n"
        << "Enter choice: ";
}