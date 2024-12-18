#include "input.h"
#include <Bounce2.h>

using namespace input;

static std::unordered_map<ColorInput::Position, Bounce> bounces{};
static ColorInput::Input value{};
static Callback changeCallback{};
static bool tickEnabled = false;
static StaticTask_t taskBuffer;
static StackType_t stack[4096];
static StaticSemaphore_t semaphoreBuffer;
static SemaphoreHandle_t semaphore;


[[noreturn]] static void task(auto *);


void input::setup() {
    log_d("Input setup");
    for (auto i : ColorInput::POSITIONS) {
        bounces[i] = Bounce{};
        bounces[i].attach(PINS(i), INPUT_PULLUP);
        bounces[i].interval(5);
    }

    semaphore = xSemaphoreCreateBinaryStatic(&semaphoreBuffer);
    xTaskCreateStatic(task, "input", std::size(stack), nullptr, 1, stack, &taskBuffer);
}

void input::loop() {
    xSemaphoreTake(semaphore, 0);
    auto newValue = value;
    for (auto i : ColorInput::POSITIONS) {
        bounces[i].update();
        newValue[i] = !bounces[i].read();
    }
    if (newValue != value) {
        value = newValue;
        log_i("Input change: %s", value.to_string().c_str());
        if (changeCallback) changeCallback(value);
    }
    xSemaphoreGive(semaphore);
}

void input::setCallback(Callback callback) { changeCallback = std::move(callback); }

const ColorInput::Input &input::get() { return value; }

void input::setTickEnabled(bool enabled) { tickEnabled = enabled; }


[[noreturn]] static void task(auto *) {
    while (true) {
        xSemaphoreTake(semaphore, pdMS_TO_TICKS(1000));
        if (!(tickEnabled && changeCallback && value.any())) continue;
        vTaskDelay(pdMS_TO_TICKS(1000));
        changeCallback(value);
    }
}
