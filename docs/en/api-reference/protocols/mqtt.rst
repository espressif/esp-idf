ESP-MQTT
========

Overview
--------

ESP-MQTT is an implementation of MQTT protocol client (MQTT is a lightweight publish/subscribe messaging protocol).


Features
--------
   * Supports MQTT over TCP, SSL with mbedtls, MQTT over Websocket, MQTT over Websocket Secure.
   * Easy to setup with URI
   * Multiple instances (Multiple clients in one application)
   * Support subscribing, publishing, authentication, last will messages, keep alive pings and all 3 QoS levels (it should be a fully functional client).


Application Example
-------------------

    * :example:`protocols/mqtt/tcp`: MQTT over tcp, default port 1883
    * :example:`protocols/mqtt/ssl`:  MQTT over tcp, default port 8883
    * :example:`protocols/mqtt/ssl_psk`: MQTT over tcp using pre-shared keys for authentication, default port 8883
    * :example:`protocols/mqtt/ws`: MQTT over Websocket, default port 80
    * :example:`protocols/mqtt/wss`: MQTT over Websocket Secure, default port 443


Configuration
-------------
URI
^^^

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
        .uri = "mqtt://mqtt.eclipseprojects.io",
        // .user_context = (void *)your_context
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

-  Note: By default mqtt client uses event loop library to post related mqtt events (connected, subscribed, published, etc.)


SSL
^^^

-  Get certificate from server, example: ``mqtt.eclipseprojects.io``
   ``openssl s_client -showcerts -connect mqtt.eclipseprojects.io:8883 </dev/null 2>/dev/null|openssl x509 -outform PEM >mqtt_eclipse_org.pem``
-  Check the sample application: ``examples/mqtt_ssl``
-  Configuration:

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://mqtt.eclipseprojects.io:8883",
        .event_handle = mqtt_event_handler,
        .cert_pem = (const char *)mqtt_eclipse_org_pem_start,
    };

If the certificate is not null-terminated then ``cert_len`` should also be set.
Other SSL related configuration parameters are:

 * ``use_global_ca_store``: use the global certificate store to verify server certificate, see ``esp-tls.h`` for more information
 * ``client_cert_pem``: pointer to certificate data in PEM or DER format for SSL mutual authentication, default is NULL, not required if mutual authentication is not needed.
 * ``client_cert_len``: length of the buffer pointed to by client_cert_pem. May be 0 for null-terminated pem.
 * ``client_key_pem``: pointer to private key data in PEM or DER format for SSL mutual authentication, default is NULL, not required if mutual authentication is not needed.
 * ``client_key_len``: length of the buffer pointed to by client_key_pem. May be 0 for null-terminated pem.
 * ``psk_hint_key``: pointer to PSK struct defined in esp_tls.h to enable PSK authentication (as alternative to certificate verification). If not NULL and server/client certificates are NULL, PSK is enabled
 * ``alpn_protos``: NULL-terminated list of protocols to be used for ALPN.

Last Will and Testament
^^^^^^^^^^^^^^^^^^^^^^^
MQTT allows for a last will and testament (LWT) message to notify other clients when a client ungracefully disconnects. This is configured by the following fields
in the ``esp_mqtt_client_config_t``-struct.

 * ``lwt_topic``: pointer to the LWT message topic
 * ``lwt_msg``: pointer to the LWT message
 * ``lwt_msg_len``: length of the LWT message, required if ``lwt_msg`` is not null-terminated
 * ``lwt_qos``: quality of service for the LWT message
 * ``lwt_retain``: specifies the retain flag of the LWT message

Other Configuration Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * ``disable_clean_session``: determines the clean session flag for the connect message, defaults to a clean session
 * ``keepalive``: determines how many seconds the client will wait for a ping response before disconnecting, default is 120 seconds.
 * ``disable_auto_reconnect``: enable to stop the client from reconnecting to server after errors or disconnects
 * ``user_context``: custom context that will be passed to the event handler
 * ``task_prio``: MQTT task priority, defaults to 5
 * ``task_stack``: MQTT task stack size, defaults to 6144 bytes, setting this will override setting from menuconfig
 * ``buffer_size``: size of MQTT send/receive buffer, default is 1024 bytes
 * ``username``: pointer to the username used for connecting to the broker
 * ``password``: pointer to the password used for connecting to the broker
 * ``client_id``: pointer to the client id, defaults to ``ESP32_%CHIPID%`` where %CHIPID% are the last 3 bytes of MAC address in hex format
 * ``host``: MQTT broker domain (ipv4 as string), setting the uri will override this
 * ``port``: MQTT broker port, specifying the port in the uri will override this
 * ``transport``: sets the transport protocol, setting the uri will override this
 * ``refresh_connection_after_ms``: refresh connection after this value (in milliseconds)
 * ``event_handle``: handle for MQTT events as a callback in legacy mode
 * ``event_loop_handle``: handle for MQTT event loop library



For more options on ``esp_mqtt_client_config_t``, please refer to API reference below

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
