
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1
    3       WR_DIS.DIS_ICACHE               EFUSE_BLK0         2               1
    4       WR_DIS.DIS_USB_JTAG             EFUSE_BLK0         2               1
    5       WR_DIS.DIS_FORCE_DOWNLOAD       EFUSE_BLK0         2               1
    6       WR_DIS.DIS_TWAI                 EFUSE_BLK0         2               1
    7       WR_DIS.JTAG_SEL_ENABLE          EFUSE_BLK0         2               1
    8       WR_DIS.DIS_PAD_JTAG             EFUSE_BLK0         2               1
    9       WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    10      WR_DIS.WDT_DELAY_SEL            EFUSE_BLK0         3               1
    11      WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1
    12      WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1
    13      WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1
    14      WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1
    15      WR_DIS.KEY_PURPOSE_0            EFUSE_BLK0         8               1
    16      WR_DIS.KEY_PURPOSE_1            EFUSE_BLK0         9               1
    17      WR_DIS.KEY_PURPOSE_2            EFUSE_BLK0         10              1
    18      WR_DIS.KEY_PURPOSE_3            EFUSE_BLK0         11              1
    19      WR_DIS.KEY_PURPOSE_4            EFUSE_BLK0         12              1
    20      WR_DIS.KEY_PURPOSE_5            EFUSE_BLK0         13              1
    21      WR_DIS.SEC_DPA_LEVEL            EFUSE_BLK0         14              1
    22      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1
    23      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    24      WR_DIS.SPI_DOWNLOAD_MSPI_DIS    EFUSE_BLK0         17              1
    25      WR_DIS.FLASH_TPUW               EFUSE_BLK0         18              1
    26      WR_DIS.DIS_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    27      WR_DIS.DIS_DIRECT_BOOT          EFUSE_BLK0         18              1
    28      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    29      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    30      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    31      WR_DIS.UART_PRINT_CONTROL       EFUSE_BLK0         18              1
    32      WR_DIS.FORCE_SEND_RESUME        EFUSE_BLK0         18              1
    33      WR_DIS.SECURE_VERSION           EFUSE_BLK0         18              1
    34      WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE    EFUSE_BLK0         19              1
    35      WR_DIS.BLK1                     EFUSE_BLK0         20              1
    36      WR_DIS.MAC                      EFUSE_BLK0         20              1
    37      WR_DIS.MAC_EXT                  EFUSE_BLK0         20              1
    38      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1
    39      WR_DIS.BLOCK_SYS_DATA1          EFUSE_BLK0         21              1
    40      WR_DIS.BLOCK_USR_DATA           EFUSE_BLK0         22              1
    41      WR_DIS.CUSTOM_MAC               EFUSE_BLK0         22              1
    42      WR_DIS.BLOCK_KEY0               EFUSE_BLK0         23              1
    43      WR_DIS.BLOCK_KEY1               EFUSE_BLK0         24              1
    44      WR_DIS.BLOCK_KEY2               EFUSE_BLK0         25              1
    45      WR_DIS.BLOCK_KEY3               EFUSE_BLK0         26              1
    46      WR_DIS.BLOCK_KEY4               EFUSE_BLK0         27              1
    47      WR_DIS.BLOCK_KEY5               EFUSE_BLK0         28              1
    48      WR_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         29              1
    49      WR_DIS.USB_EXCHG_PINS           EFUSE_BLK0         30              1
    50      WR_DIS.VDD_SPI_AS_GPIO          EFUSE_BLK0         30              1
    51      WR_DIS.SOFT_DIS_JTAG            EFUSE_BLK0         31              1
    52      RD_DIS                          EFUSE_BLK0         32              7
    53      RD_DIS.BLOCK_KEY0               EFUSE_BLK0         32              1
    54      RD_DIS.BLOCK_KEY1               EFUSE_BLK0         33              1
    55      RD_DIS.BLOCK_KEY2               EFUSE_BLK0         34              1
    56      RD_DIS.BLOCK_KEY3               EFUSE_BLK0         35              1
    57      RD_DIS.BLOCK_KEY4               EFUSE_BLK0         36              1
    58      RD_DIS.BLOCK_KEY5               EFUSE_BLK0         37              1
    59      RD_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         38              1
    60      DIS_ICACHE                      EFUSE_BLK0         40              1
    61      DIS_USB_JTAG                    EFUSE_BLK0         41              1
    62      DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1
    63      SPI_DOWNLOAD_MSPI_DIS           EFUSE_BLK0         45              1
    64      DIS_TWAI                        EFUSE_BLK0         46              1
    65      JTAG_SEL_ENABLE                 EFUSE_BLK0         47              1
    66      SOFT_DIS_JTAG                   EFUSE_BLK0         48              3
    67      DIS_PAD_JTAG                    EFUSE_BLK0         51              1
    68      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         52              1
    69      USB_EXCHG_PINS                  EFUSE_BLK0         57              1
    70      VDD_SPI_AS_GPIO                 EFUSE_BLK0         58              1
    71      KM_DISABLE_DEPLOY_MODE          EFUSE_BLK0         64              4
    72      KM_RND_SWITCH_CYCLE             EFUSE_BLK0         68              2
    73      KM_DEPLOY_ONLY_ONCE             EFUSE_BLK0         70              4
    74      FORCE_USE_KEY_MANAGER_KEY       EFUSE_BLK0         74              4
    75      FORCE_DISABLE_SW_INIT_KEY       EFUSE_BLK0         78              1
    76      WDT_DELAY_SEL                   EFUSE_BLK0         80              2
    77      SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         82              3
    78      SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         85              1
    79      SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         86              1
    80      SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         87              1
    81      KEY_PURPOSE_0                   EFUSE_BLK0         88              4
    82      KEY_PURPOSE_1                   EFUSE_BLK0         92              4
    83      KEY_PURPOSE_2                   EFUSE_BLK0         96              4
    84      KEY_PURPOSE_3                   EFUSE_BLK0        100              4
    85      KEY_PURPOSE_4                   EFUSE_BLK0        104              4
    86      KEY_PURPOSE_5                   EFUSE_BLK0        108              4
    87      SEC_DPA_LEVEL                   EFUSE_BLK0        112              2
    88      SECURE_BOOT_EN                  EFUSE_BLK0        116              1
    89      SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1
    90      KM_XTS_KEY_LENGTH_256           EFUSE_BLK0        123              1
    91      FLASH_TPUW                      EFUSE_BLK0        124              4
    92      DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1
    93      DIS_DIRECT_BOOT                 EFUSE_BLK0        129              1
    94      DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        130              1
    95      LOCK_KM_KEY                     EFUSE_BLK0        131              1
    96      DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1
    97      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1
    98      UART_PRINT_CONTROL              EFUSE_BLK0        134              2
    99      FORCE_SEND_RESUME               EFUSE_BLK0        136              1
    100     SECURE_VERSION                  EFUSE_BLK0        137              16
    101     SECURE_BOOT_DISABLE_FAST_WAKE   EFUSE_BLK0        153              1
    102     HYS_EN_PAD                      EFUSE_BLK0        154              1
    103     XTS_DPA_PSEUDO_LEVEL            EFUSE_BLK0        155              2
    104     XTS_DPA_CLK_ENABLE              EFUSE_BLK0        157              1
    105     HUK_GEN_STATE                   EFUSE_BLK0        160              9
    106     XTAL_48M_SEL                    EFUSE_BLK0        169              3
    107     XTAL_48M_SEL_MODE               EFUSE_BLK0        172              1
    108     ECDSA_DISABLE_P192              EFUSE_BLK0        173              1
    109     ECC_FORCE_CONST_TIME            EFUSE_BLK0        174              1
    110     MAC                             EFUSE_BLK1         0               8
    111     MAC                             EFUSE_BLK1         8               8
    112     MAC                             EFUSE_BLK1         16              8
    113     MAC                             EFUSE_BLK1         24              8
    114     MAC                             EFUSE_BLK1         32              8
    115     MAC                             EFUSE_BLK1         40              8
    116     MAC_EXT                         EFUSE_BLK1         48              16
    117     SYS_DATA_PART2                  EFUSE_BLK10        0              256
    118     BLOCK_SYS_DATA1                 EFUSE_BLK2         0              256
    119     USER_DATA                       EFUSE_BLK3         0              256
    120     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48
    121     KEY0                            EFUSE_BLK4         0              256
    122     KEY1                            EFUSE_BLK5         0              256
    123     KEY2                            EFUSE_BLK6         0              256
    124     KEY3                            EFUSE_BLK7         0              256
    125     KEY4                            EFUSE_BLK8         0              256
    126     KEY5                            EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [40 41] [44 52] [57 58] [64 78] [80 113] [116 117] [123 157] [160 174]

    EFUSE_BLK1
    [0 63]

    EFUSE_BLK10
    [0 255]

    EFUSE_BLK2
    [0 255]

    EFUSE_BLK3
    [0 255] [200 247]

    EFUSE_BLK4
    [0 255]

    EFUSE_BLK5
    [0 255]

    EFUSE_BLK6
    [0 255]

    EFUSE_BLK7
    [0 255]

    EFUSE_BLK8
    [0 255]

    EFUSE_BLK9
    [0 255]
    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)
