#include "GameFrame.h"
#include "GamePanel.h"

GameFrame::GameFrame(const std::vector<std::string>& names)
    : wxFrame(nullptr, wxID_ANY, "Coup Game",
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    auto* panel = new GamePanel(this, names);
    panel->SetMinSize(wxSize(864, 576));  // ensure layout respects panel size
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(panel, 1, wxEXPAND);
    SetSizer(sizer);
    SetClientSize(864, 576);
    Centre();
    Show();
}

