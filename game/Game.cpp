#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <limits>


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
        for (int i = 0; i < numberOfPlayers; i++) {
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
     * FOR TEST ONLY
     * @param names list of names
     */
    Game::Game(vector<string> names) {
        for (const string &name: names) {
            players.push_back(new Player(name));
        }

        for (size_t i = 0; i < players.size(); ++i) {
            string name = players[i]->getName();
            delete players[i];

            switch (i) {
                case 0: players[i] = new Governor(name);
                    break;
                case 1: players[i] = new Spy(name);
                    break;
                case 2: players[i] = new Baron(name);
                    break;
                case 3: players[i] = new General(name);
                    break;
                case 4: players[i] = new Judge(name);
                    break;
                case 5: players[i] = new Merchant(name);
                    break;
                default: players[i] = new Player(name);
                    break;
            }
        }
        currentPlayerTurn = 0;
        runGame();
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
        currentPlayerTurn = (currentPlayerTurn + 1) % players.size();
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

    /**
     * player gain 1 coin from gathering
     * @param currentPlayer
     */
    void Game::gather(Player *currentPlayer) {
        if (!currentPlayer->canGather) {
            throw runtime_error("Gather action failed.");
        }
        addCoins(currentPlayer, 1);
    }

    /**
     * Player use tax gain 2 coins, if player role's is governor, gain 3 coins instead of 2
     * @param currentPlayer
     */
    void Game::tax(Player *currentPlayer) {
        if (!currentPlayer->canTax) {
            throw runtime_error("Tax action failed.");
        }
        // Governor role gain 3 coins
        if (currentPlayer->getRole() == Role::Governor) {
            addCoins(currentPlayer, 3);
            return;
        }
        addCoins(currentPlayer, 2); // normal role will gain 2 coins
    }

    /**
     * Player can use bribe to gain extra turn next turn
     * @param currentPlayer
     */
    void Game::bribe(Player *currentPlayer) {
        if (currentPlayer->getCoins() < BRIBE_COST) {
            throw runtime_error("Bribe failed not enough coins.");
        }
        removeCoins(currentPlayer, BRIBE_COST);
        if (!currentPlayer->canBribe) {
            throw runtime_error("Bribe failed.");
        }
        currentPlayer->addExtraTurn();
    }

    /**
     * Player can take one coin from another player
     * @param currentPlayer
     * @param targetPlayer
     */
    void Game::arrest(Player *currentPlayer, Player *targetPlayer) {
        if (targetPlayer->getLastArrestedBy() == currentPlayer) {
            throw runtime_error("You cannot arrest the same player twice in a row.");
        }

        try {
            removeCoins(targetPlayer, 1);
            addCoins(currentPlayer, 1);
        } catch (exception &e) {
            throw runtime_error("Arrest failed: " + string(e.what()));
        }

        targetPlayer->setLastArrestedBy(currentPlayer);
    }

    /**
     * Player can block another player from using gather and tax
     * @param currentPlayer
     * @param targetPlayer
     */
    void Game::sanction(Player *currentPlayer, Player *targetPlayer) {
        if (currentPlayer->getCoins() < SANCTION_COST) {
            throw runtime_error("sanction failed not enough coins.");
        }
        try {
            removeCoins(currentPlayer, SANCTION_COST);
            targetPlayer->canGather = false;
            targetPlayer->canTax = false;
        } catch (exception &e) {
            currentPlayer->addExtraTurn(); // failed no coins reward extra turn to do something
            throw runtime_error("Sanction failed: " + string(e.what()));
        }
    }

    /**
     * check if player has the amount of coins
     * after that remove from player coins
     * and then checked if target has a shield, if target does not has shield, kick that player
     * @param currentPlayer current player
     * @param targetPlayer another player
     */
    void Game::coup(Player *currentPlayer, const Player *targetPlayer) {
        if (currentPlayer->getCoins() < COUP_COST) {
            throw runtime_error("coup failed not enough coins.");
        }

        removeCoins(currentPlayer, COUP_COST);
        if (targetPlayer->isShieldActive) {
            throw runtime_error("coup blocked by another player.");
        }
        coupKicker(targetPlayer);
    }

    /**
     * when a player uses coup, this will kick him from the game
     * @param targetPlayer couped player
     */
    void Game::coupKicker(const Player *targetPlayer) {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i] == targetPlayer) {
                // Adjust currentPlayerTurn if needed
                if (i < currentPlayerTurn) {
                    --currentPlayerTurn; // shift currentPlayerTurn back since players after 'i' will move left
                }
                players.erase(players.begin() + i);
                delete targetPlayer; // delete player, TODO check gui later for bugs
                return;
            }
        }
    }


    /**
     *  print current player turn
     */
    string Game::turn() {
        return players.at(currentPlayerTurn)->getName();
    }

    /**
     *
     * @param players list of the players
     * @return winner player
     */
    string Game::winner(const vector<Player *> &players) const {
        if (players.size() == 1) {
            return players[0]->getName();
        }
        throw logic_error("Game not finished yet.");
    }


    // void Game::useAbility(Player *currentPlayer, Player *targetPlayer) {
    //     currentPlayer->useAbility(targetPlayer);
    // }

    void Game::useAbility(Player *currentPlayer) {
    }

    int Game::choosePlayer(const vector<Player *> &players, Player *currentPlayer, const string &action) {
        cout << "Choose player to " << action << ":\n";
        for (size_t i = 0; i < players.size(); ++i) {
            if (i != currentPlayerTurn) {
                cout << i << ". " << players[i]->getName() << endl;
            }
        }
        size_t targetIndex;
        cin >> targetIndex;
        return targetIndex;
    }


    void printPlayerStats(const Player *current) {
        cout << "\n--- Turn: " << current->getName() << " ---" << endl;
        cout << "Coins: " << current->getCoins() << endl;
        cout << "Role: " << current->roleToString(current->getRole()) << endl;
    }

    void printChose(const Player *current) {
        printPlayerStats(current);

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



    void restSanction(Player *currentPlayer) {
        currentPlayer->canGather = true;
        currentPlayer->canTax = true;
    }

    /**
     * check if player used extra turn after choice
     * if true, remove extra turn (used)
     * @param currentPlayer
     */
    bool consumeExtraTurn(Player *currentPlayer) {
        if (currentPlayer->hasExtraTurn()) {
            currentPlayer->removeExtraTurn();
            restSanction(currentPlayer);
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
     * Game main logic
     */
    void Game::runGame() {
        while (players.size() > 1) {
            Player *current = players[currentPlayerTurn];
            while (true) {
                try {
                    if (forcedToCoup(current)) {
                        printPlayerStats(current);
                        const size_t targetIndex = choosePlayer(players, current, "coup");
                        coup(current, players.at(targetIndex));
                        if (consumeExtraTurn(current)) {
                            continue;
                        }
                        break;
                    }
                    printChose(current);

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
                            break;

                        case 4: {
                            const size_t targetIndex = choosePlayer(players, current, "arrest");
                            arrest(current, players.at(targetIndex));
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        }
                        case 5: {
                            const size_t targetIndex = choosePlayer(players, current, "sanction");
                            sanction(current, players.at(targetIndex));
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        }
                        case 6: {
                            const size_t targetIndex = choosePlayer(players, current, "coup");
                            coup(current, players.at(targetIndex));
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        }
                        case 7:
                            current->useAbility();
                            if (consumeExtraTurn(current)) {
                                continue;
                            }
                            break;
                        case 0:
                            cout << current->getName() << " skip turn" <<endl;
                            break;
                        default:
                            cout << "Invalid choice. Try again." << endl;
                            cin.clear();
                            continue;
                    }
                    restSanction(current); // at the end of turn rest sanction if player had
                    // action succeeded chosen
                    break;
                } catch (const exception &e) {
                    cerr << "Error: " << e.what() << "\nPlease try a different action.\n";
                }
            }
            // print winner
            if (players.size() == 1) {
                cout << "\n========= GAME OVER =========\n";
                cout << "Winner: " << winner(players) << endl;
                break;
            }

            nextTurn();
        }
    }
}
