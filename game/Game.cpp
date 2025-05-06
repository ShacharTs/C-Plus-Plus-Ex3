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
    Game::Game(const std::vector<std::string>& playerNames) {
        for (const std::string& name : playerNames) {
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


    /**
     *  swapping turns
     */
    string Game::turn() {
        return "";
    }

    /**
     *
     * @param players list of the players
     * @return winner player
     */
    string Game::winner(const vector<Player *> &players) const {
        return players.at(0)->getName();
    }

    /**
     * core of the game, all the logic inside the while loop
     */
    void Game::runGame() {
    }
}
