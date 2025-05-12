#include "../roleHeader/Judge.hpp"
#include <../../game/Game.hpp>
#include <iostream>

Judge::Judge(const std::string &name) : Player(name) {
    role = Role::Judge;
}


void Judge::passiveAbility(Player* target) {
    target->removeCoins(1);

}

void Judge::useAbility(coup::Game& game) {
    /*const std::string blockBribe = "Block a player from using bribe";
    size_t playerIndex = game.getTurn();
    playerIndex = game.choosePlayer(this, blockBribe);
    game.getPlayers().at(playerIndex)->canBribe = false;
    playerUsedTurn();*/
}


void Judge::listOptions() const {
    printPlayerStats();
    std::cout << "Choose an action:\n"
        << "1. Gather [Free: Gain 1 coin]\n"
        << "2. Tax [Free: Gain 2 coins]\n"
        << "3. Bribe [4 coins: Gain extra turn this turn]\n"
        << "4. Arrest [Free: Steal a coin from another player]\n"
        << "5. Sanction [3 Coins: Choose a player to block using Tax or Gather for one turn]\n"
        << "6. Coup [7 coins: Kick a player form the game]\n"
        << "7. Use Ability [Free: Choose a player to block them to use bribe]\n"
        << "0. Skip Turn\n"
        << "Enter choice: ";
}