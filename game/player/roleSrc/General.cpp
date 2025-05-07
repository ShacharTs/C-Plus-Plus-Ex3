#include "../roleHeader/General.hpp"
#include <iostream>

General::General(const std::string& name) :Player(getName()) {
    role = Role::General;
}

/**
 * active a shield that protected
 * @param target self or other player
 */
void General::shield(Player *target) {
}

void General::useAbility(Player* target) {

}





