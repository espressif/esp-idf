.. output_log

.. code-block:: none

    esptool --chip esp32c61 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x0 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.2.dev1
    Connected to ESP32-C61 on /dev/ttyUSB0:
    Chip type:          ESP32-C61 (revision v1.0)
    Features:           Wi-Fi 6, BT 5 (LE), Single Core, 160MHz
    Crystal frequency:  40MHz
    MAC:                30:ed:a0:eb:e1:08

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00000000 to 0x00005fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x0002dfff...
    SHA digest in image updated.
    Wrote 22528 bytes (13858 compressed) at 0x00000000 in 0.5 seconds (358.5 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (684.7 kbit/s).
    Hash of data verified.
    Wrote 121632 bytes (65182 compressed) at 0x00010000 in 1.9 seconds (507.7 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
