#pragma once
#include <wx/wx.h>
#include <vector>
#include <string>
#include <wx/notebook.h>
#include "../game/Game.hpp"
#include "../game/player/roleHeader/role.hpp"

class GameFrame : public wxFrame {
public:
    //GameFrame(const std::vector<std::string>& names);
    GameFrame(const std::vector<std::string>& names, bool debugRole = false);
    ~GameFrame();


    void UpdateHowToPlayImage(Role role);

private:
    wxStaticBitmap* instructions_ = nullptr;

    //coup::Game game;
};
