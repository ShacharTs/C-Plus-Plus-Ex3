#pragma once
#include <string>

namespace coup {
    class Game;
}

enum class Role {
    Governor,
    Spy,
    Baron,
    General,
    Judge,
    Merchant,
    Unknown
};

class Player {
private:
    int coins = 0;
    std::string playerName;
    const Player *lastArrestedBy = nullptr;
    int numberOfTurns = 1;


protected:
    Role role = Role::Unknown;

public:
    // Ability flags
    bool canGather = true;
    bool canTax = true;
    bool canBribe = true;
    bool canArrest = true;
    bool canCoup = true;
    bool coupShield = false;


public:
    explicit Player(std::string playerName);
    virtual ~Player();

    // Abilities - Self
    virtual void gather();
    virtual void tax();

    // Abilities - Targeted
    void bribe();
    void arrest(Player *targetPlayer);
    void sanction(Player *target);

    bool isTargetSelf(const Player *target);


    virtual void useAbility(coup::Game &game);
    virtual void passiveAbility();
    virtual void passiveAbility(Player *target);

    // Getters
    std::string getName() const;
    Role getRole() const;
    int getCoins() const;
    const Player *getLastArrestedPlayer() const;

    // Setters
    void setLastArrestedPlayer(const Player *ptrPlayer);

    // Coin management
    void addCoins(int amount);
    void removeCoins(int amount);

    // Turn management
    void resetPlayerTurn();
    void playerUsedTurn();
    void addExtraTurn();
    bool hasExtraTurn();

    // flag management
    bool isGatherAllow() const;
    bool isTaxAllow() const;
    bool isBribeAllow() const;
    bool isArrestAllow() const;
    bool isCoupAllow() const;
    bool isCoupShieldActive() const;



    // Misc
    void removeDebuff();
    static std::string roleToString(Role role);
    void printPlayerStats() const;

    virtual void listOptions() const;

};
