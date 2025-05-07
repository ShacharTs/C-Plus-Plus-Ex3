#include <iostream>
#include "Player.hpp"
#include "../Game.hpp"

using namespace std;

Player::Player(string playerName) : playerName(std::move(playerName)) {
}

Player::~Player() = default;

string Player::getName() const {
    return playerName;
}

Role Player::getRole() const {
    return role;
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

void Player::setLastArrestedBy(const Player *ptrPlayer) {
    lastArrestedBy = ptrPlayer;
}

const Player *Player::getLastArrestedBy() const {
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



void Player::requestGather(coup::Game &game) {

}


void Player::requestTax(coup::Game &game) {
}

void Player::requestBribe(coup::Game &game) {
}

void Player::requestArrest(coup::Game &game, Player *target) {
}

void Player::requestSanction(coup::Game &game, Player *target) {
}

void Player::requestCoup(coup::Game &game, Player *target) {
}
