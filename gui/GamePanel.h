// File: GamePanel.h
#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
//#include <wx/sound.h>
#include <map>
#include <vector>
#include "../game/Game.hpp"

class GamePanel : public wxPanel {
public:
    //GamePanel(wxWindow *parent, const std::vector<std::string> &names);
    GamePanel(wxWindow* parent, const std::vector<std::string>& names, bool useRandomRoles);


    ~GamePanel() = default;

    coup::Game* getGame();
    void RefreshUI();

    Player *AskBlock(Role blockerRole, const wxString &actionName, int cost = 0);

private:
    coup::Game game;
    // UI elements
    wxBitmap bgBmp;
    wxFont customFont_;
    wxSize dialogSize_{0, 0};
    wxPoint dialogPos_{0, 0};
    wxString dialogTitle_;
    wxRect btnGatherRect, btnTaxRect, btnBribeRect,
            btnAbilityRect, btnArrestRect,
            btnSanctionRect, btnCoupRect, btnSkipRect;

    // Pre-loaded sounds map
    bool mustCoupAlerted_{false};

    // Helpers
    void UpdateRoleWindow();

    void InitializeButtons();

    // Event handlers
    void OnPaint(wxPaintEvent &evt);

    void OnEraseBackground(wxEraseEvent &evt);

    void OnClick(wxMouseEvent &evt);

    void OnMotion(wxMouseEvent &evt);

    void showWinner(const std::string &winner);

    bool HandleMustCoup(const wxPoint &pt, Player *cur);

    bool HandleGather(const wxPoint &pt, Player *cur);

    bool HandleTax(const wxPoint &pt, Player *cur);

    bool HandleBribe(const wxPoint &pt, Player *cur);

    bool HandleAbility(const wxPoint &pt, Player *cur, Role role);

    bool HandleSkip(const wxPoint &pt, Player *cur);

    bool HandleArrest(const wxPoint &pt, Player *cur);
    bool HandleSanction(const wxPoint &pt, Player *cur);
    bool HandleCoup(const wxPoint &pt, Player *cur);

    bool HandleTargeted(const wxPoint &pt, Player *cur);


public:
    Player* getCurrentPlayer() {
        return game.getPlayers()[game.getTurn()];
    }
    wxDECLARE_EVENT_TABLE();
};
