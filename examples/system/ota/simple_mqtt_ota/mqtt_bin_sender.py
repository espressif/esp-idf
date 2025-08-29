# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import time

import paho.mqtt.client as mqtt

# ==== Configuration ====


def get_broker_from_sdkconfig(path='sdkconfig'):  # type: ignore[no-untyped-def]
    if not os.path.exists(path):
        print(f"[WARN] sdkconfig file not found at '{path}'. Using default broker.")
        return 'localhost', 1883  # Default broker and port
    with open(path, 'r') as f:
        for line in f:
            if line.startswith('CONFIG_MQTT_BROKER_URI'):
                broker_uri = line.split('=')[1].strip().strip('"')
                if broker_uri.startswith('mqtt://'):
                    broker_uri = broker_uri[7:]
                parts = broker_uri.split(':')
                broker = parts[0]
                port = int(parts[1]) if len(parts) > 1 else 1883  # Default to 1883 if port is not specified
                return broker, port
    print('[WARN] CONFIG_MQTT_BROKER_URI not found in sdkconfig. Using default broker.')
    return 'localhost', 1883  # Default to localhost and port 1883


def get_topic_from_sdkconfig(path='sdkconfig', topic_key='CONFIG_MQTT_TOPIC_'):  # type: ignore[no-untyped-def]
    topics = {}
    if not os.path.exists(path):
        print(f"[WARN] sdkconfig file not found at '{path}'. Using default topics.")
        return {'update': 'ota/update', 'firmware': 'ota/firmware', 'done': 'ota/done'}

    with open(path, 'r') as f:
        for line in f:
            if line.startswith(f'{topic_key}'):
                key = line.split('=')[0].strip().replace(f'{topic_key}', '').lower()
                value = line.split('=')[1].strip().strip('"')
                topics[key] = value

    # Return default values if some topics are not found in sdkconfig
    return {
        'update': topics.get('update', 'ota/update'),
        'firmware': topics.get('firmware', 'ota/firmware'),
        'done': topics.get('done', 'ota/done'),
    }


# Get broker and topics from sdkconfig
BROKER, PORT = get_broker_from_sdkconfig()
TOPICS = get_topic_from_sdkconfig()
BIN_PATH = 'build/simple_mqtt_ota.bin'  # Path to your firmware binary
CHUNK_SIZE = 4000  # Chunk size in bytes, if you can't change the buffer size you need to set CHUNK_SIZE to 1000

# Extract topics
TOPIC_UPDATE = TOPICS['update']
TOPIC_FIRMWARE = TOPICS['firmware']
TOPIC_DONE = TOPICS['done']

# ==== MQTT Setup ====
client = mqtt.Client()
print(f'[INFO] Connecting to MQTT broker: {BROKER}:{PORT}')
client.connect(BROKER, PORT)
client.loop_start()

# ==== Step 1: Trigger OTA start ====
print('[1/3] Sending OTA start command...')
client.publish(TOPIC_UPDATE, payload='start')
time.sleep(1)

# ==== Step 2: Send .bin in chunks ====
print('[2/3] Sending firmware:', BIN_PATH)

# Sanity check: make sure the file starts with magic byte 0xE9
with open(BIN_PATH, 'rb') as f:
    magic = f.read(1)
    if magic != b'\xe9':
        print('Invalid firmware file: expected 0xE9 at start, got', magic.hex())
        client.loop_stop()
        exit(1)
time.sleep(2)

# Now send the whole file
with open(BIN_PATH, 'rb') as f:
    total_sent = 0
    while True:
        chunk = f.read(CHUNK_SIZE)
        if not chunk:
            break
        client.publish(TOPIC_FIRMWARE, payload=chunk)
        total_sent += len(chunk)
        print(f'Sent {total_sent} bytes', end='\r')
        time.sleep(0.5)  # Delay to allow ESP time to read from mqtt and write

print(f'\nFirmware upload complete: {total_sent} bytes sent.')

# ==== Step 3: Send OTA done ====
print('[3/3] Sending OTA done command...')
client.publish(TOPIC_DONE, payload='done')
time.sleep(2)

client.loop_stop()
client.disconnect()
print('OTA update pushed successfully via MQTT.')
