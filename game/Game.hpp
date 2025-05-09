#pragma once

#include <string>
#include <vector>
#include "player/Player.hpp"



namespace coup {
    class Game {
    private:

        int currentPlayerTurn = 0;
        void addCoins(Player* targetPlayer,int amount);
        void removeCoins(Player* targetPlayer,int amount);
        void coupKicker(const Player* targetPlayer);


    public:
        std::vector<Player*> players;

        Game();

        explicit Game(const std::vector<std::string> &names); // for test only
        ~Game();

        int choosePlayer(Player *currentPlayer, const std::string &action);

        bool isGameOver(const std::vector<Player *> &players);

        void runGame();  // loop that run the game
        std::string winner() const;
        std::string turn();
        void getRandomRole(std::vector<Player*> &players);
        int getTurn();
        void nextTurn();
        const std::vector<Player*>& getPlayers() const;


        // Self-actions
        void gather(Player* currentPlayer);
        void tax( Player* currentPlayer);
        void bribe(Player* currentPlayer);
        // Targeted actions
        void arrest(Player* currentPlayer, Player* targetPlayer);
        void sanction(Player* currentPlayer, Player* targetPlayer);
        void coup(Player *currentPlayer, Player *targetPlayer);






    };
}

