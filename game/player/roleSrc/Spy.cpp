#include "../roleHeader/Spy.hpp"

#include <iostream>
#include <stdexcept>

Spy::Spy(const std::string &name) : Player(name) {
    role = Role::Spy;
}
Player* Spy::clone() const {
    return new Spy(*this);
}


std::string Spy::getCoinReport(const coup::Game& game) {
    std::string output = "\n=== Players coins ===\n";
    for (auto* p : game.getPlayers()) {
        if (p != this) {
            output += p->getName();
            output += " coins: ";
            output += std::to_string(p->getCoins());
            output += "\n";
        }
    }
    output += "=====================\n";
    return output;
}

void Spy::useAbility(coup::Game& game) {
    std::cout << "\n=== Players coins ===" << std::endl;
    for (size_t i = 0; i < game.players.size(); i++) {
        if (game.players.at(i)->getName() != this->getName()) {
            std::cout << game.players.at(i)->getName() << " coins: " << game.players.at(i)->getCoins() << std::endl;
        }
    }
    std::cout << "=== Players coins ===\n" << std::endl;
    addExtraTurn();
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
