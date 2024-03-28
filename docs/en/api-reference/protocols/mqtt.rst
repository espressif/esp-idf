ESP-MQTT
========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-MQTT is an implementation of `MQTT <https://mqtt.org/>`__ protocol client, which is a lightweight publish/subscribe messaging protocol. Now ESP-MQTT supports `MQTT v5.0 <https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html>`__.


Features
--------

   * Support MQTT over TCP, SSL with Mbed TLS, MQTT over WebSocket, and MQTT over WebSocket Secure
   * Easy to setup with URI
   * Multiple instances (multiple clients in one application)
   * Support subscribing, publishing, authentication, last will messages, keep alive pings, and all 3 Quality of Service (QoS) levels (it should be a fully functional client)


Application Examples
---------------------

   * :example:`protocols/mqtt/tcp`: MQTT over TCP, default port 1883
   * :example:`protocols/mqtt/ssl`: MQTT over TLS, default port 8883
   * :example:`protocols/mqtt/ssl_ds`: MQTT over TLS using digital signature peripheral for authentication, default port 8883
   * :example:`protocols/mqtt/ssl_mutual_auth`: MQTT over TLS using certificates for authentication, default port 8883
   * :example:`protocols/mqtt/ssl_psk`: MQTT over TLS using pre-shared keys for authentication, default port 8883
   * :example:`protocols/mqtt/ws`: MQTT over WebSocket, default port 80
   * :example:`protocols/mqtt/wss`: MQTT over WebSocket Secure, default port 443
   * :example:`protocols/mqtt5`: Uses ESP-MQTT library to connect to broker with MQTT v5.0

MQTT Message Retransmission
---------------------------

A new MQTT message is created by calling :cpp:func:`esp_mqtt_client_publish <esp_mqtt_client_publish()>` or its non blocking counterpart :cpp:func:`esp_mqtt_client_enqueue <esp_mqtt_client_enqueue()>`.

Messages with QoS 0 is sent only once. QoS 1 and 2 have different behaviors since the protocol requires extra steps to complete the process.

The ESP-MQTT library opts to always retransmit unacknowledged QoS 1 and 2 publish messages to avoid losses in faulty connections, even though the MQTT specification requires the re-transmission only on reconnect with Clean Session flag been set to 0 (set :cpp:member:`disable_clean_session <esp_mqtt_client_config_t::session_t::disable_clean_session>` to true for this behavior).

QoS 1 and 2 messages that may need retransmission are always enqueued, but first transmission try occurs immediately if :cpp:func:`esp_mqtt_client_publish <esp_mqtt_client_publish>` is used. A transmission retry for unacknowledged messages will occur after :cpp:member:`message_retransmit_timeout <esp_mqtt_client_config_t::session_t::message_retransmit_timeout>`. After :ref:`CONFIG_MQTT_OUTBOX_EXPIRED_TIMEOUT_MS` messages will expire and be deleted. If :ref:`CONFIG_MQTT_REPORT_DELETED_MESSAGES` is set, an event will be sent to notify the user.

Configuration
-------------

The configuration is made by setting fields in :cpp:class:`esp_mqtt_client_config_t` struct. The configuration struct has the following sub structs to configure different aspects of the client operation.

   * :cpp:class:`esp_mqtt_client_config_t::broker_t` - Allow to set address and security verification.
   * :cpp:class:`esp_mqtt_client_config_t::credentials_t` - Client credentials for authentication.
   * :cpp:class:`esp_mqtt_client_config_t::session_t` - Configuration for MQTT session aspects.
   * :cpp:class:`esp_mqtt_client_config_t::network_t` - Networking related configuration.
   * :cpp:class:`esp_mqtt_client_config_t::task_t` - Allow to configure FreeRTOS task.
   * :cpp:class:`esp_mqtt_client_config_t::buffer_t` - Buffer size for input and output.

In the following sections, the most common aspects are detailed.

Broker
^^^^^^^^^^^

===========
Address
===========

Broker address can be set by usage of :cpp:class:`address <esp_mqtt_client_config_t::broker_t::address_t>` struct. The configuration can be made by usage of :cpp:member:`uri <esp_mqtt_client_config_t::broker_t::address_t::uri>` field or the combination of :cpp:member:`hostname <esp_mqtt_client_config_t::broker_t::address_t::hostname>`, :cpp:member:`transport <esp_mqtt_client_config_t::broker_t::address_t::transport>` and :cpp:member:`port <esp_mqtt_client_config_t::broker_t::address_t::port>`. Optionally, :cpp:member:`path <esp_mqtt_client_config_t::broker_t::address_t::path>` could be set, this field is useful in WebSocket connections.

The :cpp:member:`uri <esp_mqtt_client_config_t::broker_t::address_t::uri>` field is used in the format ``scheme://hostname:port/path``.

-  Currently support ``mqtt``, ``mqtts``, ``ws``, ``wss`` schemes
-  MQTT over TCP samples:

   -  ``mqtt://mqtt.eclipseprojects.io``: MQTT over TCP, default port 1883
   -  ``mqtt://mqtt.eclipseprojects.io:1884``: MQTT over TCP, port 1884
   -  ``mqtt://username:password@mqtt.eclipseprojects.io:1884``: MQTT over TCP,
      port 1884, with username and password

-  MQTT over SSL samples:

   -  ``mqtts://mqtt.eclipseprojects.io``: MQTT over SSL, port 8883
   -  ``mqtts://mqtt.eclipseprojects.io:8884``: MQTT over SSL, port 8884

-  MQTT over WebSocket samples:

   -  ``ws://mqtt.eclipseprojects.io:80/mqtt``

-  MQTT over WebSocket Secure samples:

   -  ``wss://mqtt.eclipseprojects.io:443/mqtt``

-  Minimal configurations:

.. code-block:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

.. note::

   By default MQTT client uses event loop library to post related MQTT events (connected, subscribed, published, etc.).

============
Verification
============

For secure connections with TLS used, and to guarantee Broker's identity, the :cpp:class:`verification <esp_mqtt_client_config_t::broker_t::verification_t>` struct must be set.
The broker certificate may be set in PEM or DER format. To select DER, the equivalent :cpp:member:`certificate_len <esp_mqtt_client_config_t::broker_t::verification_t::certificate_len>` field must be set. Otherwise, a null-terminated string in PEM format should be provided to :cpp:member:`certificate <esp_mqtt_client_config_t::broker_t::verification_t::certificate>` field.

-  Get certificate from server, example: ``mqtt.eclipseprojects.io``
    .. code::

       openssl s_client -showcerts -connect mqtt.eclipseprojects.io:8883 < /dev/null \
       2> /dev/null | openssl x509 -outform PEM > mqtt_eclipse_org.pem

-  Check the sample application: :example:`protocols/mqtt/ssl`
-  Configuration:

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
          .address.uri = "mqtts://mqtt.eclipseprojects.io:8883",
          .verification.certificate = (const char *)mqtt_eclipse_org_pem_start,
        },
    };

For details about other fields, please check the `API Reference`_ and :ref:`esp_tls_server_verification`.

Client Credentials
^^^^^^^^^^^^^^^^^^

All client related credentials are under the :cpp:class:`credentials <esp_mqtt_client_config_t::credentials_t>` field.

 * :cpp:member:`username <esp_mqtt_client_config_t::credentials_t::username>`: pointer to the username used for connecting to the broker, can also be set by URI
 * :cpp:member:`client_id <esp_mqtt_client_config_t::credentials_t::client_id>`: pointer to the client ID, defaults to ``ESP32_%CHIPID%`` where ``%CHIPID%`` are the last 3 bytes of MAC address in hex format

==============
Authentication
==============

It is possible to set authentication parameters through the :cpp:class:`authentication <esp_mqtt_client_config_t::credentials_t::authentication_t>` field. The client supports the following authentication methods:

 * :cpp:member:`password <esp_mqtt_client_config_t::credentials_t::authentication_t::password>`: use a password by setting
 * :cpp:member:`certificate <esp_mqtt_client_config_t::credentials_t::authentication_t::certificate>` and :cpp:member:`key <esp_mqtt_client_config_t::credentials_t::authentication_t::key>`: mutual authentication with TLS, and both can be provided in PEM or DER format
 * :cpp:member:`use_secure_element <esp_mqtt_client_config_t::credentials_t::authentication_t::use_secure_element>`: use secure element (ATECC608A) interfaced to ESP32
 * :cpp:member:`ds_data <esp_mqtt_client_config_t::credentials_t::authentication_t::ds_data>`: use Digital Signature Peripheral available in some Espressif devices

Session
^^^^^^^^^^^

For MQTT session-related configurations, :cpp:class:`session <esp_mqtt_client_config_t::session_t>` fields should be used.

=======================
Last Will and Testament
=======================

MQTT allows for a last will and testament (LWT) message to notify other clients when a client ungracefully disconnects. This is configured by the following fields in the :cpp:class:`last_will <esp_mqtt_client_config_t::session_t::last_will_t>` struct.

 * :cpp:member:`topic <esp_mqtt_client_config_t::session_t::last_will_t::topic>`: pointer to the LWT message topic
 * :cpp:member:`msg <esp_mqtt_client_config_t::session_t::last_will_t::msg>`: pointer to the LWT message
 * :cpp:member:`msg_len <esp_mqtt_client_config_t::session_t::last_will_t::msg_len>`: length of the LWT message, required if :cpp:member:`msg <esp_mqtt_client_config_t::session_t::last_will_t::msg>` is not null-terminated
 * :cpp:member:`qos <esp_mqtt_client_config_t::session_t::last_will_t::qos>`: quality of service for the LWT message
 * :cpp:member:`retain <esp_mqtt_client_config_t::session_t::last_will_t::retain>`: specifies the retain flag of the LWT message

Change Settings in Project Configuration Menu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The settings for MQTT can be found using :code:`idf.py menuconfig`, under ``Component config`` > ``ESP-MQTT Configuration``.

The following settings are available:

- :ref:`CONFIG_MQTT_PROTOCOL_311`: enable 3.1.1 version of MQTT protocol

- :ref:`CONFIG_MQTT_TRANSPORT_SSL` and :ref:`CONFIG_MQTT_TRANSPORT_WEBSOCKET`: enable specific MQTT transport layer, such as SSL, WEBSOCKET, and WEBSOCKET_SECURE

- :ref:`CONFIG_MQTT_CUSTOM_OUTBOX`: disable default implementation of mqtt_outbox, so a specific implementation can be supplied


Events
------
The following events may be posted by the MQTT client:

* ``MQTT_EVENT_BEFORE_CONNECT``: The client is initialized and about to start connecting to the broker.
* ``MQTT_EVENT_CONNECTED``: The client has successfully established a connection to the broker. The client is now ready to send and receive data.
* ``MQTT_EVENT_DISCONNECTED``: The client has aborted the connection due to being unable to read or write data, e.g., because the server is unavailable.
* ``MQTT_EVENT_SUBSCRIBED``: The broker has acknowledged the client's subscribe request. The event data contains the message ID of the subscribe message.
* ``MQTT_EVENT_UNSUBSCRIBED``: The broker has acknowledged the client's unsubscribe request. The event data contains the message ID of the unsubscribe message.
* ``MQTT_EVENT_PUBLISHED``: The broker has acknowledged the client's publish message. This is only posted for QoS level 1 and 2, as level 0 does not use acknowledgements. The event data contains the message ID of the publish message.
* ``MQTT_EVENT_DATA``: The client has received a publish message. The event data contains: message ID, name of the topic it was published to, received data and its length. For data that exceeds the internal buffer, multiple ``MQTT_EVENT_DATA`` events are posted and :cpp:member:`current_data_offset <esp_mqtt_event_t::current_data_offset>` and :cpp:member:`total_data_len <esp_mqtt_event_t::total_data_len>` from event data updated to keep track of the fragmented message.
* ``MQTT_EVENT_ERROR``: The client has encountered an error. The field :cpp:type:`error_handle <esp_mqtt_error_codes_t>` in the event data contains :cpp:type:`error_type <esp_mqtt_error_type_t>` that can be used to identify the error. The type of error determines which parts of the :cpp:type:`error_handle <esp_mqtt_error_codes_t>` struct is filled.

API Reference
-------------

.. include-build-file:: inc/mqtt_client.inc
