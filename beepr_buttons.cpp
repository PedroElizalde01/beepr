#include "beepr_buttons.h"
#include "beepr_config.h"
#include "beepr_notifs.h"

#include <Arduino.h>

struct DebouncedButton
{
    uint8_t pin;
    int lastReadState;
    int stableState;
    uint32_t lastChangeMs;
};

static DebouncedButton nextBtn = {BTN_NEXT_PIN, HIGH, HIGH, 0};
static DebouncedButton clearBtn = {BTN_CLEAR_PIN, HIGH, HIGH, 0};
static volatile bool nextPressPending = false;
static volatile bool clearPressPending = false;
static volatile uint32_t nextLastIsrUs = 0;
static volatile uint32_t clearLastIsrUs = 0;
static const uint32_t BTN_DEBOUNCE_US = BTN_DEBOUNCE_MS * 1000UL;

void IRAM_ATTR onNextButtonIsr()
{
    uint32_t now = micros();
    if ((now - nextLastIsrUs) >= BTN_DEBOUNCE_US)
    {
        nextLastIsrUs = now;
        nextPressPending = true;
    }
}

void IRAM_ATTR onClearButtonIsr()
{
    uint32_t now = micros();
    if ((now - clearLastIsrUs) >= BTN_DEBOUNCE_US)
    {
        clearLastIsrUs = now;
        clearPressPending = true;
    }
}

static bool consumePress(DebouncedButton &btn, uint32_t now)
{
    int raw = digitalRead(btn.pin);
    if (raw != btn.lastReadState)
    {
        btn.lastReadState = raw;
        btn.lastChangeMs = now;
    }

    if ((now - btn.lastChangeMs) < BTN_DEBOUNCE_MS)
    {
        return false;
    }

    if (btn.stableState != btn.lastReadState)
    {
        int prevStable = btn.stableState;
        btn.stableState = btn.lastReadState;
        // Trigger exactly once on stable HIGH -> LOW transition.
        return (prevStable == HIGH && btn.stableState == LOW);
    }

    return false;
}

void BeeprButtons::begin()
{
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_CLEAR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BTN_NEXT_PIN), onNextButtonIsr, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_CLEAR_PIN), onClearButtonIsr, FALLING);

    // Seed debouncer states from actual pin levels.
    nextBtn.lastReadState = digitalRead(nextBtn.pin);
    nextBtn.stableState = nextBtn.lastReadState;
    nextBtn.lastChangeMs = millis();

    clearBtn.lastReadState = digitalRead(clearBtn.pin);
    clearBtn.stableState = clearBtn.lastReadState;
    clearBtn.lastChangeMs = millis();

    Serial.printf("Buttons ready: NEXT=%d CLEAR=%d\n", BTN_NEXT_PIN, BTN_CLEAR_PIN);
    Serial.printf("Button idle states: NEXT=%s CLEAR=%s\n",
                  nextBtn.stableState == LOW ? "LOW" : "HIGH",
                  clearBtn.stableState == LOW ? "LOW" : "HIGH");
}

void BeeprButtons::update()
{
    uint32_t now = millis();

    if (nextPressPending || consumePress(nextBtn, now))
    {
        nextPressPending = false;
        Serial.println("BTN_NEXT pressed");
        BeeprNotifs::next();
    }

    if (clearPressPending || consumePress(clearBtn, now))
    {
        clearPressPending = false;
        Serial.println("BTN_CLEAR pressed");
        BeeprNotifs::removeCurrent();
    }
}
