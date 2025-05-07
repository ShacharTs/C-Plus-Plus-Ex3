#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>


#include "player/roleHeader/Baron.hpp"
#include "player/roleHeader/General.hpp"
#include "player/roleHeader/Governor.hpp"
#include "player/roleHeader/Judge.hpp"
#include "player/roleHeader/Merchant.hpp"
#include "player/roleHeader/Spy.hpp"

using namespace std;

namespace coup {
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

    // FOR TEST ONLY
    Game::Game(const std::vector<std::string> &playerNames) {
        for (const std::string &name: playerNames) {
            players.push_back(new Player(name));
        }
        getRandomRole(players);
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

    void Game::addCoins(Player *target, const int amount) {
        target->addCoins(amount);
    }

    void Game::removeCoins(Player *target, const int amount) {
        target->removeCoins(amount);
    }


    void Game::gather(Player *actor) {
        if (!actor->canGather) {
            throw runtime_error("Gather action failed.");
        }
        addCoins(actor, 1);
    }

    void Game::tax(Player *actor) {
        if (!actor->canTax) {
            throw runtime_error("Tax action failed.");
        }
        // Governor can get 3 coins
        if (actor->getRole() == Role::Governor) {
            addCoins(actor, 3);
            return;
        }
        addCoins(actor, 2);
    }

    void Game::bribe(Player *actor) {
        if (actor->getCoins() < 4) {
            throw runtime_error("Bribe failed not enough coins.");
        }
        removeCoins(actor,4);
        if (!actor->canBribe) {
            throw runtime_error ("Bribe failed.");
        }
        actor->addExtraTurn();
    }

    void Game::arrest(Player *actor, Player *target) {
        if (target->getLastArrestedBy() == actor) {
            throw runtime_error("You cannot arrest the same player twice in a row.");
        }

        try {
            removeCoins(target,1);
            addCoins(actor, 1);
        }catch (exception &e) {
            throw runtime_error("Arrest failed: " + string(e.what()));
        }

        target->setLastArrestedBy(actor);
    }


    void Game::sanction(Player *actor, Player *target) {
        if (actor->getCoins() < 3) {
            throw runtime_error("sanction failed not enough coins.");
        }
        target->canGather = false;
        target->canTax = false;
        removeCoins(actor, 3);
    }

    /**
     * check if player has the amount of coins
     * after that remove from player coins
     * and then checked if target has a shield, if target does not has shield, kick that player
     * @param actor current player
     * @param target another player
     */
    void Game::coup(Player *actor, const Player *target) {
        if (actor->getCoins() < 7) {
            throw runtime_error("coup failed not enough coins.");
        }
        removeCoins(actor, 7);
        if (target->isShieldActive) {
            throw runtime_error("coup blocked by another player.");
        }
        coupKicker(target);
    }

    /**
     * when a player uses coup, this will kick him from the game
     * @param target couped player
     */
    void Game::coupKicker(const Player *target) {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i] == target) {
                // Adjust currentPlayerTurn if needed
                if (i < currentPlayerTurn) {
                    --currentPlayerTurn; // shift currentPlayerTurn back since players after 'i' will move left
                }
                players.erase(players.begin() + i);
                delete target; // delete player, TODO check gui later for bugs
                break;
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
        return players.at(0)->getName();
    }


    void Game::useAbility(Player* actor, Player* target) {
        actor->useAbility(target);
    }

    void Game::useAbility(Player* actor) {
        actor->useAbility();
    }



    /**
     * core of the game, all the logic inside the while loop
     */
    void Game::runGame() {
        // add while loop with try and catch
    }
}
