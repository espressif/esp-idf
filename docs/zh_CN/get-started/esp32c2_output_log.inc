.. output_log

.. code-block:: none

    ...
    esptool --chip esp32c2 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 60m --flash-size 2MB 0x0 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-C2 on /dev/ttyUSB0:
    Chip type:          ESP32-C2 (revision v2.0)
    Features:           Wi-Fi, BT 5 (LE), Single Core, 120MHz
    Crystal frequency:  40MHz
    MAC:                10:97:bd:f5:36:98

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00000000 to 0x00004fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00028fff...
    SHA digest in image updated.
    Wrote 19680 bytes (12357 compressed) at 0x00000000 in 0.5 seconds (292.1 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (296.4 kbit/s).
    Hash of data verified.
    Wrote 98416 bytes (52689 compressed) at 0x00010000 in 1.8 seconds (438.9 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
