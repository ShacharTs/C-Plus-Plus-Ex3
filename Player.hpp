#pragma once

#include <string>
    class Player {
    private:
        int coins = 0;
        std::string playerName;

    public:
        explicit Player(std::string playerName);

        virtual ~Player();


        std::string getName() const;

        int getCoins() const;

        // core actions
        void gather();

        void tax();

        void bribe();

        void arrest(Player *target);

        void sanction(Player *target);

        void coup(Player *target);


        void addCoins(int amount);

        void removeCoins(int amount);
    };

