Bootloader
=====================

Bootloader performs the following functions:

1. Minimal initial configuration of internal modules;
2. Select the application partition to boot, based on the partition table and ota_data (if any);
3. Load this image to RAM (IRAM & DRAM) and transfer management to it.

Bootloader is located at the address `0x1000` in the flash.

Customer bootloader
---------------------
The current bootloader implementation allows the customer to override it. To do this, you must copy the folder `/esp-idf/components/bootloader` and then edit `/your_project/components/bootloader/subproject/main/bootloader_main.c`.

