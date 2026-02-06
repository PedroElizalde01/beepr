#ifndef BEEPR_CONFIG_H
#define BEEPR_CONFIG_H

#include <Arduino.h>

// Pairing button to GND (GPIO33).
static const int PAIRING_PIN = 33;
// Notification navigation buttons (wired to GND, INPUT_PULLUP).
static const int BTN_NEXT_PIN = 25;  // Next notification
static const int BTN_CLEAR_PIN = 26; // Clear current notification

static const char *DEVICE_NAME = "BEEPR";

static const uint32_t KEEPALIVE_MS = 20000;
static const uint32_t BTN_DEBOUNCE_MS = 200;

// I2C pins for OLED.
static const int I2C_SDA = 21;
static const int I2C_SCL = 22;

#endif
