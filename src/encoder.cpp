#include "encoder.h"
#include <ESP32Encoder.h>
#include <utility>

static IRAM_ATTR void enc_itr_cb(void *);
static IRAM_ATTR void but_itr_cb();
static void callbackWrapper(void *, uint32_t e_);
static ESP32Encoder enc{true, enc_itr_cb};
static encoder::EncoderCallback callback;


void encoder::setup() {
    ESP32Encoder::useInternalWeakPullResistors = puType::up;
    enc.attachSingleEdge(PIN_CLK, PIN_DT);
    enc.clearCount();
    enc.setFilter(1023);
    pinMode(PIN_SW, INPUT_PULLUP);
    attachInterrupt(PIN_SW, but_itr_cb, FALLING);
}

void encoder::setCallback(const EncoderCallback &cb) { callback = cb; }
int64_t encoder::get() { return enc.getCount(); }
void encoder::set(int64_t value) { enc.setCount(value); }


static IRAM_ATTR void enc_itr_cb(void *) {
    static int64_t last = INT64_MAX;
    auto xHigherPriorityTaskWoken = pdFALSE;
    auto e = last < enc.getCount()
                 ? encoder::Event::ROTATE_LEFT
                 : encoder::Event::ROTATE_RIGHT;
    last = enc.getCount();
    xTimerPendFunctionCallFromISR(callbackWrapper,
                                  nullptr,
                                  static_cast<uint32_t>(std::to_underlying(e)),
                                  &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void IRAM_ATTR but_itr_cb() {
    auto xHigherPriorityTaskWoken = pdFALSE;
    auto e = encoder::Event::PRESS;
    xTimerPendFunctionCallFromISR(callbackWrapper,
                                  nullptr,
                                  static_cast<uint32_t>(std::to_underlying(e)),
                                  &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void callbackWrapper(void *, uint32_t e_) {
    auto e = static_cast<encoder::Event>(e_);
    switch (e) {
            using enum encoder::Event;
        case ROTATE_LEFT:
            log_i("encoder::Event::ROTATE_LEFT");
            break;
        case ROTATE_RIGHT:
            log_i("encoder::Event::ROTATE_RIGHT");
            break;
        case PRESS:
            log_i("encoder::Event::PRESS");
            break;
    }
    callback(e);
}
