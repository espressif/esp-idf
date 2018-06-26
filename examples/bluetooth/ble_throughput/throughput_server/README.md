ESP-IDF BLE throughput GATT SERVER demo
========================

This is the demo used to test the BLE throughput, this demo should used with throughput client demo together. 
The throughput of BLE can up to 720-767 bits/s between to ESP32 board.
Note: 
1. In order to maximize throughput, we need to set the uart print baud rate at 921600 or more (make menuconfig --> Component config --> ESP32-specific --> UART console baud rate --> 921600(or 1500000));
2. We can only test notify or write throughput at the same time, this demo default to test the notify throughput, if want to test the write throughput, 
please set: make menuconfig --> Component config --> Example 'GATT SERVER THROUGHPUT' Config  ---> then select the 'test the gattc write throughput' option
3. This demo only test unidirectional throughput, if you want to test the bidirectional throughput please change the demo by yourself.
4. Should change the CPU frequency to 240MHz in the make menuconfig --> Component config  ---> ESP32-specific  ---> CPU frequency (240 MHz)
5. Should change the bluetooth controller and Bluedroid run in different Core in the make menuconfig --> Component config  ---> Bluetooth  ---> The cpu core which bluetooth controller run (Core 0 (PRO CPU))   & Bluedroid Enable  ---> The cpu core which Bluedroid run (Core 1 (APP CPU))

