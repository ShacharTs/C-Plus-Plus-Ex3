#include "GameFrame.h"
#include "GamePanel.h"
#include <wx/notebook.h>
#include "../game/Game.hpp"

// Constructor
GameFrame::GameFrame(const std::vector<std::string>& names)
    : wxFrame(nullptr, wxID_ANY, "Coup Game",
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)),
      instructions_(nullptr)

{
    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);

    // --- Game Tab ---
    auto* gamePanel = new GamePanel(notebook, names);
    gamePanel->SetMinSize(wxSize(864, 576));
    gamePanel->SetMaxSize(wxSize(864, 576));
    gamePanel->SetSize(wxSize(864, 576));

    notebook->AddPage(gamePanel, "Game");

    // --- How to Play Tab ---
    wxPanel* howToPanel = new wxPanel(notebook);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Use a default placeholder image for now
    instructions_ = new wxStaticBitmap(
    howToPanel, wxID_ANY,
    wxBitmap("assets/roles/roles_howToPlay/Spy_How_To_Play.png", wxBITMAP_TYPE_PNG),
    wxDefaultPosition,
    wxSize(864, 576) // enforce correct size
);

    // Center it in the panel without expansion
    sizer->Add(instructions_, 0, wxALIGN_CENTER | wxALL, 0);

    howToPanel->SetSizer(sizer);
    notebook->AddPage(howToPanel, "How to Play");

    Player* player = gamePanel->getCurrentPlayer();

    Role role = player->getRole();
    // --- Immediately show the correct image for the first role ---
    if (gamePanel && player) {
        UpdateHowToPlayImage(role);
        instructions_->Refresh();
        instructions_->Update();
        instructions_->GetParent()->Layout();
    }

    // --- Layout Notebook in Frame ---
    auto* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(notebook, 1, wxEXPAND);
    SetSizer(frameSizer);
    SetClientSize(864, 576);
    Centre();
    Show();


    notebook->ChangeSelection(1);
    Update();
    notebook->ChangeSelection(0);
}

// Update the "How to Play" image according to the role
void GameFrame::UpdateHowToPlayImage(Role role) {
    wxBitmap img;
    switch (role) {
        case Role::Spy:
            img.LoadFile("assets/roles/roles_howToPlay/Spy_How_To_Play.png", wxBITMAP_TYPE_PNG); // add file later
            break;
        case Role::Baron:
            img.LoadFile("assets/roles/roles_howToPlay/Baron_How_To_Play.png", wxBITMAP_TYPE_PNG);
            break;
        case Role::General:
            img.LoadFile("assets/roles/roles_howToPlay/General_How_To_Play.png", wxBITMAP_TYPE_PNG);
            break;
        case Role::Governor:
            img.LoadFile("assets/roles/roles_howToPlay/Governor_How_To_Play.png", wxBITMAP_TYPE_PNG);
            break;
        case Role::Judge:
            img.LoadFile("assets/roles/roles_howToPlay/Judge_How_To_Play.png", wxBITMAP_TYPE_PNG);
            break;
        case Role::Merchant:
            img.LoadFile("assets/roles/roles_howToPlay/Merchant_How_To_Play.png", wxBITMAP_TYPE_PNG);
            break;
    }
    if (instructions_ && img.IsOk()) {
        instructions_->SetBitmap(img);
        instructions_->Refresh();
        instructions_->Update();
        instructions_->GetParent()->Layout();
    }
}
