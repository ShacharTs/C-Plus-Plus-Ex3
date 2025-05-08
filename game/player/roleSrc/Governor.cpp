#include "../roleHeader/Governor.hpp"
#include <iostream>
using namespace std;

Governor::Governor(const std::string& name) :Player(name) {
    role = Role::Governor;
}



/**
 * Ability to block another player to use tax ability for one turn
 * @param target another player
 */
void Governor::useAbility(Player* target)  {

}






