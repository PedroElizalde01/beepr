#ifndef BEEPR_DISPLAY_H
#define BEEPR_DISPLAY_H

#include <Arduino.h>

namespace BeeprDisplay
{
    void begin();
    void showStatus(const String &line1, const String &line2);
    void showNotification(const String &appName, const String &contact, const String &message,
                          size_t currentIndex, size_t totalCount);
    void showEmpty();
}

#endif
