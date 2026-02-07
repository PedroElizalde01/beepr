#include "beepr_ble.h"
#include "beepr_config.h"
#include "beepr_display.h"
#include "beepr_notifs.h"
#include "knownApps.h"

#include "esp_gap_ble_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static bool ancsReadyLogged = false;
static uint32_t lastKeepAliveMs = 0;
static QueueHandle_t pendingNotifQueue = nullptr;

enum PendingEventType : uint8_t
{
    PendingEventAdd = 0,
    PendingEventRemove = 1
};

struct PendingNotifEvent
{
    PendingEventType type;
    uint32_t uid;
    NotificationCategory category;
    uint8_t categoryCount;
    uint32_t time;
    char appName[80];
    char title[120];
    char message[200];
};

static void copyStringToBuffer(const String &src, char *dst, size_t dstSize)
{
    if (!dst || dstSize == 0)
    {
        return;
    }
    src.toCharArray(dst, dstSize);
    dst[dstSize - 1] = '\0';
}

static bool enqueuePendingEvent(const PendingNotifEvent &event)
{
    if (!pendingNotifQueue)
    {
        return false;
    }

    if (xQueueSend(pendingNotifQueue, &event, 0) == pdTRUE)
    {
        return true;
    }

    // Queue full: drop oldest and retry, so fresh events keep flowing.
    PendingNotifEvent dropped;
    xQueueReceive(pendingNotifQueue, &dropped, 0);
    return xQueueSend(pendingNotifQueue, &event, 0) == pdTRUE;
}

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

static void printNotificationCommon(const PendingNotifEvent &event)
{
    Serial.println("Notification received");
    if (event.appName[0] != '\0')
    {
        Serial.printf("App: %s\n", event.appName);
    }
    else
    {
        Serial.println("App: (unknown)");
    }
    Serial.printf("Title: %s\n", event.title[0] ? event.title : "(none)");
    Serial.printf("Message: %s\n", event.message[0] ? event.message : "(none)");

    if (event.time != 0)
    {
        Serial.printf("Date: %lu\n", (unsigned long)event.time);
    }
    else
    {
        Serial.println("Date: (not provided)");
    }

    Serial.printf("Category: %s\n", notifications.getNotificationCategoryDescription(event.category));
    Serial.printf("CategoryCount: %u\n", event.categoryCount);
    Serial.printf("UUID: %lu\n", (unsigned long)event.uid);
    Serial.printf("-------------------------------------");
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

    PendingNotifEvent event = {};
    event.type = PendingEventAdd;
    event.uid = notification->uuid;
    event.category = notification->category;
    event.categoryCount = notification->categoryCount;
    event.time = notification->time;

    String appName = notification->type.length() ? getAppName(notification->type) : String("(unknown)");
    String contact = notification->title.length() ? notification->title : String("(none)");
    String message = notification->message.length() ? notification->message : String("");

    copyStringToBuffer(appName, event.appName, sizeof(event.appName));
    copyStringToBuffer(contact, event.title, sizeof(event.title));
    copyStringToBuffer(message, event.message, sizeof(event.message));

    enqueuePendingEvent(event);
}

static void onNotificationRemoved(const ArduinoNotification *notification, const Notification *rawNotificationData)
{
    (void)rawNotificationData;
    if (notification->uuid == 0 && notification->type.length() == 0 &&
        notification->title.length() == 0 && notification->message.length() == 0)
    {
        return;
    }
    PendingNotifEvent event = {};
    event.type = PendingEventRemove;
    event.uid = notification->uuid;
    event.category = notification->category;
    event.categoryCount = notification->categoryCount;
    event.time = notification->time;
    enqueuePendingEvent(event);
}

static void processPendingEvents()
{
    if (!pendingNotifQueue)
    {
        return;
    }

    PendingNotifEvent event = {};
    uint8_t processed = 0;
    // Process one queued event per tick to keep button/UI latency low.
    while (processed < 1 && xQueueReceive(pendingNotifQueue, &event, 0) == pdTRUE)
    {
        if (event.type == PendingEventAdd)
        {
            printNotificationCommon(event);
            BeeprNotifs::add(String(event.appName), String(event.title), String(event.message), event.uid);
        }
        else
        {
            Serial.println("Notification removed");
            Serial.printf("UUID: %lu\n", (unsigned long)event.uid);
            BeeprNotifs::removeByUid(event.uid);
        }
        processed++;
    }
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

    if (!pendingNotifQueue)
    {
        pendingNotifQueue = xQueueCreate(24, sizeof(PendingNotifEvent));
        if (!pendingNotifQueue)
        {
            Serial.println("Failed to create notification queue");
        }
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
        // Always start advertising in normal mode as well.
        notifications.startAdvertising();
        logAdvertisingStarted();
    }
}

void BeeprBle::update()
{
    processPendingEvents();

    uint32_t now = millis();
    if (now - lastKeepAliveMs >= KEEPALIVE_MS)
    {
        notifications.keepAlive();
        lastKeepAliveMs = now;
    }
}
