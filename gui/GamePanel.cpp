#include "GamePanel.h"
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <chrono>
#include <map>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "../game/player/roleHeader/Spy.hpp"



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

    // ——————————————
    // Pre-load all sounds into memory
    // ——————————————
    for (auto effect : { SoundEffect::GatherCoin,
                         SoundEffect::TaxCoin,
                         SoundEffect::Victory,
                         SoundEffect::CoupKick })
    {
        // build "<exe-dir>/assets/sounds/<filename>"
        wxFileName file(wxStandardPaths::Get().GetExecutablePath());
        file.RemoveLastDir();
        file.AppendDir("assets");
        file.AppendDir("sounds");
        file.SetFullName( EffectToFilename(effect) );

        if ( file.FileExists() ) {
            sounds_.try_emplace(effect, file.GetFullPath());
        }
        else {
            wxLogError("Missing sound at startup: %s", file.GetFullPath());
        }
    }
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
    std::vector<wxPoint> baseRow = {{140,280},{356,280},{572,280},{680,20},
                                    {32,400},{248,400},{464,400},{680,400}};

    Player* current = game.getPlayers()[game.getTurn()];
    Role role = current->getRole();

    std::map<Role, std::vector<wxPoint>> positionsMap = {{Role::Spy,      baseRow},
                                                           {Role::Governor, baseRow},
                                                           {Role::Baron,    baseRow},
                                                           {Role::General,  baseRow},
                                                           {Role::Judge,    baseRow},
                                                           {Role::Merchant, baseRow}};

    const std::vector<wxPoint>& positions =
        positionsMap.count(role) ? positionsMap.at(role) : positionsMap.at(Role::Governor);

    struct Meta { wxRect* rect; const char* path; };
    std::vector<Meta> metas = {
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
    if (role == Role::Baron && current->getCoins() < 3) {
        btnAbilityRect = wxRect(0,0,0,0);
    }

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
// Refresh UI based solely on game state (no turn swapping)
//------------------------------------------------------------------------------
void GamePanel::RefreshUI() {
    if(game.isGameOver(game.getPlayers())) {
        showWinner(game.winner());
        return;
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

    if (bgBmp.IsOk()) {
        gc->DrawBitmap(gc->CreateBitmap(bgBmp), 0, 0,
                       bgBmp.GetWidth(), bgBmp.GetHeight());
    }

    struct Btn { wxRect r; const char* p; };
    std::vector<Btn> btns = {
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
        break;
    }

    for (auto& b : btns) {
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
Player* GamePanel::AskBlock(Role blockerRole,
    const wxString& actionName,
    int cost /*=0*/)
{
    Player* current = game.getPlayers()[game.getTurn()];
    for (Player* p : game.getPlayers()) {
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
void GamePanel::OnClick(wxMouseEvent& evt)
{
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
    Player* cur = game.getPlayers()[game.getTurn()];
    Role    role = cur->getRole();

    // try each handler in turn; if one returns true, we’re done
    if (HandleMustCoup(pt, cur))       return;
    if (HandleGather(pt, cur))         return;
    if (HandleTax(pt, cur))            return;
    if (HandleBribe(pt, cur))          return;
    if (HandleAbility(pt, cur, role))  return;
    if (HandleSkip(pt, cur))           return;
    if (HandleTargeted(pt, cur))       return;
}

//------------------------------------------------------------------------------
// 1) Forced-to-coup: only Coup allowed
//------------------------------------------------------------------------------
bool GamePanel::HandleMustCoup(const wxPoint& pt, Player* cur)
{
    // Not forced? reset and bail out.
    if (!game.forcedToCoup(cur)) {
        mustCoupAlerted_ = false;
        return false;
    }

    // 1) If they actually clicked the Coup button, jump straight into the coup flow:
    if (btnCoupRect.Contains(pt)) {
        wxArrayString names;
        auto targets = game.getListOfTargetPlayers(cur);
        for (auto* p : targets) names.Add(p->getName());

        wxSingleChoiceDialog dlg(
            this,
            "Choose a target to coup",
            "Forced Coup",
            names
        );
        if (dlg.ShowModal() == wxID_OK) {
            Player* tgt = targets[dlg.GetSelection()];
            PlaySound(SoundEffect::CoupKick);
            game.coup(cur, tgt);
            game.advanceTurnIfNeeded();
            RefreshUI();
        }
        // clear the alert flag for next time
        mustCoupAlerted_ = false;
        return true;
    }

    // 2) Any other click: show the popup once, then swallow all further clicks until they Coup
    if (!mustCoupAlerted_) {
        wxMessageBox(
            "You have too many coins you must coup!",
            "Forced Coup",
            wxOK | wxICON_INFORMATION,
            this
        );
        mustCoupAlerted_ = true;
    }
    // swallow the click (do nothing else)
    return true;
}




//------------------------------------------------------------------------------
// 2) Gather
//------------------------------------------------------------------------------
bool GamePanel::HandleGather(const wxPoint& pt, Player* cur)
{
    if (!btnGatherRect.Contains(pt)) return false;

    PlaySound(SoundEffect::GatherCoin);
    game.gather(cur);
    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}

//------------------------------------------------------------------------------
// 3) Tax
//------------------------------------------------------------------------------
bool GamePanel::HandleTax(const wxPoint& pt, Player* cur)
{
    if (!btnTaxRect.Contains(pt)) return false;

    if (!AskBlock(Role::Governor, "tax")) {
        PlaySound(SoundEffect::TaxCoin);
        game.tax(cur);
    }
    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}

//------------------------------------------------------------------------------
// 4) Bribe
//------------------------------------------------------------------------------
bool GamePanel::HandleBribe(const wxPoint& pt, Player* cur)
{
    if (!btnBribeRect.Contains(pt)) return false;

    if (!AskBlock(Role::Judge, "bribe")) {
        game.bribe(cur);
    }
    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}

//------------------------------------------------------------------------------
// 5) Ability (Spy/Baron)
//------------------------------------------------------------------------------
bool GamePanel::HandleAbility(const wxPoint& pt, Player* cur, Role role)
{
    if (!btnAbilityRect.Contains(pt)) return false;

    if (role == Role::Baron && cur->getCoins() < 3) {
        wxLogWarning("You need at least 3 coins to cough cough legal investment.");
        return true;    // swallow the click instead of calling useAbility
    }

    switch (role) {
    case Role::Spy: {
        Spy* spy = static_cast<Spy*>(cur);
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
bool GamePanel::HandleSkip(const wxPoint& pt, Player* cur)
{
    if (!btnSkipRect.Contains(pt)) return false;

    game.skipTurn(cur);
    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
}

//------------------------------------------------------------------------------
// 7) Arrest / Sanction / Coup
//------------------------------------------------------------------------------
bool GamePanel::HandleTargeted(const wxPoint& pt, Player* cur)
{
    bool isArrest   = btnArrestRect.Contains(pt);
    bool isSanction = btnSanctionRect.Contains(pt);
    bool isCoup     = btnCoupRect.Contains(pt);
    if (!(isArrest || isSanction || isCoup)) return false;

    wxArrayString names;
    auto targets = game.getListOfTargetPlayers(cur);
    for (auto* p : targets) names.Add(p->getName());

    wxString title = isArrest   ? "Choose target to arrest"
                     : isSanction ? "Choose target to sanction"
                                  : "Choose target to coup";

    wxSingleChoiceDialog dlg(this, title, "Target", names);
    if (dlg.ShowModal() != wxID_OK) return true;
    Player* tgt = targets[dlg.GetSelection()];

    if (isArrest) {
        Player* blocker = AskBlock(Role::Spy, "arrest");
        if (game.handleBlock(blocker, blocker != nullptr, "arrest", 0)) {
            game.advanceTurnIfNeeded();
            RefreshUI();
            return true;
        }
        game.arrest(cur, tgt);
    }
    else if (isSanction) {
        game.sanction(cur, tgt);
    }
    else { // Coup
        Player* blocker = AskBlock(Role::General, "coup", 5);
        if (game.handleBlock(blocker, blocker != nullptr, "coup", 5)) {
            game.advanceTurnIfNeeded();
            RefreshUI();
            return true;
        }
        PlaySound(SoundEffect::CoupKick);
        game.coup(cur, tgt);
    }

    game.advanceTurnIfNeeded();
    RefreshUI();
    return true;
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

void GamePanel::showWinner(const std::string& winner) {
    wxMessageDialog dlg(this, "Winner: " + winner, "Game Over", wxOK | wxICON_INFORMATION);
    PlaySound(SoundEffect::Victory);
    dlg.ShowModal();
    exit(0);
}


void GamePanel::PlaySound(SoundEffect effect) {
    auto it = sounds_.find(effect);
    if (it == sounds_.end() || !it->second.IsOk()) {
        wxLogError("Sound not preloaded or invalid: %d", int(effect));
        return;
    }
    it->second.Play(wxSOUND_ASYNC);
}