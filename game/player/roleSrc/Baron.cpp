#include "../roleHeader/Baron.hpp"
#include <iostream>

Baron::Baron(const std::string& name) : Player(name) {
    role = Role::Baron;
}


void Baron::legitInvestment() {
    std::cout << "TEST" << std::endl;
}

void Baron::useAbility() {
    legitInvestment();
}





