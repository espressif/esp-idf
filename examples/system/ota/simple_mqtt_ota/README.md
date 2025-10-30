| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Simple MQTT OTA Example

This is an **ESP-IDF example** demonstrating how to perform **Over-the-Air (OTA) firmware updates using MQTT** and the `esp_ota_ops` APIs.

Firmware is transmitted in binary chunks over MQTT topics. The device subscribes to specific topics to initiate, receive, and finalize the OTA update process.

---

## Configuration

### MQTT OTA Settings

Use `idf.py menuconfig → MQTT OTA Example Configuration` to configure:

- **MQTT Broker URI** (e.g., `mqtt://mosquitto.:1883`).
- **Start Update Topic**: default `ota/update`.
- **Firmware Chunk Topic**: default `ota/firmware`.
- **Done Topic**: default `ota/done`.

---

## OTA Workflow

The device listens for commands via the following MQTT topics:

| Topic         | Purpose                      |
|---------------|------------------------------|
| `ota/update`  | Initiates the OTA process    |
| `ota/firmware`| Receives firmware binary     |
| `ota/done`    | Finalizes OTA and reboots    |

**OTA flow**:
1. Receive `start` on `ota/update`.
2. Download firmware from `ota/firmware` in chunks.
3. Upon `ota/done`, verify and apply the update.

OTA status and errors are published back to `ota/update` as well.

---

## Using the Python Script

A helper script is provided to push firmware(.bin) to the device over MQTT.
This script can read the sdkconfig to match the broker, port and topics.

### Install Requirements

```bash
pip install paho-mqtt
```

---

## Notes

- This example uses plain MQTT (mqtt://). For production use, consider switching to MQTTS (mqtts://) with proper certificates.

- This method has been tested with both local brokers and public MQTT services (e.g., HiveMQ, Mosquitto). Some public brokers may drop chunks under load.

- You can update multiple devices at once, since the firmware is broadcast to a shared topic. Devices don't send responses during transfer.

- The OTA result (success, failure, time taken, bytes written) is published back to the same MQTT topic (ota/update) for monitoring.

- The size of the chunk can be reduced inside ***mqtt_bin_sender.py***.

- OTA status feedback published back over MQTT.

- The default mqtt buffer size for esp-idf is 1024 but for this example is 4096 (`CONFIG_MQTT_BUFFER_SIZE=4096`).

---

## Example output

### ESP UART log

```log
I (488) simple_mqtt_ota_example: [APP] Startup..
I (488) simple_mqtt_ota_example: [APP] Free memory: 277836 bytes
I (488) simple_mqtt_ota_example: [APP] IDF version: v5.5-dev-3511-g875df72342
I (498) simple_mqtt_ota_example: Starting MQTT OTA example...
I (498) simple_mqtt_ota_example: Running partition type: 0 subtype: 16 at offset 0x20000
I (538) simple_mqtt_ota_example: SHA-256 for bootloader:  bad2fbaeb7ad2675f71509253247d196cb74dc3b0dee166b0fe608a50f522c57
I (728) simple_mqtt_ota_example: SHA-256 for current firmware:  ccc93782b37a0528395b0a5656019d5be8fc66ff2df63b4ed79a50545530bcbb
I (728) example_connect: Start example_connect.
.
.
.
I (5938) simple_mqtt_ota_example: MQTT connected to broker: mqtt://192.168.31.171:1883
I (5938) simple_mqtt_ota_example: Subscribed to topic 'ota/update', msg_id=31565
I (5948) simple_mqtt_ota_example: Subscribed to topic 'ota/done', msg_id=39260
I (5958) simple_mqtt_ota_example: MQTT_EVENT_SUBSCRIBED, msg_id=31565
I (5958) simple_mqtt_ota_example: MQTT_EVENT_SUBSCRIBED, msg_id=39260
I (23368) simple_mqtt_ota_example: MQTT received data on topic ota/update
I (23368) simple_mqtt_ota_example: Received firmware update command. Starting OTA...
I (23368) simple_mqtt_ota_example: Subscribed to topic 'ota/firmware', msg_id=20233
I (29028) simple_mqtt_ota_example: OTA started. Ready to receive firmware.
I (29028) simple_mqtt_ota_example: MQTT_EVENT_SUBSCRIBED, msg_id=20233
I (29028) simple_mqtt_ota_example: MQTT received data on topic ota/firmware
.
.
.
I (138178) simple_mqtt_ota_example: Bytes received: 894752 bytes
I (138668) simple_mqtt_ota_example: MQTT received data on topic ota/done
I (138668) simple_mqtt_ota_example: Received OTA DONE message, total size written: 894752 bytes
I (138668) esp_image: segment 0: paddr=001d0020 vaddr=3f400020 size=1f528h (128296) map
I (138708) esp_image: segment 1: paddr=001ef550 vaddr=3ff80000 size=0001ch (    28)
I (138718) esp_image: segment 2: paddr=001ef574 vaddr=3ffb0000 size=00aa4h (  2724)
I (138718) esp_image: segment 3: paddr=001f0020 vaddr=400d0020 size=9f668h (652904) map
I (138898) esp_image: segment 4: paddr=0028f690 vaddr=3ffb0aa4 size=0339ch ( 13212)
I (138898) esp_image: segment 5: paddr=00292a34 vaddr=40080000 size=17cc4h ( 97476)
I (138928) esp_image: segment 0: paddr=001d0020 vaddr=3f400020 size=1f528h (128296) map
I (138968) esp_image: segment 1: paddr=001ef550 vaddr=3ff80000 size=0001ch (    28)
I (138968) esp_image: segment 2: paddr=001ef574 vaddr=3ffb0000 size=00aa4h (  2724)
I (138968) esp_image: segment 3: paddr=001f0020 vaddr=400d0020 size=9f668h (652904) map
I (139148) esp_image: segment 4: paddr=0028f690 vaddr=3ffb0aa4 size=0339ch ( 13212)
I (139158) esp_image: segment 5: paddr=00292a34 vaddr=40080000 size=17cc4h ( 97476)
I (144268) simple_mqtt_ota_example: OTA update successful. Rebooting...
```

### MQTT Messages log

```log 
Topic ota/done message: done.
Topic ota/update message: OTA successful time: 115s, bytes written: 894752.
```
