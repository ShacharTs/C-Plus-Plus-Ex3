#include "MenuFrame.h"
#include "GameFrame.h"
#include "MenuPanel.h"
#include "GamePanel.h"

MenuFrame::MenuFrame()
    : wxFrame(nullptr, wxID_ANY, "Coup", wxDefaultPosition, wxSize(1024, 576),
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    new MenuPanel(this);
    Centre();
    Show();
}

void MenuFrame::StartGame(const std::vector<std::string>& names, bool useRandom)
{
    auto* gameFrame = new GameFrame(names, useRandom);
    wxTheApp->SetTopWindow(gameFrame);
    gameFrame->Show();
    Destroy();
}


// void MenuFrame::StartGame(const std::vector<std::string>& names) {
//     // Create and hand off lifetime to wxWidgets
//     auto* gameFrame = new GameFrame(names);
//     wxTheApp->SetTopWindow(gameFrame);  // Let wxWidgets manage the window
//     gameFrame->Show();
//
//     Destroy(); // Properly destroy this frame
// }


