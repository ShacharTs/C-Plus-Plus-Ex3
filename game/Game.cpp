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
    constexpr int COUP_COST = 7;
    constexpr int BRIBE_COST = 4;
    constexpr int SANCTION_COST = 3;
    constexpr int FORCE_COUP = 10;


    /**
     * TEST ONLY TO GET ALL THE ROLES
     */
    Player *createRoleByIndex(const size_t index, const string &name) {
        switch (index) {
            case 0: return new Governor(name);
            case 1: return new Spy(name);
            case 2: return new Baron(name);
            case 3: return new General(name);
            case 4: return new Judge(name);
            case 5: return new Merchant(name);
            default: throw runtime_error("Error failed to give a role");
        }
    }

    /**
     * @param names player list
     */
    Game::Game(const vector<string> &names) {
        // Temp look for create all the roles

        for (size_t i = 0; i < names.size(); ++i) {
            players.push_back(createRoleByIndex(i, names[i]));
        }

        // Each player will get a random role
        /*for (size_t i = 0; i < names.size(); ++i) {
            players.push_back(createRandomRole(names[i]));
        }*/

        currentPlayerTurn = 0;
    }


    /**
     * each player will have a random role
     * @param name player name
     * @return role
     */
    Player *Game::createRandomRole(const std::string &name) {
        static random_device rd; // true random seed source
        static mt19937 gen(rd()); // better quality RNG
        static uniform_int_distribution<> dist(0, 5); // uniform distribution between 0-5

        switch (dist(gen)) {
            case 0: return new Governor(name);
            case 1: return new Spy(name);
            case 2: return new Baron(name);
            case 3: return new General(name);
            case 4: return new Judge(name);
            case 5: return new Merchant(name);
            default: throw runtime_error("Random number generation failed");
        }
    }


    /**
     * Destructor
     */
    Game::~Game() {
        for (Player *&player: players) {
            delete player;
        }
    }

    /**
     * each player gain a role for the game
     * @param players list of the players
     */
    void Game::getRandomRole(vector<Player *> &players) {
        srand(time(nullptr));
        for (Player *&p: players) {
            const string name = p->getName();
            delete p; // delete old player to gain new player with role

            switch (const int roll = rand() % 6 + 1) {
                case 1: p = new Governor(name);
                    break;
                case 2: p = new Spy(name);
                    break;
                case 3: p = new Baron(name);
                    break;
                case 4: p = new General(name);
                    break;
                case 5: p = new Judge(name);
                    break;
                case 6: p = new Merchant(name);
                    break;
                default:
                    throw out_of_range("Invalid roll: " + to_string(roll));
            }
        }
    }

    /**
     * 
     * @return
     */
    int Game::getTurn() {
        return currentPlayerTurn;
    }

    /**
     * swap to the next turn
     */
    void Game::nextTurn() {
        Player *current = players.at(currentPlayerTurn);
        current->resetPlayerTurn();
        current->removeDebuff();
        currentPlayerTurn = (getTurn() + 1) % getPlayers().size();
    }

    /**
     * 
     */
    void Game::advanceTurnIfNeeded() {
        Player *current = players.at(currentPlayerTurn);
        if (current->hasExtraTurn()) {
            current->playerUsedTurn();
            current->removeDebuff();
        } else {
            nextTurn();
        }
    }

    /**
     * 
     * @return
     */
    const vector<Player *> &Game::getPlayers() const {
        return players;
    }

    /**
     * 
     * @param targetPlayer 
     * @param amount 
     */
    void Game::addCoins(Player *targetPlayer, const int amount) {
        targetPlayer->addCoins(amount);
    }

    /**
     * 
     * @param targetPlayer 
     * @param amount 
     */
    void Game::removeCoins(Player *targetPlayer, const int amount) {
        targetPlayer->removeCoins(amount);
    }

    /**
     * 
     * @param blocker 
     * @param didBlock 
     * @param actionName 
     * @param cost 
     * @return 
     */
    bool Game::handleBlock(Player *blocker, bool didBlock, const std::string &actionName, int cost) {
        if (!didBlock || blocker == nullptr) {
            return false;
        }

        // Deduct coins if required
        if (cost > 0 && blocker->getCoins() >= cost) {
            removeCoins(blocker, cost);
        }

        // Consume the blocker's turn
        Player *current = players[currentPlayerTurn];
        current->playerUsedTurn();

        // Log the block
        std::cout << actionName
                << " was blocked by "
                << blocker->getName() << endl;
        return true;
    }


    /**
* Game method for handling blocks from the GUI.
* @param currentPlayer The player who is performing the action.
* @param action The action being taken.
* @param blocker The player who chose to block (or nullptr if no one did).
* @return true if the action was blocked, false otherwise.
*/
    bool Game::handleActionBlock(Player *currentPlayer, ActionType action, Player *blocker) {
        if (!blocker) return false; // no one blocked

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
                    std::cout << "Coup was blocked by " << blocker->getName() << ".\n";
                    return true;
                }
                break;
            default:
                break;
        }
        return false;
    }

    /**
     * 
     * @param current 
     * @return 
     */
    vector<Player *> Game::getListOfTargetPlayers(const Player *current) {
        vector<Player *> targetPlayers;
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i] != current) {
                targetPlayers.push_back(players[i]);
            }
        }
        return targetPlayers;
    }

    /**
     * 
     * @param currentPlayer 
     */
    void Game::skipTurn(Player *currentPlayer) {
        currentPlayer->playerUsedTurn();
    }

    /**
     * 
     * @return 
     */
    bool Game::currentPlayerHasTurn()  {
        const Player *p = getPlayers().at(getTurn());
        return p->hasExtraTurn();
    }


    /**
     * player gain 1 coin from gathering
     * @param currentPlayer
     */
    void Game::gather(Player *currentPlayer) {
        if (!currentPlayer->isGatherAllow()) {
            throw GatherError("Gather action failed.");
        }
        currentPlayer->gather();
    }

    /**
     * Player use tax gain 2 coins, if player role's is governor, gain 3 coins instead of 2
     * @param currentPlayer
     */
    void Game::tax(Player *currentPlayer) {
        if (!currentPlayer->isTaxAllow()) {
            throw TaxError("Tax action failed.");
        }

        currentPlayer->tax();
    }

    /**
     * Player can use bribe to gain extra turn next turn
     * @param currentPlayer
     */
    void Game::bribe(Player *currentPlayer) {
        if (currentPlayer->getCoins() < BRIBE_COST) {
            throw CoinsError("Not enough coins for bribe.");
        }
        removeCoins(currentPlayer, BRIBE_COST);

        if (!currentPlayer->isBribeAllow()) {
            currentPlayer->canBribe = true;
            throw JudgeBlockBribeError("Bribe blocked by judge logic.");
        }

        currentPlayer->bribe();
    }


    /**
     * Player can take one coin from another player
     * @param currentPlayer
     * @param targetPlayer
     */
    void Game::arrest(Player *currentPlayer, Player *targetPlayer) {
        if (currentPlayer->getLastArrestedPlayer() == targetPlayer) {
            throw ArrestTwiceInRow("You cannot arrest the same player twice in a row.");
        }
        if (!currentPlayer->isArrestAllow()) {
            throw ArrestError("Arrest failed. Player blocked by spy");
        }


        currentPlayer->arrest(targetPlayer);
    }

    /**
     * Player can block another player from using gather and tax
     * @param currentPlayer
     * @param targetPlayer
     */
    void Game::sanction(Player *currentPlayer, Player *targetPlayer) {
        if (currentPlayer->getCoins() < SANCTION_COST) {
            throw CoinsError("Sanction failed: not enough coins.");
        }
        currentPlayer->sanction(targetPlayer);

        if (targetPlayer->getRole() == Role::Judge) {
            // try to punish the attacker — if fails, no undo
            try {
                targetPlayer->passiveAbility(currentPlayer);
            } catch ([[maybe_unused]] const exception &e) {
                cerr << "[Sanction] Judge retaliation: " << currentPlayer->getName() << " loses an additional coin." <<
                        endl;
            }
        }
    }


    /**
     * check if player has the amount of coins
     * after that remove from player coins
     * and then checked if target has a shield, if target does not has shield, kick that player
     * @param currentPlayer current player
     * @param targetPlayer another player
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

        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i] == targetPlayer) {
                if (i < currentPlayerTurn) {
                    --currentPlayerTurn;
                }
                players.erase(players.begin() + i);
                delete targetPlayer;
                break;
            }
        }
        currentPlayer->playerUsedTurn();
    }


    /**
     *  print current player turn
     */
    string Game::turn() {
        return getPlayers().at(getTurn())->getName();
    }

    /**
     * @return winner player
     */
    string Game::winner() const {
        if (getPlayers().size() == 1) {
            return getPlayers().at(0)->getName();
        }
        throw logic_error("Game not finished yet.");
    }


    /**
     * check if player used extra turn after choice
     * if true, remove extra turn (used)
     * @param currentPlayer
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
     * 
     * @param players 
     * @return 
     */
    bool Game::isGameOver(const vector<Player *> &players) {
        if (players.size() == 1) {
            cout << "\n========= GAME OVER =========\n";
            cout << "Winner: " << winner() << endl;
            return true;
        }
        return false;
    }

    /**
     * 
     * @param e 
     * @return 
     */
    bool Game::handleException(const exception &e) {
        if (dynamic_cast<const MerchantError *>(&e))
            cerr << "[Merchant Error] " << e.what() << endl;
        else if (dynamic_cast<const CoinsError *>(&e))
            cerr << "[Coins Error] " << e.what() << endl;
        else if (dynamic_cast<const SelfError *>(&e))
            cerr << "[Self Error] " << e.what() << endl;
        else if (dynamic_cast<const GatherError *>(&e))
            cerr << "[Gather Error] " << e.what() << endl;
        else if (dynamic_cast<const TaxError *>(&e))
            cerr << "[Tax Error] " << e.what() << endl;
        else if (dynamic_cast<const ArrestError *>(&e))
            cerr << "[Arrest Error] " << e.what() << endl;
        else if (dynamic_cast<const ArrestTwiceInRow *>(&e))
            cerr << "[Arrest Error] " << e.what() << endl;
        else if (dynamic_cast<const JudgeBlockBribeError *>(&e))
            cerr << "[Judge Block Bribe Error] " << e.what() << endl;
        else if (dynamic_cast<const BribeError *>(&e))
            cerr << "[Bribe Error] " << e.what() << endl;
        else if (dynamic_cast<const CoupBlocked *>(&e))
            cerr << "[Coup Error] " << e.what() << endl;
        else {
            cerr << "Unexpected Error: " << e.what() << "\nPlease try again.\n";
            return false;
        }

        // Use dynamic_cast to decide if loop breaks
        return dynamic_cast<const GatherError *>(&e) ||
               dynamic_cast<const TaxError *>(&e) ||
               dynamic_cast<const ArrestError *>(&e) ||
               dynamic_cast<const ArrestTwiceInRow *>(&e) ||
               dynamic_cast<const JudgeBlockBribeError *>(&e) ||
               dynamic_cast<const CoupBlocked *>(&e);
    }
}
