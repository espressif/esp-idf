ESP-MQTT
========

Overview
--------

ESP-MQTT is an implementation of MQTT protocol client (MQTT is a lightweight publish/subscribe messaging protocol).


Features
--------
   * supports MQTT over TCP, SSL with mbedtls, MQTT over Websocket, MQTT over Websocket Secure.
   * Easy to setup with URI
   * Multiple instances (Multiple clients in one application)
   * Support subscribing, publishing, authentication, will messages, keep alive pings and all 3 QoS levels (it should be a fully functional client).


Application Example
-------------------

    * :example:`protocols/mqtt/tcp`: MQTT over tcp, default port 1883
    * :example:`protocols/mqtt/ssl`:  MQTT over tcp, default port 8883
    * :example:`protocols/mqtt/ws`: MQTT over Websocket, default port 80
    * :example:`protocols/mqtt/wss`: MQTT over Websocket Secure, default port 443


Configuration
-------------
URI
^^^

-  Curently support ``mqtt``, ``mqtts``, ``ws``, ``wss`` schemes
-  MQTT over TCP samples:

   -  ``mqtt://iot.eclipse.org``: MQTT over TCP, default port 1883:
   -  ``mqtt://iot.eclipse.org:1884`` MQTT over TCP, port 1884:
   -  ``mqtt://username:password@iot.eclipse.org:1884`` MQTT over TCP,
      port 1884, with username and password

-  MQTT over SSL samples:

   -  ``mqtts://iot.eclipse.org``: MQTT over SSL, port 8883
   -  ``mqtts://iot.eclipse.org:8884``: MQTT over SSL, port 8884

-  MQTT over Websocket samples:

   -  ``ws://iot.eclipse.org:80/ws``

-  MQTT over Websocket Secure samples:

   -  ``wss://iot.eclipse.org:443/ws``

-  Minimal configurations:

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://iot.eclipse.org",
        .event_handle = mqtt_event_handler,
        // .user_context = (void *)your_context
    };

-  If there are any options related to the URI in
   ``esp_mqtt_client_config_t``, the option defined by the URI will be
   overridden. Sample:

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://iot.eclipse.org:1234",
        .event_handle = mqtt_event_handler,
        .port = 4567,
    };
    //MQTT client will connect to iot.eclipse.org using port 4567

SSL
^^^

-  Get certificate from server, example: ``iot.eclipse.org``
   ``openssl s_client -showcerts -connect iot.eclipse.org:8883 </dev/null 2>/dev/null|openssl x509 -outform PEM >iot_eclipse_org.pem``
-  Check the sample application: ``examples/mqtt_ssl``
-  Configuration:

.. code:: cpp

    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://iot.eclipse.org:8883",
        .event_handle = mqtt_event_handler,
        .cert_pem = (const char *)iot_eclipse_org_pem_start,
    };

For more options on ``esp_mqtt_client_config_t``, please refer to API reference below

Change settings in ``menuconfig``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::
    make menuconfig 
    -> Component config -> ESP-MQTT Configuration 


- :ref:`CONFIG_MQTT_PROTOCOL_311`: Enables 3.1.1 version of MQTT protocol

- :ref:`CONFIG_MQTT_TRANSPORT_SSL`, :ref:`CONFIG_MQTT_TRANSPORT_WEBSOCKET`: Enables specific MQTT transport layer, such as SSL, WEBSOCKET, WEBSOCKET_SECURE

- :ref:`CONFIG_MQTT_CUSTOM_OUTBOX`: Disables default implementation of mqtt_outbox, so a specific implementaion can be supplied


API Reference
-------------

.. include:: /_build/inc/mqtt_client.inc
