#include <Arduino.h>
#include "beepr_config.h"
#include "beepr_display.h"
#include "beepr_buttons.h"
#include "beepr_notifs.h"
#include "beepr_ble.h"

BLENotifications notifications;

void setup()
{
    pinMode(PAIRING_PIN, INPUT_PULLUP);
    bool pairingMode = (digitalRead(PAIRING_PIN) == LOW);

    Serial.begin(115200);
    delay(200);

    BeeprDisplay::begin();
    BeeprButtons::begin();

    if (pairingMode)
    {
        Serial.println("PAIRING MODE");
        BeeprDisplay::showStatus("PAIRING MODE");
    }
    else
    {
        Serial.println("NORMAL MODE");
        BeeprDisplay::showStatus("NORMAL MODE");
    }

    BeeprBle::begin(pairingMode);
}

void loop()
{
    BeeprBle::update();
    BeeprButtons::update();
    delay(250);
}
