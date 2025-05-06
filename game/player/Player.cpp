#include "Player.hpp"
#include <iostream>

using namespace std;

Player::Player(string playerName) : playerName(std::move(playerName)) {}

Player::~Player() = default;

string Player::getName() const {
    return playerName;
}

int Player::getCoins() const {
    return coins;
}