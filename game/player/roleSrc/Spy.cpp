#include "../roleHeader/Spy.hpp"
#include <iostream>

Spy::Spy(const std::string &name) : Player(name) {
    role = Role::Spy;
}

/**
 * allow spy to block another player from using arrest for one turn
 * @param target another player
 */
void Spy::blockArrest(Player *target) {

}

/**
 *  Allow spy to peek another player total coins
 * @param target another player
 */
void Spy::watchCoins(Player *target) {

}

void Spy::useAbility() {

    std::cout << "SPY ABLILTY "<< std::endl;
}

void Spy::useAbility(Player *target) {
    watchCoins(target);
    blockArrest(target);
}



