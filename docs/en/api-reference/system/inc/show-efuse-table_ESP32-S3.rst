
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                              efuse_block     bit_start       bit_count
    1       WR_DIS                                  EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                           EFUSE_BLK0         0               1
    3       WR_DIS.DIS_ICACHE                       EFUSE_BLK0         2               1
    4       WR_DIS.DIS_DCACHE                       EFUSE_BLK0         2               1
    5       WR_DIS.DIS_DOWNLOAD_ICACHE              EFUSE_BLK0         2               1
    6       WR_DIS.DIS_DOWNLOAD_DCACHE              EFUSE_BLK0         2               1
    7       WR_DIS.DIS_FORCE_DOWNLOAD               EFUSE_BLK0         2               1
    8       WR_DIS.DIS_USB_OTG                      EFUSE_BLK0         2               1
    9       WR_DIS.DIS_TWAI                         EFUSE_BLK0         2               1
    10      WR_DIS.DIS_APP_CPU                      EFUSE_BLK0         2               1
    11      WR_DIS.DIS_PAD_JTAG                     EFUSE_BLK0         2               1
    12      WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    13      WR_DIS.DIS_USB_JTAG                     EFUSE_BLK0         2               1
    14      WR_DIS.DIS_USB_SERIAL_JTAG              EFUSE_BLK0         2               1
    15      WR_DIS.STRAP_JTAG_SEL                   EFUSE_BLK0         2               1
    16      WR_DIS.USB_PHY_SEL                      EFUSE_BLK0         2               1
    17      WR_DIS.VDD_SPI_XPD                      EFUSE_BLK0         3               1
    18      WR_DIS.VDD_SPI_TIEH                     EFUSE_BLK0         3               1
    19      WR_DIS.VDD_SPI_FORCE                    EFUSE_BLK0         3               1
    20      WR_DIS.WDT_DELAY_SEL                    EFUSE_BLK0         3               1
    21      WR_DIS.SPI_BOOT_CRYPT_CNT               EFUSE_BLK0         4               1
    22      WR_DIS.SECURE_BOOT_KEY_REVOKE0          EFUSE_BLK0         5               1
    23      WR_DIS.SECURE_BOOT_KEY_REVOKE1          EFUSE_BLK0         6               1
    24      WR_DIS.SECURE_BOOT_KEY_REVOKE2          EFUSE_BLK0         7               1
    25      WR_DIS.KEY_PURPOSE_0                    EFUSE_BLK0         8               1
    26      WR_DIS.KEY_PURPOSE_1                    EFUSE_BLK0         9               1
    27      WR_DIS.KEY_PURPOSE_2                    EFUSE_BLK0         10              1
    28      WR_DIS.KEY_PURPOSE_3                    EFUSE_BLK0         11              1
    29      WR_DIS.KEY_PURPOSE_4                    EFUSE_BLK0         12              1
    30      WR_DIS.KEY_PURPOSE_5                    EFUSE_BLK0         13              1
    31      WR_DIS.SECURE_BOOT_EN                   EFUSE_BLK0         15              1
    32      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    33      WR_DIS.FLASH_TPUW                       EFUSE_BLK0         18              1
    34      WR_DIS.DIS_DOWNLOAD_MODE                EFUSE_BLK0         18              1
    35      WR_DIS.DIS_DIRECT_BOOT                  EFUSE_BLK0         18              1
    36      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    37      WR_DIS.FLASH_ECC_MODE                   EFUSE_BLK0         18              1
    38      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE EFUSE_BLK0        18              1
    39      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    40      WR_DIS.UART_PRINT_CONTROL               EFUSE_BLK0         18              1
    41      WR_DIS.PIN_POWER_SELECTION              EFUSE_BLK0         18              1
    42      WR_DIS.FLASH_TYPE                       EFUSE_BLK0         18              1
    43      WR_DIS.FLASH_PAGE_SIZE                  EFUSE_BLK0         18              1
    44      WR_DIS.FLASH_ECC_EN                     EFUSE_BLK0         18              1
    45      WR_DIS.FORCE_SEND_RESUME                EFUSE_BLK0         18              1
    46      WR_DIS.SECURE_VERSION                   EFUSE_BLK0         18              1
    47      WR_DIS.DIS_USB_OTG_DOWNLOAD_MODE        EFUSE_BLK0         19              1
    48      WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0         19              1
    49      WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0         19              1
    50      WR_DIS.BLK1                             EFUSE_BLK0         20              1
    51      WR_DIS.MAC                              EFUSE_BLK0         20              1
    52      WR_DIS.SPI_PAD_CONFIG_CLK               EFUSE_BLK0         20              1
    53      WR_DIS.SPI_PAD_CONFIG_Q                 EFUSE_BLK0         20              1
    54      WR_DIS.SPI_PAD_CONFIG_D                 EFUSE_BLK0         20              1
    55      WR_DIS.SPI_PAD_CONFIG_CS                EFUSE_BLK0         20              1
    56      WR_DIS.SPI_PAD_CONFIG_HD                EFUSE_BLK0         20              1
    57      WR_DIS.SPI_PAD_CONFIG_WP                EFUSE_BLK0         20              1
    58      WR_DIS.SPI_PAD_CONFIG_DQS               EFUSE_BLK0         20              1
    59      WR_DIS.SPI_PAD_CONFIG_D4                EFUSE_BLK0         20              1
    60      WR_DIS.SPI_PAD_CONFIG_D5                EFUSE_BLK0         20              1
    61      WR_DIS.SPI_PAD_CONFIG_D6                EFUSE_BLK0         20              1
    62      WR_DIS.SPI_PAD_CONFIG_D7                EFUSE_BLK0         20              1
    63      WR_DIS.WAFER_VERSION_MINOR_LO           EFUSE_BLK0         20              1
    64      WR_DIS.PKG_VERSION                      EFUSE_BLK0         20              1
    65      WR_DIS.BLK_VERSION_MINOR                EFUSE_BLK0         20              1
    66      WR_DIS.K_RTC_LDO                        EFUSE_BLK0         20              1
    67      WR_DIS.K_DIG_LDO                        EFUSE_BLK0         20              1
    68      WR_DIS.V_RTC_DBIAS20                    EFUSE_BLK0         20              1
    69      WR_DIS.V_DIG_DBIAS20                    EFUSE_BLK0         20              1
    70      WR_DIS.DIG_DBIAS_HVT                    EFUSE_BLK0         20              1
    71      WR_DIS.WAFER_VERSION_MINOR_HI           EFUSE_BLK0         20              1
    72      WR_DIS.WAFER_VERSION_MAJOR              EFUSE_BLK0         20              1
    73      WR_DIS.ADC2_CAL_VOL_ATTEN3              EFUSE_BLK0         20              1
    74      WR_DIS.SYS_DATA_PART1                   EFUSE_BLK0         21              1
    75      WR_DIS.OPTIONAL_UNIQUE_ID               EFUSE_BLK0         21              1
    76      WR_DIS.BLK_VERSION_MAJOR                EFUSE_BLK0         21              1
    77      WR_DIS.TEMP_CALIB                       EFUSE_BLK0         21              1
    78      WR_DIS.OCODE                            EFUSE_BLK0         21              1
    79      WR_DIS.ADC1_INIT_CODE_ATTEN0            EFUSE_BLK0         21              1
    80      WR_DIS.ADC1_INIT_CODE_ATTEN1            EFUSE_BLK0         21              1
    81      WR_DIS.ADC1_INIT_CODE_ATTEN2            EFUSE_BLK0         21              1
    82      WR_DIS.ADC1_INIT_CODE_ATTEN3            EFUSE_BLK0         21              1
    83      WR_DIS.ADC2_INIT_CODE_ATTEN0            EFUSE_BLK0         21              1
    84      WR_DIS.ADC2_INIT_CODE_ATTEN1            EFUSE_BLK0         21              1
    85      WR_DIS.ADC2_INIT_CODE_ATTEN2            EFUSE_BLK0         21              1
    86      WR_DIS.ADC2_INIT_CODE_ATTEN3            EFUSE_BLK0         21              1
    87      WR_DIS.ADC1_CAL_VOL_ATTEN0              EFUSE_BLK0         21              1
    88      WR_DIS.ADC1_CAL_VOL_ATTEN1              EFUSE_BLK0         21              1
    89      WR_DIS.ADC1_CAL_VOL_ATTEN2              EFUSE_BLK0         21              1
    90      WR_DIS.ADC1_CAL_VOL_ATTEN3              EFUSE_BLK0         21              1
    91      WR_DIS.ADC2_CAL_VOL_ATTEN0              EFUSE_BLK0         21              1
    92      WR_DIS.ADC2_CAL_VOL_ATTEN1              EFUSE_BLK0         21              1
    93      WR_DIS.ADC2_CAL_VOL_ATTEN2              EFUSE_BLK0         21              1
    94      WR_DIS.BLOCK_USR_DATA                   EFUSE_BLK0         22              1
    95      WR_DIS.CUSTOM_MAC                       EFUSE_BLK0         22              1
    96      WR_DIS.BLOCK_KEY0                       EFUSE_BLK0         23              1
    97      WR_DIS.BLOCK_KEY1                       EFUSE_BLK0         24              1
    98      WR_DIS.BLOCK_KEY2                       EFUSE_BLK0         25              1
    99      WR_DIS.BLOCK_KEY3                       EFUSE_BLK0         26              1
    100     WR_DIS.BLOCK_KEY4                       EFUSE_BLK0         27              1
    101     WR_DIS.BLOCK_KEY5                       EFUSE_BLK0         28              1
    102     WR_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         29              1
    103     WR_DIS.USB_EXCHG_PINS                   EFUSE_BLK0         30              1
    104     WR_DIS.USB_EXT_PHY_ENABLE               EFUSE_BLK0         30              1
    105     WR_DIS.SOFT_DIS_JTAG                    EFUSE_BLK0         31              1
    106     RD_DIS                                  EFUSE_BLK0         32              7
    107     RD_DIS.BLOCK_KEY0                       EFUSE_BLK0         32              1
    108     RD_DIS.BLOCK_KEY1                       EFUSE_BLK0         33              1
    109     RD_DIS.BLOCK_KEY2                       EFUSE_BLK0         34              1
    110     RD_DIS.BLOCK_KEY3                       EFUSE_BLK0         35              1
    111     RD_DIS.BLOCK_KEY4                       EFUSE_BLK0         36              1
    112     RD_DIS.BLOCK_KEY5                       EFUSE_BLK0         37              1
    113     RD_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0         38              1
    114     DIS_ICACHE                              EFUSE_BLK0         40              1
    115     DIS_DCACHE                              EFUSE_BLK0         41              1
    116     DIS_DOWNLOAD_ICACHE                     EFUSE_BLK0         42              1
    117     DIS_DOWNLOAD_DCACHE                     EFUSE_BLK0         43              1
    118     DIS_FORCE_DOWNLOAD                      EFUSE_BLK0         44              1
    119     DIS_USB_OTG                             EFUSE_BLK0         45              1
    120     DIS_TWAI                                EFUSE_BLK0         46              1
    121     DIS_APP_CPU                             EFUSE_BLK0         47              1
    122     SOFT_DIS_JTAG                           EFUSE_BLK0         48              3
    123     DIS_PAD_JTAG                            EFUSE_BLK0         51              1
    124     DIS_DOWNLOAD_MANUAL_ENCRYPT             EFUSE_BLK0         52              1
    125     USB_EXCHG_PINS                          EFUSE_BLK0         57              1
    126     USB_EXT_PHY_ENABLE                      EFUSE_BLK0         58              1
    127     VDD_SPI_XPD                             EFUSE_BLK0         68              1
    128     VDD_SPI_TIEH                            EFUSE_BLK0         69              1
    129     VDD_SPI_FORCE                           EFUSE_BLK0         70              1
    130     WDT_DELAY_SEL                           EFUSE_BLK0         80              2
    131     SPI_BOOT_CRYPT_CNT                      EFUSE_BLK0         82              3
    132     SECURE_BOOT_KEY_REVOKE0                 EFUSE_BLK0         85              1
    133     SECURE_BOOT_KEY_REVOKE1                 EFUSE_BLK0         86              1
    134     SECURE_BOOT_KEY_REVOKE2                 EFUSE_BLK0         87              1
    135     KEY_PURPOSE_0                           EFUSE_BLK0         88              4
    136     KEY_PURPOSE_1                           EFUSE_BLK0         92              4
    137     KEY_PURPOSE_2                           EFUSE_BLK0         96              4
    138     KEY_PURPOSE_3                           EFUSE_BLK0        100              4
    139     KEY_PURPOSE_4                           EFUSE_BLK0        104              4
    140     KEY_PURPOSE_5                           EFUSE_BLK0        108              4
    141     SECURE_BOOT_EN                          EFUSE_BLK0        116              1
    142     SECURE_BOOT_AGGRESSIVE_REVOKE           EFUSE_BLK0        117              1
    143     DIS_USB_JTAG                            EFUSE_BLK0        118              1
    144     DIS_USB_SERIAL_JTAG                     EFUSE_BLK0        119              1
    145     STRAP_JTAG_SEL                          EFUSE_BLK0        120              1
    146     USB_PHY_SEL                             EFUSE_BLK0        121              1
    147     FLASH_TPUW                              EFUSE_BLK0        124              4
    148     DIS_DOWNLOAD_MODE                       EFUSE_BLK0        128              1
    149     DIS_DIRECT_BOOT                         EFUSE_BLK0        129              1
    150     DIS_USB_SERIAL_JTAG_ROM_PRINT           EFUSE_BLK0        130              1
    151     FLASH_ECC_MODE                          EFUSE_BLK0        131              1
    152     DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1
    153     ENABLE_SECURITY_DOWNLOAD                EFUSE_BLK0        133              1
    154     UART_PRINT_CONTROL                      EFUSE_BLK0        134              2
    155     PIN_POWER_SELECTION                     EFUSE_BLK0        136              1
    156     FLASH_TYPE                              EFUSE_BLK0        137              1
    157     FLASH_PAGE_SIZE                         EFUSE_BLK0        138              2
    158     FLASH_ECC_EN                            EFUSE_BLK0        140              1
    159     FORCE_SEND_RESUME                       EFUSE_BLK0        141              1
    160     SECURE_VERSION                          EFUSE_BLK0        142              16
    161     DIS_USB_OTG_DOWNLOAD_MODE               EFUSE_BLK0        159              1
    162     DISABLE_WAFER_VERSION_MAJOR             EFUSE_BLK0        160              1
    163     DISABLE_BLK_VERSION_MAJOR               EFUSE_BLK0        161              1
    164     MAC                                     EFUSE_BLK1         0               8
    165     MAC                                     EFUSE_BLK1         8               8
    166     MAC                                     EFUSE_BLK1         16              8
    167     MAC                                     EFUSE_BLK1         24              8
    168     MAC                                     EFUSE_BLK1         32              8
    169     MAC                                     EFUSE_BLK1         40              8
    170     SPI_PAD_CONFIG_CLK                      EFUSE_BLK1         48              6
    171     SPI_PAD_CONFIG_Q                        EFUSE_BLK1         54              6
    172     SPI_PAD_CONFIG_D                        EFUSE_BLK1         60              6
    173     SPI_PAD_CONFIG_CS                       EFUSE_BLK1         66              6
    174     SPI_PAD_CONFIG_HD                       EFUSE_BLK1         72              6
    175     SPI_PAD_CONFIG_WP                       EFUSE_BLK1         78              6
    176     SPI_PAD_CONFIG_DQS                      EFUSE_BLK1         84              6
    177     SPI_PAD_CONFIG_D4                       EFUSE_BLK1         90              6
    178     SPI_PAD_CONFIG_D5                       EFUSE_BLK1         96              6
    179     SPI_PAD_CONFIG_D6                       EFUSE_BLK1        102              6
    180     SPI_PAD_CONFIG_D7                       EFUSE_BLK1        108              6
    181     WAFER_VERSION_MINOR_LO                  EFUSE_BLK1        114              3
    182     PKG_VERSION                             EFUSE_BLK1        117              3
    183     BLK_VERSION_MINOR                       EFUSE_BLK1        120              3
    184     K_RTC_LDO                               EFUSE_BLK1        141              7
    185     K_DIG_LDO                               EFUSE_BLK1        148              7
    186     V_RTC_DBIAS20                           EFUSE_BLK1        155              8
    187     V_DIG_DBIAS20                           EFUSE_BLK1        163              8
    188     DIG_DBIAS_HVT                           EFUSE_BLK1        171              5
    189     WAFER_VERSION_MINOR_HI                  EFUSE_BLK1        183              1
    190     WAFER_VERSION_MAJOR                     EFUSE_BLK1        184              2
    191     ADC2_CAL_VOL_ATTEN3                     EFUSE_BLK1        186              6
    192     SYS_DATA_PART2                          EFUSE_BLK10        0              256
    193     OPTIONAL_UNIQUE_ID                      EFUSE_BLK2         0              128
    194     BLK_VERSION_MAJOR                       EFUSE_BLK2        128              2
    195     TEMP_CALIB                              EFUSE_BLK2        132              9
    196     OCODE                                   EFUSE_BLK2        141              8
    197     ADC1_INIT_CODE_ATTEN0                   EFUSE_BLK2        149              8
    198     ADC1_INIT_CODE_ATTEN1                   EFUSE_BLK2        157              6
    199     ADC1_INIT_CODE_ATTEN2                   EFUSE_BLK2        163              6
    200     ADC1_INIT_CODE_ATTEN3                   EFUSE_BLK2        169              6
    201     ADC2_INIT_CODE_ATTEN0                   EFUSE_BLK2        175              8
    202     ADC2_INIT_CODE_ATTEN1                   EFUSE_BLK2        183              6
    203     ADC2_INIT_CODE_ATTEN2                   EFUSE_BLK2        189              6
    204     ADC2_INIT_CODE_ATTEN3                   EFUSE_BLK2        195              6
    205     ADC1_CAL_VOL_ATTEN0                     EFUSE_BLK2        201              8
    206     ADC1_CAL_VOL_ATTEN1                     EFUSE_BLK2        209              8
    207     ADC1_CAL_VOL_ATTEN2                     EFUSE_BLK2        217              8
    208     ADC1_CAL_VOL_ATTEN3                     EFUSE_BLK2        225              8
    209     ADC2_CAL_VOL_ATTEN0                     EFUSE_BLK2        233              8
    210     ADC2_CAL_VOL_ATTEN1                     EFUSE_BLK2        241              7
    211     ADC2_CAL_VOL_ATTEN2                     EFUSE_BLK2        248              7
    212     USER_DATA                               EFUSE_BLK3         0              256
    213     USER_DATA.MAC_CUSTOM                    EFUSE_BLK3        200              48
    214     KEY0                                    EFUSE_BLK4         0              256
    215     KEY1                                    EFUSE_BLK5         0              256
    216     KEY2                                    EFUSE_BLK6         0              256
    217     KEY3                                    EFUSE_BLK7         0              256
    218     KEY4                                    EFUSE_BLK8         0              256
    219     KEY5                                    EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [22 30] [30 38] [32 38] [40 52] [57 58] [68 70] [80 111] [116 121] [124 157] [159 161]
    EFUSE_BLK1
    [0 122] [141 175] [183 191]
    EFUSE_BLK10
    [0 255]
    EFUSE_BLK2
    [0 129] [132 254]
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
