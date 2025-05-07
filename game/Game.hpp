#pragma once

#include <string>
#include <vector>
#include "player/Player.hpp"

namespace coup {
    class Game {
    private:
        std::vector<Player*> players;
        int currentPlayerTurn = 0;
        void addCoins(Player* target,int amount);

        void removeCoins(Player* target,int amount);
        void Game::coupKicker(const Player* target);

        void useAbility(Player* actor, Player* target);
        void useAbility(Player* actor);



    public:
        Game();
        Game(const std::vector<std::string>& playerNames);
        ~Game();
         void runGame();  // loop that run the game
         std::string winner(const std::vector<Player*> &players) const;
         std::string turn();
         void getRandomRole(std::vector<Player*> &players);
        int getTurn();
        void nextTurn();

        // Self-actions
        void gather(Player* actor);
        void tax( Player* actor);
        void bribe(Player* actor);
        // Targeted actions
        void arrest(Player* actor, Player* target);
        void sanction(Player* actor, Player* target);
        void coup(Player* actor, const Player* target);






    };
}

