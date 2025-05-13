#pragma once
#include <wx/wx.h>
#include <vector>
#include <string>

class MenuFrame : public wxFrame {
public:
    MenuFrame();
    void StartGame(const std::vector<std::string>& names);
};
