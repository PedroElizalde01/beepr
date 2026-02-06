/*
beepr.ino - Minimal ESP32 ANCS notification bridge (BLE only)

Folder structure (Arduino sketchbook/project root):
  /home/pedro/beepr/
    beepr.ino
    libraries/
      ESP32-ANCS-Notifications/   (local fork; can be a symlink to ../ESP32-ANCS-Notifications)

iPhone pairing steps:
1) Boot ESP32 with GPIO33 bridged to GND (PAIRING MODE).
2) iPhone: Settings → Bluetooth → find device name → tap to pair.
3) Accept pairing and accept the "Allow Notifications" prompt.
4) Reboot ESP32 without the bridge (NORMAL MODE).

Troubleshooting:
- If device doesn’t appear in iOS Settings: iOS often doesn’t list generic BLE devices; still proceed with
  pairing attempts. If needed, "Forget" the device in iOS, toggle Bluetooth off/on, and reboot the ESP32.
- If connected but no notifications: verify the permission prompt was accepted; test with a new notification.
*/

#include <Arduino.h>
#include "esp32notifications.h"
#include "esp_gap_ble_api.h"
#include "knownApps.h"
#include <Wire.h>
#include <U8g2lib.h>

// Pairing button to GND (GPIO33).
static const int PAIRING_PIN = 33;
static const char *DEVICE_NAME = "BEEPR";
static const uint32_t KEEPALIVE_MS = 20000;
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

BLENotifications notifications;
static bool ancsReadyLogged = false;
static uint32_t lastKeepAliveMs = 0;

static void logAdvertisingStarted()
{
    Serial.println("Advertising started");
}

static void clearAllBonds()
{
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num <= 0)
    {
        Serial.println("No stored bonds to clear");
        return;
    }

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!dev_list)
    {
        Serial.println("Failed to allocate bond list");
        return;
    }

    int dev_num_copy = dev_num;
    esp_ble_get_bond_device_list(&dev_num_copy, dev_list);

    for (int i = 0; i < dev_num_copy; i++)
    {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
    Serial.printf("Cleared %d stored bond(s)\n", dev_num_copy);
}

static void gapCallback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    if (event == ESP_GAP_BLE_AUTH_CMPL_EVT)
    {
        if (param->ble_security.auth_cmpl.success)
        {
            Serial.println("Bonded/Encrypted");
        }
        else
        {
            Serial.println("Bonding failed");
        }
    }
}

static void onBLEStateChanged(BLENotifications::State state)
{
    switch (state)
    {
    case BLENotifications::StateConnected:
        Serial.println("Connected");
        Serial.println("ANCS client starting (subscribing)");
        break;
    case BLENotifications::StateDisconnected:
        Serial.println("Disconnected");
        notifications.startAdvertising();
        logAdvertisingStarted();
        break;
    }
}

static void printNotificationCommon(const ArduinoNotification *notification)
{
    Serial.println("Notification received");
    if (notification->type.length())
    {
        String appName = getAppName(notification->type);
        Serial.printf("App: %s\n", appName.c_str());
    }
    else
    {
        Serial.println("App: (unknown)");
    }
    Serial.printf("Title: %s\n", notification->title.length() ? notification->title.c_str() : "(none)");
    Serial.printf("Message: %s\n", notification->message.length() ? notification->message.c_str() : "(none)");

    if (notification->time != 0)
    {
        Serial.printf("Date: %lu\n", (unsigned long)notification->time);
    }
    else
    {
        Serial.println("Date: (not provided)");
    }

    Serial.printf("Category: %s\n", notifications.getNotificationCategoryDescription(notification->category));
    Serial.printf("CategoryCount: %u\n", notification->categoryCount);
    Serial.printf("UUID: %lu\n", (unsigned long)notification->uuid);
}

static void oledShowStatus(const String &line1, const String &line2)
{
    oled.clearBuffer();
    oled.setFont(u8g2_font_6x12_tr);
    oled.drawStr(2, 12, line1.c_str());
    oled.drawStr(2, 28, line2.c_str());
    oled.setDrawColor(0);
    oled.drawBox(0, 0, 2, 64);
    oled.setDrawColor(1);
    oled.sendBuffer();
}

static void oledShowNotification(const String &appName, const String &contact, const String &message)
{
    oled.clearBuffer();
    oled.setFont(u8g2_font_6x12_tr);
    oled.drawStr(2, 12, appName.c_str());
    oled.drawStr(2, 28, contact.c_str());
    if (message.length())
    {
        oled.drawStr(2, 44, message.c_str());
    }
    oled.setDrawColor(0);
    oled.drawBox(0, 0, 2, 64);
    oled.setDrawColor(1);
    oled.sendBuffer();
}

static void onNotificationArrived(const ArduinoNotification *notification, const Notification *rawNotificationData)
{
    (void)rawNotificationData;

    if (!ancsReadyLogged)
    {
        Serial.println("ANCS ready/subscribed");
        ancsReadyLogged = true;
    }

    // Filter empty placeholders.
    if (notification->uuid == 0 && notification->type.length() == 0 &&
        notification->title.length() == 0 && notification->message.length() == 0)
    {
        return;
    }

    printNotificationCommon(notification);

    String appName = notification->type.length() ? getAppName(notification->type) : String("(unknown)");
    String contact = notification->title.length() ? notification->title : String("(none)");
    String message = notification->message.length() ? notification->message : String("");
    oledShowNotification(appName, contact, message);
}

static void onNotificationRemoved(const ArduinoNotification *notification, const Notification *rawNotificationData)
{
    (void)rawNotificationData;
    if (notification->uuid == 0 && notification->type.length() == 0 &&
        notification->title.length() == 0 && notification->message.length() == 0)
    {
        return;
    }
    Serial.println("Notification removed");
    printNotificationCommon(notification);
}

void setup()
{
    pinMode(PAIRING_PIN, INPUT_PULLUP);
    bool pairingMode = (digitalRead(PAIRING_PIN) == LOW);

    Serial.begin(115200);
    delay(200);

    Wire.begin(21, 22);
    oled.begin();
    oledShowStatus("Beeper", "Starting...");

    if (pairingMode)
    {
        Serial.println("PAIRING MODE (GPIO33=LOW)");
        oledShowStatus("PAIRING MODE", "GPIO33=LOW");
    }
    else
    {
        Serial.println("NORMAL MODE (GPIO33=HIGH)");
        oledShowStatus("NORMAL MODE", "GPIO33=HIGH");
    }

    bool ok = notifications.begin(DEVICE_NAME);
    if (ok)
    {
        Serial.println("BLE init OK");
    }
    else
    {
        Serial.println("BLE init FAILED");
    }

    notifications.setConnectionStateChangedCallback(onBLEStateChanged);
    notifications.setNotificationCallback(onNotificationArrived);
    notifications.setRemovedCallback(onNotificationRemoved);

    esp_ble_gap_register_callback(gapCallback);

    if (pairingMode)
    {
        clearAllBonds();
        notifications.startAdvertising();
        logAdvertisingStarted();
    }
    else
    {
        logAdvertisingStarted();
    }
}

void loop()
{
    // Keep the BLE link active without changing ANCS behavior.
    uint32_t now = millis();
    if (now - lastKeepAliveMs >= KEEPALIVE_MS)
    {
        notifications.keepAlive();
        lastKeepAliveMs = now;
    }
    delay(250);
}
