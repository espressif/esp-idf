.. code-block:: none

    idf.py efuse-dump

    Executing action: efuse-dump
    Running espefuse.py in directory <project-directory>
    Executing "espefuse.py dump --chip esp32"...
    espefuse.py v4.6-dev
    Connecting....
    BLOCK0          (                ) [0 ] read_regs: 00000000 7e5a6e58 00e294b9 0000a200 00000333 00100000 00000004
    BLOCK1          (flash_encryption) [1 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK2          (secure_boot_v1 s) [2 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK3          (                ) [3 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

    EFUSE_REG_DEC_STATUS        0x00000000

    === Run "dump" command ===
