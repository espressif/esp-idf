| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# Enable Socket CAN Device with bitrate 250Kbps

```bash
sudo ip link set can0 up type can bitrate 250000 restart-ms 100
```
