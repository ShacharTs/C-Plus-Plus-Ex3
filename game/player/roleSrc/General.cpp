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

void General::listOptions() const {
    printPlayerStats();
    std::cout << "Choose an action:\n"
            << "1. Gather [Free: Gain 1 coin]\n"
            << "2. Tax [Free: Gain 2 coins]\n"
            << "3. Bribe [4 coins: Gain extra turn this turn]\n"
            << "4. Arrest [Free: Steal a coin from another player]\n"
            << "5. Sanction [3 Coins: Choose a player to block using Tax or Gather for one turn]\n"
            << "6. Coup [7 coins: Kick a player form the game]\n"
            << "7. Use Ability [5 Coins: Choose a yourself or a player to active one time shield from coup]\n"
            << "0. Skip Turn\n"
            << "Enter choice: ";
}







