#pragma once

#include "../Player.hpp"

class Governor final : public Player {
public:
    explicit Governor(const std::string& name);
    void tax() override;

    void listOptions() const override;
};
