#ifndef MQTT_PARAMS_H
#define MQTT_PARAMS_H

//! @brief Client ID
//! @note Base64 encoding of 'between-time-controller' to ensure uniqueness for connecting to public MQTT brokers
constexpr auto CLIENT_ID = "YmV0d2Vlbi10aW1lLWNvbnRyb2xsZXI";
//! @brief MQTT server host used for MQTT connection in the Wokwi simulator
constexpr auto MQTT_SERVER = "test.mosquitto.org";
//! @brief MQTT topic path root
//! @note Base64 encoding of 'between-time' to obscure path for reduced visibility in public MQTT brokers
constexpr auto MQTT_ROOT = "YmV0d2Vlbi10aW1l/";

#endif //MQTT_PARAMS_H
