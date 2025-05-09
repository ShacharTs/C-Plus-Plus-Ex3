#include "../roleHeader/Judge.hpp"
#include <iostream>

Judge::Judge(const std::string &name) : Player(name) {
    role = Role::Judge;
}

/**
 * Judge can use cancel a bribe that another player cast
 * @param player another player
 */
void Judge::cancelBribe(Player *player) {

}

void Judge::useAbility(coup::Game &game) {
    useAbility(game);
}


