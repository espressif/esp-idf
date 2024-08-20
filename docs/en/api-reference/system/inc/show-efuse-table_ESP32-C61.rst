
.. code-block:: none

    $ ./efuse_table_gen.py -t IDF_TARGET_PATH_NAME {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Parsing efuse CSV input file esp32c61/esp_efuse_table.csv ...
    Verifying efuse table...
    Sorted efuse table:
    #       field_name                              efuse_block     bit_start       bit_count
    1       WR_DIS                                  EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                           EFUSE_BLK0         0               1
    3       WR_DIS.DIS_ICACHE                       EFUSE_BLK0         2               1
    4       WR_DIS.DIS_USB_JTAG                     EFUSE_BLK0         2               1
    5       WR_DIS.DIS_FORCE_DOWNLOAD               EFUSE_BLK0         2               1
    6       WR_DIS.JTAG_SEL_ENABLE                  EFUSE_BLK0         2               1
    7       WR_DIS.DIS_PAD_JTAG                     EFUSE_BLK0         2               1
    8       WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    9       WR_DIS.WDT_DELAY_SEL                    EFUSE_BLK0         3               1
    10      WR_DIS.SPI_BOOT_CRYPT_CNT               EFUSE_BLK0         4               1
    11      WR_DIS.SECURE_BOOT_KEY_REVOKE0          EFUSE_BLK0         5               1
    12      WR_DIS.SECURE_BOOT_KEY_REVOKE1          EFUSE_BLK0         6               1
    13      WR_DIS.SECURE_BOOT_KEY_REVOKE2          EFUSE_BLK0         7               1
    14      WR_DIS.KEY_PURPOSE_0                    EFUSE_BLK0         8               1
    15      WR_DIS.KEY_PURPOSE_1                    EFUSE_BLK0         9               1
    16      WR_DIS.KEY_PURPOSE_2                    EFUSE_BLK0         10              1
    17      WR_DIS.KEY_PURPOSE_3                    EFUSE_BLK0         11              1
    18      WR_DIS.KEY_PURPOSE_4                    EFUSE_BLK0         12              1
    19      WR_DIS.KEY_PURPOSE_5                    EFUSE_BLK0         13              1
    20      WR_DIS.SEC_DPA_LEVEL                    EFUSE_BLK0         14              1
    21      WR_DIS.SECURE_BOOT_EN                   EFUSE_BLK0         15              1
    22      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    23      WR_DIS.SPI_DOWNLOAD_MSPI_DIS            EFUSE_BLK0         17              1
    24      WR_DIS.FLASH_TPUW                       EFUSE_BLK0         18              1
    25      WR_DIS.DIS_DOWNLOAD_MODE                EFUSE_BLK0         18              1
    26      WR_DIS.DIS_DIRECT_BOOT                  EFUSE_BLK0         18              1
    27      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    28      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE EFUSE_BLK0        18              1
    29      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    30      WR_DIS.UART_PRINT_CONTROL               EFUSE_BLK0         18              1
    31      WR_DIS.FORCE_SEND_RESUME                EFUSE_BLK0         18              1
    32      WR_DIS.SECURE_VERSION                   EFUSE_BLK0         18              1
    33      WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE    EFUSE_BLK0         19              1
    34      WR_DIS.BLK1                             EFUSE_BLK0         20              1
    35      WR_DIS.MAC                              EFUSE_BLK0         20              1
    36      WR_DIS.SYS_DATA_PART1                   EFUSE_BLK0         21              1
    37      WR_DIS.BLOCK_SYS_DATA1                  EFUSE_BLK0         21              1
    38      WR_DIS.BLOCK_USR_DATA                   EFUSE_BLK0         22              1
    39      WR_DIS.CUSTOM_MAC                       EFUSE_BLK0         22              1
    40      WR_DIS.BLOCK_KEY0                       EFUSE_BLK0         23              1
    41      WR_DIS.BLOCK_KEY1                       EFUSE_BLK0         24              1
    42      WR_DIS.BLOCK_KEY2                       EFUSE_BLK0         25              1
    43      WR_DIS.BLOCK_KEY3                       EFUSE_BLK0         26              1
    44      WR_DIS.BLOCK_KEY4                       EFUSE_BLK0         27              1
    45      WR_DIS.BLOCK_KEY5                       EFUSE_BLK0         28              1
    46      WR_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         29              1
    47      WR_DIS.USB_EXCHG_PINS                   EFUSE_BLK0         30              1
    48      WR_DIS.VDD_SPI_AS_GPIO                  EFUSE_BLK0         30              1
    49      RD_DIS                                  EFUSE_BLK0         32              7
    50      RD_DIS.BLOCK_KEY0                       EFUSE_BLK0         32              1
    51      RD_DIS.BLOCK_KEY1                       EFUSE_BLK0         33              1
    52      RD_DIS.BLOCK_KEY2                       EFUSE_BLK0         34              1
    53      RD_DIS.BLOCK_KEY3                       EFUSE_BLK0         35              1
    54      RD_DIS.BLOCK_KEY4                       EFUSE_BLK0         36              1
    55      RD_DIS.BLOCK_KEY5                       EFUSE_BLK0         37              1
    56      RD_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         38              1
    57      DIS_ICACHE                              EFUSE_BLK0         39              1
    58      DIS_USB_JTAG                            EFUSE_BLK0         40              1
    59      DIS_FORCE_DOWNLOAD                      EFUSE_BLK0         42              1
    60      SPI_DOWNLOAD_MSPI_DIS                   EFUSE_BLK0         43              1
    61      JTAG_SEL_ENABLE                         EFUSE_BLK0         44              1
    62      DIS_PAD_JTAG                            EFUSE_BLK0         45              1
    63      DIS_DOWNLOAD_MANUAL_ENCRYPT             EFUSE_BLK0         46              1
    64      USB_EXCHG_PINS                          EFUSE_BLK0         51              1
    65      VDD_SPI_AS_GPIO                         EFUSE_BLK0         52              1
    66      WDT_DELAY_SEL                           EFUSE_BLK0         53              2
    67      SPI_BOOT_CRYPT_CNT                      EFUSE_BLK0         55              3
    68      SECURE_BOOT_KEY_REVOKE0                 EFUSE_BLK0         58              1
    69      SECURE_BOOT_KEY_REVOKE1                 EFUSE_BLK0         59              1
    70      SECURE_BOOT_KEY_REVOKE2                 EFUSE_BLK0         60              1
    71      KEY_PURPOSE_0                           EFUSE_BLK0         64              4
    72      KEY_PURPOSE_1                           EFUSE_BLK0         68              4
    73      KEY_PURPOSE_2                           EFUSE_BLK0         72              4
    74      KEY_PURPOSE_3                           EFUSE_BLK0         76              4
    75      KEY_PURPOSE_4                           EFUSE_BLK0         80              4
    76      KEY_PURPOSE_5                           EFUSE_BLK0         84              4
    77      SEC_DPA_LEVEL                           EFUSE_BLK0         88              2
    78      SECURE_BOOT_EN                          EFUSE_BLK0         90              1
    79      SECURE_BOOT_AGGRESSIVE_REVOKE           EFUSE_BLK0         91              1
    80      FLASH_TPUW                              EFUSE_BLK0         92              4
    81      DIS_DOWNLOAD_MODE                       EFUSE_BLK0         96              1
    82      DIS_DIRECT_BOOT                         EFUSE_BLK0         97              1
    83      DIS_USB_SERIAL_JTAG_ROM_PRINT           EFUSE_BLK0         98              1
    84      DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0         99              1
    85      ENABLE_SECURITY_DOWNLOAD                EFUSE_BLK0        100              1
    86      UART_PRINT_CONTROL                      EFUSE_BLK0        101              2
    87      FORCE_SEND_RESUME                       EFUSE_BLK0        103              1
    88      SECURE_VERSION                          EFUSE_BLK0        104              16
    89      SECURE_BOOT_DISABLE_FAST_WAKE           EFUSE_BLK0        120              1
    90      HYS_EN_PAD                              EFUSE_BLK0        121              1
    91      XTS_DPA_CLK_ENABLE                      EFUSE_BLK0        122              1
    92      XTS_DPA_PSEUDO_LEVEL                    EFUSE_BLK0        123              2
    93      DIS_WIFI6                               EFUSE_BLK0        125              1
    94      ECDSA_DISABLE_P192                      EFUSE_BLK0        126              1
    95      ECC_FORCE_CONST_TIME                    EFUSE_BLK0        127              1
    96      MAC                                     EFUSE_BLK1         0               8
    97      MAC                                     EFUSE_BLK1         8               8
    98      MAC                                     EFUSE_BLK1         16              8
    99      MAC                                     EFUSE_BLK1         24              8
    100     MAC                                     EFUSE_BLK1         32              8
    101     MAC                                     EFUSE_BLK1         40              8
    102     SYS_DATA_PART2                          EFUSE_BLK10        0              256
    103     BLOCK_SYS_DATA1                         EFUSE_BLK2         0              256
    104     USER_DATA                               EFUSE_BLK3         0              256
    105     USER_DATA.MAC_CUSTOM                    EFUSE_BLK3        200              48
    106     KEY0                                    EFUSE_BLK4         0              256
    107     KEY1                                    EFUSE_BLK5         0              256
    108     KEY2                                    EFUSE_BLK6         0              256
    109     KEY3                                    EFUSE_BLK7         0              256
    110     KEY4                                    EFUSE_BLK8         0              256
    111     KEY5                                    EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [20 21] [21 22] [22 30] [30 30] [32 38] [32 40] [42 46] [51 60] [64 127]
    EFUSE_BLK1
    [0 47]
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
