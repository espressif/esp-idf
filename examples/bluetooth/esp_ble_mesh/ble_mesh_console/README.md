| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# ble mesh node console demo
## Introduction  
This demo implements ble mesh node basic features.Based on this demo, node can be scaned and proved by provisioner, reply get/set message to provisioner.  

Demo steps:  
1. Build the ble mesh node console demo with sdkconfig.default  
2. register node and set oob info, load model to init ble mesh node  
3. enable bearer, so that it can be scaned and provisioned by provisioner  

