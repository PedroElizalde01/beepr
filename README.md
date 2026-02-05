# BEEPR
> A modern BLE notification pager for iOS

BEEPR is a modern take on the classic belt-mounted pager used by office workers and on-call staff.  
Instead of phone calls, BEEPR mirrors **all incoming iOS notifications** using Bluetooth Low Energy (BLE).

BEEPR runs on an ESP32, connects directly to an iPhone via Apple Notification Center Service (ANCS), and exposes notifications in a hardware-friendly way.  
In its current form, notifications are **logged to the Serial Monitor**. No companion iOS app is used.

---

## Overview

BEEPR is built on the Arduino framework and uses a local fork of the `ESP32-ANCS-Notifications` library.

The ESP32 advertises as a BLE peripheral, is paired manually from **iOS → Settings → Bluetooth**, establishes a bonded (encrypted) connection, and subscribes to ANCS.  
Once connected, incoming notifications are received and logged over Serial.

- BLE only (no Wi-Fi)
- No cloud services
- No companion app
- Designed as a foundation for physical pager-style hardware

---

## Concept

BEEPR is inspired by the small pager devices once clipped to a belt or pocket — devices that quietly signaled *who needed your attention* without pulling you into a screen.

Instead of calls, BEEPR mirrors modern smartphone notifications:
- Messages
- Emails
- App alerts
- System notifications

The ESP32 acts as a **notification bridge** between iOS and future hardware outputs (display, LED, vibration, sound), allowing notifications to be surfaced without unlocking a phone.

---

## What Is ANCS?

Apple Notification Center Service (ANCS) is a BLE service provided by iOS.

Once a BLE device is paired and bonded, iOS can deliver notification metadata such as:
- App identifier
- Title
- Message
- Date
- Category

BEEPR subscribes to ANCS and logs the received metadata to the Serial Monitor.

> ANCS exposes metadata only. Notification availability and visible fields are fully controlled by iOS privacy and notification settings.

---

## Hardware Requirements

- ESP32 board (Arduino-compatible)
- USB cable (for flashing and Serial Monitor)
- Momentary switch or temporary jumper (optional, for pairing mode)
- Two jumper wires (GPIO → GND when entering pairing mode)

---

## Software Requirements

- Arduino IDE
- ESP32 Arduino core **2.x or 3.x**
- Local fork of `ESP32-ANCS-Notifications` (included directly in this repository)

---

## Pairing Mode (GPIO Selection)

Pairing mode is selected **at boot time** using a GPIO pin.

- **GPIO33 connected to GND at boot** → Pairing Mode
- **GPIO33 not connected to GND** → Normal Mode

> Pairing mode is evaluated only during boot. Changing the GPIO state while running has no effect.

Pairing mode can also be forced by overriding the `pairingMode` logic directly in the sketch.

---

## iOS Pairing Steps

If the device was previously paired and is not working correctly, first tap  
**Settings → Bluetooth → (BEEPR) → Forget This Device**.

1. Boot the ESP32 **with GPIO33 connected to GND** (Pairing Mode).
2. On the iPhone, open **Settings → Bluetooth**.
3. Tap the device name (default: `beepr-ancs`) to pair.
4. Accept the Bluetooth pairing dialog.
5. Accept the **“Allow Notifications”** prompt.
6. Reboot the ESP32 **without GPIO33 connected** (Normal Mode).

In Normal Mode, BEEPR uses the stored bond and automatically reconnects when the iPhone is in range.

## Expected Serial Output (Example)
```
App: Discord
Title: John
Message: Hello!
Date: 28
Category: social
CategoryCount: 1
UUID: 41
```
---


## Where This Project Is Right Now

BEEPR is currently in its **foundational phase**.

The BLE and ANCS pipeline is working end-to-end:
- ESP32 pairs and bonds with iOS
- ANCS subscription is established
- Incoming notifications are received reliably
- Metadata is logged to the Serial Monitor

All notification handling, parsing, and reconnection logic lives here and is considered the stable core.

## Next Steps

The immediate focus of BEEPR is to harden the pager core before adding hardware complexity:

- Improve resilience to iOS edge cases (missing or delayed ANCS attributes)
- Normalize notification payloads for hardware-friendly output
- Refine pairing and reconnection behavior across ESP32 core versions
- Establish a clean internal API for future output modules
- Reduce power usage while maintaining BLE reliability

Once the notification core is stable and well-defined, physical output layers can be explored on top.
