#ifndef MQTT_H
#define MQTT_H

#include <functional>


namespace mqtt {
    void setup();
    void setClientID(const char *);
    void setServer(const char *);
    const char* getServer();
    void setOnConnect(std::function<void()>);
    void loop();

    struct Topic {
        using Callback = std::function<void(uint8_t *, unsigned int)>;
        static const Topic &create(const char *topic, Callback = {});
        Topic() = default;
        virtual ~Topic() = default;
        Topic(const Topic &) = delete;
        Topic(Topic &&) = delete;
        Topic &operator=(const Topic &) = delete;
        Topic &operator=(Topic &&) = delete;
        virtual bool subscribe() const = 0;
        virtual bool publish(const char *message) const = 0;
    };
}


#endif //MQTT_H
