#include  "../roleHeader/Merchant.hpp"
#include <iostream>

Merchant::Merchant(const std::string &name) : Player(name) {
    role = Role::Merchant;
}

Player* Merchant::clone() const {
    return new Merchant(*this);
}


void Merchant::passiveAbility() {
        addCoins(1);
}

void Merchant::passiveAbility(Player *target) {
}

void Merchant::useAbility(coup::Game& game) {
   // addCoins(1);
}

void Merchant::listOptions() const {
    printPlayerStats();
    std::cout << "Choose an action:\n"
            << "1. Gather [Free: Gain 1 coin]\n"
            << "2. Tax [Free: Gain 2 coins]\n"
            << "3. Bribe [4 coins: Gain extra turn this turn]\n"
            << "4. Arrest [Free: Steal a coin from another player]\n"
            << "5. Sanction [3 Coins: Choose a player to block using Tax or Gather for one turn]\n"
            << "6. Coup [7 coins: Kick a player form the game]\n"
            << "0. Skip Turn\n"
            << "Enter choice: ";
}
