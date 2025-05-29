#include "App.h"
#include "MenuFrame.h"

bool App::OnInit() {
    wxInitAllImageHandlers();
    SetExitOnFrameDelete(true);
    MenuFrame* frame = new MenuFrame();
    frame->Show();

    return true;
}

wxIMPLEMENT_APP(App);