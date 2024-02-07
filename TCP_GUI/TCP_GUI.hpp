#pragma once
#include "common.h"
class ConfigurationDialog : public wxDialog {
public:
    ConfigurationDialog(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE)
        : wxDialog(parent, id, title, pos, size, style) {
        // Setup the dialog with controls for IP address and port
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        wxTextCtrl* ipInput = new wxTextCtrl(this, wxID_ANY);
        wxTextCtrl* portInput = new wxTextCtrl(this, wxID_ANY);

        vbox->Add(new wxStaticText(this, wxID_ANY, "IP Address"), 0, wxALL, 5);
        vbox->Add(ipInput, 0, wxEXPAND | wxALL, 5);
        vbox->Add(new wxStaticText(this, wxID_ANY, "Port"), 0, wxALL, 5);
        vbox->Add(portInput, 0, wxEXPAND | wxALL, 5);

        wxButton* okButton = new wxButton(this, wxID_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);
        vbox->Add(okButton, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

        SetSizer(vbox);
    }
};

class TCPSessionPanel : public wxFrame {
private:
    std::map<std::string, int> CONFIGURATION;
public:
    void OnConfigure(wxCommandEvent& event)
    {
        ConfigurationDialog dialog(this, wxID_ANY, "Configure Session");
        if (dialog.ShowModal() == wxID_OK) {
            // Store the configuration
            wxString ipAddress = dialog.GetIPAddress();
            long port = dialog.GetPort();

            // Assuming CONFIGURATION is a std::map<std::string, std::string>
            int IP;
            ipAddress.ToInt(&IP);
            CONFIGURATION["IPAddress"] = IP;
            CONFIGURATION["Port"] = port;
        }
    }
    TCPSessionPanel(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
        // Create a menu bar
        wxMenuBar* menuBar = new wxMenuBar;

        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(wxID_ANY, "Help");
        fileMenu->Append(wxID_ANY, "History");
        fileMenu->Append(wxID_ANY, "Save");
        fileMenu->Append(ID_CONFIGURE, "Configure"); // Add this line

        menuBar->Append(fileMenu, "File");
        SetMenuBar(menuBar);
        Bind(wxEVT_MENU, &TCPSessionPanel::OnConfigure, this, ID_CONFIGURE);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxGridSizer* headerSizer = new wxGridSizer(2, 0, 0, 0);

        headerSizer->Add(new wxStaticText(this, wxID_ANY, "Source Port"), 1);
        headerSizer->Add(new wxStaticText(this, wxID_ANY, "Destination Port"), 1);
        headerSizer->Add(new wxStaticText(this, wxID_ANY, "SEQ"), 1);
        headerSizer->Add(new wxStaticText(this, wxID_ANY, "ACK"), 1);
        headerSizer->Add(new wxStaticText(this, wxID_ANY, "Data Offset"), 1);
        headerSizer->Add(new wxStaticText(this, wxID_ANY, "Flags"), 1); // Placeholder for the label

        wxTextCtrl* SOURCE_PORT_INPUT = new wxTextCtrl(this, ID::SOURCE_PORT_INPUT_ID, "", wxDefaultPosition, wxSize(-1, -1), 0, wxTextValidator(wxFILTER_DIGITS));
        wxTextCtrl* DESTINATION_PORT_INPUT = new wxTextCtrl(this, ID::SOURCE_PORT_INPUT_ID, "", wxDefaultPosition, wxSize(-1, -1), 0, wxTextValidator(wxFILTER_DIGITS));
        wxTextCtrl* SEQ_INPUT = new wxTextCtrl(this, ID::SEQ_INPUT_ID, "", wxDefaultPosition, wxSize(-1, -1), 0, wxTextValidator(wxFILTER_DIGITS));
        wxTextCtrl* ACK_INPUT = new wxTextCtrl(this, ID::ACK_INPUT_ID, "", wxDefaultPosition, wxSize(-1, -1), 0, wxTextValidator(wxFILTER_DIGITS));
        wxTextCtrl* DATA_OFFSET_INPUT = new wxTextCtrl(this, ID::DATA_OFFSET_INPUT_ID, "", wxDefaultPosition, wxSize(-1, -1), 0, wxTextValidator(wxFILTER_DIGITS));
        // Add value fields to the second row
        headerSizer->Add(SOURCE_PORT_INPUT, 1, wxEXPAND);
        headerSizer->Add(DESTINATION_PORT_INPUT, 1, wxEXPAND);
        headerSizer->Add(SEQ_INPUT, 1, wxEXPAND);
        headerSizer->Add(ACK_INPUT, 1, wxEXPAND);
        headerSizer->Add(DATA_OFFSET_INPUT, 1, wxEXPAND);
        wxPanel* flagsPanel = new wxPanel(this);
        wxBoxSizer* flagsSizer = new wxBoxSizer(wxHORIZONTAL);

        // Define the TCP flags
        wxCheckBox* cbURG = new wxCheckBox(flagsPanel, FLAGS_OFFSET_INPUT_ID_URG, "URG");
        wxCheckBox* cbACK = new wxCheckBox(flagsPanel, FLAGS_OFFSET_INPUT_ID_ACK, "ACK");
        wxCheckBox* cbPSH = new wxCheckBox(flagsPanel, FLAGS_OFFSET_INPUT_ID_PSH, "PSH");
        wxCheckBox* cbRST = new wxCheckBox(flagsPanel, FLAGS_OFFSET_INPUT_ID_RST, "RST");
        wxCheckBox* cbSYN = new wxCheckBox(flagsPanel, FLAGS_OFFSET_INPUT_ID_SYN, "SYN");
        wxCheckBox* cbFIN = new wxCheckBox(flagsPanel, FLAGS_OFFSET_INPUT_ID_FIN, "FIN");

        // Add checkboxes to the flags sizer
        flagsSizer->Add(cbURG, 1, wxEXPAND | wxALL, 1);
        flagsSizer->Add(cbACK, 1, wxEXPAND | wxALL, 1);
        flagsSizer->Add(cbPSH, 1, wxEXPAND | wxALL, 1);
        flagsSizer->Add(cbRST, 1, wxEXPAND | wxALL, 1);
        flagsSizer->Add(cbSYN, 1, wxEXPAND | wxALL, 1);
        flagsSizer->Add(cbFIN, 1, wxEXPAND | wxALL, 1);

        flagsPanel->SetSizer(flagsSizer);

        // Add the flags panel to the grid sizer in the appropriate cell
        headerSizer->Add(flagsPanel, 1, wxEXPAND | wxALL, 5); // The panel containing the checkboxes

        // Add the header grid sizer to the main sizer
        mainSizer->Add(headerSizer, 0, wxEXPAND | wxALL, 5);

        // Create a horizontal sizer for the main content area
        wxBoxSizer* contentSizer = new wxBoxSizer(wxHORIZONTAL);

        // Add a text area for data input on the left
        wxTextCtrl* dataInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), wxTE_MULTILINE);
        contentSizer->Add(dataInput, 1, wxEXPAND | wxALL, 5);

        // Add an information panel on the right
        wxPanel* infoPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1));
        wxBoxSizer* infoSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* infoText = new wxStaticText(infoPanel, wxID_ANY, "SEQ & IP port info...\nTHIS IS WHATEVER THIS IS TOO LONG LET'S SEE HOW IT BEHAVES");
        infoSizer->Add(infoText, 1, wxALL, 5);
        infoPanel->SetSizer(infoSizer);
        contentSizer->Add(infoPanel, 0, wxEXPAND | wxALL, 5);

        // Add the main content sizer to the main sizer
        mainSizer->Add(contentSizer, 1, wxEXPAND | wxALL, 5);

        // Add a send button below the content area
        wxButton* sendButton = new wxButton(this, wxID_ANY, "Send");
        mainSizer->Add(sendButton, 0, wxALIGN_LEFT | wxALL, 5);

        // Set the sizer for the panel
        this->SetSizer(mainSizer);
        this->Fit();
        this->Center();
    }
    // Override the close event handler to perform any additional cleanup if needed
    void OnClose(wxCloseEvent& event) {
        this->Destroy(); // Destroy the session panel
    }
};

class MainFrame : public wxFrame {
private:
    TCPSessionPanel* sessionPanel1 = nullptr; // Pointer to the session panel
    TCPSessionPanel* sessionPanel2 = nullptr; // Pointer to the session panel
    wxDECLARE_EVENT_TABLE();
public:
    void OnStartTCPClicked(wxCommandEvent& event) {
        if (!sessionPanel1) {
            // Create the session panel if it doesn't exist
            sessionPanel1 = new TCPSessionPanel("TCP Session 1");
            sessionPanel1->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnSessionPanelClosed, this);
        }
        sessionPanel1->Show();
        if (!sessionPanel2) {
            // Create the session panel if it doesn't exist
            sessionPanel2 = new TCPSessionPanel("TCP Session 2");
            sessionPanel2->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnSessionPanelClosed, this);
        }
        sessionPanel2->Show();
        this->Hide(); // Hide the main frame
    }

    void OnSessionPanelClosed(wxCloseEvent& event) {
        this->Show(); // Show the main frame when the session panel is closed
        sessionPanel1->Destroy(); // Destroy the session panel
        sessionPanel1 = nullptr; // Reset the pointer
        sessionPanel2->Destroy(); // Destroy the session panel
        sessionPanel2 = nullptr; // Reset the pointer
    }

    MainFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

        // Create a menu bar
        wxMenuBar* menuBar = new wxMenuBar;

        // Create a help menu
        wxMenu* helpMenu = new wxMenu;
        helpMenu->Append(wxID_ANY, "Help");

        // Create a history menu
        wxMenu* historyMenu = new wxMenu;
        historyMenu->Append(wxID_ANY, "History");

        // Create a save menu
        wxMenu* saveMenu = new wxMenu;
        saveMenu->Append(wxID_ANY, "Save");

        // Add menus to the menu bar
        menuBar->Append(helpMenu, "Help");
        menuBar->Append(historyMenu, "History");
        menuBar->Append(saveMenu, "Save");

        // Attach the menu bar to the frame
        SetMenuBar(menuBar);

        // Create a panel that will contain all other widgets
        wxPanel* mainPanel = new wxPanel(this);

        // Create a sizer that will lay out widgets horizontally
        wxBoxSizer* hBoxSizer = new wxBoxSizer(wxHORIZONTAL);

        // Create the list control for old commands
        wxListCtrl* listOldCommands = new wxListCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(200, -1),
            wxLC_REPORT);
        listOldCommands->AppendColumn("Session ID");
        listOldCommands->AppendColumn("IP address");
        listOldCommands->AppendColumn("Port");

        // Create the list control for previous data
        wxListCtrl* listPreviousData = new wxListCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(200, -1),
            wxLC_REPORT);
        listPreviousData->AppendColumn("Previous Data");

        // Create a sizer that will lay out widgets vertically on the right side
        wxBoxSizer* vBoxSizer = new wxBoxSizer(wxVERTICAL);

        // Add input field (disabled)
        wxTextCtrl* sessionCount = new wxTextCtrl(mainPanel, wxID_ANY, "2 (for now you could only have two sessions)", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_CENTRE);
        sessionCount->Disable();
        wxFont font = sessionCount->GetFont();
        font.SetPointSize(font.GetPointSize() + 4);
        sessionCount->SetFont(font);
        vBoxSizer->Add(sessionCount, 0, wxEXPAND | wxALL, 5);

        // Add start session button
        wxButton* startSimulation = new wxButton(mainPanel, START_SIMULATION_BUTTON, "Start TCP Simulation");


        vBoxSizer->Add(startSimulation, 0, wxEXPAND | wxALL, 5);
        font = startSimulation->GetFont();
        font.SetPointSize(font.GetPointSize() + 4);
        startSimulation->SetFont(font);
        // Add the vertical sizer to the horizontal sizer
        hBoxSizer->Add(listOldCommands, 1, wxEXPAND | wxALL, 5);
        hBoxSizer->Add(listPreviousData, 1, wxEXPAND | wxALL, 5);
        hBoxSizer->Add(vBoxSizer, 1, wxEXPAND | wxALL, 5);

        // Set the sizer for the panel
        mainPanel->SetSizer(hBoxSizer);

        // Fit the frame to the size of the sizer
        this->SetMinSize(wxSize(900, 500));
        this->Fit();
    }
};

// Define the event table
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(START_SIMULATION_BUTTON, MainFrame::OnStartTCPClicked)
wxEND_EVENT_TABLE()
