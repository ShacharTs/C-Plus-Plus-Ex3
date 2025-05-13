#include "GameFrame.h"
#include "GamePanel.h"

GameFrame::GameFrame(const std::vector<std::string>& names)
    : wxFrame(nullptr, wxID_ANY, "Coup Game",
        wxDefaultPosition, wxSize(864, 576),
        wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{   
    new GamePanel(this, names);
    Centre();
    Show();
}