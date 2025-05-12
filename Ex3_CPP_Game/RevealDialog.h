#pragma once
#include <wx/wx.h>

// A modal dialog that reveals a single player's role
class RevealDialog : public wxDialog {
public:
    RevealDialog(wxWindow* parent,
        const wxString& playerName,
        const wxString& roleName,
        const wxString& imagePath);
};