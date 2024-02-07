#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
#include <wx/modalhook.h>
#include <map>
#include "type_def.hpp"
typedef enum wxIDs {
    // INPUT IDs
    SOURCE_PORT_INPUT_ID = wxID_HIGHEST + 1,
    SOURCE_ADDRESS_INPUT,
    TARGET_IP_ADDRESS_INPUT,
    DESTINATION_PORT_INPUT_ID,
    SEQ_INPUT_ID,
    ACK_INPUT_ID,
    DATA_OFFSET_INPUT_ID,
    FLAGS_OFFSET_INPUT_ID_URG,
    FLAGS_OFFSET_INPUT_ID_ACK,
    FLAGS_OFFSET_INPUT_ID_PSH,
    FLAGS_OFFSET_INPUT_ID_RST,
    FLAGS_OFFSET_INPUT_ID_SYN,
    FLAGS_OFFSET_INPUT_ID_FIN,

    // BUTTONS IDs
    START_SIMULATION_BUTTON,

    // HELPERS
    ID_CONFIGURE,
} ID;