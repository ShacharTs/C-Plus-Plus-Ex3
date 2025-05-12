// File: GamePanel.h
#pragma once
#include <wx/wx.h>
#include <wx/graphics.h>
#include <vector>
#include "game/Game.hpp"  // adjust include path for your Game class

class GamePanel : public wxPanel {
public:
    GamePanel(wxFrame* parent, const std::vector<std::string>& names);
    void RefreshUI();
    //bool AskBlock(Role blockerRole, const wxString& actionName, int cost = 0);
    Player* AskBlock(Role blockerRole, const wxString& actionName, int cost = 0);

private:
   
    // Core game instance
    coup::Game game;
    // Background image bitmap
    wxBitmap bgBmp;
    // Custom font
    wxFont customFont_;
    // Last choice dialog size and position
    wxSize dialogSize_{0,0};  // stores last dialog size (width,height)
    wxPoint dialogPos_{0,0};  // stores last dialog position (x,y)
    wxString dialogTitle_;  // stores last dialog title
    // Button hitboxes
    wxRect btnGatherRect;
    wxRect btnTaxRect;
    wxRect btnBribeRect;
    wxRect btnAbilityRect;
    wxRect btnArrestRect;
    wxRect btnSanctionRect;
    wxRect btnCoupRect;
    wxRect btnSkipRect;

    // Helpers
    void UpdateRoleWindow();
    void InitializeButtons();

    // Event handlers
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnClick(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);

    wxDECLARE_EVENT_TABLE();
};