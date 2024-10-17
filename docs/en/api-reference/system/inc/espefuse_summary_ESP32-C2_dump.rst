.. code-block:: none

    idf.py efuse-dump

    Executing action: efuse-dump
    Running espefuse.py in directory <project-directory>
    Executing "espefuse.py dump --chip esp32c2"...
    espefuse.py v4.6-dev
    Connecting....
    BLOCK0          (BLOCK0          ) [0 ] read_regs: 00000000 00000000
    BLOCK1          (BLOCK1          ) [1 ] read_regs: 00000000 00000000 00000000
    BLOCK2          (BLOCK2          ) [2 ] read_regs: 8d5c4b94 8252083a 5c01e953 80d0a824 c0860b18 00006890 00000000 4b000000
    BLOCK_KEY0      (BLOCK3          ) [3 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

    BLOCK0          (BLOCK0          ) [0 ] err__regs: 00000000 00000000
    EFUSE_RD_RS_ERR_REG         0x00000000

    === Run "dump" command ===
