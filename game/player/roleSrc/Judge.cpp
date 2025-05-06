#include "../roleHeader/Judge.hpp"
#include <iostream>

Judge::Judge(const std::string &name) : Player(getName()) {}

/**
 * Judge can use cancel a bribe that another player cast
 * @param player another player
 */
void Judge::undoBribe(Player *player) {

}

void Judge::passiveAbility() {

}


