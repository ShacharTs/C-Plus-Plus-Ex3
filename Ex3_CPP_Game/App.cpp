#include "App.h"
#include "MenuFrame.h"

bool App::OnInit() {
    wxInitAllImageHandlers();
    SetExitOnFrameDelete(true);
    new MenuFrame();
    return true;
}

wxIMPLEMENT_APP(App);