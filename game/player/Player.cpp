#include <iostream>
#include "Player.hpp"
#include "../GameExceptions.hpp"

using namespace std;

//------------------------------------------------------------------------------
// Construction / Destruction
//------------------------------------------------------------------------------

/**
 * @brief Construct a new Player instance with the given name.
 * @param playerName Name identifier for this player.
 */
Player::Player(string playerName)
    : playerName(std::move(playerName)) {
}

// Player *Player::clone() const {
// }


/**
 * @brief Default virtual destructor for Player.
 */
Player::~Player() {
}

// Copy constructor
Player::Player(const Player& other)
    : playerName(other.playerName),
      role(other.role),
      coins(other.coins),
      numberOfTurns(other.numberOfTurns),
      canGather(other.canGather),
      canTax(other.canTax),
      canBribe(other.canBribe),
      canArrest(other.canArrest),
      canCoup(other.canCoup),
      coupShield(other.coupShield),
      lastArrestedBy(other.lastArrestedBy) {
}

// Copy assignment operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        playerName = other.playerName;
        role = other.role;
        coins = other.coins;
        numberOfTurns = other.numberOfTurns;
        canGather = other.canGather;
        canTax = other.canTax;
        canBribe = other.canBribe;
        canArrest = other.canArrest;
        canCoup = other.canCoup;
        coupShield = other.coupShield;
        lastArrestedBy = other.lastArrestedBy;
    }
    return *this;
}



//------------------------------------------------------------------------------
// Accessors
//------------------------------------------------------------------------------

/**
 * @return The name of this player.
 */
string Player::getName() const {
    return playerName;
}

/**
 * @return The role enum of this player.
 */
Role Player::getRole() const {
    return role;
}

//------------------------------------------------------------------------------
// Turn Management
//------------------------------------------------------------------------------

/**
 * @brief Reset the player's available turns to the default of 1.
 */
void Player::resetPlayerTurn() {
    numberOfTurns = 1;
}

//------------------------------------------------------------------------------
// Debug / UI Helpers
//------------------------------------------------------------------------------

/**
 * @brief Convert a Role value to its string representation.
 * @param role The role value.
 * @return Corresponding role name string.
 */
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

/**
 * @brief Print player stats (name, coins, role) to std::cout.
 */
void Player::printPlayerStats() const {
    cout << "\n--- Turn: " << getName() << " ---" << endl;
    cout << "Coins: " << getCoins() << endl;
    cout << "Role: " << roleToString(getRole()) << endl;
}

/**
 * @brief Display the list of available actions for this player.
 */
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

//------------------------------------------------------------------------------
// Coin & Turn Counters
//------------------------------------------------------------------------------

/**
 * @return The current coin balance.
 */
int Player::getCoins() const {
    return coins;
}

/**
 * @brief Use up one of the player's available turns at end of action.
 */
void Player::playerUsedTurn() {
    if (numberOfTurns > 0) {
        --numberOfTurns;
        removeDebuff();
    }
}

/**
 * @brief Grant an extra turn to the player.
 */
void Player::addExtraTurn() {
    numberOfTurns += 1;
}

/**
 * @return True if the player still has turns remaining.
 */
bool Player::hasExtraTurn() const {
    return numberOfTurns > 0;
}

//------------------------------------------------------------------------------
// Ability Flag Queries
//------------------------------------------------------------------------------

bool Player::isGatherAllow() const { return canGather; }
bool Player::isTaxAllow() const { return canTax; }
bool Player::isBribeAllow() const { return canBribe; }
bool Player::isArrestAllow() const { return canArrest; }
bool Player::isCoupAllow() const { return canCoup; }
bool Player::isCoupShieldActive() const { return coupShield; }

//------------------------------------------------------------------------------
// Core Actions
//------------------------------------------------------------------------------

/**
 * @brief Gather action: add one coin and consume a turn.
 */
void Player::gather() {
    if (isGatherAllow()) {
        addCoins(1);
        playerUsedTurn();
    }else {
        throw GatherError("Gather action is not allowed.");
    }
}

/**
 * @brief Tax action: add two coins and consume a turn.
 */
void Player::tax() {
    if (isTaxAllow()) {
        addCoins(2);
        playerUsedTurn();
    }else {
        throw TaxError("Tax action is not allowed.");
    }

}

/**
 * @brief Bribe action: grant an extra turn.
 */
void Player::bribe() {
    addExtraTurn();
}

//------------------------------------------------------------------------------
// Targeted Actions
//------------------------------------------------------------------------------

/**
 * @brief Arrest action: steal one coin from target player.
 * @throws SelfError if targeting self.
 * @throws ArrestError if action fails or is blocked.
 */
void Player::arrest(Player *targetPlayer) {
    if (isTargetSelf(targetPlayer)) {
        throw SelfError("You cannot arrest yourself.");
    }
    if (targetPlayer->getRole() == Role::General) {
       try {
           playerUsedTurn();
           setLastArrestedPlayer(targetPlayer);
           return;
       } catch (const exception &e) {
           throw ArrestError(string("Arrest failed: ") + e.what());
       }
    }
    if (targetPlayer->getRole() == Role::Merchant) {
        try {
            targetPlayer->removeCoins(2);
            playerUsedTurn();
            setLastArrestedPlayer(targetPlayer);
            return;
        } catch (const exception &e) {
            throw ArrestError(string("Arrest failed: ") + e.what());
        }
    }
    try {
        targetPlayer->removeCoins(1);
        addCoins(1);
        playerUsedTurn();
        setLastArrestedPlayer(targetPlayer);
    } catch (const exception &e) {
        throw ArrestError(string("Arrest failed: ") + e.what());
    }
}

/**
 * @brief Sanction action: remove 3 coins and disable target's gather/tax.
 * @throws SelfError if targeting self.
 */
void Player::sanction(Player *target) {
    if (isTargetSelf(target)) {
        throw SelfError("You cannot sanction yourself.");
    }
    removeCoins(3);
    if (target->getRole() == Role::Baron) {
        target->addCoins(1);
    }
    target->canGather = false;
    target->canTax = false;
    playerUsedTurn();
}

/**
 * @brief Check if the target equals this player.
 * @return True if pointers match.
 */
bool Player::isTargetSelf(const Player *target) {
    return this == target;
}

//------------------------------------------------------------------------------
// State Mutators
//------------------------------------------------------------------------------

void Player::setLastArrestedPlayer(const Player *ptrPlayer) {
    lastArrestedBy = ptrPlayer;
}

const Player *Player::getLastArrestedPlayer() const {
    return lastArrestedBy;
}

//------------------------------------------------------------------------------
// Coin Management
//------------------------------------------------------------------------------

/**
 * @brief Add specified coins to balance.
 */
void Player::addCoins(const int amount) {
    if (amount < 0) {
        throw CoinsError("Cannot add negative coins.");
    }
    coins += amount;
}

/**
 * @brief Remove specified coins, throwing if insufficient.
 * @throws CoinsError
 */
void Player::removeCoins(const int amount) {
    if (amount < 0) {
        throw CoinsError("Negative coins.");
    }
    if (amount > coins) {
        throw CoinsError("Not enough coins to remove.");
    }
    coins -= amount;
}

/**
 * @return Remaining turns count.
 */
int Player::getNumOfTurns() {
    return numberOfTurns;
}

//------------------------------------------------------------------------------
// Debuff Management
//------------------------------------------------------------------------------

/**
 * @brief Reset all action restriction flags to allowed.
 */
void Player::removeDebuff() {
    canGather = true;
    canTax = true;
    canArrest = true;
}
