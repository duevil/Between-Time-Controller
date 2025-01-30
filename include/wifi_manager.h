#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <functional>
#include <WString.h>


namespace wifi_manager {
    bool setup(const char *mqttServer, const std::function<void(const char *)> &onMqttServerSet);
    String ip();
}


#endif //WIFI_MANAGER_H
