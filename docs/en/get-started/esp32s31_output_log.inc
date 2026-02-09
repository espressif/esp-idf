.. output_log

.. code-block:: none

    ...
    esptool --chip esp32s31 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x0 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-S31 on /dev/ttyUSB0:
    Chip type:          ESP32-S31 (revision v0.0)
    Features:           Wi-Fi, BT 5 (LE), IEEE802.15.4, Dual Core, 240MHz, Embedded PSRAM
    Crystal frequency:  40MHz
    MAC:                f4:12:fa:87:c6:dc

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00000000 to 0x00005fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00037fff...
    SHA digest in image updated.
    Wrote 21280 bytes (13578 compressed) at 0x00000000 in 0.6 seconds (298.1 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (315.1 kbit/s).
    Hash of data verified.
    Wrote 161744 bytes (90141 compressed) at 0x00010000 in 2.5 seconds (513.7 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
