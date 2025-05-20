# Between Time Controller

Firmware for the physical controller for Between Time, a small Coop-VR-Puzzle-Game made in Unity. In the game, two players need to solve puzzles within a tight time frame to extract all timecores, closing a time rift that threatens to collapse the whole of the time-space-continuum.

While one player interacts in the virtuell world in VR, the second player uses this controller to assist him, needing both player to communicate and work together to solve all puzzles in time.

The controller uses MQTT to send puzzle values, inputs and game states to Unity.

![Between Time Controller](https://github.com/user-attachments/assets/21278b35-9b62-4610-ae9e-7c2f5607ace1)

## Instructions

### Setup and usage

- Connect the controller to a computer via a Micro-USB cable (or to any 5V USB source if Serial communication is not
  needed).
- The controller will automatically start up and try to establish a WiFi connection (if configured previously).
- If no WiFi connection is available, the controller will start in Access Point mode and can be configured via a web
  interface.
- The config portal can be accessed by connecting to the controllers WiFi AP `BetweenTime Controller` and, if the
  captive portal does not open automatically, navigating to `http://192.168.4.1`.
- To configure the controller's WiFi connection, navigate to the `Configure WiFi` tab and enter the SSID and password of
  the desired network.
- The controller will then try to connect to the network and, if successful, display the assigned IP address on the OLED
  display, waiting for a connection to the MQTT server.
- If previously assigned, the controller will automatically try to connect to the MQTT server. If no server is
  available, the MQTT server's IP address can be configured via the controller's web portal.
- To configure the MQTT server's IP address, connect to the controller's IP address and navigate to the `Setup` tab. The
  port is hardcoded to 1883.
- After updating the MQTT server's IP address, the controller will automatically try to connect to the new server.
- Both the WiFi credentials and the MQTT server's IP address are stored in the controller's non-volatile flash memory
  and will be retained even after a power cycle.
- After connecting to the MQTT server, the controller will wait for incoming messages send by the Unity application.

### (Re)building and flashing the firmware

For (re)building and/or flashing the firmware, the following procedure can be followed:

- [Install PlatformIO](https://platformio.org/install).
- Clone the repository and navigate to the project directory.
- Load the project in PlatformIO.
- Run `pio run -t upload` to build and flash the firmware to the connected controller.
- Press and hold down the `BOOT` button on the controller (the button to the right of the USB port; accessing it through
  the USB port is possible but can be a bit finicky. If in doubt, simple screwing the enclosure open and pressing the
  button directly is also an option) and release it after the upload process has started.

## Hardware and Software Components

The controller uses an ESP32 DevKitC v4 as its main microcontroller and connects to the following hardware components:

- A simple mechanical rotary encoder with push button ([header](include/encoder.h))
- Five simple mechanical switches ([header](include/input.h))
- Five WS2812B RGB LEDs ([header](include/leds.h))
- A small 128x64 graphical OLED display ([header](include/graphic_display.h))
- A four-digit seven segment display ([header](include/timecode_display.h))

The software also implements a few software-sided components:

- A WiFi manager to manage WiFi credentials and connection and the MQTT server
  parameter ([header](include/wifi_manager.h))
- An easy-to-use callback-based MQTT api ([header](include/mqtt.h))
- A framework for the state variables wrapping the MQTT api ([header](include/states.h))

The software for the controller is designed to be modular and loosely coupled. The components are written to have no
reference to and knowledge of the other components and are only linked in the [main source file](src/main.cpp) and
the [logic definition](src/logic.cpp), where the controller's main state machine is implemented.

The puzzle values are hardcoded and can be found in the [values header](include/values.h).

## Simulation

Though mainly for development and testing purposes, this program includes an environment for simulating the device and
its functionality using the [WOKWI simulation platform](https://wokwi.com/). As the simulation does not allow private
networking and therefore uses hardcoded credentials instead of the WiFi manager.

A browser version of the simulated device can be found [here](https://wokwi.com/projects/415267767949041665). To start
the simulation, build the firmware and upload it to the simulation platform (the firmware is located in
the [Wokwi build directory](.pio/build/wokwi)).
The host address of the MQTT server can be set in the [MQTT Params](include/mqtt_params.h) file. This must be a publicly
available MQTT server (e.g. [test.mosquitto.org](http://test.mosquitto.org)), as the simulation is not capable of
connecting to private networks.
