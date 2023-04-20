Espressif's Frameworks
======================

Here you will find a collection of the official Espressif libraries and frameworks.

Espressif Audio Development Framework
-------------------------------------

The ESP-ADF is a comprehensive framework for audio applications including:

* CODEC's HAL
* Music Players and Recorders
* Audio Processing
* Bluetooth Speakers
* Internet Radios
* Hands-free devices
* Speech Recognition

This framework is available at GitHub: `ESP-ADF <https://github.com/espressif/esp-adf>`_.

ESP-CSI
-------

ESP-CSI is an experimental implementation that uses the Wi-Fi Channel State Information to detect the presence of a human body.

See `ESP-CSI <https://github.com/espressif/esp-csi>`_ project for more information about it.

Espressif DSP Library
---------------------

The library provides algorithms optimized specifically for digital signal processing applications.
This library supports:

* Matrix multiplication
* Dot product
* FFT (Fast Fourier Transform)
* IIR (Infinite Impulse Response)
* FIR (Finite Impulse Response)
* Vector math operations

This library is available here: `ESP-DSP library <https://github.com/espressif/esp-dsp>`_.

ESP-WIFI-MESH Development Framework
-----------------------------------

This framework is based on the ESP-WIFI-MESH protocol with the following features:

* Fast network configuration
* Stable upgrade
* Efficient debugging
* LAN control
* Various application demos

`ESP-MDF <https://github.com/espressif/esp-mdf>`_.

ESP-WHO
-------

The ESP-WHO is a face detection and recognition framework using the ESP32 and camera.
To know more about the project, see `ESP-WHO <https://github.com/espressif/esp-who>`_ on GitHub.

ESP RainMaker
-------------

`ESP RainMaker <https://rainmaker.espressif.com/>`_ is a complete solution for accelerated AIoT development.
Using ESP RainMaker, you can create AIoT devices from the firmware to the integration with voice-assistant, phone apps and cloud backend.

`ESP RainMaker on GitHub <https://github.com/espressif/esp-rainmaker>`_.

ESP-IoT-Solution
----------------

`ESP-IoT-Solution <https://docs.espressif.com/projects/espressif-esp-iot-solution/en/latest/>`_ contains commonly used device drivers and code frameworks when developing IoT systems. The device drivers and code frameworks within the ESP-IoT-Solution are organized as separate components, allowing them to be easily integrated into an ESP-IDF project.

ESP-IoT-Solution includes:

* Device drivers for sensors, display, audio, GUI, input, actuators, etc.
* Framework and documentation for low power, security, storage, etc.
* Guide for Espressif open source solutions from practical application point.

`ESP-IoT-Solution on GitHub <https://github.com/espressif/esp-iot-solution>`_.


ESP-Protocols
-------------

`ESP-Protocols <https://github.com/espressif/esp-protocols>`_ repository contains collection of protocol components for ESP-IDF. The code within the ESP-Protocols is organized into separate components, allowing them to be easily integrated into an ESP-IDF project. In addition to that, each component is available in `IDF Component Registry <https://components.espressif.com/>`_.

ESP-Protocols components:

* `esp_modem <https://components.espressif.com/component/espressif/esp_modem>`_ enables connectivity with GSM/LTE modems using AT commands or PPP protocol, see the `esp_modem documentation <https://docs.espressif.com/projects/esp-protocols/esp_modem/docs/latest/index.html>`_.

* `mdns <https://components.espressif.com/component/espressif/mdns>`_ (mDNS) is a multicast UDP service that is used to provide local network service and host discovery, see the `mdns documentation <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html>`_.

* `esp_websocket_client <https://components.espressif.com/component/espressif/esp_websocket_client>`_ is a managed component for `esp-idf` that contains implementation of [WebSocket protocol client](https://datatracker.ietf.org/doc/html/rfc6455) for ESP32, see the `esp_websocket_client documentation <https://docs.espressif.com/projects/esp-protocols/esp_websocket_client/docs/latest/index.html>`_.

* `asio <https://components.espressif.com/component/espressif/asio>`_ is a cross-platform C++ library, see https://think-async.com/Asio/. It provides a consistent asynchronous model using a modern C++ approach. , see the `asio documentation <https://docs.espressif.com/projects/esp-protocols/asio/docs/latest/index.html>`_.

ESP-BSP
-------

`ESP-BSP <https://github.com/espressif/esp-bsp>`_ repository contains Board Support Packages (BSPs) for various Espressif's and 3rd party development boards. BSPs are useful for quick start on a supported board. Usually they contain pinout definition and helper functions, that will initialize peripherals for the specific board. Additionally, the BSP would contain drivers for external chips populated on the development board, such as sensors, displays, audio codecs etc.

ESP-IDF-CXX
-----------

`ESP-IDF-CXX <https://github.com/espressif/esp-idf-cxx>`_ contains C++ wrappers for part of ESP-IDF. The focus is on ease of use, safety, automatic resource management and shifting checks to compile time instead of failing at run time. There are C++ classes for ESP-Timer, I2C, SPI, GPIO and other peripherals or features of ESP-IDF. ESP-IDF-CXX is `available as a component <https://components.espressif.com/components/espressif/esp-idf-cxx>`_ from the component registry. Please check the project's `README.md <https://github.com/espressif/esp-idf-cxx/blob/main/README.md>`_ for more information.
