ESP-IDF BT-SPP-VFS-INITATOR demo
======================

Demo of SPP initator role

This is the demo for user to use ESP_APIs to create a SPP initator.

Options choose step:
    1. idf.py menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and "SPP Profile"
    4. choose your options.

After the program started, It will connect to bt_spp_vfs_acceptor and send data.
