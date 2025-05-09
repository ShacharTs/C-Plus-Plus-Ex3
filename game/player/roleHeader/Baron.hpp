#pragma once

#include "../Player.hpp"

class Baron final : public Player {
public:
    explicit Baron(const std::string& name);

    void useAbility(coup::Game &game) override;

    void listOptions() const;
};
