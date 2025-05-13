#include "RevealDialog.h"
#include <wx/statbmp.h>

RevealDialog::RevealDialog(wxWindow* parent,
    const wxString& playerName,
    const wxString& roleName,
    const wxString& imagePath)
    : wxDialog(parent, wxID_ANY, "Your Role", wxDefaultPosition)
{
    SetBackgroundColour(*wxWHITE);
    wxBoxSizer* vs = new wxBoxSizer(wxVERTICAL);

    vs->Add(new wxStaticText(this, wxID_ANY, "Player: " + playerName),
        0, wxALIGN_CENTER | wxALL, 8);
    vs->Add(new wxStaticText(this, wxID_ANY, "Role: " + roleName),
        0, wxALIGN_CENTER | wxALL, 8);

    wxBitmap bmp(imagePath, wxBITMAP_TYPE_PNG);
    vs->Add(new wxStaticBitmap(this, wxID_ANY, bmp),
        0, wxALIGN_CENTER | wxALL, 8);

    vs->Add(new wxButton(this, wxID_OK, "OK"),
        0, wxALIGN_CENTER | wxALL, 8);

    SetSizerAndFit(vs);
    CentreOnParent();
}