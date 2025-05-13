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

        // Internal helpers
        void addCoins(Player* targetPlayer, int amount);
        void removeCoins(Player* targetPlayer, int amount);

    public:
        // Players
        std::vector<Player*> players;
        const std::vector<Player*>& getPlayers() const;
        std::vector<Player*> getListOfTargetPlayers(const Player* current);

        // Constructor & destructor
        explicit Game(const std::vector<std::string>& names);

        Player *createRandomRole(const std::string &name);

        ~Game();

        // Turn management
        void advanceTurnIfNeeded();
        void skipTurn(Player* currentPlayer);

        bool currentPlayerHasTurn();



        void nextTurn();
        int getTurn();
        std::string turn();

        // Game state
        bool isGameOver(const std::vector<Player*>& players);
        std::string winner() const;
        bool handleException(const std::exception& e);

        // Self-actions
        void gather(Player* currentPlayer);
        void tax(Player* currentPlayer);
        void bribe(Player* currentPlayer);

        // Targeted actions
        void arrest(Player* currentPlayer, Player* targetPlayer);
        void sanction(Player* currentPlayer, Player* targetPlayer);
        void coup(Player* currentPlayer, Player* targetPlayer);

        // GUI-related logic
        bool handleActionBlock(Player* currentPlayer, ActionType action, Player* blocker);
        bool handleBlock(Player* blocker, bool didBlock, const std::string& actionName, int cost = 0);

        // Testing or role setup helper
        void getRandomRole(std::vector<Player*>& players);
    };

} // namespace coup
