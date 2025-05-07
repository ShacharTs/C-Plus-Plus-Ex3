#include "../roleHeader/Baron.hpp"
#include <iostream>


Baron::Baron(const std::string& name) : Player(getName()) {
    role = Role::Baron;
}

void Baron::legitInvestment() {

}

void Baron::useAbility() {
    legitInvestment();
}





