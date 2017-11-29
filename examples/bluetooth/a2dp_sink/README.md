ESP-IDF A2DP-SINK demo
======================

Demo of A2DP audio sink role

This is the demo for user to use ESP_APIs to create a GATT Server.

Options choose step:
    1. make menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and do not choose "Release DRAM from Classic BT controller"
    4. choose your options.

For the I2S codec, pick whatever chip or board works for you; this code was written using a PCM5102 chip, but other I2S boards and chips will probably work as well. The default I2S connections are shown below, but these can be changed in menuconfig:

+-----------+--------------+
| ESP pin   | I2S signal   |
+===========+==============+
| GPIO22    | LRCK         |
+-----------+--------------+
| GPIO25    | DATA         |
+-----------+--------------+
| GPIO26    | BCK          |
+-----------+--------------+

If the internal DAC is selected, analog audio will be available on GPIO25 and GPIO26. The output resolution on these pins will always be limited to 8 bit because of the internal structure of the DACs.


After the program started, other bluetooth devices such as smart phones can discover this device named "ESP_SPEAKER", and after connection is established, audio data can be transmitted and there will occur a count of audio data packets printed.
