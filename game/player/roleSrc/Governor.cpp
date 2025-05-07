#include "../roleHeader/Governor.hpp"
#include <iostream>

Governor::Governor(const std::string& name) :Player(name) {
    role = Role::Governor;
}


/**
 * Ability to block another player to use tax ability for one turn
 * @param target another player
 */
void Governor::blockTax(Player *target) {
        target->canTax = false;
}

void Governor::useAbility(Player* target)  {
    blockTax(target);
}






