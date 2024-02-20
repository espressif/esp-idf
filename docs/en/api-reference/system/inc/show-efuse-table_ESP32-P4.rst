
.. code-block:: none

    $ ./efuse_table_gen.py -t IDF_TARGET_PATH_NAME {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Parsing efuse CSV input file ../esp32p4/esp_efuse_table.csv ...
    Verifying efuse table...
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1
    3       WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1
    4       WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1
    5       WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1
    6       WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1
    7       WR_DIS.KEY_PURPOSE_0            EFUSE_BLK0         8               1
    8       WR_DIS.KEY_PURPOSE_1            EFUSE_BLK0         9               1
    9       WR_DIS.KEY_PURPOSE_2            EFUSE_BLK0         10              1
    10      WR_DIS.KEY_PURPOSE_3            EFUSE_BLK0         11              1
    11      WR_DIS.KEY_PURPOSE_4            EFUSE_BLK0         12              1
    12      WR_DIS.KEY_PURPOSE_5            EFUSE_BLK0         13              1
    13      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1
    14      WR_DIS.BLK1                     EFUSE_BLK0         20              1
    15      WR_DIS.MAC                      EFUSE_BLK0         20              1
    16      WR_DIS.MAC_EXT                  EFUSE_BLK0         20              1
    17      WR_DIS.BLOCK_SYS_DATA1          EFUSE_BLK0         21              1
    18      WR_DIS.BLOCK_USR_DATA           EFUSE_BLK0         22              1
    19      WR_DIS.BLOCK_KEY0               EFUSE_BLK0         23              1
    20      WR_DIS.BLOCK_KEY1               EFUSE_BLK0         24              1
    21      WR_DIS.BLOCK_KEY2               EFUSE_BLK0         25              1
    22      WR_DIS.BLOCK_KEY3               EFUSE_BLK0         26              1
    23      WR_DIS.BLOCK_KEY4               EFUSE_BLK0         27              1
    24      WR_DIS.BLOCK_KEY5               EFUSE_BLK0         28              1
    25      WR_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         29              1
    26      RD_DIS                          EFUSE_BLK0         32              7
    27      RD_DIS.BLOCK_KEY0               EFUSE_BLK0         32              1
    28      RD_DIS.BLOCK_KEY1               EFUSE_BLK0         33              1
    29      RD_DIS.BLOCK_KEY2               EFUSE_BLK0         34              1
    30      RD_DIS.BLOCK_KEY3               EFUSE_BLK0         35              1
    31      RD_DIS.BLOCK_KEY4               EFUSE_BLK0         36              1
    32      RD_DIS.BLOCK_KEY5               EFUSE_BLK0         37              1
    33      RD_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         38              1
    34      USB_DEVICE_EXCHG_PINS           EFUSE_BLK0         39              1
    35      USB_OTG11_EXCHG_PINS            EFUSE_BLK0         40              1
    36      DIS_USB_JTAG                    EFUSE_BLK0         41              1
    37      POWERGLITCH_EN                  EFUSE_BLK0         42              1
    38      DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1
    39      SPI_DOWNLOAD_MSPI_DIS           EFUSE_BLK0         45              1
    40      DIS_TWAI                        EFUSE_BLK0         46              1
    41      JTAG_SEL_ENABLE                 EFUSE_BLK0         47              1
    42      SOFT_DIS_JTAG                   EFUSE_BLK0         48              3
    43      DIS_PAD_JTAG                    EFUSE_BLK0         51              1
    44      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         52              1
    45      USB_PHY_SEL                     EFUSE_BLK0         57              1
    46      KM_HUK_GEN_STATE_LOW            EFUSE_BLK0         58              6
    47      KM_HUK_GEN_STATE_HIGH           EFUSE_BLK0         64              3
    48      KM_RND_SWITCH_CYCLE             EFUSE_BLK0         67              2
    49      KM_DEPLOY_ONLY_ONCE             EFUSE_BLK0         69              4
    50      FORCE_USE_KEY_MANAGER_KEY       EFUSE_BLK0         73              4
    51      FORCE_DISABLE_SW_INIT_KEY       EFUSE_BLK0         77              1
    52      XTS_KEY_LENGTH_256              EFUSE_BLK0         78              1
    53      WDT_DELAY_SEL                   EFUSE_BLK0         80              2
    54      SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         82              3
    55      SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         85              1
    56      SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         86              1
    57      SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         87              1
    58      KEY_PURPOSE_0                   EFUSE_BLK0         88              4
    59      KEY_PURPOSE_1                   EFUSE_BLK0         92              4
    60      KEY_PURPOSE_2                   EFUSE_BLK0         96              4
    61      KEY_PURPOSE_3                   EFUSE_BLK0        100              4
    62      KEY_PURPOSE_4                   EFUSE_BLK0        104              4
    63      KEY_PURPOSE_5                   EFUSE_BLK0        108              4
    64      SEC_DPA_LEVEL                   EFUSE_BLK0        112              2
    65      ECDSA_ENABLE_SOFT_K             EFUSE_BLK0        114              1
    66      CRYPT_DPA_ENABLE                EFUSE_BLK0        115              1
    67      SECURE_BOOT_EN                  EFUSE_BLK0        116              1
    68      SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1
    69      FLASH_TYPE                      EFUSE_BLK0        119              1
    70      FLASH_PAGE_SIZE                 EFUSE_BLK0        120              2
    71      FLASH_ECC_EN                    EFUSE_BLK0        122              1
    72      DIS_USB_OTG_DOWNLOAD_MODE       EFUSE_BLK0        123              1
    73      FLASH_TPUW                      EFUSE_BLK0        124              4
    74      DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1
    75      DIS_DIRECT_BOOT                 EFUSE_BLK0        129              1
    76      DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        130              1
    77      LOCK_KM_KEY                     EFUSE_BLK0        131              1
    78      DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1
    79      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1
    80      UART_PRINT_CONTROL              EFUSE_BLK0        134              2
    81      FORCE_SEND_RESUME               EFUSE_BLK0        136              1
    82      SECURE_VERSION                  EFUSE_BLK0        137              16
    83      SECURE_BOOT_DISABLE_FAST_WAKE   EFUSE_BLK0        153              1
    84      HYS_EN_PAD                      EFUSE_BLK0        154              1
    85      DCDC_VSET                       EFUSE_BLK0        155              5
    86      PXA0_TIEH_SEL_0                 EFUSE_BLK0        160              2
    87      PXA0_TIEH_SEL_1                 EFUSE_BLK0        162              2
    88      PXA0_TIEH_SEL_2                 EFUSE_BLK0        164              2
    89      PXA0_TIEH_SEL_3                 EFUSE_BLK0        166              2
    90      KM_DISABLE_DEPLOY_MODE          EFUSE_BLK0        168              4
    91      HP_PWR_SRC_SEL                  EFUSE_BLK0        178              1
    92      DCDC_VSET_EN                    EFUSE_BLK0        179              1
    93      DIS_WDT                         EFUSE_BLK0        180              1
    94      DIS_SWD                         EFUSE_BLK0        181              1
    95      MAC                             EFUSE_BLK1         0               8
    96      MAC                             EFUSE_BLK1         8               8
    97      MAC                             EFUSE_BLK1         16              8
    98      MAC                             EFUSE_BLK1         24              8
    99      MAC                             EFUSE_BLK1         32              8
    100     MAC                             EFUSE_BLK1         40              8
    101     MAC_EXT                         EFUSE_BLK1         48              8
    102     MAC_EXT                         EFUSE_BLK1         56              8
    103     SYS_DATA_PART2                  EFUSE_BLK10        0              256
    104     BLOCK_SYS_DATA1                 EFUSE_BLK2         0              256
    105     USER_DATA                       EFUSE_BLK3         0              256
    106     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48
    107     KEY0                            EFUSE_BLK4         0              256
    108     KEY1                            EFUSE_BLK5         0              256
    109     KEY2                            EFUSE_BLK6         0              256
    110     KEY3                            EFUSE_BLK7         0              256
    111     KEY4                            EFUSE_BLK8         0              256
    112     KEY5                            EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [4 13] [15 15] [20 20] [20 20] [20 29] [32 38] [32 42] [44 52] [57 78] [80 117] [119 171] [178 181]

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
