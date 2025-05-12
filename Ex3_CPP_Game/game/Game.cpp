#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <random>

#include <wx/string.h>

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
    Player* createRoleByIndex(const size_t index, const string& name) {
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
    Game::Game(const vector<string>& names) {
        // Temp look for create all the roles

        for (size_t i = 0; i < names.size(); ++i) {
            players.push_back(createRoleByIndex(i, names[i]));
        }

        // Each player will get a random role

         /*for (size_t i = 0; i < names.size(); ++i) {
             players.push_back(createRandomRole(names[i]));
         }*/


        currentPlayerTurn = 0;
        //runGame();  // REMOVE FOR GUI
    }


    /**
     * each player will have a random role
     * @param name player name
     * @return role
     */
    Player* Game::createRandomRole(const std::string& name) {
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
        for (Player*& player : players) {
            delete player;
        }
    }

    /**
     * each player gain a role for the game
     * @param players list of the players
     */
    void Game::getRandomRole(vector<Player*>& players) {
        srand(time(nullptr));
        for (Player*& p : players) {
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

    int Game::getTurn() {
        return currentPlayerTurn;
    }

    /**
     * swap to the next turn
     */
    void Game::nextTurn() {
        Player* current = players.at(currentPlayerTurn);
        current->removeDebuff();
        currentPlayerTurn = (getTurn() + 1) % getPlayers().size();
    }

    const vector<Player*>& Game::getPlayers() const {
        return players;
    }

    void Game::addCoins(Player* targetPlayer, const int amount) {
        targetPlayer->addCoins(amount);
    }

    void Game::removeCoins(Player* targetPlayer, const int amount) {
        targetPlayer->removeCoins(amount);
    }

    bool Game::checkRoleBlock(Player* currentPlayer, Role role, const string& action) {
        for (const Player* player : players) {
            if (player != currentPlayer && player->getRole() == role) {
                string answer;
                cout << player->getName() << " (" << player->roleToString(player->getRole()) << ")"
                    << ", do you want to block the " << action << " of "
                    << currentPlayer->getName() << "? (y/n): ";
                cin >> answer;
                if (answer == "y" || answer == "Y") {
                    cout << action << " was blocked by " << player->getName() << ".\n";
                    currentPlayer->playerUsedTurn();
                    return true;
                }
            }
        }
        return false;
    }


    bool Game::handleActionBlock(Player* currentPlayer, const ActionType action) {
        switch (action) {
        case ActionType::Tax:
            return checkRoleBlock(currentPlayer, Role::Governor, "tax");
        case ActionType::Bribe:
            return checkRoleBlock(currentPlayer, Role::Judge, "bribe");
        case ActionType::Arrest:
            return checkRoleBlock(currentPlayer, Role::Spy, "arrest");
        case ActionType::Coup:
            for (Player* player : players) {
                if (player != currentPlayer && player->getRole() == Role::General && player->getCoins() >= 5) {
                    string answer;
                    cout << player->getName() << " " << player->roleToString(player->getRole()) <<
                        " , do you want to block the coup against "
                        << currentPlayer->getName() << " for 5 coins? (y/n): ";
                    cin >> answer;
                    if (answer == "y" || answer == "Y") {
                        player->removeCoins(5);
                        currentPlayer->playerUsedTurn();
                        cout << "Coup was blocked by " << player->getName() << ".\n";
                        return true;
                    }
                }
            }
            return false;
        default:
            return false;
        }
    }

    bool Game::handleGuiBlock(Player* blocker,
        bool    didBlock,
        const std::string& actionName,
        int     cost)
    {
        if (!didBlock || blocker == nullptr)
            return false;

        // Deduct coins if required
        if (cost > 0 && blocker->getCoins() >= cost)
            removeCoins(blocker, cost);

        // Consume the blocker's turn
        Player* current = players[currentPlayerTurn];
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
    bool Game::handleActionBlockGui(Player* currentPlayer, ActionType action, Player* blocker) {
        if (!blocker) return false;  // no one blocked

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

    vector<Player*> Game::getListOfTargetPlayers(const Player* current) {
		vector<Player*> targetPlayers;
		for (size_t i = 0; i < players.size(); ++i) {
			if (players[i] != current) {
				targetPlayers.push_back(players[i]);
			}
		}
		return targetPlayers;
    }

    void Game::skipTurn(Player* currentPlayer) {
		currentPlayer->playerUsedTurn();
    }

    /**
     * player gain 1 coin from gathering
     * @param currentPlayer
     */
    void Game::gather(Player* currentPlayer) {
        if (!currentPlayer->isGatherAllow()) {
            throw GatherError("Gather action failed.");
        }
        currentPlayer->gather();
    }

    /**
     * Player use tax gain 2 coins, if player role's is governor, gain 3 coins instead of 2
     * @param currentPlayer
     */
    void Game::tax(Player* currentPlayer) {
        if (!currentPlayer->isTaxAllow()) {
            throw TaxError("Tax action failed.");
        }
        if (handleActionBlock(currentPlayer, ActionType::Tax)) {
            return;
        }
        currentPlayer->tax();
    }

    /**
     * Player can use bribe to gain extra turn next turn
     * @param currentPlayer
     */
    void Game::bribe(Player* currentPlayer) {
        if (currentPlayer->getCoins() < BRIBE_COST) {
            throw CoinsError("Not enough coins for bribe.");
        }
        removeCoins(currentPlayer, BRIBE_COST);

        if (handleActionBlock(currentPlayer, ActionType::Bribe)) {
            throw JudgeBlockBribeError("Bribe blocked before executing.");
        }

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
    void Game::arrest(Player* currentPlayer, Player* targetPlayer) {
        if (currentPlayer->getLastArrestedPlayer() == targetPlayer) {
            throw ArrestTwiceInRow("You cannot arrest the same player twice in a row.");
        }
        if (!currentPlayer->isArrestAllow()) {
            throw ArrestError("Arrest failed. Player blocked by spy");
        }

        if (handleActionBlock(currentPlayer, ActionType::Arrest)) {
            return;
        }

        currentPlayer->arrest(targetPlayer);
    }

    /**
     * Player can block another player from using gather and tax
     * @param currentPlayer
     * @param targetPlayer
     */
    void Game::sanction(Player* currentPlayer, Player* targetPlayer) {
        if (currentPlayer->getCoins() < SANCTION_COST) {
            throw CoinsError("Sanction failed: not enough coins.");
        }
        currentPlayer->sanction(targetPlayer);
        // passive reactions — after turn is already consumed
        if (targetPlayer->getRole() == Role::Baron) {
            targetPlayer->passiveAbility();
        }

        if (targetPlayer->getRole() == Role::Judge) {
            // try to punish the attacker — if fails, no undo
            try {
                targetPlayer->passiveAbility(currentPlayer);
            }
            catch ([[maybe_unused]] const exception& e) {
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
    void Game::coup(Player* currentPlayer, Player* targetPlayer) {
        if (currentPlayer->getCoins() < COUP_COST) {
            throw CoinsError("Coup failed: not enough coins.");
        }
        if (currentPlayer == targetPlayer) {
            throw SelfError("You cannot coup yourself.");
        }

        removeCoins(currentPlayer, COUP_COST);

        if (handleActionBlock(currentPlayer, ActionType::Coup)) {
            return;
        }

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


    int Game::choosePlayer(const string& action) {
        cout << "Choose player to " << action << ":\n";
        for (size_t i = 0; i < getPlayers().size(); ++i) {
            cout << i << ". " << getPlayers()[i]->getName() << endl;
        }
        size_t targetIndex;
        cin >> targetIndex;
        if (targetIndex >= getPlayers().size() || targetIndex == currentPlayerTurn) {
            throw SelfError("Invalid selection.");
        }

        return targetIndex;
    }


    /**
     * check if player used extra turn after choice
     * if true, remove extra turn (used)
     * @param currentPlayer
     */
    bool consumeExtraTurn(Player* currentPlayer) {
        if (currentPlayer->hasExtraTurn()) {
            currentPlayer->playerUsedTurn();
            currentPlayer->removeDebuff();
            return true;
        }
        return false;
    }

    /**
     * check if a player is forced to coup
     * @param currentPlayer
     */
    bool forcedToCoup(const Player* currentPlayer) {
        if (currentPlayer->getCoins() >= FORCE_COUP) {
            return true;
        }
        return false;
    }


    bool Game::isGameOver(const vector<Player*>& players) {
        if (players.size() == 1) {
            cout << "\n========= GAME OVER =========\n";
            cout << "Winner: " << winner() << endl;
            return true;
        }
        return false;
    }

    bool Game::handleException(const exception& e) {
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

        // Use dynamic_cast to decide if loop breaks
        return dynamic_cast<const GatherError*>(&e) ||
            dynamic_cast<const TaxError*>(&e) ||
            dynamic_cast<const ArrestError*>(&e) ||
            dynamic_cast<const ArrestTwiceInRow*>(&e) ||
            dynamic_cast<const JudgeBlockBribeError*>(&e) ||
            dynamic_cast<const CoupBlocked*>(&e);
    }


    void Game::runGame() {
        while (getPlayers().size() > 1) {
            Player* current = getPlayers()[currentPlayerTurn];
            while (true) {
                try {
                    if (forcedToCoup(current)) {
                        current->printPlayerStats();
                        const size_t targetIndex = choosePlayer("coup");
                        coup(current, getPlayers().at(targetIndex));
                        if (consumeExtraTurn(current)) continue;
                        break;
                    }
                    current->listOptions();

                    int choice;
                    cin >> choice;
                    switch (choice) {
                    case 1: gather(current);
                        break;
                    case 2: tax(current);
                        break;
                    case 3: bribe(current);
                        continue;
                    case 4: arrest(current, getPlayers().at(choosePlayer("arrest")));
                        break;
                    case 5: sanction(current, getPlayers().at(choosePlayer("sanction")));
                        break;
                    case 6: coup(current, getPlayers().at(choosePlayer("coup")));
                        break;
                    case 7:
                        if (current->getRole() == Role::Merchant) throw MerchantError("Invalid input");
                        current->useAbility(*this);
                        break;
                    case 0:
                        cout << current->getName() << " skip turn" << endl;
                        current->playerUsedTurn();
                        break;
                    default:
                        cout << "Invalid choice. Try again." << endl;
                        cin.clear();
                        continue;
                    }

                    if (consumeExtraTurn(current)) continue;
                    break;
                }
                catch (const exception& e) {
                    if (!handleException(e)) {
                        continue;
                    }
                    break;
                }
            }
            if (isGameOver(getPlayers())) return;
            nextTurn();
        }
    }
}
