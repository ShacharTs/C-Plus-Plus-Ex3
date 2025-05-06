#pragma once

#include <string>
#include <vector>
#include "player/Player.hpp"

namespace coup {
    class Game {
    private:
        std::vector<Player*> players;
        int turns = 0;

    public:
        Game();
        Game(const std::vector<std::string>& playerNames);
        ~Game();
         void runGame();  // loop that run the game
         std::string winner(const std::vector<Player*> &players) const;
         std::string turn();
         void getRandomRole(std::vector<Player*> &players);



    };
}

