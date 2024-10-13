
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                              efuse_block     bit_start       bit_count
    1       WR_DIS                                  EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                           EFUSE_BLK0         0               1
    3       WR_DIS.DIS_ICACHE                       EFUSE_BLK0         2               1
    4       WR_DIS.DIS_USB_JTAG                     EFUSE_BLK0         2               1
    5       WR_DIS.DIS_DOWNLOAD_ICACHE              EFUSE_BLK0         2               1
    6       WR_DIS.DIS_USB_SERIAL_JTAG              EFUSE_BLK0         2               1
    7       WR_DIS.DIS_FORCE_DOWNLOAD               EFUSE_BLK0         2               1
    8       WR_DIS.DIS_TWAI                         EFUSE_BLK0         2               1
    9       WR_DIS.JTAG_SEL_ENABLE                  EFUSE_BLK0         2               1
    10      WR_DIS.DIS_PAD_JTAG                     EFUSE_BLK0         2               1
    11      WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    12      WR_DIS.WDT_DELAY_SEL                    EFUSE_BLK0         3               1
    13      WR_DIS.SPI_BOOT_CRYPT_CNT               EFUSE_BLK0         4               1
    14      WR_DIS.SECURE_BOOT_KEY_REVOKE0          EFUSE_BLK0         5               1
    15      WR_DIS.SECURE_BOOT_KEY_REVOKE1          EFUSE_BLK0         6               1
    16      WR_DIS.SECURE_BOOT_KEY_REVOKE2          EFUSE_BLK0         7               1
    17      WR_DIS.KEY_PURPOSE_0                    EFUSE_BLK0         8               1
    18      WR_DIS.KEY_PURPOSE_1                    EFUSE_BLK0         9               1
    19      WR_DIS.KEY_PURPOSE_2                    EFUSE_BLK0         10              1
    20      WR_DIS.KEY_PURPOSE_3                    EFUSE_BLK0         11              1
    21      WR_DIS.KEY_PURPOSE_4                    EFUSE_BLK0         12              1
    22      WR_DIS.KEY_PURPOSE_5                    EFUSE_BLK0         13              1
    23      WR_DIS.SECURE_BOOT_EN                   EFUSE_BLK0         15              1
    24      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    25      WR_DIS.FLASH_TPUW                       EFUSE_BLK0         18              1
    26      WR_DIS.DIS_DOWNLOAD_MODE                EFUSE_BLK0         18              1
    27      WR_DIS.DIS_DIRECT_BOOT                  EFUSE_BLK0         18              1
    28      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    29      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE EFUSE_BLK0        18              1
    30      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    31      WR_DIS.UART_PRINT_CONTROL               EFUSE_BLK0         18              1
    32      WR_DIS.FORCE_SEND_RESUME                EFUSE_BLK0         18              1
    33      WR_DIS.SECURE_VERSION                   EFUSE_BLK0         18              1
    34      WR_DIS.ERR_RST_ENABLE                   EFUSE_BLK0         19              1
    35      WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0         19              1
    36      WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0         19              1
    37      WR_DIS.BLK1                             EFUSE_BLK0         20              1
    38      WR_DIS.MAC                              EFUSE_BLK0         20              1
    39      WR_DIS.SPI_PAD_CONFIG_CLK               EFUSE_BLK0         20              1
    40      WR_DIS.SPI_PAD_CONFIG_Q                 EFUSE_BLK0         20              1
    41      WR_DIS.SPI_PAD_CONFIG_D                 EFUSE_BLK0         20              1
    42      WR_DIS.SPI_PAD_CONFIG_CS                EFUSE_BLK0         20              1
    43      WR_DIS.SPI_PAD_CONFIG_HD                EFUSE_BLK0         20              1
    44      WR_DIS.SPI_PAD_CONFIG_WP                EFUSE_BLK0         20              1
    45      WR_DIS.SPI_PAD_CONFIG_DQS               EFUSE_BLK0         20              1
    46      WR_DIS.SPI_PAD_CONFIG_D4                EFUSE_BLK0         20              1
    47      WR_DIS.SPI_PAD_CONFIG_D5                EFUSE_BLK0         20              1
    48      WR_DIS.SPI_PAD_CONFIG_D6                EFUSE_BLK0         20              1
    49      WR_DIS.SPI_PAD_CONFIG_D7                EFUSE_BLK0         20              1
    50      WR_DIS.WAFER_VERSION_MINOR_LO           EFUSE_BLK0         20              1
    51      WR_DIS.PKG_VERSION                      EFUSE_BLK0         20              1
    52      WR_DIS.BLK_VERSION_MINOR                EFUSE_BLK0         20              1
    53      WR_DIS.K_RTC_LDO                        EFUSE_BLK0         20              1
    54      WR_DIS.K_DIG_LDO                        EFUSE_BLK0         20              1
    55      WR_DIS.V_RTC_DBIAS20                    EFUSE_BLK0         20              1
    56      WR_DIS.V_DIG_DBIAS20                    EFUSE_BLK0         20              1
    57      WR_DIS.DIG_DBIAS_HVT                    EFUSE_BLK0         20              1
    58      WR_DIS.THRES_HVT                        EFUSE_BLK0         20              1
    59      WR_DIS.WAFER_VERSION_MINOR_HI           EFUSE_BLK0         20              1
    60      WR_DIS.WAFER_VERSION_MAJOR              EFUSE_BLK0         20              1
    61      WR_DIS.SYS_DATA_PART1                   EFUSE_BLK0         21              1
    62      WR_DIS.OPTIONAL_UNIQUE_ID               EFUSE_BLK0         21              1
    63      WR_DIS.BLK_VERSION_MAJOR                EFUSE_BLK0         21              1
    64      WR_DIS.TEMP_CALIB                       EFUSE_BLK0         21              1
    65      WR_DIS.OCODE                            EFUSE_BLK0         21              1
    66      WR_DIS.ADC1_INIT_CODE_ATTEN0            EFUSE_BLK0         21              1
    67      WR_DIS.ADC1_INIT_CODE_ATTEN1            EFUSE_BLK0         21              1
    68      WR_DIS.ADC1_INIT_CODE_ATTEN2            EFUSE_BLK0         21              1
    69      WR_DIS.ADC1_INIT_CODE_ATTEN3            EFUSE_BLK0         21              1
    70      WR_DIS.ADC1_CAL_VOL_ATTEN0              EFUSE_BLK0         21              1
    71      WR_DIS.ADC1_CAL_VOL_ATTEN1              EFUSE_BLK0         21              1
    72      WR_DIS.ADC1_CAL_VOL_ATTEN2              EFUSE_BLK0         21              1
    73      WR_DIS.ADC1_CAL_VOL_ATTEN3              EFUSE_BLK0         21              1
    74      WR_DIS.BLOCK_USR_DATA                   EFUSE_BLK0         22              1
    75      WR_DIS.CUSTOM_MAC                       EFUSE_BLK0         22              1
    76      WR_DIS.BLOCK_KEY0                       EFUSE_BLK0         23              1
    77      WR_DIS.BLOCK_KEY1                       EFUSE_BLK0         24              1
    78      WR_DIS.BLOCK_KEY2                       EFUSE_BLK0         25              1
    79      WR_DIS.BLOCK_KEY3                       EFUSE_BLK0         26              1
    80      WR_DIS.BLOCK_KEY4                       EFUSE_BLK0         27              1
    81      WR_DIS.BLOCK_KEY5                       EFUSE_BLK0         28              1
    82      WR_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         29              1
    83      WR_DIS.USB_EXCHG_PINS                   EFUSE_BLK0         30              1
    84      WR_DIS.VDD_SPI_AS_GPIO                  EFUSE_BLK0         30              1
    85      WR_DIS.SOFT_DIS_JTAG                    EFUSE_BLK0         31              1
    86      RD_DIS                                  EFUSE_BLK0         32              7
    87      RD_DIS.BLOCK_KEY0                       EFUSE_BLK0         32              1
    88      RD_DIS.BLOCK_KEY1                       EFUSE_BLK0         33              1
    89      RD_DIS.BLOCK_KEY2                       EFUSE_BLK0         34              1
    90      RD_DIS.BLOCK_KEY3                       EFUSE_BLK0         35              1
    91      RD_DIS.BLOCK_KEY4                       EFUSE_BLK0         36              1
    92      RD_DIS.BLOCK_KEY5                       EFUSE_BLK0         37              1
    93      RD_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         38              1
    94      DIS_ICACHE                              EFUSE_BLK0         40              1
    95      DIS_USB_JTAG                            EFUSE_BLK0         41              1
    96      DIS_DOWNLOAD_ICACHE                     EFUSE_BLK0         42              1
    97      DIS_USB_SERIAL_JTAG                     EFUSE_BLK0         43              1
    98      DIS_FORCE_DOWNLOAD                      EFUSE_BLK0         44              1
    99      DIS_TWAI                                EFUSE_BLK0         46              1
    100     JTAG_SEL_ENABLE                         EFUSE_BLK0         47              1
    101     SOFT_DIS_JTAG                           EFUSE_BLK0         48              3
    102     DIS_PAD_JTAG                            EFUSE_BLK0         51              1
    103     DIS_DOWNLOAD_MANUAL_ENCRYPT             EFUSE_BLK0         52              1
    104     USB_EXCHG_PINS                          EFUSE_BLK0         57              1
    105     VDD_SPI_AS_GPIO                         EFUSE_BLK0         58              1
    106     WDT_DELAY_SEL                           EFUSE_BLK0         80              2
    107     SPI_BOOT_CRYPT_CNT                      EFUSE_BLK0         82              3
    108     SECURE_BOOT_KEY_REVOKE0                 EFUSE_BLK0         85              1
    109     SECURE_BOOT_KEY_REVOKE1                 EFUSE_BLK0         86              1
    110     SECURE_BOOT_KEY_REVOKE2                 EFUSE_BLK0         87              1
    111     KEY_PURPOSE_0                           EFUSE_BLK0         88              4
    112     KEY_PURPOSE_1                           EFUSE_BLK0         92              4
    113     KEY_PURPOSE_2                           EFUSE_BLK0         96              4
    114     KEY_PURPOSE_3                           EFUSE_BLK0        100              4
    115     KEY_PURPOSE_4                           EFUSE_BLK0        104              4
    116     KEY_PURPOSE_5                           EFUSE_BLK0        108              4
    117     SECURE_BOOT_EN                          EFUSE_BLK0        116              1
    118     SECURE_BOOT_AGGRESSIVE_REVOKE           EFUSE_BLK0        117              1
    119     FLASH_TPUW                              EFUSE_BLK0        124              4
    120     DIS_DOWNLOAD_MODE                       EFUSE_BLK0        128              1
    121     DIS_DIRECT_BOOT                         EFUSE_BLK0        129              1
    122     DIS_USB_SERIAL_JTAG_ROM_PRINT           EFUSE_BLK0        130              1
    123     DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1
    124     ENABLE_SECURITY_DOWNLOAD                EFUSE_BLK0        133              1
    125     UART_PRINT_CONTROL                      EFUSE_BLK0        134              2
    126     FORCE_SEND_RESUME                       EFUSE_BLK0        141              1
    127     SECURE_VERSION                          EFUSE_BLK0        142              16
    128     ERR_RST_ENABLE                          EFUSE_BLK0        159              1
    129     DISABLE_WAFER_VERSION_MAJOR             EFUSE_BLK0        160              1
    130     DISABLE_BLK_VERSION_MAJOR               EFUSE_BLK0        161              1
    131     MAC                                     EFUSE_BLK1         0               8
    132     MAC                                     EFUSE_BLK1         8               8
    133     MAC                                     EFUSE_BLK1         16              8
    134     MAC                                     EFUSE_BLK1         24              8
    135     MAC                                     EFUSE_BLK1         32              8
    136     MAC                                     EFUSE_BLK1         40              8
    137     SPI_PAD_CONFIG_CLK                      EFUSE_BLK1         48              6
    138     SPI_PAD_CONFIG_Q                        EFUSE_BLK1         54              6
    139     SPI_PAD_CONFIG_D                        EFUSE_BLK1         60              6
    140     SPI_PAD_CONFIG_CS                       EFUSE_BLK1         66              6
    141     SPI_PAD_CONFIG_HD                       EFUSE_BLK1         72              6
    142     SPI_PAD_CONFIG_WP                       EFUSE_BLK1         78              6
    143     SPI_PAD_CONFIG_DQS                      EFUSE_BLK1         84              6
    144     SPI_PAD_CONFIG_D4                       EFUSE_BLK1         90              6
    145     SPI_PAD_CONFIG_D5                       EFUSE_BLK1         96              6
    146     SPI_PAD_CONFIG_D6                       EFUSE_BLK1        102              6
    147     SPI_PAD_CONFIG_D7                       EFUSE_BLK1        108              6
    148     WAFER_VERSION_MINOR_LO                  EFUSE_BLK1        114              3
    149     PKG_VERSION                             EFUSE_BLK1        117              3
    150     BLK_VERSION_MINOR                       EFUSE_BLK1        120              3
    151     K_RTC_LDO                               EFUSE_BLK1        135              7
    152     K_DIG_LDO                               EFUSE_BLK1        142              7
    153     V_RTC_DBIAS20                           EFUSE_BLK1        149              8
    154     V_DIG_DBIAS20                           EFUSE_BLK1        157              8
    155     DIG_DBIAS_HVT                           EFUSE_BLK1        165              5
    156     THRES_HVT                               EFUSE_BLK1        170              10
    157     WAFER_VERSION_MINOR_HI                  EFUSE_BLK1        183              1
    158     WAFER_VERSION_MAJOR                     EFUSE_BLK1        184              2
    159     SYS_DATA_PART2                          EFUSE_BLK10        0              256
    160     OPTIONAL_UNIQUE_ID                      EFUSE_BLK2         0              128
    161     BLK_VERSION_MAJOR                       EFUSE_BLK2        128              2
    162     TEMP_CALIB                              EFUSE_BLK2        131              9
    163     OCODE                                   EFUSE_BLK2        140              8
    164     ADC1_INIT_CODE_ATTEN0                   EFUSE_BLK2        148              10
    165     ADC1_INIT_CODE_ATTEN1                   EFUSE_BLK2        158              10
    166     ADC1_INIT_CODE_ATTEN2                   EFUSE_BLK2        168              10
    167     ADC1_INIT_CODE_ATTEN3                   EFUSE_BLK2        178              10
    168     ADC1_CAL_VOL_ATTEN0                     EFUSE_BLK2        188              10
    169     ADC1_CAL_VOL_ATTEN1                     EFUSE_BLK2        198              10
    170     ADC1_CAL_VOL_ATTEN2                     EFUSE_BLK2        208              10
    171     ADC1_CAL_VOL_ATTEN3                     EFUSE_BLK2        218              10
    172     USER_DATA                               EFUSE_BLK3         0              256
    173     USER_DATA.MAC_CUSTOM                    EFUSE_BLK3        200              48
    174     KEY0                                    EFUSE_BLK4         0              256
    175     KEY1                                    EFUSE_BLK5         0              256
    176     KEY2                                    EFUSE_BLK6         0              256
    177     KEY3                                    EFUSE_BLK7         0              256
    178     KEY4                                    EFUSE_BLK8         0              256
    179     KEY5                                    EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [40 44] [46 52] [57 58] [80 111] [116 117] [124 130] [132 135] [141 157] [159 161]
    EFUSE_BLK1
    [0 122] [135 179] [183 185]
    EFUSE_BLK10
    [0 255]
    EFUSE_BLK2
    [0 129] [131 227]
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
