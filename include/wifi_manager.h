#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <functional>


namespace wifi_manager {
    bool setup(const char *mqttServer, const std::function<void(const char *)> &onMqttServerSet);
}


#endif //WIFI_MANAGER_H
