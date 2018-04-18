ESP-IDF BT-INQUIRY demo
======================

Demo of Classic Bluetooth Device and Service Discovery

This is the demo for user to use ESP_APIs to perform inquiry to search for a target device and then performs service search via SDP.

Options choose step:
    1. make menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and do not choose "Release DRAM from Classic BT controller"
    4. choose your options.

After the program started, the device will start inquiry to search for a device with Major device type "PHONE" in the Class of Device Field. Then it will cancel the inquiry and started to perform service discovering on this remote device.
