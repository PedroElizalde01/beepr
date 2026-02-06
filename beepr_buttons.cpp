#include "beepr_buttons.h"
#include "beepr_config.h"
#include "beepr_notifs.h"

#include <Arduino.h>

static int lastNextState = HIGH;
static int lastClearState = HIGH;
static uint32_t lastNextMs = 0;
static uint32_t lastClearMs = 0;

void BeeprButtons::begin()
{
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_CLEAR_PIN, INPUT_PULLUP);
}

void BeeprButtons::update()
{
    uint32_t now = millis();

    // Next button: one press -> next notification.
    int nextState = digitalRead(BTN_NEXT_PIN);
    if (nextState == LOW && lastNextState == HIGH && (now - lastNextMs) > BTN_DEBOUNCE_MS)
    {
        BeeprNotifs::next();
        lastNextMs = now;
    }
    lastNextState = nextState;

    // Clear button: one press -> remove current notification.
    int clearState = digitalRead(BTN_CLEAR_PIN);
    if (clearState == LOW && lastClearState == HIGH && (now - lastClearMs) > BTN_DEBOUNCE_MS)
    {
        BeeprNotifs::removeCurrent();
        lastClearMs = now;
    }
    lastClearState = clearState;
}
