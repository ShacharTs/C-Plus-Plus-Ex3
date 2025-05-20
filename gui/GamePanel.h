// File: GamePanel.h
#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/sound.h>
#include <map>
#include <vector>
#include "../game/Game.hpp"

class GamePanel : public wxPanel {
public:
    enum class SoundEffect {
        GatherCoin,
        TaxCoin,
        Victory,
        CoupKick,
        // …add more as needed
    };

    GamePanel(wxFrame* parent, const std::vector<std::string>& names);
    ~GamePanel() = default;

    void RefreshUI();
    Player* AskBlock(Role blockerRole, const wxString& actionName, int cost = 0);

    // Play one of the pre-loaded sounds
    void PlaySound(SoundEffect effect);

private:
    // Core game instance
    coup::Game game;

    // UI elements
    wxBitmap      bgBmp;
    wxFont        customFont_;
    wxSize        dialogSize_{0,0};
    wxPoint       dialogPos_{0,0};
    wxString      dialogTitle_;
    wxRect        btnGatherRect, btnTaxRect, btnBribeRect,
                  btnAbilityRect, btnArrestRect,
                  btnSanctionRect, btnCoupRect, btnSkipRect;

    // Pre-loaded sounds map
    std::map<SoundEffect, wxSound> sounds_;
    bool mustCoupAlerted_{false};
    // Helpers
    void UpdateRoleWindow();
    void InitializeButtons();

    // Event handlers
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnClick(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void showWinner(const std::string& winner);

    bool HandleMustCoup(const wxPoint& pt, Player* cur);
    bool HandleGather(const wxPoint& pt, Player* cur);
    bool HandleTax(const wxPoint& pt, Player* cur);
    bool HandleBribe(const wxPoint& pt, Player* cur);
    bool HandleAbility(const wxPoint& pt, Player* cur, Role role);
    bool HandleSkip(const wxPoint& pt, Player* cur);
    bool HandleTargeted(const wxPoint& pt, Player* cur);

    // Convert enum → filename
    static wxString EffectToFilename(SoundEffect effect) {
        switch (effect) {
            case SoundEffect::GatherCoin: return "gather_coin.wav";
            case SoundEffect::TaxCoin:    return "tax_coin.wav";
            case SoundEffect::Victory:    return "player_winner.wav";
            case SoundEffect::CoupKick:   return "coup_kick.wav";
            default:                      return "";
        }
    }

    wxDECLARE_EVENT_TABLE();
};
