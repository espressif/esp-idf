.. output_log

.. code-block:: none

    ...
    esptool --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 40m --flash-size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32 on /dev/ttyUSB0:
    Chip type:          ESP32-U4WDH (revision v3.0)
    Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Embedded Flash, Vref calibration in eFuse, Coding Scheme None
    Crystal frequency:  40MHz
    MAC:                24:0a:c4:05:b9:14

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00001000 to 0x00007fff...
    Flash will be erased from 0x00010000 to 0x00031fff...
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (279.3 kbit/s).
    Hash of data verified.
    SHA digest in image updated.
    Wrote 26352 bytes (16660 compressed) at 0x00001000 in 0.9 seconds (239.3 kbit/s).
    Hash of data verified.
    Wrote 137312 bytes (78774 compressed) at 0x00010000 in 2.2 seconds (490.8 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
