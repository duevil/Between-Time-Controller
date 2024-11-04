#include "mqtt.h"
#include <utility>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Preferences.h>

static std::unordered_map<std::string_view, mqtt::Topic::Callback> callbacks{};
static WiFiClient wifiClient{};
static PubSubClient client{wifiClient};
static Preferences prefs{};
static auto clientID = "esp32-mqtt-client";
static String host{};
static std::function<void()> onConnect{};

static void mainCallback(const char *topic, uint8_t *payload, unsigned int length);
static bool connect();
static void logState();


/*!
 * @brief Loads the MQTT server host from non-volatile storage and initializes the MQTT client
 */
void mqtt::setup() {
    prefs.begin("mqtt", false);
    host = prefs.getString("host", "");
    client.setServer(host.c_str(), 1883);
    client.setCallback(mainCallback);
}

/*!
 * @brief Sets the MQTT client ID
 * @param id the MQTT client ID
 */
void mqtt::setClientID(const char *id) {
    clientID = id;
    log_i("MQTT client id set to %s", clientID);
}

/*!
 * @brief Sets the MQTT server host and saves it to non-volatile storage
 * @param server the MQTT server host
 */
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

/*!
 * @brief Sets the callback function to be called when a connection to the MQTT server is established
 * @param callback the callback function
 */
void mqtt::setOnConnect(std::function<void()> callback) { onConnect = std::move(callback); }

/*!
 * @brief Processes MQTT messages and maintains the connection
 */
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


/*!
 * @brief Constructor for a topic
 * @param topic The topic to subscribe to
 * @param callback The callback function to be called when a message is received
 */
mqtt::Topic::Topic(const char *topic, Callback callback) : topic(topic) { callbacks[topic] = std::move(callback); }

/*!
 * @brief Subscribes to the topic and registers the callback
 * @return true if successful, false otherwise
 */
bool mqtt::Topic::subscribe() const {
    const auto b = client.subscribe(topic);
    if (!b) {
        log_e("Failed to subscribe to topic '%s'", topic);
    } else {
        log_i("Subscribed to topic '%s'", topic);
    }
    return b;
}

/*!
 * @brief Publishes a message to the topic
 * @param message the message to publish
 * @return true if successful, false otherwise
 */
bool mqtt::Topic::publish(const char *message) const {
    const auto b = client.publish(topic, message, true);
    if (!b) {
        log_e("Failed to publish to topic '%s'", topic);
    } else {
        log_i("Published '%s' to topic '%s'", message, topic);
    }
    return b;
}


//! Callback for MQTT messages; calls registered callback for the topic
static void mainCallback(const char *topic, uint8_t *payload, unsigned int length) {
    log_d("Received message on topic '%s': %.*s", topic, length, payload);
    if (const auto it = callbacks.find(topic); it != callbacks.end()) {
        it->second(payload, length);
    } else {
        log_w("No callback registered for topic '%s'", topic);
    }
}

//! try to connect to MQTT server
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

//! Log MQTT client state
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
