#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <functional>

/*!
 * @brief Methods for basic MQTT operations
 * @details This namespace provides methods for initializing the MQTT client, setting the server to connect to,
 * and handling subscriptions and publications. The server port is fixed to 1883. If a the server host it configured,
 * connection is established automatically.
 * @note This namespace is an easy-to-use wrapper around the PubSubClient library
 * @author Malte Kasolowsky
 */
namespace mqtt {
#ifdef WOKWI
    //! @brief Client ID
    //! @note Base64 encoding of 'between-time-controller' to ensure uniqueness for connecting to public MQTT brokers
    constexpr auto CLIENT_ID = "YmV0d2Vlbi10aW1lLWNvbnRyb2xsZXI";
#else
    //! @brief Client ID;
    constexpr auto CLIENT_ID = "between-time-controller";
#endif

    void setup();
    void setServer(const char *);
    void setOnConnect(std::function<void()>);
    void loop();

    /*!
     * @brief Struct for handling subscriptions and publications to a specific MQTT topic
     * @note To handle messages received on the subscribed topic, a callback function can be provided that is called
     * when a message is received.
     */
    struct Topic {
        using Callback = std::function<void(uint8_t *, unsigned int)>;
        explicit Topic(const char *topic, Callback = {});
        bool subscribe() const;
        bool publish(const char *message) const;
    private:
        const char *const topic;
    };
}

#endif //MQTT_H
