| Supported Targets | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-MQTT SSL Mutual Authentication with Digital Signature
(See the README.md file in the upper level 'examples' directory for more information about examples.)

Espressif's ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-H2 and ESP32-P4 MCU have a built-in Digital Signature (DS) Peripheral, which provides hardware acceleration for RSA signature. More details can be found at [Digital Signature with ESP-TLS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/protocols/esp_tls.html#digital-signature-with-esp-tls).

This example connects to the broker test.mosquitto.org using ssl transport with client certificate(RSA) and as a demonstration subscribes/unsubscribes and sends a message on certain topic.The RSA signature operation required in the ssl connection is performed with help of the Digital Signature (DS) peripheral.
(Please note that the public broker is maintained by the community so may not be always available, for details please visit http://test.mosquitto.org)

It uses ESP-MQTT library which implements mqtt client to connect to mqtt broker.
## How to use example

### Hardware Required

This example can be executed on any of the supported ESP32 family board (which has a built-in DS peripheral), the only required interface is WiFi/Ethernet and connection to internet.

### Configure the project

#### 1) Selecting the target

Please select the supported target with the following command:
```
idf.py set-target /* target */
```
More detials can be found at [Selecting the target](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#selecting-the-target).

#### 2) Generate your client key and certificate

Navigate to the main directory

```
cd main
```

Generate a client key and a CSR. When you are generating the CSR, do not use the default values. At a minimum, the CSR must include the Country, Organisation and Common Name fields.

```
openssl genrsa -out client.key
openssl req -out client.csr -key client.key -new
```

Paste the generated CSR in the [Mosquitto test certificate signer](https://test.mosquitto.org/ssl/index.php), click Submit and downloaded the `client.crt`. This `client.crt` file shall be used as the device certificate.

#### 3) Configure the DS peripheral

* i) Install the [esp_secure_cert configuration utility](https://github.com/espressif/esp_secure_cert_mgr/tree/main/tools#esp_secure_cert-configuration-tool) with following command:
```
pip install esp-secure-cert-tool
```
* ii) The DS peripheral can be configured by executing the following command:

```
configure_esp_secure_cert.py -p /* Serial port */ --device-cert /* Device cert */ --private-key /* RSA priv key */ --target_chip /* target chip */ --configure_ds  --skip_flash
```
This command shall generate a partition named `esp_secure_cert.bin` in the `esp_secure_cert_data` directory. This partition would be aumatically detected by the build system and flashed at appropriate offset when `idf.py flash` command is used. For this process, the command must be executed in the current folder only.

In the command USB COM port is nothing but the serial port to which the ESP chip is connected. see
[check serial port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html#check-port-on-windows) for more details.
RSA private key is nothing but the client private key ( RSA ) generated in Step 2.

> Note: More details about the `esp-secure-cert-tool` utility can be found [here](https://github.com/espressif/esp_secure_cert_mgr/tree/main/tools).

#### 4) Connection cofiguration
* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (3714) event: sta ip: 192.168.0.139, mask: 255.255.255.0, gw: 192.168.0.2
I (3714) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (3964) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
I (4164) MQTTS_EXAMPLE: MQTT_EVENT_CONNECTED
I (4174) MQTTS_EXAMPLE: sent publish successful, msg_id=41464
I (4174) MQTTS_EXAMPLE: sent subscribe successful, msg_id=17886
I (4174) MQTTS_EXAMPLE: sent subscribe successful, msg_id=42970
I (4184) MQTTS_EXAMPLE: sent unsubscribe successful, msg_id=50241
I (4314) MQTTS_EXAMPLE: MQTT_EVENT_PUBLISHED, msg_id=41464
I (4484) MQTTS_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=17886
I (4484) MQTTS_EXAMPLE: sent publish successful, msg_id=0
I (4684) MQTTS_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=42970
I (4684) MQTTS_EXAMPLE: sent publish successful, msg_id=0
I (4884) MQTT_CLIENT: deliver_publish, message_length_read=19, message_length=19
I (4884) MQTTS_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
I (5194) MQTT_CLIENT: deliver_publish, message_length_read=19, message_length=19
I (5194) MQTTS_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
```
