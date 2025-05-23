#pragma once
#include "../Player.hpp"




class General final : public virtual Player {
public:
    explicit General(const std::string& name);
    void listOptions() const override;
    Player* clone() const override;

};