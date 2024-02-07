#pragma once
#include "common.h"
#include "TCP_GUI.hpp"
class TcpApp : public wxApp {
public:
    virtual bool OnInit() {
        MainFrame* mainFrame = new MainFrame("TCP Session");
        mainFrame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(TcpApp);