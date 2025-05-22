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


void MenuFrame::StartGame(const std::vector<std::string>& names) {
    // Remove the old panel and open the new game frame
    auto* gameFrame = new GameFrame(names);
    gameFrame->Show();

    Close(); // or Destroy(); to close the menu window
}

