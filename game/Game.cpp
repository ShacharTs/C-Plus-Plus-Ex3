#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <limits>

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


    Game::Game() {
        cout << "========= Starting New Game =========" << endl;
        int numberOfPlayers = 0;
        while (true) {
            try {
                cout << "Enter number of players (2–6): ";
                cin >> numberOfPlayers;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw invalid_argument("Invalid input: not a number.");
                }

                if (numberOfPlayers < 2 || numberOfPlayers > 6) {
                    throw out_of_range("Players must be between 2 and 6.");
                }

                //  valid input
                break;
            } catch (const exception &e) {
                cerr << "Error: " << e.what() << "\nPlease try again.\n" << endl;
            }
        }

        // adding player names
        for (size_t i = 0; i < numberOfPlayers; i++) {
            cout << "Enter player name" << endl;
            string name;
            cin >> name;
            Player *player = new Player(name);
            players.push_back(player);
        }

        getRandomRole(players); // each player get it role

        // all the players are set, start the game
        runGame();
    }

    /**
     * FOR TESTING ONLY
     */
    Player *createRoleByIndex(const size_t index, const string &name) {
        switch (index) {
            case 0: return new Governor(name);
            case 1: return new Spy(name);
            case 2: return new Baron(name);
            case 3: return new General(name);
            case 4: return new Judge(name);
            case 5: return new Merchant(name);
            default: return new Player(name);
        }
    }


    Game::Game(const vector<string> &names) {
        for (size_t i = 0; i < names.size(); ++i) {
            players.push_back(createRoleByIndex(i, names[i]));
        }
        currentPlayerTurn = 0;
        runGame();
    }

    Player *createRandomRole(const std::string &name) {
        switch (rand() % 6) {
            case 0: return new Governor(name);
            case 1: return new Spy(name);
            case 2: return new Baron(name);
            case 3: return new General(name);
            case 4: return new Judge(name);
            case 5: return new Merchant(name);
            default: return new Player(name);
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

    int Game::getTurn() {
        return currentPlayerTurn;
    }

    /**
     * swap to the next turn
     */
    void Game::nextTurn() {
        currentPlayerTurn = (getTurn() + 1) % getPlayers().size();
    }

    const vector<Player *> &Game::getPlayers() const {
        return players;
    }

    void Game::addCoins(Player *targetPlayer, const int amount) {
        targetPlayer->addCoins(amount);
    }

    void Game::removeCoins(Player *targetPlayer, const int amount) {
        targetPlayer->removeCoins(amount);
    }

    bool Game::checkRoleBlock(Player *currentPlayer, Role role, const string &action) {
        for (const Player *player: players) {
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

    bool Game::handleActionBlock(Player *currentPlayer, const ActionType action) {
        switch (action) {
            case ActionType::Tax:
                return checkRoleBlock(currentPlayer, Role::Governor, "tax");
            case ActionType::Bribe:
                return checkRoleBlock(currentPlayer, Role::Judge, "bribe");
            case ActionType::Arrest:
                return checkRoleBlock(currentPlayer, Role::Spy, "arrest");
            case ActionType::Coup:
                for (Player *player: players) {
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
        if (handleActionBlock(currentPlayer, ActionType::Tax)) {
            return;
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
    void Game::arrest(Player *currentPlayer, Player *targetPlayer) {
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
    void Game::sanction(Player *currentPlayer, Player *targetPlayer) {
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
            } catch ([[maybe_unused]] const exception &e) {
                cerr << "[Sanction] Judge retaliation: " << currentPlayer->getName() << " loses an additional coin." << endl;
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
                delete targetPlayer; // TODO check GUI for bugs
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


    int Game::choosePlayer(Player *currentPlayer, const string &action) {
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
    bool consumeExtraTurn(Player *currentPlayer) {
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
    bool forcedToCoup(const Player *currentPlayer) {
        if (currentPlayer->getCoins() >= FORCE_COUP) {
            return true;
        }
        return false;
    }


    /**
     * check if current player's role is merchant and checks if has more than 2 coins to active passtive ability
     * @param currentPlayer
     */
    void tryMerchantAbility(Player *currentPlayer) {
        if (currentPlayer->getRole() == Role::Merchant && currentPlayer->getCoins() > 2) {
            currentPlayer->passiveAbility();
        }
    }

    bool Game::isGameOver(const vector<Player *> &players) {
        if (players.size() == 1) {
            cout << "\n========= GAME OVER =========\n";
            cout << "Winner: " << winner() << endl;
            return true;
        }
        return false;
    }


    /**
     * Game main logic
     */
    void Game::runGame() {
        while (getPlayers().size() > 1) {
            Player *current = getPlayers()[currentPlayerTurn];
            current->resetPlayerTurn();
            while (true) {
                try {
                    tryMerchantAbility(current);
                    if (forcedToCoup(current)) {
                        // check if player must to coup
                        current->printPlayerStats();
                        const size_t targetIndex = choosePlayer(current, "coup");
                        coup(current, getPlayers().at(targetIndex));
                        if (consumeExtraTurn(current)) {
                            continue;
                        }
                        break;
                    }
                    current->listOptions();

                    int choice;
                    cin >> choice;
                    switch (choice) {
                        case 1:
                            gather(current);
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        case 2:
                            tax(current);
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;

                        case 3:
                            bribe(current);
                            continue;

                        case 4: {
                            const size_t targetIndex = choosePlayer(current, "arrest");
                            arrest(current, getPlayers().at(targetIndex));
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        }
                        case 5: {
                            const size_t targetIndex = choosePlayer(current, "sanction");
                            sanction(current, getPlayers().at(targetIndex));
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        }
                        case 6: {
                            const size_t targetIndex = choosePlayer(current, "coup");
                            coup(current, getPlayers().at(targetIndex));
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        }
                        case 7:
                            if (current->getRole() == Role::Merchant) {
                                throw MerchantError("Invaild input");
                                continue;
                            }
                            current->useAbility(*this);
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        case 0:
                            cout << current->getName() << " skip turn" << endl;
                            break;
                        default:
                            cout << "Invalid choice. Try again." << endl;
                            cin.clear();
                            continue;
                    }
                    // action succeeded
                    break;


                } catch (const MerchantError &e) {
                    cerr << "[Merchant Error] " << e.what() << endl;
                }
                catch (const CoinsError &e) {
                    cerr << "[Coins Error] " << e.what() << endl;
                }
                catch (const SelfError &e) {
                    cerr << "[Self Error] " << e.what() << endl;
                }
                catch (const GatherError &e) {
                    cerr << "[Gather Error] " << e.what() << endl;
                    break;
                }
                catch (const TaxError &e) {
                    cerr << "[Tax Error] " << e.what() << endl;
                    break;
                }
                catch (const ArrestError &e) {
                    cerr << "[Arrest Error] " << e.what() << endl;
                    break;
                }
                catch (const ArrestTwiceInRow &e) {
                    cerr << "[Arrest Error] " << e.what() << endl;
                    break;
                }
                catch (const JudgeBlockBribeError &e) {
                    cerr << "[Judge Block Bribe Error] " << e.what() << endl;
                    break;
                }
                catch (const BribeError &e) {
                    cerr << "[Bribe Error] " << e.what() << endl;
                }
                catch (const CoupBlocked &e) {
                    cerr << "[Coup Error] " << e.what() << endl;
                    break;
                }
                catch (const exception &e) {
                    cerr << "Unexpected Error: " << e.what() << "\nPlease try again.\n";
                }
            }
            // check if one player is left
            if (isGameOver(getPlayers())) {
                return;
            }
            current->removeDebuff();
            nextTurn();
        }
    }
}
