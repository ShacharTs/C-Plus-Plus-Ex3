#include "MenuFrame.h"
#include "MenuPanel.h"

MenuFrame::MenuFrame()
    : wxFrame(nullptr, wxID_ANY, "Coup", wxDefaultPosition, wxSize(1024, 576),
        wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    new MenuPanel(this);
    Centre();
    Show();
}