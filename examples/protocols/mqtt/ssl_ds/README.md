| Supported Targets | ESP32-S2 | ESP32-C3 | ESP32-S3 |
# ESP-MQTT SSL Mutual Authentication with Digital Signature
(See the README.md file in the upper level 'examples' directory for more information about examples.)

Espressif's ESP32-S2, ESP32-S3 and ESP32-C3 MCU have a built-in Digital Signature (DS) Peripheral, which provides hardware acceleration for RSA signature. More details can be found at [Digital Signature with ESP-TLS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/protocols/esp_tls.html#digital-signature-with-esp-tls).

This example connects to the broker test.mosquitto.org using ssl transport with client certificate(RSA) and as a demonstration subscribes/unsubscribes and sends a message on certain topic.The RSA signature operation required in the ssl connection is performed with help of the Digital Signature (DS) peripheral.
(Please note that the public broker is maintained by the community so may not be always available, for details please visit http://test.mosquitto.org)

It uses ESP-MQTT library which implements mqtt client to connect to mqtt broker.
## How to use example

### Hardware Required

This example can be executed on any ESP32-S2, ESP32-S3, ESP32-C3 board (which has a built-in DS peripheral), the only required interface is WiFi and connection to internet.

### Configure the project

#### 1) Selecting the target
As the project is to be built for the target ESP32-S2, ESP32-S3, ESP32-C3 it should be selected with the following command
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

Paste the generated CSR in the [Mosquitto test certificate signer](https://test.mosquitto.org/ssl/index.php), click Submit and copy the downloaded `client.crt` in the `main` directory.

Please note, that the supplied file `client.crt` in the `main` directory is only a placeholder for your client certificate (i.e. the example "as is" would compile but would not connect to the broker)

#### 3) Configure the DS peripheral

*  The DS peripheral can be configured with the python script [configure_ds.py](README.md#configure_ds-py) by executing the following command

```
    python configure_ds.py --port /* USB COM port */ --private-key /* RSA priv key */
```

In the command USB COM port is nothing but the serial port to which the ESP chip is connected. see
[check serial port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html#check-port-on-windows) for more details.
RSA private key is nothing but the client private key ( RSA ) generated in Step 2.

#### 4) Connection cofiguration
* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* When using Make build system, set `Default serial port` under `Serial flasher config`.

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


### configure_ds.py
The script [configure_ds.py](./configure_ds.py) is used for configuring the DS peripheral on the ESP32-S2/ESP32-S3/ESP32-C3 SoC. The steps in the script are based on technical details of certain operations in the Digital Signature calculation, which can be found at Digital Signature Section of [ESP32-S2 TRM](https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf)

The configuration script performs the following steps -

1. Take the client private key ( RSA key ) as input.
    (*required parameter for the script)
can be provided with
```
    python configure_ds.py --private-key /* path to client (rsa) prv key */
```

2.  Randomly Calculate the `HMAC_KEY` and the `initialization vector`(IV). Then calculate the encrypted private key parameters from client private key (step i) and newly generated parameters. These encrypted private key parameters are required for the DS peripheral to perform the Digital Signature operation.

3.  Store the `HMAC_KEY` in one of the efuse key blocks (in the hardware).
    The ID of the efuse key block ( should be in range 1-5) can be provided with the following option. (default value of 1 is used if not provided),
```
    python configure_ds.py --efuse_key_id /* key id in range 1-5 */
```

Currently for development purposes, the `HMAC_KEY` is stored in the efuse key block  without read protection so that read operation can be performed on the same key block.
> You can burn (write) a key on an efuse key block only once. Please use a different key block ID if you want to use a different `HMAC_KEY` for the DS operation.

4. Create an NVS partition of the name `pre_prov.csv` (in `esp_ds_data` folder) which contains the required encrypted private key parameters. A bin file of the nvs partition (`pre_prov.bin`) is also created. As we have added a custom partition, the example is set to use the custom partition table by adding the required option in `sdkconfig.defaults`.

5. (optional) The script can be made to print the summary of the efuse on the chip by providing the following option. When this option is enabled, no other operations in the script are performed.
```
    python configure_ds.py --summary
```

6. (optional) If the user wants to keep the encrypted private key data and the randomly generated `HMAC_KEY` on the host machine for testing purpose. The following option may be used.
```
    python configure_ds.py --keep_ds_data_on_host
```
 The respective files will be stored in the `esp_ds_data` folder which is generated by the script in the same directory. The contents of the `esp_ds_data` folder may be overwritten when the `configure_ds.py` script is executed again.

7. (optional) If the user wants to use the script for production usecase then this option can be used.
Currently for development purpose, the script disables the read protection on the efuse key block by default.
In case of a production usecase it is recommeneded to enable the read protection for the efuse key block. It can be done by providing following option along with other required options:
```
    python configure_ds.py --production
```

> A list of all the supported options in the script can be obtained by executing `python configure_ds.py --help`.
