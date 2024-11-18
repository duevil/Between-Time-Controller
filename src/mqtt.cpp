#include "mqtt.h"
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ranges>
#include <unordered_set>


struct TopicImpl final : mqtt::Topic {
    std::string topic;
    Callback callback;

    explicit TopicImpl(const char *topic, Callback callback);
    ~TopicImpl() override;
    bool subscribe() const override;
    bool publish(const char *message) const override;
};


static std::unordered_set<std::unique_ptr<TopicImpl>> topics{};
static WiFiClient wifiClient{};
static PubSubClient client{wifiClient};
static Preferences prefs{};
static auto clientID = "esp32-mqtt-client";
static String host{};
static std::function<void()> onConnect{};

static void mainCallback(const char *topic, uint8_t *payload, unsigned int length);
static bool connect();
static void logState();


void mqtt::setup() {
    log_d("MQTT setup");
    prefs.begin("mqtt", false);
    host = prefs.getString("host", "");
    client.setServer(host.c_str(), 1883);
    client.setCallback(mainCallback);
}

void mqtt::setClientID(const char *id) {
    clientID = id;
    log_i("MQTT client id set to %s", clientID);
}

void mqtt::setServer(const char *server) {
    host = server;
    if (prefs.getString("host", "") == host) {
        log_d("MQTT server already set to %s", host.c_str());
        return;
    }
    prefs.putString("host", host);
    client.setServer(host.c_str(), 1883);
    log_i("MQTT server set to %s", host.c_str());
}


void mqtt::setOnConnect(std::function<void()> callback) { onConnect = std::move(callback); }


void mqtt::loop() {
    // Check MQTT connection and process messages
    if (!client.connected()) {
        logState();
        // attempt to reconnect every 5 seconds
        log_d("Trying to reconnect to MQTT server %s ...", host.c_str());
        if (connect()) {
            client.loop();
        } else {
            delay(5000);
        }
    } else {
        client.loop();
    }
}


const mqtt::Topic &mqtt::Topic::create(const char *topic, Callback callback) {
    log_d("Creating topic: %s", topic);
    auto [ref, ok] = topics.emplace(std::make_unique<TopicImpl>(topic, std::move(callback)));
    if (!ok) {
        log_w("Topic already exists: %s", topic);
    } else {
        log_i("Created topic: %s", topic);
    }
    return **ref;
}

TopicImpl::TopicImpl(const char *topic, Callback callback): topic{topic}, callback{std::move(callback)} {}

TopicImpl::~TopicImpl() {
    if (auto res = std::erase_if(topics, [this](const auto &t) { return t.get() == this; }); res == 0) {
        log_w("Topic not found in topics list");
    }
}

bool TopicImpl::subscribe() const {
    auto res = client.subscribe(topic.c_str());
    if (res)
        log_i("Subscribed to topic '%s'", topic.c_str());
    else
        log_e("Failed to subscribe to topic '%s'", topic.c_str());
    return res;
}

bool TopicImpl::publish(const char *message) const {
    auto res = client.publish(topic.c_str(), message);
    if (res)
        log_i("Published message on topic '%s': %s", topic.c_str(), message);
    else
        log_e("Failed to publish message on topic '%s': %s", topic.c_str(), message);
    return res;
}


static void mainCallback(const char *topic, uint8_t *payload, unsigned int length) {
    log_d("Received message on topic '%s': %.*s", topic, length, payload);
    for (const auto &t : topics) {
        if (t->topic == topic) {
            t->callback(payload, length);
            return;
        }
    }
    log_w("No callback found for topic '%s'", topic);
}

static bool connect() {
    if (!WiFi.isConnected()) {
        log_w("WiFi not connected");
        return false;
    }
    if (host.length() == 0) {
        log_w("MQTT server not configured");
        return false;
    }
    // wait until connected to MQTT server
    log_d("Connecting to MQTT server %s ...", host.c_str());
    // attempt to connect
    if (client.connect(clientID)) {
        log_i("Connected to MQTT server %s", host.c_str());
        if (onConnect) onConnect();
    } else {
        log_e("Failed to connect to MQTT server, rc=%d. Retrying in 5 seconds", client.state());
    }
    logState();
    return client.connected();
}

static void logState() {
    switch (client.state()) {
        case MQTT_CONNECTION_TIMEOUT:
            log_w("Connection timeout");
            break;
        case MQTT_CONNECTION_LOST:
            log_w("Connection lost");
            break;
        case MQTT_CONNECT_FAILED:
            log_w("Connection failed");
            break;
        case MQTT_DISCONNECTED:
            log_w("Disconnected");
            break;
        case MQTT_CONNECTED:
            log_i("Connected");
            break;
        case MQTT_CONNECT_BAD_PROTOCOL:
            log_e("Connect bad protocol");
            break;
        case MQTT_CONNECT_BAD_CLIENT_ID:
            log_e("Connect bad client ID");
            break;
        case MQTT_CONNECT_UNAVAILABLE:
            log_e("Connect unavailable");
            break;
        case MQTT_CONNECT_BAD_CREDENTIALS:
            log_e("Connect bad credentials");
            break;
        case MQTT_CONNECT_UNAUTHORIZED:
            log_e("Connect unauthorized");
            break;
        default:
            log_w("Unknown state '%d'", client.state());
    }
}
