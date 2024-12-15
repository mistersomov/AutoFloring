#ifndef IRRIGATION_EVENT_HPP
#define IRRIGATION_EVENT_HPP

#include "esp_event.h"
#include "esp_event_cxx.hpp"

ESP_EVENT_DECLARE_BASE(IRRIGATION_EVENT_BASE);

namespace autflr {
    constexpr uint16_t EVENT_ID_SYNC_TIME = 0;
    constexpr uint16_t EVENT_ID_IRRIGATE = 1;
    constexpr uint16_t EVENT_ID_SETTINGS = 2;

    inline const idf::event::ESPEvent SYNC_TIME(IRRIGATION_EVENT_BASE, idf::event::ESPEventID(EVENT_ID_SYNC_TIME));
    inline const idf::event::ESPEvent IRRIGATE(IRRIGATION_EVENT_BASE, idf::event::ESPEventID(EVENT_ID_IRRIGATE));
    inline const idf::event::ESPEvent SETTINGS(IRRIGATION_EVENT_BASE, idf::event::ESPEventID(EVENT_ID_SETTINGS));

}

#endif