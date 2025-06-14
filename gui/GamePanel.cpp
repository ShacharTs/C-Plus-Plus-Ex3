﻿#include "GamePanel.h"
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <chrono>
#include <map>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "GameFrame.h"
#include "../game/player/roleHeader/Spy.hpp"
#include "../game/GameExceptions.hpp"

//------------------------------------------------------------------------------
// Event table binding paint, click, erase, and motion events
//------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(GamePanel, wxPanel)
EVT_PAINT (GamePanel::OnPaint)
EVT_ERASE_BACKGROUND (GamePanel::OnEraseBackground)
EVT_LEFT_DOWN (GamePanel::OnClick)
EVT_MOTION (GamePanel::OnMotion)

wxEND_EVENT_TABLE()

static constexpr int CLICK_INTERVAL_MS = 150;
static std::chrono::steady_clock::time_point lastClickTime;

//------------------------------------------------------------------------------
// Constructor: sets up panel, loads font, background, and button positions
//------------------------------------------------------------------------------
// GamePanel::GamePanel(wxFrame *parent, const std::vector<std::string> &names)
//     : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(864, 576))
//       , game(names) {
GamePanel::GamePanel(wxWindow *parent, const std::vector<std::string> &names,bool useRandomRoles)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(864, 576))
      , game(names,useRandomRoles) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);

#ifdef __WXMSW__
    customFont_ = wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                         wxFONTWEIGHT_BOLD, false, "Segoe UI");
#else
    customFont_ = wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                         wxFONTWEIGHT_BOLD, false, "Arial");
#endif

    UpdateRoleWindow();
    InitializeButtons();
    RefreshUI();
}




//------------------------------------------------------------------------------
// Suppress default erase to avoid white flicker
//------------------------------------------------------------------------------
void GamePanel::OnEraseBackground(wxEraseEvent & WXUNUSED(evt)) {
    // intentionally empty
}

//------------------------------------------------------------------------------
// Update background bitmap based on current player role
//------------------------------------------------------------------------------
void GamePanel::UpdateRoleWindow() {
    Player *current = game.getPlayers()[game.getTurn()];
    Role role = current->getRole();
    wxString path;
    switch (role) {
        case Role::Governor: path = "assets/roles/roles_stand/governor.png";
            break;
        case Role::Spy: path = "assets/roles/roles_stand/Spy.png";
            break;
        case Role::Baron: path = "assets/roles/roles_stand/Baron.png";
            break;
        case Role::General: path = "assets/roles/roles_stand/general.png";
            break;
        case Role::Judge: path = "assets/roles/roles_stand/judge.png";
            break;
        case Role::Merchant: path = "assets/roles/roles_stand/merchant.png";
            break;
        default: path = "assets/menu/game_menu.png";
            break;
    }
    bgBmp = wxBitmap(wxImage(path, wxBITMAP_TYPE_PNG));
}

//------------------------------------------------------------------------------
// Calculate button rectangles and store their hitboxes
//------------------------------------------------------------------------------
void GamePanel::InitializeButtons() {
    std::vector<wxPoint> baseRow = {
        {140, 280}, {356, 280}, {572, 280}, {680, 20},
        {32, 400}, {248, 400}, {464, 400}, {680, 400}
    };

    Player *current = game.getPlayers()[game.getTurn()];
    Role role = current->getRole();

    std::map<Role, std::vector<wxPoint> > positionsMap = {
        {Role::Spy, baseRow},
        {Role::Governor, baseRow},
        {Role::Baron, baseRow},
        {Role::General, baseRow},
        {Role::Judge, baseRow},
        {Role::Merchant, baseRow}
    };

    const std::vector<wxPoint> &positions =
            positionsMap.count(role) ? positionsMap.at(role) : positionsMap.at(Role::Governor);

    struct Meta {
        wxRect *rect;
        const char *path;
    };
    std::vector<Meta> metas = {
        {&btnGatherRect, "assets/buttons/Gather_Button.png"},
        {&btnTaxRect, "assets/buttons/Tax_Button.png"},
        {&btnBribeRect, "assets/buttons/Bribe_Button.png"},
        {
            &btnAbilityRect, role == Role::Spy
                                 ? "assets/buttons/Watch_Coins_Button.png"
                                 : role == Role::Baron
                                       ? "assets/buttons/Legal_investment_Button.png"
                                       : "assets/buttons/button_empty.png"
        },

        {&btnArrestRect, "assets/buttons/Arrest_Button.png"},
        {&btnSanctionRect, "assets/buttons/Sanction_Button.png"},
        {&btnCoupRect, "assets/buttons/Coup_Button.png"},
        {&btnSkipRect, "assets/buttons/Skip_Turn.png"}
    };
    if (role == Role::Baron && current->getCoins() < 3) {
        btnAbilityRect = wxRect(0, 0, 0, 0);
    }

    for (size_t i = 0; i < metas.size() && i < positions.size(); ++i) {
        wxBitmap bmp(metas[i].path, wxBITMAP_TYPE_PNG);
        metas[i].rect->x = positions[i].x;
        metas[i].rect->y = positions[i].y;
        metas[i].rect->width = bmp.IsOk() ? bmp.GetWidth() : 0;
        metas[i].rect->height = bmp.IsOk() ? bmp.GetHeight() : 0;
    }
}

//------------------------------------------------------------------------------
// Refresh UI based solely on game state (no turn swapping)
//------------------------------------------------------------------------------
void GamePanel::RefreshUI() {
    if (game.isGameOver(game.getPlayers())) {
        showWinner(game.winner());
        return;
    }
    UpdateRoleWindow();
    InitializeButtons();


    GameFrame *frame = dynamic_cast<GameFrame *>(GetParent()->GetParent());
    if (frame) {
        Player *current = game.getPlayers()[game.getTurn()];
        frame->UpdateHowToPlayImage(current->getRole());
    }


    Refresh();
}

//------------------------------------------------------------------------------
// Paint handler: background, buttons, and custom text
//------------------------------------------------------------------------------
void GamePanel::OnPaint(wxPaintEvent & WXUNUSED(evt)) {
    wxAutoBufferedPaintDC dc(this);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    if (bgBmp.IsOk()) {
        gc->DrawBitmap(gc->CreateBitmap(bgBmp), 0, 0,
                       bgBmp.GetWidth(), bgBmp.GetHeight());
    }

    struct Btn {
        wxRect r;
        const char *p;
    };
    std::vector<Btn> btns = {
        {btnGatherRect, "assets/buttons/Gather_Button.png"},
        {btnTaxRect, "assets/buttons/Tax_Button.png"},
        {btnBribeRect, "assets/buttons/Bribe_Button.png"},
        {btnArrestRect, "assets/buttons/Arrest_Button.png"},
        {btnSanctionRect, "assets/buttons/Sanction_Button.png"},
        {btnCoupRect, "assets/buttons/Coup_Button.png"},
        {btnSkipRect, "assets/buttons/Skip_Turn.png"}
    };

    Player *curr = game.getPlayers()[game.getTurn()];
    Role role = curr->getRole();

    switch (role) {
        case Role::Spy:
            btns.push_back({btnAbilityRect, "assets/buttons/Watch_Coins_Button.png"});
            break;
        case Role::Baron:
            btns.push_back({btnAbilityRect, "assets/buttons/Legal_investment_Button.png"});
            break;
        default:
            break;
    }

    for (auto &b: btns) {
        wxBitmap bmp(b.p, wxBITMAP_TYPE_PNG);
        if (bmp.IsOk()) {
            gc->DrawBitmap(gc->CreateBitmap(bmp), b.r.x, b.r.y, b.r.width, b.r.height);
        }
    }

    gc->SetFont(customFont_, *wxWHITE);
    wxString name = curr->getName() + "'s Turn";
    wxString roleS = "Role: " + curr->roleToString(curr->getRole());
    wxString coinsS = "Coins: " + wxString::Format("%d", curr->getCoins());
    wxString turnsS = "Turns Left: " + wxString::Format("%d", curr->getNumOfTurns());

    gc->DrawText(name, 40, 20);
    gc->DrawText(roleS, 40, 52);
    gc->DrawText(coinsS, 40, 84);
    gc->DrawText(turnsS, 40, 116);

    delete gc;
}

//------------------------------------------------------------------------------
// GUI-based block prompt (returns the blocking player if someone blocked)
//------------------------------------------------------------------------------
Player *GamePanel::AskBlock(Role blockerRole,
                            const wxString &actionName,
                            int cost /*=0*/) {
    Player *current = game.getPlayers()[game.getTurn()];
    for (Player *p: game.getPlayers()) {
        if (p != current && p->getRole() == blockerRole) {
            wxString question = wxString::Format(
                "%s (%s)\nBlock %s's %s%s?",
                p->getName(), p->roleToString(p->getRole()),
                current->getName(), actionName,
                cost > 0 ? wxString::Format(" for %d coins", cost) : wxString()
            );
            wxMessageDialog dlg(this, question, "Block Action",
                                wxYES_NO | wxICON_QUESTION);
            if (dlg.ShowModal() == wxID_YES) {
                return p;
            }
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
// Mouse click handler: button detection and game actions
//------------------------------------------------------------------------------
void GamePanel::OnClick(wxMouseEvent &evt) {
    // click‐throttle
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastClickTime).count() < CLICK_INTERVAL_MS)
        return;
    lastClickTime = now;

    // refresh state
    UpdateRoleWindow();
    InitializeButtons();

    wxPoint pt = evt.GetPosition();
    Player *cur = game.getPlayers()[game.getTurn()];
    Role role = cur->getRole();

    // try each handler in turn; if one returns true, we’re done
    if (HandleMustCoup(pt, cur)) return;
    if (HandleGather(pt, cur)) return;
    if (HandleTax(pt, cur)) return;
    if (HandleBribe(pt, cur)) return;
    if (HandleAbility(pt, cur, role)) return;
    if (HandleSkip(pt, cur)) return;
    if (HandleArrest(pt, cur)) return;
    if (HandleCoup(pt, cur)) return;
    if (HandleSanction(pt, cur)) return;
    //if (HandleTargeted(pt, cur)) return;
}

//------------------------------------------------------------------------------
// 1) Forced-to-coup: only Coup allowed
//------------------------------------------------------------------------------
bool GamePanel::HandleMustCoup(const wxPoint &pt, Player *cur) {
    if (!game.forcedToCoup(cur)) {
        mustCoupAlerted_ = false;
        return false;
    }

    if (btnCoupRect.Contains(pt)) {
        wxArrayString names;
        auto targets = game.getListOfTargetPlayers(cur);
        for (auto *p: targets) names.Add(p->getName());

        wxSingleChoiceDialog dlg(this, "Choose a target to coup", "Forced Coup", names);
        if (dlg.ShowModal() != wxID_OK) return true;

        Player *tgt = targets[dlg.GetSelection()];

        // Check if a General is present with enough coins to block
        Player *general = nullptr;
        for (auto *p : game.getPlayers()) {
            if (p != cur && p->getRole() == Role::General && p->getCoins() >= 5) {
                general = p;
                break;
            }
        }

        bool blocked = false;
        Player *isGeneral = AskBlock(Role::General, "coup", 5);
        if (general && isGeneral) {
            try {
                game.playerPayAfterBlock(isGeneral, Role::General);
                wxLogWarning("General blocked the coup.");
                wxMessageBox("Your coup was blocked by the General!",
                             "Coup Blocked", wxOK | wxICON_INFORMATION, this);
                blocked = true;
            } catch (const std::exception &e) {
                wxLogWarning("General block failed: %s", e.what());
                wxMessageBox("General failed to block (not enough coins). Proceeding with coup.",
                             "Block Failed", wxOK | wxICON_WARNING, this);
                // Continue to coup
            }
        }

        // If not blocked, perform the coup
        if (!blocked) {
            try {
                game.coup(cur, tgt);
                //PlaySound(SoundEffect::CoupKick);
            } catch (const CoinsError &e) {
                wxMessageBox("Coup failed: You don't have enough coins.", "Coup Failed",
                             wxOK | wxICON_WARNING, this);
                return true;
            } catch (const SelfError &e) {
                wxMessageBox("You cannot coup yourself.", "Coup Failed",
                             wxOK | wxICON_WARNING, this);
                return true;
            } catch (const CoupBlocked &e) {
                wxMessageBox("Coup was blocked by a shield!", "Blocked",
                             wxOK | wxICON_INFORMATION, this);
                return true;
            } catch (const std::exception &e) {
                wxLogWarning("Unexpected error: %s", e.what());
                wxMessageBox(e.what(), "Coup Failed", wxOK | wxICON_ERROR, this);
                return true;
            }
        }

        game.advanceTurnIfNeeded();
        RefreshUI();
        mustCoupAlerted_ = false;
        return true;
    }

    wxMessageBox("You have too many coins\nYou must coup!",
                 "Forced Coup", wxOK | wxICON_INFORMATION, this);
    return true;
}



//------------------------------------------------------------------------------
// 2) Gather
//------------------------------------------------------------------------------
bool GamePanel::HandleGather(const wxPoint &pt, Player *cur) {
    if (!btnGatherRect.Contains(pt)) return false;
    try {
        game.gather(cur);
    } catch (const GatherError &e) {
        // show any "can’t gather" error
        wxLogWarning("%s", e.what());
        return true; // swallow the click so they can try again
    }
    catch (const std::exception &e) {
        // show any "can’t gather" error
        wxLogWarning("%s", e.what());
        return true; // swallow the click so they can try again
    }

    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}


//------------------------------------------------------------------------------
// 3) Tax
//------------------------------------------------------------------------------
bool GamePanel::HandleTax(const wxPoint &pt, Player *cur) {
    if (!btnTaxRect.Contains(pt)) return false;
    try {
        // Give the Governor a chance to block
        if (AskBlock(Role::Governor, "tax")) {
            game.playerPayAfterBlock(cur, Role::Governor);
            wxLogWarning("Governor blocked tax action.");
            RefreshUI();
            return true;
        }
        // If not blocked, perform the tax as normal
        game.tax(cur);
        game.advanceTurnIfNeeded();
        RefreshUI();
        return true;
    } catch (const TaxError &e) {
        wxLogWarning("%s", e.what());
        return true;
    }
    catch (const std::exception &e) {
        wxLogWarning("%s", e.what());
        return true;
    }
}

//------------------------------------------------------------------------------
// 4) Bribe
//------------------------------------------------------------------------------
bool GamePanel::HandleBribe(const wxPoint &pt, Player *cur) {
    if (!btnBribeRect.Contains(pt)) return false;

    // First, give the Judge a chance to block
    if (AskBlock(Role::Judge, "bribe")) {
        try {
            game.playerPayAfterBlock(cur, Role::Judge);
            wxLogWarning("Judge blocked bribe action.");
        } catch (const BribeError &e) {
            wxLogWarning("%s", e.what());
            return true;
        } catch (const std::exception &e) {
            wxLogWarning("%s", e.what());
            return true;
        }

        RefreshUI();
        return true;
    }

    // If not blocked, try to perform the bribe as normal
    try {
        game.bribe(cur); // <-- Attempt the action
    } catch (const std::exception &e) {
        wxLogWarning("%s", e.what());
        return true;
    }

    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}



    //------------------------------------------------------------------------------
    // 5) Ability (Spy/Baron)
    //------------------------------------------------------------------------------
    bool GamePanel::HandleAbility(const wxPoint &pt, Player *cur, Role role) {
        if (!btnAbilityRect.Contains(pt)) return false;

        if (role == Role::Baron && cur->getCoins() < 3) {
            wxLogWarning("You need at least 3 coins to cough cough legal investment.");
            return true; // swallow the click instead of calling useAbility
        }

        switch (role) {
            case Role::Spy: {
                Spy *spy = dynamic_cast<Spy *>(cur); // Down-casting to spy to get coin report
                wxString msg = spy->getCoinReport(game);
                wxMessageDialog dlg(this, msg, "Coins Report",
                                    wxOK | wxICON_INFORMATION);
                dlg.ShowModal();
                break;
            }
            case Role::Baron:
                cur->useAbility(game);
                break;
            default:
                return false;
        }
        game.advanceTurnIfNeeded();
        RefreshUI();
        return true;
    }

    //------------------------------------------------------------------------------
    // 6) Skip Turn
    //------------------------------------------------------------------------------
    bool GamePanel::HandleSkip(const wxPoint &pt, Player *cur) {
        if (!btnSkipRect.Contains(pt)) return false;
        game.skipTurn(cur);
        game.advanceTurnIfNeeded();
        RefreshUI();
        return true;
    }


bool GamePanel::HandleArrest(const wxPoint &pt, Player *cur) {
    if (!btnArrestRect.Contains(pt)) return false;

    // Choose target dialog
    wxArrayString names;
    auto targets = game.getListOfTargetPlayers(cur);
    for (auto *p: targets) names.Add(p->getName());

    wxSingleChoiceDialog dlg(this, "Choose target to arrest", "Target", names);
    if (dlg.ShowModal() != wxID_OK) return true;

    Player *tgt = targets[dlg.GetSelection()];

    // Spy can block Arrest
    Player* blocker = AskBlock(Role::Spy, "arrest");
    if (blocker) {
        try {
            game.playerPayAfterBlock(cur, Role::Spy);  // cur pays cost for failed action
            wxLogWarning("Spy blocked arrest action.");
        } catch (const std::exception &e) {
            wxLogWarning("Spy block failed: %s", e.what());
            return true; // swallow the click regardless
        }

        RefreshUI();
        return true;
    }

    // If not blocked, try to perform arrest
    try {
        game.arrest(cur, tgt);
    } catch (const std::exception &e) {
        wxLogWarning("%s", e.what());
        return true;
    }

    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}



    bool GamePanel::HandleSanction(const wxPoint &pt, Player *cur) {
        if (!btnSanctionRect.Contains(pt)) return false;

        // Choose target dialog
        wxArrayString names;
        auto targets = game.getListOfTargetPlayers(cur);
        for (auto *p: targets) names.Add(p->getName());

        wxSingleChoiceDialog dlg(this, "Choose target to sanction", "Target", names);
        if (dlg.ShowModal() != wxID_OK) return true;
        Player *tgt = targets[dlg.GetSelection()];

        try {
            game.sanction(cur, tgt);
        } catch (const std::exception &e) {
            wxLogWarning("%s", e.what());
            return true;
        }

        game.advanceTurnIfNeeded();
        RefreshUI();
        return true;
    }

    bool GamePanel::HandleCoup(const wxPoint &pt, Player *cur) {
        if (!btnCoupRect.Contains(pt)) return false;

        // Choose target dialog
        wxArrayString names;
        auto targets = game.getListOfTargetPlayers(cur);
        for (auto *p: targets) names.Add(p->getName());

        wxSingleChoiceDialog dlg(this, "Choose target to coup", "Target", names);
        if (dlg.ShowModal() != wxID_OK) return true;

        Player *tgt = targets[dlg.GetSelection()];

        // Ask for General block and get the actual blocker if one accepts
        Player *blocker = AskBlock(Role::General, "coup", 5);
        bool blocked = false;

        if (blocker) {
            try {
                game.playerPayAfterBlock(blocker, Role::General);
                wxLogWarning("General blocked coup action.");
                blocked = true; // Block was successful
            } catch (const std::exception &e) {
                wxLogWarning("General block failed: %s", e.what());
                wxMessageBox("Block failed: General didn't have enough coins. Proceeding with coup.",
                             "Block Failed", wxOK | wxICON_WARNING, this);
            }
        }

        if (!blocked) {
            try {
                game.coup(cur, tgt); // May throw CoinsError, SelfError, CoupBlocked
            } catch (const CoinsError &e) {
                wxMessageBox("Coup failed: You don't have enough coins.", "Coup Failed",
                             wxOK | wxICON_WARNING, this);
                return true;
            } catch (const SelfError &e) {
                wxMessageBox("You cannot coup yourself.", "Coup Failed",
                             wxOK | wxICON_WARNING, this);
                return true;
            } catch (const CoupBlocked &e) {
                wxMessageBox("Coup was blocked by a shield!", "Blocked",
                             wxOK | wxICON_INFORMATION, this);
                return true;
            } catch (const std::exception &e) {
                wxLogWarning("Unexpected coup error: %s", e.what());
                wxMessageBox(e.what(), "Coup Failed", wxOK | wxICON_ERROR, this);
                return true;
            }
        }

        game.advanceTurnIfNeeded();
        RefreshUI();
        return true;
    }


    //------------------------------------------------------------------------------
    // Mouse motion handler: change cursor when hovering buttons
    //------------------------------------------------------------------------------
    void GamePanel::OnMotion(wxMouseEvent &evt) {
        wxPoint pt = evt.GetPosition();
        bool hover = false;

        for (wxRect *r: {
                 &btnGatherRect, &btnTaxRect, &btnBribeRect,
                 &btnArrestRect, &btnSanctionRect, &btnCoupRect, &btnSkipRect
             }) {
            if (r->Contains(pt)) {
                hover = true;
                break;
            }
        }

        Player *cur = game.getPlayers()[game.getTurn()];
        Role role = cur->getRole();

        if (!hover && (role == Role::Spy || role == Role::Baron)) {
            hover = btnAbilityRect.Contains(pt);
        }

        SetCursor(hover ? wxCursor(wxCURSOR_HAND) : wxCursor(wxCURSOR_ARROW));
    }

    void GamePanel::showWinner(const std::string &winner) {
        wxMessageDialog dlg(this, "Winner: " + winner, "Game Over", wxOK | wxICON_INFORMATION);
        //PlaySound(SoundEffect::Victory);
        dlg.ShowModal();
        exit(0);
    }
