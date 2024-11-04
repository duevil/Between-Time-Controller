#include "timecode_display.h"
#include <TM1637Display.h>

using namespace timecode_display;


static inline constexpr auto BLINK_INTERVAL = 100;
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
}

void timecode_display::toggleBlinking(const bool enable) {
    if (enable) {
        log_i("Enabling blinking");
        xEventGroupSetBits(eventGroup, BIT0);
    } else {
        log_i("Disabling blinking");
        xEventGroupClearBits(eventGroup, BIT0);
    }
}

bool timecode_display::isBlinking() { return xEventGroupGetBits(eventGroup); }


[[noreturn]] static void blinkLoop(auto *) {
    static bool on = true;
    for (;;) {
        if (!xEventGroupWaitBits(eventGroup, BIT0, pdFALSE, pdFALSE, portMAX_DELAY)) continue;
        on = !on;
        display.setBrightness(BRIGHTNESS, on);
        display.showNumberHexEx(lastTimecode, 0, true);
        vTaskDelay(pdMS_TO_TICKS(BLINK_INTERVAL));
    }
}
