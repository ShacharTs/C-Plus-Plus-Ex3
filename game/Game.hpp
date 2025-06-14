﻿#pragma once

#include <string>
#include <vector>
#include "player/Player.hpp"

namespace coup {
    /**
     * @brief Types of actions a player can perform during their turn.
     */
    enum class ActionType {
        Tax,      ///< Collect coins based on role advantages
        Bribe,    ///< Pay coins to gain an extra turn
        Arrest,   ///< Steal a coin from another player
        Sanction, ///< Impose a penalty on another player
        Coup      ///< Eliminate a player at high cost
    };

    /**
     * @class Game
     * @brief Main controller for the Coup game logic.
     *
     * Manages players, turn order, action execution, and game state.
     */
    class Game {
    private:
        int currentPlayerTurn = 0;  ///< Index of the player whose turn it is

        //------------------------------------------------------------------------
        // Internal helpers for coin management
        //------------------------------------------------------------------------

        /**
         * @brief Add coins to a player's balance.
         * @param targetPlayer Player receiving coins
         * @param amount Number of coins to add
         */
        void addCoins(Player* targetPlayer, int amount);

        /**
         * @brief Remove coins from a player's balance.
         * @param targetPlayer Player losing coins
         * @param amount Number of coins to remove
         */
        void removeCoins(Player* targetPlayer, int amount);

    public:

        //------------------------------------------------------------------------------
        // Game action cost constants
        //------------------------------------------------------------------------------
       static  constexpr int COUP_COST = 7; ///< Coins required to perform a coup
       static  constexpr int BRIBE_COST = 4; ///< Coins required to perform a bribe
       static  constexpr int SANCTION_COST = 3; ///< Coins required to impose a sanction
       static  constexpr int FORCE_COUP = 10; ///< Coins required to force a coup

        //------------------------------------------------------------------------
        // Players list access
        //------------------------------------------------------------------------

        std::vector<Player*> players;  ///< All players in the current game

        /**
         * @brief Get a const reference to all active players.
         * @return Vector of player pointers
         */
        const std::vector<Player*>& getPlayers() const;

        /**
         * @brief Generate a list of valid target players for actions.
         * @param current Pointer to the acting player
         * @return Vector of pointers to targetable players
         */
        std::vector<Player*> getListOfTargetPlayers(const Player* current);

        //------------------------------------------------------------------------
        // Construction and destruction
        //------------------------------------------------------------------------


        explicit Game(const std::vector<std::string>& names);
        /**
         * @brief Initialize a new game with a list of player names.
         * Roles are assigned in order based on the names vector.
         * @param names List of player names
         * @param debugRole true for random roles
         */
        explicit Game(const std::vector<std::string> &names, bool debugRole);

        /**
         * @brief Randomly assign a role to a player by name.
         * @param name The name of the player
         * @return Pointer to a new Player instance of a random role
         */
        Player* createRandomRole(const std::string& name);

        /**
         * @brief Clean up all allocated Player instances.
         */
        ~Game();

        Game(const Game&);
        Game& operator=(const Game&);


        //------------------------------------------------------------------------
        // Turn management
        //------------------------------------------------------------------------

        /**
         * @brief Advance to the next player's turn if no extra turns remain.
         */
        void advanceTurnIfNeeded();

        /**
         * @brief Force the current player to skip their turn.
         * @param currentPlayer Player who will skip
         */
        void skipTurn(Player* currentPlayer);

        /**
         * @return True if the current player still has actions remaining.
         */
        bool currentPlayerHasTurn();

        /**
         * @brief Move the turn to the next player, resetting state.
         */
        void nextTurn();

        /**
         * @return Index of the player whose turn it currently is.
         */
        int getTurn();

        void isMerchantTurn(Player *current);

        /**
         * @return Name of the player whose turn it currently is.
         */
        std::string turn();

        //------------------------------------------------------------------------
        // Game state queries
        //------------------------------------------------------------------------

        /**
         * @brief Check if only one player remains.
         * @param players Vector of active players
         * @return True if the game is over
         */
        bool isGameOver(const std::vector<Player*>& players);

        /**
         * @brief Get the name of the winning player when game is over.
         * @return Winner's name
         */
        std::string winner() const;

        /**
         * @brief Handle exceptions thrown during gameplay.
         * @param e The exception to process
         * @return True if the error is recoverable
         */
        bool handleException(const std::exception& e);

        //------------------------------------------------------------------------
        // Player action methods
        //------------------------------------------------------------------------

        /**
         * @brief Perform the gather action to collect coins.
         * @param currentPlayer Acting player
         */
        void gather(Player* currentPlayer);

        /**
         * @brief Perform the tax action to collect multiple coins.
         * @param currentPlayer Acting player
         */
        void tax(Player* currentPlayer);

        /**
         * @brief Perform the bribe action for an extra turn.
         * @param currentPlayer Acting player
         */
        void bribe(Player* currentPlayer);

        /**
         * @brief Arrest another player, stealing a coin.
         * @param currentPlayer Acting player
         * @param targetPlayer Target of the arrest
         */
        void arrest(Player* currentPlayer, Player* targetPlayer);

        /**
         * @brief Impose a sanction on another player.
         * @param currentPlayer Acting player
         * @param targetPlayer Target of the sanction
         */
        void sanction(Player* currentPlayer, Player* targetPlayer);

        /**
         * @brief Perform a coup to eliminate a player.
         * @param currentPlayer Acting player
         * @param targetPlayer Target of the coup
         */
        void coup(Player* currentPlayer, Player* targetPlayer);

        /**
         * @brief Check if player has 10 coins, forcing a coup.
         * @param currentPlayer Acting player
         * @return True if the player must perform a coup
         */
        bool forcedToCoup(const Player *currentPlayer);

        //------------------------------------------------------------------------
        // GUI-related logic and blocking
        //------------------------------------------------------------------------

        // /**
        //  * @brief Check and apply blocking for a specific action.
        //  * @param currentPlayer Acting player
        //  * @param action The action type to block
        //  * @param blocker Player attempting the block
        //  * @return True if the action was blocked
        //  */
        // bool handleActionBlock(Player* currentPlayer, ActionType action, Player* blocker);

        /**
         * @brief Generic block handler that deducts cost and consumes turn.
         * @param blocker Player performing the block
         * @param didBlock Whether the condition to block was met
         * @param actionName Name of the blocked action (for logging)
         * @param cost Coins required to perform block
         * @return True if the block succeeded
         */
        bool handleBlock(Player* blocker, bool didBlock, const std::string& actionName, int cost = 0);

        //------------------------------------------------------------------------
        // Testing and setup helpers
        //------------------------------------------------------------------------

        /**
         * @brief Reassign random roles to an existing list of players (test only).
         * @param players Vector of player pointers to update
         */
        void getRandomRole(std::vector<Player*>& players);

        /**
         * @brief When blocked, the player pays the cost, by the blocker's role and loses their turn.
         * @param target
         * @param role player role
         */
        void playerPayAfterBlock(Player *target, Role role);
    };

} // namespace coup
