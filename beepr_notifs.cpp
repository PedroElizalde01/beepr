#include "beepr_notifs.h"
#include "beepr_display.h"

struct StoredNotification
{
    String app;
    String contact;
    String message;
    uint32_t uuid;
};

static const size_t MAX_NOTIFS = 10;
static StoredNotification notifBuf[MAX_NOTIFS];
static size_t notifCount = 0;
static size_t notifHead = 0;   // index of oldest
static size_t currentIndex = 0; // offset from head

static void displayCurrent()
{
    if (notifCount == 0)
    {
        BeeprDisplay::showEmpty();
        return;
    }
    size_t idx = (notifHead + currentIndex) % MAX_NOTIFS;
    BeeprDisplay::showNotification(notifBuf[idx].app, notifBuf[idx].contact, notifBuf[idx].message);
}

void BeeprNotifs::showCurrent()
{
    displayCurrent();
}

void BeeprNotifs::add(const String &app, const String &contact, const String &message, uint32_t uuid)
{
    size_t idx;
    if (notifCount < MAX_NOTIFS)
    {
        idx = (notifHead + notifCount) % MAX_NOTIFS;
        notifCount++;
    }
    else
    {
        // Drop the oldest, append new at end.
        notifHead = (notifHead + 1) % MAX_NOTIFS;
        idx = (notifHead + notifCount - 1) % MAX_NOTIFS;
    }

    notifBuf[idx] = {app, contact, message, uuid};
    currentIndex = notifCount - 1;
    displayCurrent();
}

void BeeprNotifs::removeCurrent()
{
    if (notifCount == 0)
    {
        return;
    }

    // Shift items down to fill the removed slot.
    for (size_t i = currentIndex; i + 1 < notifCount; ++i)
    {
        size_t fromIdx = (notifHead + i + 1) % MAX_NOTIFS;
        size_t toIdx = (notifHead + i) % MAX_NOTIFS;
        notifBuf[toIdx] = notifBuf[fromIdx];
    }

    notifCount--;
    if (notifCount == 0)
    {
        currentIndex = 0;
    }
    else if (currentIndex >= notifCount)
    {
        currentIndex = 0;
    }
    displayCurrent();
}

void BeeprNotifs::next()
{
    if (notifCount == 0)
    {
        return;
    }
    currentIndex = (currentIndex + 1) % notifCount;
    displayCurrent();
}
