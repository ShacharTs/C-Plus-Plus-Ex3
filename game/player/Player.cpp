#include <iostream>
#include "Player.hpp"

#include "../GameExceptions.hpp"


using namespace std;

Player::Player(string playerName) : playerName(std::move(playerName)) {
}

Player::~Player() = default;


// Virtual ability for roles
void Player::useAbility(coup::Game &game) {

}

void Player::passiveAbility() {
}

void Player::passiveAbility(Player *target) {
}


string Player::getName() const {
    return playerName;
}

Role Player::getRole() const {
    return role;
}

void Player::resetPlayerTurn() {
    numberOfTurns = 1;
}


string Player::roleToString(const Role role) {
    switch (role) {
        case Role::Governor: return "Governor";
        case Role::Spy: return "Spy";
        case Role::Baron: return "Baron";
        case Role::General: return "General";
        case Role::Judge: return "Judge";
        case Role::Merchant: return "Merchant";
        default: return "Unknown";
    }
}

void Player::printPlayerStats() const {
    cout << "\n--- Turn: " << getName() << " ---" << endl;
    cout << "Coins: " << getCoins() << endl;
    cout << "Role: " << roleToString(getRole()) << endl;
}

void Player::listOptions() const {
    printPlayerStats();
    cout << "Choose an action:\n"
            << "1. Gather\n"
            << "2. Tax\n"
            << "3. Bribe\n"
            << "4. Arrest\n"
            << "5. Sanction\n"
            << "6. Coup\n"
            << "7. Use Ability\n"
            << "0. Skip Turn\n"
            << "Enter choice: ";
}


int Player::getCoins() const {
    return coins;
}

/**
 * if a player has extra turn, at the end of his turn, disable it
 */
void Player::playerUsedTurn() {
    if (numberOfTurns > 0) {
        numberOfTurns--;
    }
}

/**
 *
 */
void Player::addExtraTurn() {
    numberOfTurns +=1;
}

bool Player::hasExtraTurn() {
    return numberOfTurns > 0;
}

bool Player::isGatherAllow() const {
    return canGather;
}

bool Player::isTaxAllow() const {
    return canTax;
}

bool Player::isBribeAllow() const {
    return canBribe;
}

bool Player::isArrestAllow() const {
    return canArrest;
}



bool Player::isCoupAllow() const {
    return canCoup;
}

bool Player::isCoupShieldActive() const {
    return coupShield;
}

void Player::gather() {
    addCoins(1);
    playerUsedTurn();
}

void Player::tax() {
    addCoins(2);
    playerUsedTurn();
}

void Player::bribe() {
    addExtraTurn();
    addExtraTurn();
}


void Player::arrest(Player *targetPlayer) {
    if (isTargetSelf(targetPlayer)) {
        throw SelfError("You cannot arrest yourself.");
    }

    if (targetPlayer->getRole() == Role::General) {
        this->playerUsedTurn();
        this->setLastArrestedPlayer(targetPlayer);
        throw ArrestError("Coin stole from General. Action cancelled.");
    }

    try {
        targetPlayer->removeCoins(1);
        this->addCoins(1);
    } catch (const std::exception& e) {
        throw ArrestError("Arrest failed: " + std::string(e.what()));
    }

    this->playerUsedTurn();
    this->setLastArrestedPlayer(targetPlayer);
}


void Player::sanction(Player *target) {
    if (isTargetSelf(target)) {
        throw SelfError("You cannot sanction yourself.");
    }
    removeCoins(3);
    target->canGather = false;
    target->canTax = false;
    playerUsedTurn();
}

bool Player::isTargetSelf(const Player *target) {
    return (this == target);
}

void Player::setLastArrestedPlayer(const Player *ptrPlayer) {
    lastArrestedBy = ptrPlayer;
}

const Player *Player::getLastArrestedPlayer() const {
    return lastArrestedBy;
}


void Player::addCoins(const int amount) {
    coins += amount;
}

void Player::removeCoins(const int amount) {
    if (coins < amount) {
        throw CoinsError("Not enough coins.");
    }
    coins -= amount;
}

void Player::removeDebuff() {
    canGather = true;
    canTax = true;
    canArrest = true;
}



