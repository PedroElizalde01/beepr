#ifndef BEEPR_NOTIFS_H
#define BEEPR_NOTIFS_H

#include <Arduino.h>

namespace BeeprNotifs
{
    void add(const String &app, const String &contact, const String &message, uint32_t uuid);
    void removeCurrent();
    bool removeAt(size_t index);
    int findIndexByUid(uint32_t uid);
    bool removeByUid(uint32_t uid);
    void next();
    void showCurrent();
}

#endif
