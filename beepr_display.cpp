#include "beepr_display.h"
#include "beepr_config.h"

#include <Wire.h>
#include <U8g2lib.h>

static U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

void BeeprDisplay::begin()
{
    Wire.begin(I2C_SDA, I2C_SCL);
    oled.begin();
    showStatus("Beeper", "Starting...");
}

void BeeprDisplay::showStatus(const String &line1, const String &line2)
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

void BeeprDisplay::showNotification(const String &appName, const String &contact, const String &message)
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

void BeeprDisplay::showEmpty()
{
    showStatus("No", "Notifications");
}
