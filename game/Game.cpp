#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include "GameExceptions.hpp"
#include "player/roleHeader/Baron.hpp"
#include "player/roleHeader/General.hpp"
#include "player/roleHeader/Governor.hpp"
#include "player/roleHeader/Judge.hpp"
#include "player/roleHeader/Merchant.hpp"
#include "player/roleHeader/Spy.hpp"

using namespace std;

namespace coup {

//------------------------------------------------------------------------------
// Game action cost constants
//------------------------------------------------------------------------------
constexpr int COUP_COST      = 7;  ///< Coins required to perform a coup
constexpr int BRIBE_COST     = 4;  ///< Coins required to perform a bribe
constexpr int SANCTION_COST  = 3;  ///< Coins required to impose a sanction
constexpr int FORCE_COUP     = 10; ///< Unused: threshold for forced coup

/**
 * @brief Test-only factory: create a role instance by index.
 * @param index Role identifier (0..5)
 * @param name Player name for the new role
 * @return Pointer to a new Player subclass
 * @throws runtime_error if index is invalid
 */
Player *createRoleByIndex(const size_t index, const string &name) {
    switch (index) {
        case 0: return new Governor(name);
        case 1: return new Spy(name);
        case 2: return new Baron(name);
        case 3: return new General(name);
        case 4: return new Judge(name);
        case 5: return new Merchant(name);
        default:
            throw runtime_error("Error: invalid role index");
    }
}

/**
 * @brief Construct a Game with fixed roles assigned by index.
 * @param names Vector of player names
 */
Game::Game(const vector<string> &names) {
    // Assign each player a specific role for testing
    for (size_t i = 0; i < names.size(); ++i) {
        players.push_back(createRoleByIndex(i, names[i]));
    }
    currentPlayerTurn = 0;  // Start with the first player
}

/**
 * @brief Create a random role instance for a given player name.
 * Uses high-quality C++11 random facilities.
 * @param name Player name
 * @return Pointer to a new Player subclass of random type
 */
Player *Game::createRandomRole(const std::string &name) {
    static random_device rd;                         // Seed source
    static mt19937 gen(rd());                        // Mersenne Twister RNG
    static uniform_int_distribution<> dist(0, 5);    // Uniform distribution [0..5]

    switch (dist(gen)) {
        case 0: return new Governor(name);
        case 1: return new Spy(name);
        case 2: return new Baron(name);
        case 3: return new General(name);
        case 4: return new Judge(name);
        case 5: return new Merchant(name);
        default:
            throw runtime_error("Random number generation failed");
    }
}

/**
 * @brief Destructor: clean up all dynamically allocated players.
 */
Game::~Game() {
    for (Player *&player : players) {
        delete player;
    }
}

/**
 * @brief Reassign random roles to an existing list of players.
 * @param players Vector of player pointers to update
 */
void Game::getRandomRole(vector<Player *> &players) {
    srand(static_cast<unsigned>(time(nullptr)));
    for (Player *&p : players) {
        const string name = p->getName();
        delete p;  // Remove old role
        int roll = rand() % 6 + 1;  // Random number 1..6
        switch (roll) {
            case 1: p = new Governor(name); break;
            case 2: p = new Spy(name);      break;
            case 3: p = new Baron(name);    break;
            case 4: p = new General(name);  break;
            case 5: p = new Judge(name);    break;
            case 6: p = new Merchant(name); break;
            default:
                throw out_of_range("Invalid roll: " + to_string(roll));
        }
    }
}

/**
 * @brief Get zero-based index of the current player's turn.
 * @return Current player turn index
 */
int Game::getTurn() {
    return currentPlayerTurn;
}

/**
 * @brief Advance to the next player's turn, resetting state flags.
 */
void Game::nextTurn() {
    Player *current = players.at(currentPlayerTurn);
    current->resetPlayerTurn();   // Reset per-turn flags
    current->removeDebuff();      // Clear status effects
    currentPlayerTurn = (getTurn() + 1) % players.size();
}

/**
 * @brief Continue turn if extra moves remain, else advance normally.
 */
void Game::advanceTurnIfNeeded() {
    if(isGameOver(players)) {
        cout << "Game is over. No more turns." << endl;
        return;
    }
    Player *current = players.at(currentPlayerTurn);
    if (current->hasExtraTurn()) {
        //current->playerUsedTurn();  // line for test do not remove
        current->removeDebuff(); // player used turn
    } else {
        nextTurn();
    }
}

/**
 * @brief Retrieve the list of active players.
 * @return Constant reference to players vector
 */
const vector<Player *> &Game::getPlayers() const {
    return players;
}

/**
 * @brief Add coins to a specific player.
 * @param targetPlayer Player receiving coins
 * @param amount Number of coins to add
 */
void Game::addCoins(Player *targetPlayer, const int amount) {
    targetPlayer->addCoins(amount);
}

/**
 * @brief Remove coins from a specific player.
 * @param targetPlayer Player losing coins
 * @param amount Number of coins to remove
 */
void Game::removeCoins(Player *targetPlayer, const int amount) {
    targetPlayer->removeCoins(amount);
}

/**
 * @brief Handle a generic block, deducting cost and consuming turn.
 * @param blocker Player performing the block
 * @param didBlock Whether block condition is true
 * @param actionName Name of action for logging
 * @param cost Coin cost to block (default 0)
 * @return True if action was blocked
 */
bool Game::handleBlock(Player *blocker, bool didBlock, const std::string &actionName, int cost) {
    if (!didBlock || !blocker) {
        return false;
    }
    if (cost > 0 && blocker->getCoins() >= cost) {
        removeCoins(blocker, cost);
    }
    Player *current = players[currentPlayerTurn];
    current->playerUsedTurn();
    cout << actionName << " was blocked by " << blocker->getName() << endl;
    return true;
}

/**
 * @brief GUI-friendly block handler checking roles and costs.
 * @param currentPlayer Player performing the action
 * @param action Type of action to block
 * @param blocker Player attempting to block
 * @return True if block succeeded
 */
bool Game::handleActionBlock(Player *currentPlayer, ActionType action, Player *blocker) {
    if (!blocker) return false;
    switch (action) {
        case ActionType::Tax:
            if (blocker->getRole() == Role::Governor) {
                currentPlayer->playerUsedTurn();
                return true;
            }
            break;
        case ActionType::Bribe:
            if (blocker->getRole() == Role::Judge) {
                currentPlayer->playerUsedTurn();
                return true;
            }
            break;
        case ActionType::Arrest:
            if (blocker->getRole() == Role::Spy) {
                currentPlayer->playerUsedTurn();
                return true;
            }
            break;
        case ActionType::Coup:
            if (blocker->getRole() == Role::General && blocker->getCoins() >= 5) {
                blocker->removeCoins(5);
                currentPlayer->playerUsedTurn();
                cout << "Coup was blocked by " << blocker->getName() << ".\n";
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

/**
 * @brief Get a list of valid target players (excluding current).
 * @param current Pointer to current player
 * @return Vector of pointers to other players
 */
vector<Player *> Game::getListOfTargetPlayers(const Player *current) {
    vector<Player *> targets;
    for (Player *p : players) {
        if (p != current) targets.push_back(p);
    }
    return targets;
}

/**
 * @brief Force the current player to skip their turn.
 * @param currentPlayer Player to skip
 */
void Game::skipTurn(Player *currentPlayer) {
    currentPlayer->playerUsedTurn();
}

/**
 * @brief Check if current player has remaining turn.
 * @return True if extra turn flag is set
 */
bool Game::currentPlayerHasTurn() {
    return players.at(getTurn())->hasExtraTurn();
}

/**
 * @brief Gather action: collect default coins.
 * @param currentPlayer Acting player
 * @throws GatherError on illegal gather
 */
void Game::gather(Player *currentPlayer) {
    if (players[currentPlayerTurn] != currentPlayer) {
        throw TurnError("It's not your turn.");
    }
    if (!currentPlayer->isGatherAllow()) {
        throw GatherError("Gather action failed.");
    }
    currentPlayer->gather();
}

/**
 * @brief Tax action: collect coins (role-specific amount).
 * @param currentPlayer Acting player
 * @throws TaxError on illegal tax
 */
void Game::tax(Player *currentPlayer) {
    if (players[currentPlayerTurn] != currentPlayer) {
        throw TurnError("It's not your turn.");
    }
    if (!currentPlayer->isTaxAllow()) {
        throw TaxError("Tax action failed.");
    }

    currentPlayer->tax();
}

/**
 * @brief Bribe action: pay coins to gain an extra turn.
 * @param currentPlayer Acting player
 * @throws CoinsError or JudgeBlockBribeError on failure
 */
void Game::bribe(Player *currentPlayer) {
    if (currentPlayer->getCoins() < BRIBE_COST) {
        throw CoinsError("Not enough coins for bribe.");
    }

    removeCoins(currentPlayer, BRIBE_COST);
    if (!currentPlayer->isBribeAllow()) {
        currentPlayer->canBribe = true;  // Flag for judge retaliation
        throw JudgeBlockBribeError("Bribe blocked by judge logic.");
    }
    currentPlayer->bribe();
}

/**
 * @brief Arrest action: steal a coin from a target player.
 * @param currentPlayer Acting player
 * @param targetPlayer Player to arrest
 * @throws ArrestTwiceInRow or ArrestError on invalid use
 */
void Game::arrest(Player *currentPlayer, Player *targetPlayer) {
    if (currentPlayer->getLastArrestedPlayer() == targetPlayer) {
        throw ArrestTwiceInRow("Cannot arrest the same player twice");
    }
    if (currentPlayer == targetPlayer) {
        throw SelfError("You can not arrest yourself");
    }
    if (!currentPlayer->isArrestAllow()) {
        throw ArrestError("Arrest failed: blocked by spy");
    }
    currentPlayer->arrest(targetPlayer);
}

/**
 * @brief Sanction action: impose a penalty on a target player.
 * @param currentPlayer Acting player
 * @param targetPlayer Player to sanction
 * @throws CoinsError if insufficient funds
 */
void Game::sanction(Player *currentPlayer, Player *targetPlayer) {
    if (currentPlayer->getCoins() < SANCTION_COST) {
        throw CoinsError("Sanction failed: not enough coins.");
    }
    if (currentPlayer == targetPlayer) {
        throw SelfError("You can not sanction yourself");
    }
    currentPlayer->sanction(targetPlayer);
    // Handle judge retaliation
    if (targetPlayer->getRole() == Role::Judge) {
        try {
            targetPlayer->passiveAbility(currentPlayer);
        } catch (const exception &e) {
            cerr << "[Sanction] Judge retaliation: " << currentPlayer->getName()
                 << " loses an additional coin." << endl;
        }
    }
}

/**
 * @brief Coup action: eliminate a target player at high cost.
 * @param currentPlayer Acting player
 * @param targetPlayer Player to eliminate
 * @throws CoinsError, SelfError, or CoupBlocked on failure
 */
void Game::coup(Player *currentPlayer, Player *targetPlayer) {
    if (currentPlayer->getCoins() < COUP_COST) {
        throw CoinsError("Coup failed: not enough coins.");
    }
    if (currentPlayer == targetPlayer) {
        throw SelfError("You cannot coup yourself.");
    }
    removeCoins(currentPlayer, COUP_COST);
    if (targetPlayer->isCoupShieldActive()) {
        targetPlayer->coupShield = false;
        currentPlayer->playerUsedTurn();
        throw CoupBlocked("Coup blocked by shield.");
    }
    // Remove target from players list
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i] == targetPlayer) {
            if (i < currentPlayerTurn) --currentPlayerTurn;
            players.erase(players.begin() + i);
            delete targetPlayer;
            break;
        }
    }
    currentPlayer->playerUsedTurn();
}

/**
 * @brief Get the name of the player whose turn it is.
 * @return Current player's name
 */
string Game::turn() {
    return players.at(getTurn())->getName();
}

/**
 * @brief Determine the winner when only one player remains.
 * @return Winner's name
 * @throws logic_error if game is not over
 */
string Game::winner() const {
    if (players.size() == 1) {
        return players.at(0)->getName();
    }
    throw GameOverError("Game not finished yet.");
}

/**
 * @brief Check if a player had an extra turn and consume it.
 * @param currentPlayer Player to check
 * @return True if extra turn was consumed
 */
bool consumeExtraTurn(Player *currentPlayer) {
    if (currentPlayer->hasExtraTurn()) {
        currentPlayer->playerUsedTurn();
        currentPlayer->removeDebuff();
        return true;
    }
    return false;
}

/**
 * @brief Check if the game is over (one player left) and log outcome.
 * @param players Vector of active players
 * @return True if game ended
 */
bool Game::isGameOver(const vector<Player *>& players) {
    if (players.size() == 1) {
        cout << "\n========= GAME OVER =========\n";
        cout << "Winner: " << winner() << endl;
        return true;
    }
    return false;
}

/**
 * @brief Handle various game exceptions and log appropriate errors.
 * @param e Exception reference
 * @return True if error is recoverable
 */
bool Game::handleException(const exception &e) {
    if (dynamic_cast<const MerchantError*>(&e))
        cerr << "[Merchant Error] " << e.what() << endl;
    else if (dynamic_cast<const CoinsError*>(&e))
        cerr << "[Coins Error] " << e.what() << endl;
    else if (dynamic_cast<const SelfError*>(&e))
        cerr << "[Self Error] " << e.what() << endl;
    else if (dynamic_cast<const GatherError*>(&e))
        cerr << "[Gather Error] " << e.what() << endl;
    else if (dynamic_cast<const TaxError*>(&e))
        cerr << "[Tax Error] " << e.what() << endl;
    else if (dynamic_cast<const ArrestError*>(&e))
        cerr << "[Arrest Error] " << e.what() << endl;
    else if (dynamic_cast<const ArrestTwiceInRow*>(&e))
        cerr << "[Arrest Error] " << e.what() << endl;
    else if (dynamic_cast<const JudgeBlockBribeError*>(&e))
        cerr << "[Judge Block Bribe Error] " << e.what() << endl;
    else if (dynamic_cast<const BribeError*>(&e))
        cerr << "[Bribe Error] " << e.what() << endl;
    else if (dynamic_cast<const CoupBlocked*>(&e))
        cerr << "[Coup Error] " << e.what() << endl;
    else {
        cerr << "Unexpected Error: " << e.what() << "\nPlease try again.\n";
        return false;
    }
    // Recoverable errors
    return dynamic_cast<const GatherError*>(&e) ||
           dynamic_cast<const TaxError*>(&e)    ||
           dynamic_cast<const ArrestError*>(&e)  ||
           dynamic_cast<const ArrestTwiceInRow*>(&e) ||
           dynamic_cast<const JudgeBlockBribeError*>(&e) ||
           dynamic_cast<const CoupBlocked*>(&e);
}

} // namespace coup