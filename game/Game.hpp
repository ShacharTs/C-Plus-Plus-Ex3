#pragma once
#include <string>
#include <vector>
#include "player/Player.hpp"


namespace coup {
    enum class ActionType {
        Tax,
        Bribe,
        Arrest,
        Sanction,
        Coup
    };
    

    class Game {
    private:
        int currentPlayerTurn = 0;
        void addCoins(Player* targetPlayer, int amount);
        void removeCoins(Player* targetPlayer, int amount);

        bool checkRoleBlock(Player* currentPlayer, Role role, const std::string& action);
        bool handleActionBlock(Player* currentPlayer, ActionType action);
        void coupKicker(const Player* targetPlayer);
        Player* createRandomRole(const std::string& name);

    public:
        std::vector<Player*> getListOfTargetPlayers(const Player* current);

        std::vector<Player*> players;
        bool handleActionBlockGui(Player* currentPlayer, ActionType action, Player* blocker);
        Game();
        explicit Game(const std::vector<std::string>& names); // for test only
        ~Game();
        
        //Player* checkRoleBlockGui(Player* currentPlayer, Role blockerRole, const std::string& actionName, int cost = 0);


        void skipTurn(Player* currentPlayer);
        int choosePlayer(const std::string& action);
        bool isGameOver(const std::vector<Player*>& players);
        void runGame();

        bool handleException(const std::exception& e);

        std::string winner() const;
        std::string turn();
        void getRandomRole(std::vector<Player*>& players);
        int getTurn();
        void nextTurn();
        const std::vector<Player*>& getPlayers() const;

        // Self-actions
        void gather(Player* currentPlayer);
        void tax(Player* currentPlayer);
        void bribe(Player* currentPlayer);

        // Targeted actions
        void arrest(Player* currentPlayer, Player* targetPlayer);
        void sanction(Player* currentPlayer, Player* targetPlayer);
        void coup(Player* currentPlayer, Player* targetPlayer);

        bool handleGuiBlock(Player* blocker,
            bool    didBlock,
            const std::string& actionName,
            int     cost = 0);

        
    };

	
}
