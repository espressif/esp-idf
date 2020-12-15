// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "mqtt_client.h"

namespace idf::mqtt {



/**
 * @brief Broker addresss
 *
 * Use this to set the broker without parsing the URI string.
 *
 */
struct Host {
    std::string address; /*!< Host name*/
    std::string path;    /*!< Route path of the broker in host*/
    esp_mqtt_transport_t transport; /*!< Transport scheme to use. */
};

/**
 * @brief Broker addresss URI
 *
 * Use this to set the broker address using the URI.
 *
 */
struct URI {
    std::string address; /*!< Broker adddress URI*/
};


/**
 * @brief Broker addresss.
 *
 */
struct BrokerAddress {
  std::variant<Host, URI> address; /*!< Address, defined by URI or Host struct */
  uint32_t port = 0; /*!< Port used, defaults to 0 to select common port for the scheme used */
};

/**
 * @brief PEM formated data
 *
 *  Store certificates, keys and cryptographic data.
 *
 */
struct PEM {
    const char *data;
};

/**
 * @brief DER formated data
 *
 *  Store certificates, keys and cryptographic data.
 *
 */
struct DER {
    const char *data;
    size_t len;
};

/**
 * @brief Holds cryptography related information
 *
 *  Hold PEM or DER formated cryptographic data.
 *
 */
using CryptographicInformation = std::variant<PEM, DER>;


/**
 * @brief Do not verify broker certificate.
 *
 * To be used when doing MQTT over TLS connection but not verify broker's certificates.
 *
 */
struct Insecure {};

/**
 * @brief Use global CA store
 *
 * To be used when client should use the Global CA Store to get trusted certificates for the broker.
 *
 */
struct GlobalCAStore {};

/**
 * @brief Use a pre shared key for broker authentication.
 *
 * To be used when client should use a PSK to authenticate the broker.
 *
 */
struct PSK {
    const struct psk_key_hint *hint_key;/*  Pointer to PSK struct defined in esp_tls.h to enable PSK authentication */
};


/**
 * @brief Authentication method for Broker
 *
 * Selects the method for authentication based on the type it holds.
 *
 */
using BrokerAuthentication = std::variant<Insecure, GlobalCAStore, CryptographicInformation, PSK>;

/**
 * @brief Password related data.
 *
 */
struct Password {
    std::string data;
};

/**
 * @brief Data to authenticate client with certificates.
 *
 */
struct ClientCertificate {
  CryptographicInformation certificate; /*!< Certificate in PEM or DER format.*/
  CryptographicInformation key; /*!< Key data in PEM or DER format.*/
  std::optional<Password> key_password = std::nullopt; /*!< Optional password for key */
};

/**
 * @brief Used to select usage of Secure Element
 *
 *  Enables the usage of the secure element present in ESP32-WROOM-32SE.
 *
 */
struct SecureElement {};


/**
 * @brief Used to select usage of Digital Signature Peripheral.
 *
 *  Enables the usage of the Digital Signature hardware accelerator.
 *
 */
struct DigitalSignatureData {
    void *ds_data;                          /* carrier of handle for digital signature parameters */
};

using AuthenticationFactor = std::variant<Password, ClientCertificate, SecureElement>;

struct BrokerConfiguration {
    BrokerAddress address;
    BrokerAuthentication security;
};

struct ClientCredentials {
    std::optional<std::string> username;    // MQTT username
    AuthenticationFactor authentication;
    std::vector<std::string> alpn_protos;               /*!< List of supported application protocols to be used for ALPN */
    /* default is ``ESP32_%CHIPID%`` where %CHIPID% are last 3 bytes of MAC address in hex format */
    std::optional<std::string > client_id = std::nullopt;
};

struct Event {
    mqtt_event_callback_t event_handle;     /*!< handle for MQTT events as a callback in legacy mode */
    esp_event_loop_handle_t event_loop_handle; /*!< handle for MQTT event loop library */
};

struct LastWill {
    const char *lwt_topic;                  /*!< LWT (Last Will and Testament) message topic (NULL by default) */
    const char *lwt_msg;                    /*!< LWT message (NULL by default) */
    int lwt_qos;                            /*!< LWT message qos */
    int lwt_retain;                         /*!< LWT retained message flag */
    int lwt_msg_len;                        /*!< LWT message length */
};

struct Session {
    LastWill last_will;
    int disable_clean_session;              /*!< mqtt clean session, default clean_session is true */
    int keepalive;                          /*!< mqtt keepalive, default is 120 seconds */
    bool disable_keepalive;                 /*!< Set disable_keepalive=true to turn off keep-alive mechanism, false by default (keepalive is active by default). Note: setting the config value `keepalive` to `0` doesn't disable keepalive feature, but uses a default keepalive period */
    esp_mqtt_protocol_ver_t protocol_ver;   /*!< MQTT protocol version used for connection, defaults to value from menuconfig*/
};

struct Task {
    int task_prio;                          /*!< MQTT task priority, default is 5, can be changed in ``make menuconfig`` */
    int task_stack;                         /*!< MQTT task stack size, default is 6144 bytes, can be changed in ``make menuconfig`` */
};

struct Connection {
    esp_mqtt_transport_t transport;         /*!< overrides URI transport */
    int reconnect_timeout_ms;               /*!< Reconnect to the broker after this value in miliseconds if auto reconnect is not disabled (defaults to 10s) */
    int network_timeout_ms;                 /*!< Abort network operation if it is not completed after this value, in milliseconds (defaults to 10s) */
    int refresh_connection_after_ms;        /*!< Refresh connection after this value (in milliseconds) */
    bool disable_auto_reconnect;            /*!< this mqtt client will reconnect to server (when errors/disconnect). Set disable_auto_reconnect=true to disable */
};

struct Configuration {
    Event event;
    Task task;
    Session session;
    Connection connection;
    void *user_context;                     /*!< pass user context to this option, then can receive that context in ``event->user_context`` */
    int buffer_size;                        /*!< size of MQTT send/receive buffer, default is 1024 (only receive buffer size if ``out_buffer_size`` defined) */
    int out_buffer_size;                    /*!< size of MQTT output buffer. If not defined, both output and input buffers have the same size defined as ``buffer_size`` */
};

} // idf::mqtt
