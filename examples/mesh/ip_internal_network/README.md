| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | -------- |

# Mesh IP Internal Networking example

This example demonstrates how to use mesh to create a IP capable sub-network.
As a demonstration all nodes publish their IP and their internal mesh layer to MQTT broker
and uses internal communication at the same time

## Functionality

This example uses experimental NAT feature to translate addresses/ports from an internal subnet, that is created
by the root node running a DHCP server. At the same time, the nodes communicate using low level mesh send/receive
API to exchange data, such as routing table from root to all nodes and an event notification from one node
to all other nodes in the mesh. As a demonstration, the same event is also published at the mqtt broker
on a subscribed topic, so both internal mesh_recv() notification as well as mqtt data event are to be received.

Note, that this example in not supported for IPv6-only configuration.

### Hardware Required

This example can be executed on any platform board, the only required interface is WiFi and connection to internet.

### Configure the project

Open the project configuration menu (`idf.py menuconfig`) to configure the mesh network channel, router SSID, router password and mesh softAP settings.

### Build and Flash

Build the project and flash it to multiple boards forming a mesh network, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Output sample from mesh node
```
I (7749) mesh_main: <MESH_EVENT_TODS_REACHABLE>state:0
I (7749) mesh_main: <MESH_EVENT_ROOT_ADDRESS>root address:24:0a:c4:09:88:5d
I (7899) wifi:AP's beacon interval = 307200 us, DTIM period = 2
I (8809) mesh_main: <IP_EVENT_STA_GOT_IP>IP:10.0.0.3
I (8819) mesh_main: Tried to publish layer:2 IP:10.0.0.3
I (8819) mesh_mqtt: Other event id:7
I (9189) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
I (9469) mesh_mqtt: MQTT_EVENT_CONNECTED
I (9569) mesh_mqtt: sent publish returned msg_id=42728
I (9839) mesh_mqtt: MQTT_EVENT_SUBSCRIBED, msg_id=60327
I (9919) mesh_mqtt: MQTT_EVENT_PUBLISHED, msg_id=42728
...
I (1218786) MESH_MQTT: sent publish returned msg_id=13978
W (1218836) mesh_main: Key pressed!
I (1218836) MESH_MQTT: sent publish returned msg_id=15808
I (1218846) mesh_main: Sending to [0] 30:ae:a4:80:5b:18: sent with err code: 0
I (1218906) MESH_MQTT: MQTT_EVENT_PUBLISHED, msg_id=13978
I (1219016) MESH_MQTT: MQTT_EVENT_PUBLISHED, msg_id=15808
I (1219366) MESH_MQTT: MQTT_EVENT_DATA
TOPIC=/topic/ip_mesh/key_pressed
DATA=24:0a:c4:09:88:5c
I (1220036) mesh_main: Received Routing table [0] 30:ae:a4:80:5b:18
I (1220036) mesh_main: Received Routing table [1] 24:0a:c4:09:88:5c
```

### Output sample from the root node
```
I (11957) mesh_main: <MESH_EVENT_TODS_REACHABLE>state:0
I (11967) mesh_main: <MESH_EVENT_ROOT_ADDRESS>root address:24:0a:c4:09:88:5d
I (11967) wifi:AP's beacon interval = 102400 us, DTIM period = 3
I (12767) esp_netif_handlers: sta ip: 192.168.2.3, mask: 255.255.255.0, gw: 192.168.2.1
I (12767) mesh_main: <IP_EVENT_STA_GOT_IP>IP:192.168.2.3
...
I (1253864) mesh_main: Received Routing table [0] 30:ae:a4:80:5b:18
I (1253864) mesh_main: Received Routing table [1] 24:0a:c4:09:88:5c
I (1253874) mesh_main: Sending routing table to [0] 30:ae:a4:80:5b:18: sent with err code: 0
I (1253884) mesh_main: Sending routing table to [1] 24:0a:c4:09:88:5c: sent with err code: 0
I (1253974) MESH_MQTT: MQTT_EVENT_PUBLISHED, msg_id=18126
W (1254714) mesh_main: Keypressed detected on node: 24:0a:c4:09:88:5c
I (1254814) MESH_MQTT: MQTT_EVENT_DATA
TOPIC=/topic/ip_mesh/key_pressed
DATA=24:0a:c4:09:88:5c
```
