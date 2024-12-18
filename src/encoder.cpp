#include "encoder.h"
#include <Bounce2.h>
#include <ESP32Encoder.h>

static IRAM_ATTR void enc_itr_cb(auto *);
static ESP32Encoder enc{true, enc_itr_cb};
static Bounce2::Button but{};
static encoder::EncoderCallback callback;
static volatile bool rotated = false;
static int64_t encLast = INT64_MAX;


void encoder::setup() {
    log_d("Encoder setup");
#ifdef WOKWI
    ESP32Encoder::useInternalWeakPullResistors = puType::up;
    but.attach(PIN_SW, INPUT_PULLUP);
    but.setPressedState(HIGH);
#else
    ESP32Encoder::useInternalWeakPullResistors = puType::none;
    but.attach(PIN_SW, INPUT);
    but.setPressedState(LOW);
#endif
    but.interval(5);
    enc.attachSingleEdge(PIN_DT, PIN_CLK);
    enc.clearCount();
    enc.setFilter(1023);
}

void encoder::setCallback(const EncoderCallback &cb) { callback = cb; }
int64_t encoder::get() { return enc.getCount(); }

void encoder::set(int64_t value) {
    log_d("Encoder set: %lld", value);
    enc.setCount(value);
}

void encoder::loop() {
    using enum Event;
    but.update();
    if (but.pressed()) {
        log_d("Encoder button pressed");
        callback(PRESS);
    }
    if (rotated) {
        if (encLast < enc.getCount()) {
            log_d("Encoder rotated clockwise: %lli", enc.getCount());
            callback(ROTATE_CW);
        } else {
            log_d("Encoder rotated counterclockwise: %lli", enc.getCount());
            callback(ROTATE_CCW);
        }
        encLast = enc.getCount();
        rotated = false;
    }
    // reset system when button is held for 10 seconds
    if (but.isPressed() && but.currentDuration() > 10000) {
        log_w("Encoder button long pressed triggered, resetting system");
        delay(100);
        ESP.restart();
    }
}


static IRAM_ATTR void enc_itr_cb(auto *) { rotated = true; }
