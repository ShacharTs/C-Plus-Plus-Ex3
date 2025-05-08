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
    // flags is ability doable
    bool canGather = true;
    bool canTax = true;
    bool canBribe = true;
    bool canArrest = true;
    bool canSanction = true;
    bool isShieldActive = false;



public:
    explicit Player(std::string playerName);

    virtual ~Player();

    virtual void useAbility(coup::Game &game);
    virtual void gather();
    virtual void tax();


    std::string getName() const;
    Role getRole() const;


    void resetPlayerTurn();
    static std::string roleToString(Role role);

    int getCoins() const;
    const Player *getLastArrestedPlayer() const;
    void setLastArrestedPlayer(const Player *ptrPlayer);

    void addCoins(int amount);
    void removeCoins(int amount);
    void playerUseTurn();
    void addExtraTurn();
    bool hasExtraTurn();


};


