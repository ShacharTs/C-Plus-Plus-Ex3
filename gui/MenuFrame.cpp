#include "MenuFrame.h"
#include "GameFrame.h"
#include "MenuPanel.h"
#include "GamePanel.h"

MenuFrame::MenuFrame()
    : wxFrame(nullptr, wxID_ANY, "Coup", wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    auto* panel = new MenuPanel(this);
    SetClientSize(panel->GetSize());  // <- Match frame size to panel size
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

MenuFrame::~MenuFrame() {
    // Currently nothing to clean up
}



