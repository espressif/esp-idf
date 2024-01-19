
.. code-block:: none

    $ ./efuse_table_gen.py -t IDF_TARGET_PATH_NAME {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Parsing efuse CSV input file esp32c6/esp_efuse_table.csv ...
    Verifying efuse table...
    Sorted efuse table:
    #       field_name                              efuse_block     bit_start       bit_count
    1       WR_DIS                                  EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                           EFUSE_BLK0         0               1
    3       WR_DIS.CRYPT_DPA_ENABLE                 EFUSE_BLK0         1               1
    4       WR_DIS.SWAP_UART_SDIO_EN                EFUSE_BLK0         2               1
    5       WR_DIS.DIS_ICACHE                       EFUSE_BLK0         2               1
    6       WR_DIS.DIS_USB_JTAG                     EFUSE_BLK0         2               1
    7       WR_DIS.DIS_DOWNLOAD_ICACHE              EFUSE_BLK0         2               1
    8       WR_DIS.DIS_USB_SERIAL_JTAG              EFUSE_BLK0         2               1
    9       WR_DIS.DIS_FORCE_DOWNLOAD               EFUSE_BLK0         2               1
    10      WR_DIS.DIS_TWAI                         EFUSE_BLK0         2               1
    11      WR_DIS.JTAG_SEL_ENABLE                  EFUSE_BLK0         2               1
    12      WR_DIS.DIS_PAD_JTAG                     EFUSE_BLK0         2               1
    13      WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    14      WR_DIS.WDT_DELAY_SEL                    EFUSE_BLK0         3               1
    15      WR_DIS.SPI_BOOT_CRYPT_CNT               EFUSE_BLK0         4               1
    16      WR_DIS.SECURE_BOOT_KEY_REVOKE0          EFUSE_BLK0         5               1
    17      WR_DIS.SECURE_BOOT_KEY_REVOKE1          EFUSE_BLK0         6               1
    18      WR_DIS.SECURE_BOOT_KEY_REVOKE2          EFUSE_BLK0         7               1
    19      WR_DIS.KEY_PURPOSE_0                    EFUSE_BLK0         8               1
    20      WR_DIS.KEY_PURPOSE_1                    EFUSE_BLK0         9               1
    21      WR_DIS.KEY_PURPOSE_2                    EFUSE_BLK0         10              1
    22      WR_DIS.KEY_PURPOSE_3                    EFUSE_BLK0         11              1
    23      WR_DIS.KEY_PURPOSE_4                    EFUSE_BLK0         12              1
    24      WR_DIS.KEY_PURPOSE_5                    EFUSE_BLK0         13              1
    25      WR_DIS.SEC_DPA_LEVEL                    EFUSE_BLK0         14              1
    26      WR_DIS.SECURE_BOOT_EN                   EFUSE_BLK0         15              1
    27      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    28      WR_DIS.SPI_DOWNLOAD_MSPI_DIS            EFUSE_BLK0         17              1
    29      WR_DIS.FLASH_TPUW                       EFUSE_BLK0         18              1
    30      WR_DIS.DIS_DOWNLOAD_MODE                EFUSE_BLK0         18              1
    31      WR_DIS.DIS_DIRECT_BOOT                  EFUSE_BLK0         18              1
    32      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    33      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE EFUSE_BLK0        18              1
    34      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    35      WR_DIS.UART_PRINT_CONTROL               EFUSE_BLK0         18              1
    36      WR_DIS.FORCE_SEND_RESUME                EFUSE_BLK0         18              1
    37      WR_DIS.SECURE_VERSION                   EFUSE_BLK0         18              1
    38      WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE    EFUSE_BLK0         19              1
    39      WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0         19              1
    40      WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0         19              1
    41      WR_DIS.BLK1                             EFUSE_BLK0         20              1
    42      WR_DIS.MAC                              EFUSE_BLK0         20              1
    43      WR_DIS.MAC_EXT                          EFUSE_BLK0         20              1
    44      WR_DIS.WAFER_VERSION_MINOR              EFUSE_BLK0         20              1
    45      WR_DIS.WAFER_VERSION_MAJOR              EFUSE_BLK0         20              1
    46      WR_DIS.PKG_VERSION                      EFUSE_BLK0         20              1
    47      WR_DIS.BLK_VERSION_MINOR                EFUSE_BLK0         20              1
    48      WR_DIS.BLK_VERSION_MAJOR                EFUSE_BLK0         20              1
    49      WR_DIS.FLASH_CAP                        EFUSE_BLK0         20              1
    50      WR_DIS.FLASH_TEMP                       EFUSE_BLK0         20              1
    51      WR_DIS.FLASH_VENDOR                     EFUSE_BLK0         20              1
    52      WR_DIS.SYS_DATA_PART1                   EFUSE_BLK0         21              1
    53      WR_DIS.OPTIONAL_UNIQUE_ID               EFUSE_BLK0         21              1
    54      WR_DIS.BLOCK_USR_DATA                   EFUSE_BLK0         22              1
    55      WR_DIS.CUSTOM_MAC                       EFUSE_BLK0         22              1
    56      WR_DIS.BLOCK_KEY0                       EFUSE_BLK0         23              1
    57      WR_DIS.BLOCK_KEY1                       EFUSE_BLK0         24              1
    58      WR_DIS.BLOCK_KEY2                       EFUSE_BLK0         25              1
    59      WR_DIS.BLOCK_KEY3                       EFUSE_BLK0         26              1
    60      WR_DIS.BLOCK_KEY4                       EFUSE_BLK0         27              1
    61      WR_DIS.BLOCK_KEY5                       EFUSE_BLK0         28              1
    62      WR_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         29              1
    63      WR_DIS.USB_EXCHG_PINS                   EFUSE_BLK0         30              1
    64      WR_DIS.VDD_SPI_AS_GPIO                  EFUSE_BLK0         30              1
    65      WR_DIS.SOFT_DIS_JTAG                    EFUSE_BLK0         31              1
    66      RD_DIS                                  EFUSE_BLK0         32              7
    67      RD_DIS.BLOCK_KEY0                       EFUSE_BLK0         32              1
    68      RD_DIS.BLOCK_KEY1                       EFUSE_BLK0         33              1
    69      RD_DIS.BLOCK_KEY2                       EFUSE_BLK0         34              1
    70      RD_DIS.BLOCK_KEY3                       EFUSE_BLK0         35              1
    71      RD_DIS.BLOCK_KEY4                       EFUSE_BLK0         36              1
    72      RD_DIS.BLOCK_KEY5                       EFUSE_BLK0         37              1
    73      RD_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         38              1
    74      SWAP_UART_SDIO_EN                       EFUSE_BLK0         39              1
    75      DIS_ICACHE                              EFUSE_BLK0         40              1
    76      DIS_USB_JTAG                            EFUSE_BLK0         41              1
    77      DIS_DOWNLOAD_ICACHE                     EFUSE_BLK0         42              1
    78      DIS_USB_SERIAL_JTAG                     EFUSE_BLK0         43              1
    79      DIS_FORCE_DOWNLOAD                      EFUSE_BLK0         44              1
    80      SPI_DOWNLOAD_MSPI_DIS                   EFUSE_BLK0         45              1
    81      DIS_TWAI                                EFUSE_BLK0         46              1
    82      JTAG_SEL_ENABLE                         EFUSE_BLK0         47              1
    83      SOFT_DIS_JTAG                           EFUSE_BLK0         48              3
    84      DIS_PAD_JTAG                            EFUSE_BLK0         51              1
    85      DIS_DOWNLOAD_MANUAL_ENCRYPT             EFUSE_BLK0         52              1
    86      USB_EXCHG_PINS                          EFUSE_BLK0         57              1
    87      VDD_SPI_AS_GPIO                         EFUSE_BLK0         58              1
    88      WDT_DELAY_SEL                           EFUSE_BLK0         80              2
    89      SPI_BOOT_CRYPT_CNT                      EFUSE_BLK0         82              3
    90      SECURE_BOOT_KEY_REVOKE0                 EFUSE_BLK0         85              1
    91      SECURE_BOOT_KEY_REVOKE1                 EFUSE_BLK0         86              1
    92      SECURE_BOOT_KEY_REVOKE2                 EFUSE_BLK0         87              1
    93      KEY_PURPOSE_0                           EFUSE_BLK0         88              4
    94      KEY_PURPOSE_1                           EFUSE_BLK0         92              4
    95      KEY_PURPOSE_2                           EFUSE_BLK0         96              4
    96      KEY_PURPOSE_3                           EFUSE_BLK0        100              4
    97      KEY_PURPOSE_4                           EFUSE_BLK0        104              4
    98      KEY_PURPOSE_5                           EFUSE_BLK0        108              4
    99      SEC_DPA_LEVEL                           EFUSE_BLK0        112              2
    100     CRYPT_DPA_ENABLE                        EFUSE_BLK0        114              1
    101     SECURE_BOOT_EN                          EFUSE_BLK0        116              1
    102     SECURE_BOOT_AGGRESSIVE_REVOKE           EFUSE_BLK0        117              1
    103     FLASH_TPUW                              EFUSE_BLK0        124              4
    104     DIS_DOWNLOAD_MODE                       EFUSE_BLK0        128              1
    105     DIS_DIRECT_BOOT                         EFUSE_BLK0        129              1
    106     DIS_USB_SERIAL_JTAG_ROM_PRINT           EFUSE_BLK0        130              1
    107     DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1
    108     ENABLE_SECURITY_DOWNLOAD                EFUSE_BLK0        133              1
    109     UART_PRINT_CONTROL                      EFUSE_BLK0        134              2
    110     FORCE_SEND_RESUME                       EFUSE_BLK0        141              1
    111     SECURE_VERSION                          EFUSE_BLK0        142              16
    112     SECURE_BOOT_DISABLE_FAST_WAKE           EFUSE_BLK0        158              1
    113     DISABLE_WAFER_VERSION_MAJOR             EFUSE_BLK0        160              1
    114     DISABLE_BLK_VERSION_MAJOR               EFUSE_BLK0        161              1
    115     MAC                                     EFUSE_BLK1         0               8
    116     MAC                                     EFUSE_BLK1         8               8
    117     MAC                                     EFUSE_BLK1         16              8
    118     MAC                                     EFUSE_BLK1         24              8
    119     MAC                                     EFUSE_BLK1         32              8
    120     MAC                                     EFUSE_BLK1         40              8
    121     MAC_EXT                                 EFUSE_BLK1         48              16
    122     WAFER_VERSION_MINOR                     EFUSE_BLK1        114              4
    123     WAFER_VERSION_MAJOR                     EFUSE_BLK1        118              2
    124     PKG_VERSION                             EFUSE_BLK1        120              3
    125     BLK_VERSION_MINOR                       EFUSE_BLK1        123              3
    126     BLK_VERSION_MAJOR                       EFUSE_BLK1        126              2
    127     FLASH_CAP                               EFUSE_BLK1        128              3
    128     FLASH_TEMP                              EFUSE_BLK1        131              2
    129     FLASH_VENDOR                            EFUSE_BLK1        133              3
    130     SYS_DATA_PART2                          EFUSE_BLK10        0              256
    131     OPTIONAL_UNIQUE_ID                      EFUSE_BLK2         0              128
    132     USER_DATA                               EFUSE_BLK3         0              256
    133     USER_DATA.MAC_CUSTOM                    EFUSE_BLK3        200              48
    134     KEY0                                    EFUSE_BLK4         0              256
    135     KEY1                                    EFUSE_BLK5         0              256
    136     KEY2                                    EFUSE_BLK6         0              256
    137     KEY3                                    EFUSE_BLK7         0              256
    138     KEY4                                    EFUSE_BLK8         0              256
    139     KEY5                                    EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 2] [2 2] ... [30 38] [32 52] [57 58] [80 114] [116 117] [124 130] [132 135] [141 158] [160 161]
    EFUSE_BLK1
    [0 63] [114 135]
    EFUSE_BLK10
    [0 255]
    EFUSE_BLK2
    [0 127]
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
