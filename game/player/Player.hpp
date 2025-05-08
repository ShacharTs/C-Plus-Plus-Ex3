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
    bool extraTurn = false;

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

    virtual void useAbility(Player* target);
    virtual void useAbility();



    std::string getName() const;
    Role getRole() const;
    static std::string roleToString(Role role);

    int getCoins() const;
    const Player *getLastArrestedBy() const;
    void setLastArrestedBy(const Player *ptrPlayer);

    void addCoins(int amount);
    void removeCoins(int amount);
    void removeExtraTurn();
    void addExtraTurn();
    bool hasExtraTurn();

private:
    // core actions
    void requestGather(coup::Game &game);

    void requestTax(coup::Game &game);

    void requestBribe(coup::Game &game);

    void requestArrest(coup::Game &game, Player *target);


    void requestSanction(coup::Game &game, Player *target);

    void requestCoup(coup::Game &game, Player *target);



};


