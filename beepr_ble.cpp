#include "beepr_ble.h"
#include "beepr_config.h"
#include "beepr_display.h"
#include "beepr_notifs.h"
#include "knownApps.h"

#include "esp_gap_ble_api.h"

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
    BeeprNotifs::add(appName, contact, message, notification->uuid);
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

void BeeprBle::begin(bool pairingMode)
{
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

void BeeprBle::update()
{
    uint32_t now = millis();
    if (now - lastKeepAliveMs >= KEEPALIVE_MS)
    {
        notifications.keepAlive();
        lastKeepAliveMs = now;
    }
}
