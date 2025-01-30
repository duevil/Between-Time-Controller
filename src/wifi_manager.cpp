#include "wifi_manager.h"
#ifdef WOKWI
#include "mqtt_params.h"
#include <WiFi.h>

inline constexpr auto WIFI_SSID = "Wokwi-GUEST";
inline constexpr auto WIFI_PASS = "";
#else
#include <WiFiManager.h>

static WiFiManager wiFiManager;
static WiFiManagerParameter wm_mqtt_host("server", "MQTT Server", "", 40);
static StaticTask_t task;
static StackType_t stack[8192];
#endif


[[noreturn]] static void task_func(auto *) {
    while (true) {
        wiFiManager.process();
        delay(1); // prevent watchdog reset
    }
}


bool wifi_manager::setup(const char *mqttServer, const std::function<void(const char *)> &onMqttServerSet) {
#ifdef WOKWI
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        log_e("WiFi Failed!");
        while (!WiFi.isConnected()) delay(1000);
    } else {
        log_i("Connected to %s at %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }
    onMqttServerSet(MQTT_SERVER);
    (void) mqttServer; // suppress unused parameter warning
    return WiFi.isConnected();
#else
    wm_mqtt_host.setValue(mqttServer, 40);
    wiFiManager.setHostname("BetweenTime Controller");
    wiFiManager.setTitle("Config Portal");
    const char *menu[]{"wifi", "info", "param", "close"};
    wiFiManager.setMenu(menu, std::size(menu));
    wiFiManager.setDarkMode(true);
    wiFiManager.addParameter(&wm_mqtt_host);
    wiFiManager.setConfigPortalBlocking(true); // wait for credentials input
    wiFiManager.setSaveParamsCallback([onMqttServerSet] { onMqttServerSet(wm_mqtt_host.getValue()); });
    log_d("Starting WiFiManager, waiting for connection...");
    auto configured = wiFiManager.autoConnect("BetweenTime Controller");
    // Check if configured
    if (configured) {
        log_i("Device is configured");
    } else {
        log_i("Device is not configured");
    }
    wiFiManager.startWebPortal(); // start web portal to run in the background
    // we'll use a new task ("Thread") to process WiFiManager in the background
    xTaskCreateStaticPinnedToCore(task_func,
                                  "WiFiManager",
                                  std::size(stack),
                                  nullptr,
                                  configMAX_PRIORITIES - 1,
                                  stack,
                                  &task,
                                  0);
    return configured;
#endif
}

String wifi_manager::ip() { return WiFi.localIP().toString(); }
