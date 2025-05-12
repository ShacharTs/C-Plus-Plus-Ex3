#pragma once
#include <wx/wx.h>
#include <vector>
#include <string>

class GameFrame : public wxFrame {
public:
    GameFrame(const std::vector<std::string>& names);
};