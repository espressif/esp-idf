ESP-IDF A2DP-SINK demo
======================

Demo of A2DP audio sink role

This is the demo for user to use ESP_APIs to create a GATT Server.

Options choose step:
    1. make menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and do not choose "Release DRAM from Classic BT controller"
    4. choose your options.

After the program started, other bluetooth devices such as smart phones can discover this device named "ESP_SPEAKER", and after connection is established, audio data can be transmitted and there will occur a count of audio data packets printed.
