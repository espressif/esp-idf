.. code-block:: none

    $ ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file esp32h21/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1
    3       WR_DIS.DIS_ICACHE               EFUSE_BLK0         2               1
    4       WR_DIS.DIS_USB_JTAG             EFUSE_BLK0         2               1
    5       WR_DIS.POWERGLITCH_EN           EFUSE_BLK0         2               1
    6       WR_DIS.DIS_FORCE_DOWNLOAD       EFUSE_BLK0         2               1
    7       WR_DIS.SPI_DOWNLOAD_MSPI_DIS    EFUSE_BLK0         2               1
    8       WR_DIS.DIS_TWAI                 EFUSE_BLK0         2               1
    9       WR_DIS.JTAG_SEL_ENABLE          EFUSE_BLK0         2               1
    10      WR_DIS.DIS_PAD_JTAG             EFUSE_BLK0         2               1
    11      WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    12      WR_DIS.WDT_DELAY_SEL            EFUSE_BLK0         3               1
    13      WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1
    14      WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1
    15      WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1
    16      WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1
    17      WR_DIS.KEY_PURPOSE_0            EFUSE_BLK0         8               1
    18      WR_DIS.KEY_PURPOSE_1            EFUSE_BLK0         9               1
    19      WR_DIS.KEY_PURPOSE_2            EFUSE_BLK0         10              1
    20      WR_DIS.KEY_PURPOSE_3            EFUSE_BLK0         11              1
    21      WR_DIS.KEY_PURPOSE_4            EFUSE_BLK0         12              1
    22      WR_DIS.KEY_PURPOSE_5            EFUSE_BLK0         13              1
    23      WR_DIS.SEC_DPA_LEVEL            EFUSE_BLK0         14              1
    24      WR_DIS.CRYPT_DPA_ENABLE         EFUSE_BLK0         14              1
    25      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1
    26      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    27      WR_DIS.ECDSA_CURVE_MODE         EFUSE_BLK0         17              1
    28      WR_DIS.ECC_FORCE_CONST_TIME     EFUSE_BLK0         17              1
    29      WR_DIS.FLASH_TPUW               EFUSE_BLK0         18              1
    30      WR_DIS.DIS_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    31      WR_DIS.DIS_DIRECT_BOOT          EFUSE_BLK0         18              1
    32      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    33      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    34      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    35      WR_DIS.UART_PRINT_CONTROL       EFUSE_BLK0         18              1
    36      WR_DIS.FORCE_SEND_RESUME        EFUSE_BLK0         18              1
    37      WR_DIS.SECURE_VERSION           EFUSE_BLK0         18              1
    38      WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE    EFUSE_BLK0         18              1
    39      WR_DIS.HYS_EN_PAD0              EFUSE_BLK0         19              1
    40      WR_DIS.HYS_EN_PAD1              EFUSE_BLK0         19              1
    41      WR_DIS.BLK1                     EFUSE_BLK0         20              1
    42      WR_DIS.MAC                      EFUSE_BLK0         20              1
    43      WR_DIS.MAC_EXT                  EFUSE_BLK0         20              1
    44      WR_DIS.RXIQ_VERSION             EFUSE_BLK0         20              1
    45      WR_DIS.RXIQ_0                   EFUSE_BLK0         20              1
    46      WR_DIS.RXIQ_1                   EFUSE_BLK0         20              1
    47      WR_DIS.ACTIVE_HP_DBIAS          EFUSE_BLK0         20              1
    48      WR_DIS.ACTIVE_LP_DBIAS          EFUSE_BLK0         20              1
    49      WR_DIS.DSLP_DBIAS               EFUSE_BLK0         20              1
    50      WR_DIS.DBIAS_VOL_GAP            EFUSE_BLK0         20              1
    51      WR_DIS.WAFER_VERSION_MINOR      EFUSE_BLK0         20              1
    52      WR_DIS.WAFER_VERSION_MAJOR      EFUSE_BLK0         20              1
    53      WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0         20              1
    54      WR_DIS.FLASH_CAP                EFUSE_BLK0         20              1
    55      WR_DIS.FLASH_TEMP               EFUSE_BLK0         20              1
    56      WR_DIS.FLASH_VENDOR             EFUSE_BLK0         20              1
    57      WR_DIS.PKG_VERSION              EFUSE_BLK0         20              1
    58      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1
    59      WR_DIS.OPTIONAL_UNIQUE_ID       EFUSE_BLK0         21              1
    60      WR_DIS.BLK_VERSION_MINOR        EFUSE_BLK0         21              1
    61      WR_DIS.BLK_VERSION_MAJOR        EFUSE_BLK0         21              1
    62      WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0         21              1
    63      WR_DIS.TEMP_CALIB               EFUSE_BLK0         21              1
    64      WR_DIS.ADC1_AVE_INITCODE_ATTEN0         EFUSE_BLK0         21              1
    65      WR_DIS.ADC1_AVE_INITCODE_ATTEN1         EFUSE_BLK0         21              1
    66      WR_DIS.ADC1_AVE_INITCODE_ATTEN2         EFUSE_BLK0         21              1
    67      WR_DIS.ADC1_AVE_INITCODE_ATTEN3         EFUSE_BLK0         21              1
    68      WR_DIS.ADC1_HI_DOUT_ATTEN0      EFUSE_BLK0         21              1
    69      WR_DIS.ADC1_HI_DOUT_ATTEN1      EFUSE_BLK0         21              1
    70      WR_DIS.ADC1_HI_DOUT_ATTEN2      EFUSE_BLK0         21              1
    71      WR_DIS.ADC1_HI_DOUT_ATTEN3      EFUSE_BLK0         21              1
    72      WR_DIS.ADC1_CH0_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    73      WR_DIS.ADC1_CH1_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    74      WR_DIS.ADC1_CH2_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    75      WR_DIS.ADC1_CH3_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    76      WR_DIS.ADC1_CH4_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    77      WR_DIS.BLOCK_USR_DATA           EFUSE_BLK0         22              1
    78      WR_DIS.CUSTOM_MAC               EFUSE_BLK0         22              1
    79      WR_DIS.BLOCK_KEY0               EFUSE_BLK0         23              1
    80      WR_DIS.BLOCK_KEY1               EFUSE_BLK0         24              1
    81      WR_DIS.BLOCK_KEY2               EFUSE_BLK0         25              1
    82      WR_DIS.BLOCK_KEY3               EFUSE_BLK0         26              1
    83      WR_DIS.BLOCK_KEY4               EFUSE_BLK0         27              1
    84      WR_DIS.BLOCK_KEY5               EFUSE_BLK0         28              1
    85      WR_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         29              1
    86      WR_DIS.USB_EXCHG_PINS           EFUSE_BLK0         30              1
    87      WR_DIS.VDD_SPI_AS_GPIO          EFUSE_BLK0         30              1
    88      WR_DIS.SOFT_DIS_JTAG            EFUSE_BLK0         31              1
    89      RD_DIS                          EFUSE_BLK0         32              7
    90      RD_DIS.BLOCK_KEY0               EFUSE_BLK0         32              1
    91      RD_DIS.BLOCK_KEY1               EFUSE_BLK0         33              1
    92      RD_DIS.BLOCK_KEY2               EFUSE_BLK0         34              1
    93      RD_DIS.BLOCK_KEY3               EFUSE_BLK0         35              1
    94      RD_DIS.BLOCK_KEY4               EFUSE_BLK0         36              1
    95      RD_DIS.BLOCK_KEY5               EFUSE_BLK0         37              1
    96      RD_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         38              1
    97      PVT_GLITCH_EN                   EFUSE_BLK0         39              1
    98      DIS_ICACHE                      EFUSE_BLK0         40              1
    99      DIS_USB_JTAG                    EFUSE_BLK0         41              1
    100     POWERGLITCH_EN                  EFUSE_BLK0         42              1
    101     DIS_USJ                         EFUSE_BLK0         43              1
    102     DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1
    103     SPI_DOWNLOAD_MSPI_DIS           EFUSE_BLK0         45              1
    104     DIS_TWAI                        EFUSE_BLK0         46              1
    105     JTAG_SEL_ENABLE                 EFUSE_BLK0         47              1
    106     SOFT_DIS_JTAG                   EFUSE_BLK0         48              3
    107     DIS_PAD_JTAG                    EFUSE_BLK0         51              1
    108     DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         52              1
    109     USB_DREFH                       EFUSE_BLK0         53              2
    110     USB_DREFL                       EFUSE_BLK0         55              2
    111     USB_EXCHG_PINS                  EFUSE_BLK0         57              1
    112     VDD_SPI_AS_GPIO                 EFUSE_BLK0         58              1
    113     ECDSA_CURVE_MODE                EFUSE_BLK0         59              2
    114     ECC_FORCE_CONST_TIME            EFUSE_BLK0         61              1
    115     XTS_DPA_PSEUDO_LEVEL            EFUSE_BLK0         62              2
    116     IO_LDO_ADJUST                   EFUSE_BLK0         64              8
    117     VDD_SPI_LDO_ADJUST              EFUSE_BLK0         72              8
    118     WDT_DELAY_SEL                   EFUSE_BLK0         80              2
    119     SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         82              3
    120     SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         85              1
    121     SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         86              1
    122     SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         87              1
    123     KEY_PURPOSE_0                   EFUSE_BLK0         88              4
    124     KEY_PURPOSE_1                   EFUSE_BLK0         92              4
    125     KEY_PURPOSE_2                   EFUSE_BLK0         96              4
    126     KEY_PURPOSE_3                   EFUSE_BLK0        100              4
    127     KEY_PURPOSE_4                   EFUSE_BLK0        104              4
    128     KEY_PURPOSE_5                   EFUSE_BLK0        108              4
    129     SEC_DPA_LEVEL                   EFUSE_BLK0        112              2
    130     IO_LDO_1P8                      EFUSE_BLK0        114              1
    131     CRYPT_DPA_ENABLE                EFUSE_BLK0        115              1
    132     SECURE_BOOT_EN                  EFUSE_BLK0        116              1
    133     SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1
    134     POWERGLITCH_EN1                 EFUSE_BLK0        118              5
    135     FLASH_TPUW                      EFUSE_BLK0        124              4
    136     DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1
    137     DIS_DIRECT_BOOT                 EFUSE_BLK0        129              1
    138     DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        130              1
    139     DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1
    140     ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1
    141     UART_PRINT_CONTROL              EFUSE_BLK0        134              2
    142     FORCE_SEND_RESUME               EFUSE_BLK0        136              1
    143     SECURE_VERSION                  EFUSE_BLK0        137              16
    144     SECURE_BOOT_DISABLE_FAST_WAKE   EFUSE_BLK0        153              1
    145     HYS_EN_PAD0                     EFUSE_BLK0        154              6
    146     HYS_EN_PAD1                     EFUSE_BLK0        160              22
    147     MAC                             EFUSE_BLK1         0               8
    148     MAC                             EFUSE_BLK1         8               8
    149     MAC                             EFUSE_BLK1         16              8
    150     MAC                             EFUSE_BLK1         24              8
    151     MAC                             EFUSE_BLK1         32              8
    152     MAC                             EFUSE_BLK1         40              8
    153     MAC_EXT                         EFUSE_BLK1         48              8
    154     MAC_EXT                         EFUSE_BLK1         56              8
    155     RXIQ_VERSION                    EFUSE_BLK1         64              3
    156     RXIQ_0                          EFUSE_BLK1         67              7
    157     RXIQ_1                          EFUSE_BLK1         74              7
    158     ACTIVE_HP_DBIAS                 EFUSE_BLK1         81              5
    159     ACTIVE_LP_DBIAS                 EFUSE_BLK1         86              5
    160     DSLP_DBIAS                      EFUSE_BLK1         91              4
    161     DBIAS_VOL_GAP                   EFUSE_BLK1         95              5
    162     WAFER_VERSION_MINOR             EFUSE_BLK1        114              3
    163     WAFER_VERSION_MAJOR             EFUSE_BLK1        117              2
    164     DISABLE_WAFER_VERSION_MAJOR     EFUSE_BLK1        119              1
    165     FLASH_CAP                       EFUSE_BLK1        120              3
    166     FLASH_TEMP                      EFUSE_BLK1        123              2
    167     FLASH_VENDOR                    EFUSE_BLK1        125              3
    168     PKG_VERSION                     EFUSE_BLK1        128              3
    169     SYS_DATA_PART2                  EFUSE_BLK10        0              256
    170     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128
    171     BLK_VERSION_MINOR               EFUSE_BLK2        130              3
    172     BLK_VERSION_MAJOR               EFUSE_BLK2        133              2
    173     DISABLE_BLK_VERSION_MAJOR       EFUSE_BLK2        135              1
    174     TEMP_CALIB                      EFUSE_BLK2        136              9
    175     ADC1_AVE_INITCODE_ATTEN0        EFUSE_BLK2        145              10
    176     ADC1_AVE_INITCODE_ATTEN1        EFUSE_BLK2        155              10
    177     ADC1_AVE_INITCODE_ATTEN2        EFUSE_BLK2        165              10
    178     ADC1_AVE_INITCODE_ATTEN3        EFUSE_BLK2        175              10
    179     ADC1_HI_DOUT_ATTEN0             EFUSE_BLK2        185              10
    180     ADC1_HI_DOUT_ATTEN1             EFUSE_BLK2        195              10
    181     ADC1_HI_DOUT_ATTEN2             EFUSE_BLK2        205              10
    182     ADC1_HI_DOUT_ATTEN3             EFUSE_BLK2        215              10
    183     ADC1_CH0_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        225              4
    184     ADC1_CH1_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        229              4
    185     ADC1_CH2_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        233              4
    186     ADC1_CH3_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        237              4
    187     ADC1_CH4_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        241              4
    188     USER_DATA                       EFUSE_BLK3         0              256
    189     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48
    190     KEY0                            EFUSE_BLK4         0              256
    191     KEY1                            EFUSE_BLK5         0              256
    192     KEY2                            EFUSE_BLK6         0              256
    193     KEY3                            EFUSE_BLK7         0              256
    194     KEY4                            EFUSE_BLK8         0              256
    195     KEY5                            EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [21 21] [21 22] [22 30] [30 38] [32 122] [124 130] [132 181]

    EFUSE_BLK1
    [0 99] [114 130]

    EFUSE_BLK10
    [0 255]

    EFUSE_BLK2
    [0 127] [130 244]

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
