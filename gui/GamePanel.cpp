#include "GamePanel.h"
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <chrono>
#include <map>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "../game/player/roleHeader/Spy.hpp"
#include <wx/stdpaths.h>
#include <wx/filename.h>

//------------------------------------------------------------------------------
// Event table binding paint, click, erase, and motion events
//------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(GamePanel, wxPanel)
EVT_PAINT(GamePanel::OnPaint)
EVT_ERASE_BACKGROUND(GamePanel::OnEraseBackground)
EVT_LEFT_DOWN(GamePanel::OnClick)
EVT_MOTION(GamePanel::OnMotion)
wxEND_EVENT_TABLE()

static constexpr int CLICK_INTERVAL_MS = 150;
static std::chrono::steady_clock::time_point lastClickTime;

//------------------------------------------------------------------------------
// Constructor: sets up panel, loads font, background, and button positions
//------------------------------------------------------------------------------
GamePanel::GamePanel(wxFrame* parent, const std::vector<std::string>& names)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(864, 576))
    , game(names)
{
    // Enable double-buffered paint to prevent flicker
    SetBackgroundStyle(wxBG_STYLE_PAINT);

#ifdef __WXMSW__
    // Use system-safe font on Windows
    customFont_ = wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                         wxFONTWEIGHT_BOLD, false, "Segoe UI");
#else
    // Use system-safe font on other platforms
    customFont_ = wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                         wxFONTWEIGHT_BOLD, false, "Arial");
#endif

    wxLogStatus("Font in use: %s", customFont_.GetFaceName());



    // Initialize background image and button hitboxes
    UpdateRoleWindow();
    InitializeButtons();
}



//------------------------------------------------------------------------------
// Suppress default erase to avoid white flicker
//------------------------------------------------------------------------------
void GamePanel::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

//------------------------------------------------------------------------------
// Update background bitmap based on current player role
//------------------------------------------------------------------------------
void GamePanel::UpdateRoleWindow()
{
    Player* current = game.getPlayers()[game.getTurn()];
    Role role = current->getRole();
    wxString path;
    switch (role)
    {
    case Role::Governor:  path = "assets/roles/roles_stand/governor.png"; break;
    case Role::Spy:       path = "assets/roles/roles_stand/spy.png";      break;
    case Role::Baron:     path = "assets/roles/roles_stand/baron.png";    break;
    case Role::General:   path = "assets/roles/roles_stand/general.png";  break;
    case Role::Judge:     path = "assets/roles/roles_stand/judge.png";    break;
    case Role::Merchant:  path = "assets/roles/roles_stand/merchant.png"; break;
    default:              path = "assets/menu/game_menu.png";             break;
    }
    bgBmp = wxBitmap(wxImage(path, wxBITMAP_TYPE_PNG));
}

//------------------------------------------------------------------------------
// Calculate button rectangles and store their hitboxes
//------------------------------------------------------------------------------
void GamePanel::InitializeButtons()
{
    // Predefined positions for role-based button layout
    std::vector<wxPoint> baseRow = {
        {140, 280}, {356, 280}, {572, 280}, {680, 20},
        { 32, 400}, {248, 400}, {464, 400}, {680,400}
    };

    Player* current = game.getPlayers()[game.getTurn()];
    Role role = current->getRole();

    // All roles use same layout for now
    std::map<Role, std::vector<wxPoint>> positionsMap =
    {
        {Role::Spy,      baseRow},
        {Role::Governor, baseRow},
        {Role::Baron,    baseRow},
        {Role::General,  baseRow},
        {Role::Judge,    baseRow},
        {Role::Merchant, baseRow}
    };

    const std::vector<wxPoint>& positions =
        positionsMap.count(role) ? positionsMap.at(role) : positionsMap.at(Role::Governor);

    struct Meta { wxRect* rect; const char* path; };
    std::vector<Meta> metas =
    {
        {&btnGatherRect,   "assets/buttons/Gather_Button.png"},
        {&btnTaxRect,      "assets/buttons/Tax_Button.png"},
        {&btnBribeRect,    "assets/buttons/Bribe_Button.png"},
        {&btnAbilityRect,  role == Role::Spy ? "assets/buttons/Watch_Coins_Button.png"
                          : role == Role::Baron ? "assets/buttons/Legal_investment_Button.png"
                                              : "assets/buttons/button_empty.png"},
        {&btnArrestRect,   "assets/buttons/Arrest_Button.png"},
        {&btnSanctionRect, "assets/buttons/Sanction_Button.png"},
        {&btnCoupRect,     "assets/buttons/Coup_Button.png"},
        {&btnSkipRect,     "assets/buttons/Skip_Turn.png"}
    };

    // Assign rectangle position and size from bitmap dimensions
    for (size_t i = 0; i < metas.size() && i < positions.size(); ++i)
    {
        wxBitmap bmp(metas[i].path, wxBITMAP_TYPE_PNG);
        metas[i].rect->x = positions[i].x;
        metas[i].rect->y = positions[i].y;
        metas[i].rect->width = bmp.IsOk() ? bmp.GetWidth() : 0;
        metas[i].rect->height = bmp.IsOk() ? bmp.GetHeight() : 0;
    }
}

//------------------------------------------------------------------------------
// Advance turn if no extra turn and refresh UI
//------------------------------------------------------------------------------
void GamePanel::RefreshUI() {
    Player* current = game.getPlayers()[game.getTurn()];
    if (!current->hasExtraTurn()) {
        game.advanceTurnIfNeeded();
    }

    UpdateRoleWindow();
    InitializeButtons();
    Refresh();
}




//------------------------------------------------------------------------------
// Paint handler: background, buttons, and custom text
//------------------------------------------------------------------------------
void GamePanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    // 1) Draw background image
    if (bgBmp.IsOk())
    {
        gc->DrawBitmap(gc->CreateBitmap(bgBmp), 0, 0,
            bgBmp.GetWidth(), bgBmp.GetHeight());
    }

    // 2) Draw buttons
    struct Btn { wxRect r; const char* p; };
    std::vector<Btn> btns =
    {
        {btnGatherRect,   "assets/buttons/Gather_Button.png"},
        {btnTaxRect,      "assets/buttons/Tax_Button.png"},
        {btnBribeRect,    "assets/buttons/Bribe_Button.png"},
        {btnArrestRect,   "assets/buttons/Arrest_Button.png"},
        {btnSanctionRect, "assets/buttons/Sanction_Button.png"},
        {btnCoupRect,     "assets/buttons/Coup_Button.png"},
        {btnSkipRect,     "assets/buttons/Skip_Turn.png"}
    };

    Player* curr = game.getPlayers()[game.getTurn()];
    Role role = curr->getRole();

    switch (role) {
    case Role::Spy:
        btns.push_back({ btnAbilityRect, "assets/buttons/Watch_Coins_Button.png" });
        break;
    case Role::Baron:
        btns.push_back({ btnAbilityRect, "assets/buttons/Legal_investment_Button.png" });
        break;
    default:
        // no ability button for other roles
        break;
    }

    for (auto& b : btns)
    {
        wxBitmap bmp(b.p, wxBITMAP_TYPE_PNG);
        if (bmp.IsOk())
        {
            gc->DrawBitmap(gc->CreateBitmap(bmp), b.r.x, b.r.y, b.r.width, b.r.height);
        }
    }

    // 3) Draw custom text using the chosen font
    gc->SetFont(customFont_, *wxWHITE);
    wxString name = curr->getName() + "'s Turn";
    wxString roleS = "Role: " + curr->roleToString(curr->getRole());
    wxString coinsS = "Coins: " + wxString::Format("%d", curr->getCoins());
    int visibleTurns = curr->getNumOfTurns();
	wxString turnsS = "Turns Left: " + wxString::Format("%d", visibleTurns);


    gc->DrawText(name, 40, 20);
    gc->DrawText(roleS, 40, 52);
    gc->DrawText(coinsS, 40, 84);
    gc->DrawText(turnsS, 40, 116);


    delete gc;
}


//------------------------------------------------------------------------------
// GUI‐based block prompt (returns the blocking player if someone blocked)
//------------------------------------------------------------------------------
Player* GamePanel::AskBlock(Role blockerRole,
    const wxString& actionName,
    int cost /*=0*/)
{
    Player* current = game.getPlayers()[game.getTurn()];
    for (Player* p : game.getPlayers())
    {
        if (p != current && p->getRole() == blockerRole)
        {
            wxString question = wxString::Format(
                "%s (%s)\nBlock %s's %s%s?",
                p->getName(),
                p->roleToString(p->getRole()),
                current->getName(),
                actionName,
                cost > 0 ? wxString::Format(" for %d coins", cost) : wxString()
            );
            wxMessageDialog dlg(
                this,
                question,
                "Block Action",
                wxYES_NO | wxICON_QUESTION
            );
            if (dlg.ShowModal() == wxID_YES) {
                return p;  // Return the blocking player
            }
        }
    }
    return nullptr;  // No one blocked
}


//------------------------------------------------------------------------------
// Mouse click handler: button detection and game actions
//------------------------------------------------------------------------------
void GamePanel::OnClick(wxMouseEvent& evt)
{
    // Prevent rapid repeated clicks
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count() < CLICK_INTERVAL_MS)
        return;
    lastClickTime = now;

    UpdateRoleWindow();
    InitializeButtons();

    wxPoint pt = evt.GetPosition();
    Player* cur = game.getPlayers()[game.getTurn()];
    Role role = cur->getRole();

    try {
        // 1) Gather
        if (btnGatherRect.Contains(pt)) {
            game.gather(cur);
            game.advanceTurnIfNeeded();
            RefreshUI();
            return;
        }

        // 2) Tax
        if (btnTaxRect.Contains(pt)) {
            if (!AskBlock(Role::Governor, "tax")) {
                game.tax(cur);
            }
            game.advanceTurnIfNeeded();
            RefreshUI();
            return;
        }

        // 3) Bribe
        if (btnBribeRect.Contains(pt)) {
            if (!AskBlock(Role::Judge, "bribe")) {
                game.bribe(cur);  // Gives extra turns, do NOT advance turn
            }
            RefreshUI();
            return;
        }

        // 4) Use Role Ability
        if (btnAbilityRect.Contains(pt)) {
            switch (role) {
                case Role::Spy: {
                    Spy* spy = static_cast<Spy*>(cur);
                    wxString msg = spy->getCoinReport(game);
                    wxMessageDialog dlg(this, msg, "Coins Report", wxOK | wxICON_INFORMATION);
                    dlg.ShowModal();
                    break;
                }
                case Role::Baron: {
                    cur->useAbility(game);
                    break;
                }
                default:
                    break; // Other roles have no ability
            }
            RefreshUI();
            return;
        }

        // 5) Skip Turn
        if (btnSkipRect.Contains(pt)) {
            game.skipTurn(cur);
            game.advanceTurnIfNeeded();
            RefreshUI();
            return;
        }

        // 6) Targeted Actions
        if (btnArrestRect.Contains(pt) || btnSanctionRect.Contains(pt) || btnCoupRect.Contains(pt)) {
            wxArrayString names;
            std::vector<Player*> targets = game.getListOfTargetPlayers(cur);
            for (auto* p : game.getPlayers())
                if (p != cur)
                    names.Add(p->getName());

            wxString title =
                btnArrestRect.Contains(pt) ? "Choose target to arrest" :
                btnSanctionRect.Contains(pt) ? "Choose target to sanction" :
                                               "Choose target to coup";

            wxSingleChoiceDialog dlg(this, title, "Target", names);
            if (dlg.ShowModal() != wxID_OK)
                return;
            Player* tgt = targets[dlg.GetSelection()];

            if (btnArrestRect.Contains(pt)) {
                Player* blocker = AskBlock(Role::Spy, "arrest");
                if (game.handleBlock(blocker, blocker != nullptr, "arrest", 0)) {
                    RefreshUI();
                    return;
                }
                game.arrest(cur, tgt);
            }
            else if (btnSanctionRect.Contains(pt)) {
                // Player* blocker = AskBlock(Role::Judge, "sanction");
                // if (game.handleBlock(blocker, blocker != nullptr, "sanction", 0)) {
                //     RefreshUI();
                //     return;
                // }
                game.sanction(cur, tgt);
                RefreshUI();
                return;
            }
            else { // Coup
                Player* blocker = AskBlock(Role::General, "coup", 5);
                if (game.handleBlock(blocker, blocker != nullptr, "coup", 5)) {
                    RefreshUI();
                    return;
                }
                game.coup(cur, tgt);
            }

            game.advanceTurnIfNeeded();
            RefreshUI();
            return;
        }
    }
    catch (const std::exception& ex) {
        wxLogError(ex.what());
    }
}





//------------------------------------------------------------------------------
// Mouse motion handler: change cursor when hovering buttons
//------------------------------------------------------------------------------
void GamePanel::OnMotion(wxMouseEvent& evt)
{
    wxPoint pt = evt.GetPosition();
    bool hover = false;

    for (wxRect* r : { &btnGatherRect, &btnTaxRect, &btnBribeRect,
                       &btnArrestRect, &btnSanctionRect, &btnCoupRect, &btnSkipRect })
    {
        if (r->Contains(pt))
        {
            hover = true;
            break;
        }
    }

    Player* cur = game.getPlayers()[game.getTurn()];
    Role role = cur->getRole();

    if (!hover &&(role == Role::Spy|| role == Role::Baron)){
        hover = btnAbilityRect.Contains(pt);
    }

    SetCursor(hover ? wxCursor(wxCURSOR_HAND) : wxCursor(wxCURSOR_ARROW));
}
