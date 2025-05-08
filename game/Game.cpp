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



    /**
     * constructor
     */
    Game::Game() {
        cout << "========= Starting New Game =========" << endl;
        int numberOfPlayers = 0;
        while (true) {
            try {
                cout << "Enter number of players (2–6): ";
                cin >> numberOfPlayers;

                if (cin.fail()) {
                    cin.clear(); // clear the error flag
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
            auto *player = new Player(name);
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
    Game::Game(vector<string>names) {
        for (const string &name : names) {
            players.push_back(new Player(name));
        }


        for (size_t i = 0; i < players.size(); ++i) {
            string name = players[i]->getName();
            delete players[i];

            switch (i) {
                case 0: players[i] = new Governor(name); break;
                case 1: players[i] = new Spy(name); break;
                case 2: players[i] = new Baron(name); break;
                case 3: players[i] = new General(name); break;
                case 4: players[i] = new Judge(name); break;
                case 5: players[i] = new Merchant(name); break;
                default: players[i] = new Player(name); break;
            }
        }
        currentPlayerTurn = 0;
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
    void Game::getRandomRole(std::vector<Player *> &players) {
        srand(std::time(nullptr));
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

    const std::vector<Player *> &Game::getPlayers() const {
        return players;
    }

    void Game::addCoins(Player *targetPlayer, const int amount) {
        targetPlayer->addCoins(amount);
    }

    void Game::removeCoins(Player *targetPlayer, const int amount) {
        targetPlayer->removeCoins(amount);
    }

    /**
     * 
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
     * @param currentPlayer current player turn
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
     * 
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
     * 
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
     * 
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


    void Game::useAbility(Player *currentPlayer, Player *targetPlayer) {
        currentPlayer->useAbility(targetPlayer);
    }

    void Game::useAbility(Player *currentPlayer) {
        currentPlayer->useAbility();
    }


    /**
     * core of the game, all the logic inside the while loop
     */
    void Game::runGame() {
        // add while loop with try and catch
    }
}
