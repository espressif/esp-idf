ESP-IDF BT-SPP-ACCEPTOR demo
======================

Demo of SPP acceptor role

This is the demo for user to use ESP_APIs to create a SPP acceptor.

Options choose step:
    1. make menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and "SPP Profile"
    4. choose your options.

Then set SPP_SHOW_MODE as SPP_SHOW_DATA or SPP_SHOW_SPEED in code(should be same with bt_spp_initator).

After the program started, bt_spp_initator will connect it and send data.