.. output_log

.. code-block:: none

    ...
    esptool --chip esp32c3 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x0 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-C3 on /dev/ttyUSB0:
    Chip type:          ESP32-C3 (QFN32) (revision v1.0)
    Features:           Wi-Fi, BT 5 (LE), Single Core, 160MHz, Embedded Flash 4MB (XMC)
    Crystal frequency:  40MHz
    MAC:                60:55:f9:f7:7d:20

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00000000 to 0x00005fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00031fff...
    SHA digest in image updated.
    Wrote 21072 bytes (13320 compressed) at 0x00000000 in 0.7 seconds (232.1 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (240.8 kbit/s).
    Hash of data verified.
    Wrote 135328 bytes (74697 compressed) at 0x00010000 in 2.6 seconds (419.7 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
