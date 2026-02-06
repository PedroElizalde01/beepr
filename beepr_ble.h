#ifndef BEEPR_BLE_H
#define BEEPR_BLE_H

#include <Arduino.h>
#include "esp32notifications.h"

extern BLENotifications notifications;

namespace BeeprBle
{
    void begin(bool pairingMode);
    void update();
}

#endif
