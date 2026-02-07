#include <Arduino.h>
#include "beepr_config.h"
#include "beepr_display.h"
#include "beepr_buttons.h"
#include "beepr_notifs.h"
#include "beepr_ble.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

BLENotifications notifications;
static TaskHandle_t buttonTaskHandle = nullptr;
static TaskHandle_t bleTaskHandle = nullptr;

static void buttonTask(void *param)
{
    (void)param;
    for (;;)
    {
        BeeprButtons::update();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

static void bleTask(void *param)
{
    (void)param;
    for (;;)
    {
        BeeprBle::update();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

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
        BeeprDisplay::showStatus("PAIRING", "MODE");
    }
    else
    {
        Serial.println("NORMAL MODE");
        BeeprDisplay::showStatus("NORMAL", "MODE");
    }

    BeeprBle::begin(pairingMode);

    xTaskCreatePinnedToCore(buttonTask, "beepr_buttons", 4096, nullptr, 3, &buttonTaskHandle, 1);
    xTaskCreatePinnedToCore(bleTask, "beepr_ble", 6144, nullptr, 2, &bleTaskHandle, 0);
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
