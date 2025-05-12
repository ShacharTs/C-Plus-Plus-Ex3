#pragma once
#include <wx/wx.h>
#include <wx/graphics.h>

class MenuPanel : public wxPanel {
public:
    MenuPanel(wxFrame* parent);

private:
    wxBitmap bgBmp, btnBmp;
    wxRect   btnRect;

    void OnPaint(wxPaintEvent& e);
    void OnClick(wxMouseEvent& e);
    void OnMotion(wxMouseEvent& e);

    wxDECLARE_EVENT_TABLE();
};