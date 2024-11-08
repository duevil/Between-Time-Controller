#include "timecode_display.h"
#include <TM1637Display.h>

using namespace timecode_display;


static inline constexpr auto BLINK_INTERVAL = 330;
static inline constexpr auto BLINK_TASK_STACK_SIZE = 2 << 11;
static inline constexpr auto BRIGHTNESS = 7;

static TM1637Display display{PIN_CLK, PIN_DIO};
static StaticEventGroup_t eventGroupBuffer;
static EventGroupHandle_t eventGroup;
static StaticTask_t task;
static StackType_t taskStack[BLINK_TASK_STACK_SIZE]{};
static uint16_t lastTimecode;

[[noreturn]] static void blinkLoop(auto *);


void timecode_display::setup() {
    log_d("Timecode display setup");
    eventGroup = xEventGroupCreateStatic(&eventGroupBuffer);
    xTaskCreateStatic(blinkLoop,
                      "BlinkLoop",
                      BLINK_TASK_STACK_SIZE,
                      nullptr,
                      1,
                      taskStack,
                      &task);
    display.setBrightness(BRIGHTNESS);
    display.clear();
}

void timecode_display::set(const uint16_t timecode) {
    lastTimecode = timecode;
    display.showNumberHexEx(timecode, 0, true);
    log_d("Timecode display show number hex: 0x%04X", timecode);
}

void timecode_display::toggleBlinking(const bool enable) {
    if (enable) {
        log_d("Enabling blinking");
        xEventGroupSetBits(eventGroup, BIT0);
    } else {
        log_d("Disabling blinking");
        xEventGroupClearBits(eventGroup, BIT0);
        display.setBrightness(BRIGHTNESS);
        display.showNumberHexEx(lastTimecode, 0, true);
    }
}

bool timecode_display::isBlinking() { return xEventGroupGetBits(eventGroup); }


[[noreturn]] static void blinkLoop(auto *) {
    auto on = true;
    for (;;) {
        if (!xEventGroupWaitBits(eventGroup, BIT0, pdFALSE, pdFALSE, portMAX_DELAY)) {
            on = false;
            continue;
        }
        on = !on;
        display.setBrightness(on ? BRIGHTNESS : 0);
        display.showNumberHexEx(lastTimecode, 0, true);
        vTaskDelay(pdMS_TO_TICKS(BLINK_INTERVAL));
    }
}
