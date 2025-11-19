ESP-MQTT
========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-MQTT is an implementation of `MQTT <https://mqtt.org/>`__ protocol client, which is a lightweight publish/subscribe messaging protocol. Now ESP-MQTT supports `MQTT v5.0 <https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html>`__.

The ESP-IDF component ``ESP-MQTT`` has been moved from ESP-IDF since version v6.0 to a separate repository:

* `esp-mqtt component on GitHub <https://github.com/espressif/esp-mqtt>`__

To add ESP-MQTT component in your project, please run ``idf.py add-dependency espressif/mqtt``.

Hosted Documentation
---------------------

The documentation can be found on the link below:

* `esp-mqtt documentation <https://docs.espressif.com/projects/esp-mqtt/en/latest/>`__

Application Examples
--------------------

Two reference examples are available in ESP-IDF:

- MQTT over TLS (certificate bundle): ``examples/protocols/mqtt``

  - Validates the broker with ESP-IDF's certificate bundle (default).
  - Default broker URI targets ``mqtts://test.mosquitto.org:8886`` (Let's Encrypt chain).
  - Alternative configuration pins Mosquitto CA and uses ``mqtts://test.mosquitto.org:8883``.

- MQTT 5.0 over TLS (certificate bundle): ``examples/protocols/mqtt5``

  - Uses MQTT v5.0 with TLS and validation via the certificate bundle.
  - Default broker URI targets ``mqtts://test.mosquitto.org:8886``.

For additional transport modes (TCP, WS/WSS, mutual auth, etc.), see the examples in the `espressif/mqtt component repository <https://components.espressif.com/components/espressif/mqtt>`__.
