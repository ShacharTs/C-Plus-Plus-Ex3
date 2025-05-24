#pragma once
#include "roleHeader/Role.hpp"
#include <string>

// for coup kick
namespace coup {
    class Game;
}

/**
 * @class Player
 * @brief Abstract base class representing a game participant.
 *
 * Manages core player state including coins, turns, role, and ability flags.
 */
class Player {
private:
    int coins = 0;                         ///< Current coin balance
    std::string playerName;               ///< Unique identifier for the player
    const Player* lastArrestedBy = nullptr; ///< Pointer to player who last arrested this one
    int numberOfTurns = 1;                ///< Remaining actions this turn

protected:
    Role role = Role::Unknown;            ///< Assigned role for this player

public:
    // Ability availability flags
    bool canGather = true;  ///< True if gather action is currently allowed
    bool canTax = true;     ///< True if tax action is currently allowed
    bool canBribe = true;   ///< True if bribe action is currently allowed
    bool canArrest = true;  ///< True if arrest action is currently allowed
    bool canCoup = true;    ///< True if coup action is currently allowed
    bool coupShield = false;///< True if shield against coup is active

public:
    /**
     * @brief Construct a new Player with the given name.
     * @param playerName Name identifier for the player
     */
    explicit Player(std::string playerName);

    //virtual Player* clone() const;
    virtual Player *clone() const = 0;

    /**
     * @brief Virtual destructor for proper cleanup in derived classes.
     */
    virtual ~Player();

    //------------------------------------------------------------------------
    // Core abilities (self-targeted)
    //------------------------------------------------------------------------

    /**
     * @brief Perform gather action to gain a coin.
     * @throws GatherError if action is disallowed
     */
    virtual void gather();

    /**
     * @brief Perform tax action to gain multiple coins.
     * @throws TaxError if action is disallowed
     */
    virtual void tax();

    //------------------------------------------------------------------------
    // Targeted abilities
    //------------------------------------------------------------------------

    /**
     * @brief Perform bribe action, spending coins for an extra turn.
     * @throws BribeError or JudgeBlockBribeError on failure
     */
    void bribe();

    /**
     * @brief Arrest another player, stealing one coin.
     * @param targetPlayer Target of the arrest
     * @throws ArrestError or ArrestTwiceInRow on invalid use
     */
    void arrest(Player* targetPlayer);

    /**
     * @brief Impose a sanction on another player.
     * @param target Target of the sanction
     */
    void sanction(Player* target);

    /**
     * @brief Check if the target is the player themself.
     * @param target Pointer to target player
     * @return True if target is self
     */
    bool isTargetSelf(const Player* target);

    //------------------------------------------------------------------------
    // Role-specific abilities
    //------------------------------------------------------------------------

    /**
     * @brief Execute the role-specific special ability.
     * @param game Reference to the game engine
     */
    //virtual void useAbility(coup::Game& game);
    virtual void useAbility(coup::Game& game) = 0;

    /**
     * @brief Execute passive ability without target.
     */
    //virtual void passiveAbility();
    virtual void passiveAbility() = 0;

    /**
     * @brief Execute passive ability affecting a target.
     * @param target Player affected by passive ability
     */
    //virtual void passiveAbility(Player* target);
    virtual void passiveAbility(Player *target) = 0;


    //------------------------------------------------------------------------
    // Accessors
    //------------------------------------------------------------------------

    /** @return Player's name */
    std::string getName() const;

    /** @return Player's role */
    Role getRole() const;

    /** @return Current coin balance */
    int getCoins() const;

    /** @return Number of turns remaining */
    int getNumOfTurns();

    /** @return Pointer to the player who last arrested this one */
    const Player* getLastArrestedPlayer() const;

    //------------------------------------------------------------------------
    // Mutators
    //------------------------------------------------------------------------

    /**
     * @brief Record the player who arrested this one.
     * @param ptrPlayer Arresting player pointer
     */
    void setLastArrestedPlayer(const Player* ptrPlayer);

    //------------------------------------------------------------------------
    // Coin management
    //------------------------------------------------------------------------

    /**
     * @brief Increase coin balance.
     * @param amount Number of coins to add
     */
    void addCoins(int amount);

    /**
     * @brief Decrease coin balance.
     * @param amount Number of coins to remove
     */
    void removeCoins(int amount);

    //------------------------------------------------------------------------
    // Turn management
    //------------------------------------------------------------------------

    /** @brief Reset turn-based flags. */
    void resetPlayerTurn();

    /** @brief Consume one available turn. */
    void playerUsedTurn();

    /** @brief Grant an extra turn. */
    void addExtraTurn();

    /** @return True if extra turn remains */
    bool hasExtraTurn() const;

    //------------------------------------------------------------------------
    // Ability flag checks
    //------------------------------------------------------------------------

    bool isGatherAllow() const;     ///< Check gather availability
    bool isTaxAllow() const;        ///< Check tax availability
    bool isBribeAllow() const;      ///< Check bribe availability
    bool isArrestAllow() const;     ///< Check arrest availability
    bool isCoupAllow() const;       ///< Check coup availability
    bool isCoupShieldActive() const;///< Check if coup shield is active

    //------------------------------------------------------------------------
    // Miscellaneous utilities
    //------------------------------------------------------------------------

    /** @brief Remove any temporary debuffs. */
    void removeDebuff();

    /**
     * @brief Convert a Role enum to string.
     * @param role Role to convert
     * @return String representation of role
     */
    static std::string roleToString(Role role);

    /** @brief Print player statistics for debugging. */
    void printPlayerStats() const;

    /** @brief List available actions for the player. */
    virtual void listOptions() const;

};