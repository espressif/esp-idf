ESP-MQTT
========

Overview
--------

ESP-MQTT is an implementation of [MQTT](mqtt.org) protocol client (MQTT is a lightweight publish/subscribe messaging protocol).


Features
--------
   * Supports MQTT over TCP, SSL with mbedtls, MQTT over Websocket, MQTT over Websocket Secure.
   * Easy to setup with URI
   * Multiple instances (Multiple clients in one application)
   * Support subscribing, publishing, authentication, last will messages, keep alive pings and all 3 QoS levels (it should be a fully functional client).


Application Example
-------------------

    * :example:`protocols/mqtt/tcp`: MQTT over tcp, default port 1883
    * :example:`protocols/mqtt/ssl`:  MQTT over tls, default port 8883
    * :example:`protocols/mqtt/ssl_ds`:  MQTT over tls using digital signature peripheral for authentication, default port 8883.
    * :example:`protocols/mqtt/ssl_mutual_auth`:  MQTT over tls using certificates for authentication, default port 8883
    * :example:`protocols/mqtt/ssl_psk`: MQTT over tls using pre-shared keys for authentication, default port 8883.
    * :example:`protocols/mqtt/ws`: MQTT over Websocket, default port 80
    * :example:`protocols/mqtt/wss`: MQTT over Websocket Secure, default port 443


Configuration
-------------

The configuration is made by setting fields in ``esp_mqtt_client_config_t`` struct. The configuration struct has the following sub structs to configure different aspects of the client operation. 

  * :cpp:member:`broker<esp_mqtt_client_config::broker>` - Allow to set address and security verification.
  * :cpp:member:`credentials<esp_mqtt_client_config::credentials>` - Client credentials for authentication.
  * :cpp:member:`session<esp_mqtt_client_config::session>` - Configuration for MQTT session aspects.
  * :cpp:member:`network<esp_mqtt_client_config::network>` - Networking related configuration. 
  * :cpp:member:`task<esp_mqtt_client_config::task>` - Allow to configure FreeRTOS task.
  * :cpp:member:`buffer<esp_mqtt_client_config::buffer>` - Buffer size for input and output.

In the following session the most common aspects are detailed. 

Broker
^^^^^^^^^^^

===========
Address
===========

Broker address can be set by usage of ``broker.address`` struct. The configuration can be made by usage of ``uri`` field 
or the combination of ``hostname``, ``transport`` and ``port``. Optionally, `path` could be set, this field is useful in 
websocket connections.

The ``uri`` field is used in the following format ``scheme://hostname:port/path``.
-  Curently support ``mqtt``, ``mqtts``, ``ws``, ``wss`` schemes
-  MQTT over TCP samples:

   -  ``mqtt://mqtt.eclipseprojects.io``: MQTT over TCP, default port 1883:
   -  ``mqtt://mqtt.eclipseprojects.io:1884`` MQTT over TCP, port 1884:
   -  ``mqtt://username:password@mqtt.eclipseprojects.io:1884`` MQTT over TCP,
      port 1884, with username and password

-  MQTT over SSL samples:

   -  ``mqtts://mqtt.eclipseprojects.io``: MQTT over SSL, port 8883
   -  ``mqtts://mqtt.eclipseprojects.io:8884``: MQTT over SSL, port 8884

-  MQTT over Websocket samples:

   -  ``ws://mqtt.eclipseprojects.io:80/mqtt``

-  MQTT over Websocket Secure samples:

   -  ``wss://mqtt.eclipseprojects.io:443/mqtt``

-  Minimal configurations:

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

-  Note: By default mqtt client uses event loop library to post related mqtt events (connected, subscribed, published, etc.)

============
Verification
============

For secure connections TLS is used, and to guarantee Broker's identity the ``broker.verification`` struct must be set.
The broker certificate may be set in PEM or DER format. To select DER the equivalent ``_len`` field must be set, 
otherwise a NULL terminated string in PEM format should be provided to ``certificate`` field.

-  Get certificate from server, example: ``mqtt.eclipseprojects.io``
   ``openssl s_client -showcerts -connect mqtt.eclipseprojects.io:8883 </dev/null 2>/dev/null|openssl x509 -outform PEM >mqtt_eclipse_org.pem``
-  Check the sample application: ``examples/mqtt_ssl``
-  Configuration:

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
          .address.uri = "mqtts://mqtt.eclipseprojects.io:8883",
          .verification.certificate = (const char *)mqtt_eclipse_org_pem_start,
        },
    };

To details on other fields check the Reference API and :ref:`esp_tls_server_verification`.

Client Credentials
^^^^^^^^^^^^^^^^^^

All client related credentials are under the ``credentials`` field.

 * ``username``: pointer to the username used for connecting to the broker, can also be set by URI. 
 * ``client_id``: pointer to the client id, defaults to ``ESP32_%CHIPID%`` where %CHIPID% are the last 3 bytes of MAC address in hex format

==============
Authentication
==============

It's possible to set authentication parameters through the ``authentication`` field. The client supports the following authentication methods:

 * Using a password by setting ``authentication.password``.
 * Muthual authentication with TLS by setting ``authentication.certificate`` and ``authentication.key``, both can be provided in PEM or DER format.
 * Using secure element available in ESP32-WROOM-32SE, setting ``authentication.use_secure_element``.
 * Using Digital Signature Peripheral available in some Espressif devices, setting ``authentication.ds_data``.   

Session
^^^^^^^^^^^

For MQTT session related configurations ``section`` fields should be used. 

=======================
Last Will and Testament
=======================

MQTT allows for a last will and testament (LWT) message to notify other clients when a client ungracefully disconnects. This is configured by the following fields
in the ``esp_mqtt_client_config_t.session.last_will``-struct.

 * ``topic``: pointer to the LWT message topic
 * ``msg``: pointer to the LWT message
 * ``msg_len``: length of the LWT message, required if ``msg`` is not null-terminated
 * ``qos``: quality of service for the LWT message
 * ``retain``: specifies the retain flag of the LWT message

Change settings in Project Configuration Menu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The settings for MQTT can be found using ``idf.py menuconfig``, under Component config -> ESP-MQTT Configuration

The following settings are available:

- :ref:`CONFIG_MQTT_PROTOCOL_311`: Enables 3.1.1 version of MQTT protocol

- :ref:`CONFIG_MQTT_TRANSPORT_SSL`, :ref:`CONFIG_MQTT_TRANSPORT_WEBSOCKET`: Enables specific MQTT transport layer, such as SSL, WEBSOCKET, WEBSOCKET_SECURE

- :ref:`CONFIG_MQTT_CUSTOM_OUTBOX`: Disables default implementation of mqtt_outbox, so a specific implementaion can be supplied


Events
------
The following events may be posted by the MQTT client:

* ``MQTT_EVENT_BEFORE_CONNECT``: The client is initialized and about to start connecting to the broker.
* ``MQTT_EVENT_CONNECTED``: The client has successfully established a connection to the broker. The client is now ready to send and receive data.
* ``MQTT_EVENT_DISCONNECTED``: The client has aborted the connection due to being unable to read or write data, e.g. because the server is unavailable.
* ``MQTT_EVENT_SUBSCRIBED``: The broker has acknowledged the client's subscribe request. The event data will contain the message ID of the subscribe message.
* ``MQTT_EVENT_UNSUBSCRIBED``: The broker has acknowledged the client's unsubscribe request. The event data will contain the message ID of the unsubscribe message.
* ``MQTT_EVENT_PUBLISHED``: The broker has acknowledged the client's publish message. This will only be posted for Quality of Service level 1 and 2, as level 0 does not use acknowledgements. The event data will contain the message ID of the publish message.
* ``MQTT_EVENT_DATA``: The client has received a publish message. The event data contains: message ID, name of the topic it was published to, received data and its length. For data that exceeds the internal buffer multiple `MQTT_EVENT_DATA` will be posted and `current_data_offset` and `total_data_len` from event data updated to keep track of the fragmented message.
* ``MQTT_EVENT_ERROR``: The client has encountered an error. `esp_mqtt_error_type_t` from `error_handle` in the event data can be used to further determine the type of the error. The type of error will determine which parts of the `error_handle` struct is filled.

API Reference
-------------

.. include-build-file:: inc/mqtt_client.inc
