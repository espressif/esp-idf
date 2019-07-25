ESP-IDF BT-SPP-VFS-ACCEPTOR demo
======================

Demo of SPP acceptor role

This is the demo for user to use ESP_APIs to create a SPP acceptor.

Options choose step:
    1. idf.py menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and "SPP Profile"
    4. choose your options.

After the program started, bt_spp_vfs_initator will connect it and send data.
