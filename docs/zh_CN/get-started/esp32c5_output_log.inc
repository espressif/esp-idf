.. output_log

.. code-block:: none

    ...
    esptool --chip esp32c5 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset --no-stub write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x2000 bootloader/bootloader.bin 0x10000 hello_world.bin 0x8000 partition_table/partition-table.bin
    esptool v5.0.2
    Connected to ESP32-C5 on /dev/ttyUSB0:
    Chip type:          ESP32-C5 (revision v1.0)
    Features:           Wi-Fi 6 (dual-band), BT 5 (LE), IEEE802.15.4, Single Core + LP Core, 240MHz
    Crystal frequency:  48MHz
    MAC:                30:ed:a0:ff:fe:e0:af:38
    BASE MAC:           30:ed:a0:e0:af:38
    MAC_EXT:            ff:fe

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00002000 to 0x00007fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00032fff...
    SHA digest in image updated.
    Wrote 21888 bytes (13489 compressed) at 0x00002000 in 0.5 seconds (348.1 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (316.0 kbit/s).
    Hash of data verified.
    Wrote 141248 bytes (75783 compressed) at 0x00010000 in 2.1 seconds (536.0 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
