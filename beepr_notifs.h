#ifndef BEEPR_NOTIFS_H
#define BEEPR_NOTIFS_H

#include <Arduino.h>

namespace BeeprNotifs
{
    void add(const String &app, const String &contact, const String &message, uint32_t uuid);
    void removeCurrent();
    void next();
    void showCurrent();
}

#endif
