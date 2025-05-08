#include <iostream>
#include "Player.hpp"


using namespace std;

Player::Player(string playerName) : playerName(std::move(playerName)) {
}

Player::~Player() = default;



void Player::useAbility(coup::Game& game) {

}


string Player::getName() const {
    return playerName;
}

Role Player::getRole() const {
    return role;
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


int Player::getCoins() const {
    return coins;
}

/**
 * if a player has extra turn, at the end of his turn, disable it
 */
void Player::removeExtraTurn() {
    if (extraTurn) {
        extraTurn = false;
    }
}

void Player::addExtraTurn() {
    extraTurn = true;
}

bool Player::hasExtraTurn() {
    return extraTurn;
}

void Player::gather() {
    addCoins(1);
}

void Player::tax() {
    addCoins(2);
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

void Player::removeCoins(int amount) {
    if (coins < amount) {
        throw runtime_error("Not enough coins.");
    }
    coins -= amount;
}



