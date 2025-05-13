#include "MenuPanel.h"
#include "MenuFrame.h"
#include "RevealDialog.h"
#include <wx/numdlg.h>
#include <wx/textdlg.h>
#include <wx/dcbuffer.h>   // for wxAutoBufferedPaintDC
#include <wx/graphics.h>   // for wxGraphicsContext
#include "GameFrame.h"

wxBEGIN_EVENT_TABLE(MenuPanel, wxPanel)
EVT_PAINT(MenuPanel::OnPaint)
EVT_LEFT_DOWN(MenuPanel::OnClick)
EVT_MOTION(MenuPanel::OnMotion)
wxEND_EVENT_TABLE()

MenuPanel::MenuPanel(wxFrame* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 576))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    bgBmp = wxBitmap(wxImage("assets/menu/game_menu.png", wxBITMAP_TYPE_PNG));
    btnBmp = wxBitmap(wxImage("assets/buttons/Start_Game_Button.png", wxBITMAP_TYPE_PNG));
    int bx = (1024 - btnBmp.GetWidth()) / 2;
    int by = 576 - btnBmp.GetHeight() - 60;
    btnRect = wxRect(bx, by, btnBmp.GetWidth(), btnBmp.GetHeight());
}

void MenuPanel::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        gc->DrawBitmap(gc->CreateBitmap(bgBmp), 0, 0, bgBmp.GetWidth(), bgBmp.GetHeight());
        gc->DrawBitmap(gc->CreateBitmap(btnBmp), btnRect.x, btnRect.y, btnRect.width, btnRect.height);
        delete gc;
    }
    else {
        dc.DrawBitmap(bgBmp, 0, 0, false);
        dc.DrawBitmap(btnBmp, btnRect.x, btnRect.y, true);
    }
}

void MenuPanel::OnClick(wxMouseEvent& e)
{
    if (!btnRect.Contains(e.GetPosition())) return;

    wxNumberEntryDialog numDlg(this,
        "How many players?", "Players", "Select number of players:", 2, 2, 6);
    if (numDlg.ShowModal() != wxID_OK) return;
    int n = numDlg.GetValue();

    std::vector<std::string> names;
    names.reserve(n);
    for (int i = 1; i <= n; ++i) {
        wxTextEntryDialog nameDlg(this,
            wxString::Format("Enter name for player %d:", i),
            "Player Name", "");
        if (nameDlg.ShowModal() != wxID_OK) return;
        names.push_back(nameDlg.GetValue().ToStdString());
    }

    new GameFrame(names);
    GetParent()->Hide();
    GetParent()->Destroy();
}

void MenuPanel::OnMotion(wxMouseEvent& e)
{
    SetCursor(btnRect.Contains(e.GetPosition()) ? wxCursor(wxCURSOR_HAND) : wxCursor(wxCURSOR_ARROW));
}