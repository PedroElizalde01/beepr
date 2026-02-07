#include "beepr_notifs.h"
#include "beepr_display.h"
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct StoredNotification
{
    String app;
    String contact;
    String message;
    uint32_t uid;
};

static std::vector<StoredNotification> notifList;
static size_t currentIndex = 0;
static SemaphoreHandle_t notifMutex = nullptr;
static SemaphoreHandle_t displayMutex = nullptr;
static portMUX_TYPE mutexInitMux = portMUX_INITIALIZER_UNLOCKED;

struct DisplaySnapshot
{
    bool hasNotification;
    String app;
    String contact;
    String message;
    size_t current;
    size_t total;
};

static SemaphoreHandle_t getNotifMutex()
{
    if (notifMutex == nullptr)
    {
        portENTER_CRITICAL(&mutexInitMux);
        if (notifMutex == nullptr)
        {
            notifMutex = xSemaphoreCreateMutex();
        }
        portEXIT_CRITICAL(&mutexInitMux);
    }
    return notifMutex;
}

static SemaphoreHandle_t getDisplayMutex()
{
    if (displayMutex == nullptr)
    {
        portENTER_CRITICAL(&mutexInitMux);
        if (displayMutex == nullptr)
        {
            displayMutex = xSemaphoreCreateMutex();
        }
        portEXIT_CRITICAL(&mutexInitMux);
    }
    return displayMutex;
}

static int findIndexByUidLocked(uint32_t uid)
{
    for (size_t i = 0; i < notifList.size(); ++i)
    {
        if (notifList[i].uid == uid)
        {
            return (int)i;
        }
    }
    return -1;
}

static DisplaySnapshot buildSnapshotLocked()
{
    DisplaySnapshot snapshot = {false, "", "", "", 0, 0};
    const size_t count = notifList.size();
    if (count == 0)
    {
        currentIndex = 0;
        return snapshot;
    }
    if (currentIndex >= count)
    {
        currentIndex = count - 1;
    }
    const StoredNotification &n = notifList[currentIndex];
    snapshot.hasNotification = true;
    snapshot.app = n.app;
    snapshot.contact = n.contact;
    snapshot.message = n.message;
    snapshot.current = currentIndex;
    snapshot.total = count;
    return snapshot;
}

static void renderSnapshot(const DisplaySnapshot &snapshot)
{
    SemaphoreHandle_t d = getDisplayMutex();
    if (!d || xSemaphoreTake(d, portMAX_DELAY) != pdTRUE)
    {
        return;
    }

    if (!snapshot.hasNotification)
    {
        BeeprDisplay::showEmpty();
        xSemaphoreGive(d);
        return;
    }
    BeeprDisplay::showNotification(snapshot.app, snapshot.contact, snapshot.message,
                                   snapshot.current, snapshot.total);
    xSemaphoreGive(d);
}

static bool removeAtLocked(size_t index, size_t *newCount, DisplaySnapshot *snapshotOut)
{
    const size_t count = notifList.size();
    if (count == 0 || index >= count)
    {
        return false;
    }

    if (index < currentIndex)
    {
        currentIndex--;
    }

    notifList.erase(notifList.begin() + static_cast<std::vector<StoredNotification>::difference_type>(index));

    if (newCount)
    {
        *newCount = notifList.size();
    }
    if (snapshotOut)
    {
        *snapshotOut = buildSnapshotLocked();
    }
    return true;
}

void BeeprNotifs::showCurrent()
{
    SemaphoreHandle_t m = getNotifMutex();
    if (!m)
    {
        BeeprDisplay::showEmpty();
        return;
    }

    if (xSemaphoreTake(m, portMAX_DELAY) != pdTRUE)
    {
        BeeprDisplay::showEmpty();
        return;
    }
    DisplaySnapshot snapshot = buildSnapshotLocked();
    xSemaphoreGive(m);
    renderSnapshot(snapshot);
}

void BeeprNotifs::add(const String &app, const String &contact, const String &message, uint32_t uuid)
{
    SemaphoreHandle_t m = getNotifMutex();
    if (!m)
    {
        return;
    }

    if (xSemaphoreTake(m, portMAX_DELAY) != pdTRUE)
    {
        return;
    }

    int existingIndex = findIndexByUidLocked(uuid);
    if (existingIndex >= 0)
    {
        StoredNotification &n = notifList[(size_t)existingIndex];
        n.app = app;
        n.contact = contact;
        n.message = message;
        currentIndex = (size_t)existingIndex;
    }
    else
    {
        notifList.push_back({app, contact, message, uuid});
        currentIndex = notifList.size() - 1;
    }

    size_t count = notifList.size();
    DisplaySnapshot snapshot = buildSnapshotLocked();
    xSemaphoreGive(m);

    Serial.printf("Local notifications: %u\n", (unsigned)count);
    renderSnapshot(snapshot);
}

bool BeeprNotifs::removeAt(size_t index)
{
    SemaphoreHandle_t m = getNotifMutex();
    if (!m)
    {
        return false;
    }

    if (xSemaphoreTake(m, portMAX_DELAY) != pdTRUE)
    {
        return false;
    }

    size_t newCount = 0;
    DisplaySnapshot snapshot = {false, "", "", "", 0, 0};
    bool removed = removeAtLocked(index, &newCount, &snapshot);
    xSemaphoreGive(m);

    if (!removed)
    {
        return false;
    }

    Serial.printf("Local notifications: %u\n", (unsigned)newCount);
    renderSnapshot(snapshot);
    return true;
}

void BeeprNotifs::removeCurrent()
{
    // Remove the currently displayed notification.
    if (removeAt(currentIndex))
    {
        Serial.println("Local notification removed");
    }
    else
    {
        Serial.println("Remove skipped (no notifications)");
    }
}

int BeeprNotifs::findIndexByUid(uint32_t uid)
{
    SemaphoreHandle_t m = getNotifMutex();
    if (!m)
    {
        return -1;
    }

    if (xSemaphoreTake(m, portMAX_DELAY) != pdTRUE)
    {
        return -1;
    }
    int idx = findIndexByUidLocked(uid);
    xSemaphoreGive(m);
    return idx;
}

bool BeeprNotifs::removeByUid(uint32_t uid)
{
    // Used by ANCS "Removed" events to keep local list in sync.
    SemaphoreHandle_t m = getNotifMutex();
    if (!m)
    {
        return false;
    }
    if (xSemaphoreTake(m, portMAX_DELAY) != pdTRUE)
    {
        return false;
    }

    int idx = findIndexByUidLocked(uid);
    if (idx < 0)
    {
        xSemaphoreGive(m);
        return false;
    }

    size_t newCount = 0;
    DisplaySnapshot snapshot = {false, "", "", "", 0, 0};
    bool removed = removeAtLocked((size_t)idx, &newCount, &snapshot);
    xSemaphoreGive(m);

    if (!removed)
    {
        return false;
    }

    Serial.printf("Local notifications: %u\n", (unsigned)newCount);
    renderSnapshot(snapshot);
    return true;
}

void BeeprNotifs::next()
{
    SemaphoreHandle_t m = getNotifMutex();
    if (!m)
    {
        return;
    }
    if (xSemaphoreTake(m, portMAX_DELAY) != pdTRUE)
    {
        return;
    }

    const size_t count = notifList.size();
    if (count == 0)
    {
        xSemaphoreGive(m);
        return;
    }
    currentIndex = (currentIndex + 1) % count;
    DisplaySnapshot snapshot = buildSnapshotLocked();
    xSemaphoreGive(m);
    renderSnapshot(snapshot);
}
