
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32s2/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1
    3       WR_DIS.DIS_ICACHE               EFUSE_BLK0         2               1
    4       WR_DIS.DIS_DCACHE               EFUSE_BLK0         2               1
    5       WR_DIS.DIS_DOWNLOAD_ICACHE      EFUSE_BLK0         2               1
    6       WR_DIS.DIS_DOWNLOAD_DCACHE      EFUSE_BLK0         2               1
    7       WR_DIS.DIS_FORCE_DOWNLOAD       EFUSE_BLK0         2               1
    8       WR_DIS.DIS_USB                  EFUSE_BLK0         2               1
    9       WR_DIS.DIS_TWAI                 EFUSE_BLK0         2               1
    10      WR_DIS.DIS_BOOT_REMAP           EFUSE_BLK0         2               1
    11      WR_DIS.SOFT_DIS_JTAG            EFUSE_BLK0         2               1
    12      WR_DIS.HARD_DIS_JTAG            EFUSE_BLK0         2               1
    13      WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    14      WR_DIS.VDD_SPI_XPD              EFUSE_BLK0         3               1
    15      WR_DIS.VDD_SPI_TIEH             EFUSE_BLK0         3               1
    16      WR_DIS.VDD_SPI_FORCE            EFUSE_BLK0         3               1
    17      WR_DIS.WDT_DELAY_SEL            EFUSE_BLK0         3               1
    18      WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1
    19      WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1
    20      WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1
    21      WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1
    22      WR_DIS.KEY_PURPOSE_0            EFUSE_BLK0         8               1
    23      WR_DIS.KEY_PURPOSE_1            EFUSE_BLK0         9               1
    24      WR_DIS.KEY_PURPOSE_2            EFUSE_BLK0         10              1
    25      WR_DIS.KEY_PURPOSE_3            EFUSE_BLK0         11              1
    26      WR_DIS.KEY_PURPOSE_4            EFUSE_BLK0         12              1
    27      WR_DIS.KEY_PURPOSE_5            EFUSE_BLK0         13              1
    28      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1
    29      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    30      WR_DIS.FLASH_TPUW               EFUSE_BLK0         18              1
    31      WR_DIS.DIS_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    32      WR_DIS.DIS_LEGACY_SPI_BOOT      EFUSE_BLK0         18              1
    33      WR_DIS.UART_PRINT_CHANNEL       EFUSE_BLK0         18              1
    34      WR_DIS.DIS_USB_DOWNLOAD_MODE    EFUSE_BLK0         18              1
    35      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    36      WR_DIS.UART_PRINT_CONTROL       EFUSE_BLK0         18              1
    37      WR_DIS.PIN_POWER_SELECTION      EFUSE_BLK0         18              1
    38      WR_DIS.FLASH_TYPE               EFUSE_BLK0         18              1
    39      WR_DIS.FORCE_SEND_RESUME        EFUSE_BLK0         18              1
    40      WR_DIS.SECURE_VERSION           EFUSE_BLK0         18              1
    41      WR_DIS.BLK1                     EFUSE_BLK0         20              1
    42      WR_DIS.MAC                      EFUSE_BLK0         20              1
    43      WR_DIS.SPI_PAD_CONFIG_CLK       EFUSE_BLK0         20              1
    44      WR_DIS.SPI_PAD_CONFIG_Q         EFUSE_BLK0         20              1
    45      WR_DIS.SPI_PAD_CONFIG_D         EFUSE_BLK0         20              1
    46      WR_DIS.SPI_PAD_CONFIG_CS        EFUSE_BLK0         20              1
    47      WR_DIS.SPI_PAD_CONFIG_HD        EFUSE_BLK0         20              1
    48      WR_DIS.SPI_PAD_CONFIG_WP        EFUSE_BLK0         20              1
    49      WR_DIS.SPI_PAD_CONFIG_DQS       EFUSE_BLK0         20              1
    50      WR_DIS.SPI_PAD_CONFIG_D4        EFUSE_BLK0         20              1
    51      WR_DIS.SPI_PAD_CONFIG_D5        EFUSE_BLK0         20              1
    52      WR_DIS.SPI_PAD_CONFIG_D6        EFUSE_BLK0         20              1
    53      WR_DIS.SPI_PAD_CONFIG_D7        EFUSE_BLK0         20              1
    54      WR_DIS.WAFER_VERSION_MAJOR      EFUSE_BLK0         20              1
    55      WR_DIS.WAFER_VERSION_MINOR_HI   EFUSE_BLK0         20              1
    56      WR_DIS.FLASH_VERSION            EFUSE_BLK0         20              1
    57      WR_DIS.BLK_VERSION_MAJOR        EFUSE_BLK0         20              1
    58      WR_DIS.PSRAM_VERSION            EFUSE_BLK0         20              1
    59      WR_DIS.PKG_VERSION              EFUSE_BLK0         20              1
    60      WR_DIS.WAFER_VERSION_MINOR_LO   EFUSE_BLK0         20              1
    61      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1
    62      WR_DIS.OPTIONAL_UNIQUE_ID       EFUSE_BLK0         21              1
    63      WR_DIS.ADC_CALIB                EFUSE_BLK0         21              1
    64      WR_DIS.BLK_VERSION_MINOR        EFUSE_BLK0         21              1
    65      WR_DIS.TEMP_CALIB               EFUSE_BLK0         21              1
    66      WR_DIS.RTCCALIB_V1IDX_A10H      EFUSE_BLK0         21              1
    67      WR_DIS.RTCCALIB_V1IDX_A11H      EFUSE_BLK0         21              1
    68      WR_DIS.RTCCALIB_V1IDX_A12H      EFUSE_BLK0         21              1
    69      WR_DIS.RTCCALIB_V1IDX_A13H      EFUSE_BLK0         21              1
    70      WR_DIS.RTCCALIB_V1IDX_A20H      EFUSE_BLK0         21              1
    71      WR_DIS.RTCCALIB_V1IDX_A21H      EFUSE_BLK0         21              1
    72      WR_DIS.RTCCALIB_V1IDX_A22H      EFUSE_BLK0         21              1
    73      WR_DIS.RTCCALIB_V1IDX_A23H      EFUSE_BLK0         21              1
    74      WR_DIS.RTCCALIB_V1IDX_A10L      EFUSE_BLK0         21              1
    75      WR_DIS.RTCCALIB_V1IDX_A11L      EFUSE_BLK0         21              1
    76      WR_DIS.RTCCALIB_V1IDX_A12L      EFUSE_BLK0         21              1
    77      WR_DIS.RTCCALIB_V1IDX_A13L      EFUSE_BLK0         21              1
    78      WR_DIS.RTCCALIB_V1IDX_A20L      EFUSE_BLK0         21              1
    79      WR_DIS.RTCCALIB_V1IDX_A21L      EFUSE_BLK0         21              1
    80      WR_DIS.RTCCALIB_V1IDX_A22L      EFUSE_BLK0         21              1
    81      WR_DIS.RTCCALIB_V1IDX_A23L      EFUSE_BLK0         21              1
    82      WR_DIS.BLOCK_USR_DATA           EFUSE_BLK0         22              1
    83      WR_DIS.CUSTOM_MAC               EFUSE_BLK0         22              1
    84      WR_DIS.BLOCK_KEY0               EFUSE_BLK0         23              1
    85      WR_DIS.BLOCK_KEY1               EFUSE_BLK0         24              1
    86      WR_DIS.BLOCK_KEY2               EFUSE_BLK0         25              1
    87      WR_DIS.BLOCK_KEY3               EFUSE_BLK0         26              1
    88      WR_DIS.BLOCK_KEY4               EFUSE_BLK0         27              1
    89      WR_DIS.BLOCK_KEY5               EFUSE_BLK0         28              1
    90      WR_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         29              1
    91      WR_DIS.USB_EXCHG_PINS           EFUSE_BLK0         30              1
    92      WR_DIS.USB_EXT_PHY_ENABLE       EFUSE_BLK0         30              1
    93      WR_DIS.USB_FORCE_NOPERSIST      EFUSE_BLK0         30              1
    94      WR_DIS.BLOCK0_VERSION           EFUSE_BLK0         30              1
    95      RD_DIS                          EFUSE_BLK0         32              7
    96      RD_DIS.BLOCK_KEY0               EFUSE_BLK0         32              1
    97      RD_DIS.BLOCK_KEY1               EFUSE_BLK0         33              1
    98      RD_DIS.BLOCK_KEY2               EFUSE_BLK0         34              1
    99      RD_DIS.BLOCK_KEY3               EFUSE_BLK0         35              1
    100     RD_DIS.BLOCK_KEY4               EFUSE_BLK0         36              1
    101     RD_DIS.BLOCK_KEY5               EFUSE_BLK0         37              1
    102     RD_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         38              1
    103     DIS_ICACHE                      EFUSE_BLK0         40              1
    104     DIS_DCACHE                      EFUSE_BLK0         41              1
    105     DIS_DOWNLOAD_ICACHE             EFUSE_BLK0         42              1
    106     DIS_DOWNLOAD_DCACHE             EFUSE_BLK0         43              1
    107     DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1
    108     DIS_USB                         EFUSE_BLK0         45              1
    109     DIS_TWAI                        EFUSE_BLK0         46              1
    110     DIS_BOOT_REMAP                  EFUSE_BLK0         47              1
    111     SOFT_DIS_JTAG                   EFUSE_BLK0         49              1
    112     HARD_DIS_JTAG                   EFUSE_BLK0         50              1
    113     DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         51              1
    114     USB_EXCHG_PINS                  EFUSE_BLK0         56              1
    115     USB_EXT_PHY_ENABLE              EFUSE_BLK0         57              1
    116     USB_FORCE_NOPERSIST             EFUSE_BLK0         58              1
    117     BLOCK0_VERSION                  EFUSE_BLK0         59              2
    118     VDD_SPI_XPD                     EFUSE_BLK0         68              1
    119     VDD_SPI_TIEH                    EFUSE_BLK0         69              1
    120     VDD_SPI_FORCE                   EFUSE_BLK0         70              1
    121     WDT_DELAY_SEL                   EFUSE_BLK0         80              2
    122     SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         82              3
    123     SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         85              1
    124     SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         86              1
    125     SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         87              1
    126     KEY_PURPOSE_0                   EFUSE_BLK0         88              4
    127     KEY_PURPOSE_1                   EFUSE_BLK0         92              4
    128     KEY_PURPOSE_2                   EFUSE_BLK0         96              4
    129     KEY_PURPOSE_3                   EFUSE_BLK0        100              4
    130     KEY_PURPOSE_4                   EFUSE_BLK0        104              4
    131     KEY_PURPOSE_5                   EFUSE_BLK0        108              4
    132     SECURE_BOOT_EN                  EFUSE_BLK0        116              1
    133     SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1
    134     FLASH_TPUW                      EFUSE_BLK0        124              4
    135     DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1
    136     DIS_LEGACY_SPI_BOOT             EFUSE_BLK0        129              1
    137     UART_PRINT_CHANNEL              EFUSE_BLK0        130              1
    138     DIS_USB_DOWNLOAD_MODE           EFUSE_BLK0        132              1
    139     ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1
    140     UART_PRINT_CONTROL              EFUSE_BLK0        134              2
    141     PIN_POWER_SELECTION             EFUSE_BLK0        136              1
    142     FLASH_TYPE                      EFUSE_BLK0        137              1
    143     FORCE_SEND_RESUME               EFUSE_BLK0        138              1
    144     SECURE_VERSION                  EFUSE_BLK0        139              16
    145     DISABLE_WAFER_VERSION_MAJOR     EFUSE_BLK0        160              1
    146     DISABLE_BLK_VERSION_MAJOR       EFUSE_BLK0        161              1
    147     MAC                             EFUSE_BLK1         0               8
    148     MAC                             EFUSE_BLK1         8               8
    149     MAC                             EFUSE_BLK1         16              8
    150     MAC                             EFUSE_BLK1         24              8
    151     MAC                             EFUSE_BLK1         32              8
    152     MAC                             EFUSE_BLK1         40              8
    153     SPI_PAD_CONFIG_CLK              EFUSE_BLK1         48              6
    154     SPI_PAD_CONFIG_Q                EFUSE_BLK1         54              6
    155     SPI_PAD_CONFIG_D                EFUSE_BLK1         60              6
    156     SPI_PAD_CONFIG_CS               EFUSE_BLK1         66              6
    157     SPI_PAD_CONFIG_HD               EFUSE_BLK1         72              6
    158     SPI_PAD_CONFIG_WP               EFUSE_BLK1         78              6
    159     SPI_PAD_CONFIG_DQS              EFUSE_BLK1         84              6
    160     SPI_PAD_CONFIG_D4               EFUSE_BLK1         90              6
    161     SPI_PAD_CONFIG_D5               EFUSE_BLK1         96              6
    162     SPI_PAD_CONFIG_D6               EFUSE_BLK1        102              6
    163     SPI_PAD_CONFIG_D7               EFUSE_BLK1        108              6
    164     WAFER_VERSION_MAJOR             EFUSE_BLK1        114              2
    165     WAFER_VERSION_MINOR_HI          EFUSE_BLK1        116              1
    166     FLASH_VERSION                   EFUSE_BLK1        117              4
    167     BLK_VERSION_MAJOR               EFUSE_BLK1        121              2
    168     PSRAM_VERSION                   EFUSE_BLK1        124              4
    169     PKG_VERSION                     EFUSE_BLK1        128              4
    170     WAFER_VERSION_MINOR_LO          EFUSE_BLK1        132              3
    171     SYS_DATA_PART2                  EFUSE_BLK10        0              256
    172     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128
    173     ADC_CALIB                       EFUSE_BLK2        128              4
    174     BLK_VERSION_MINOR               EFUSE_BLK2        132              3
    175     TEMP_CALIB                      EFUSE_BLK2        135              9
    176     RTCCALIB_V1IDX_A10H             EFUSE_BLK2        144              8
    177     RTCCALIB_V1IDX_A11H             EFUSE_BLK2        152              8
    178     RTCCALIB_V1IDX_A12H             EFUSE_BLK2        160              8
    179     RTCCALIB_V1IDX_A13H             EFUSE_BLK2        168              8
    180     RTCCALIB_V1IDX_A20H             EFUSE_BLK2        176              8
    181     RTCCALIB_V1IDX_A21H             EFUSE_BLK2        184              8
    182     RTCCALIB_V1IDX_A22H             EFUSE_BLK2        192              8
    183     RTCCALIB_V1IDX_A23H             EFUSE_BLK2        200              8
    184     RTCCALIB_V1IDX_A10L             EFUSE_BLK2        208              6
    185     RTCCALIB_V1IDX_A11L             EFUSE_BLK2        214              6
    186     RTCCALIB_V1IDX_A12L             EFUSE_BLK2        220              6
    187     RTCCALIB_V1IDX_A13L             EFUSE_BLK2        226              6
    188     RTCCALIB_V1IDX_A20L             EFUSE_BLK2        232              6
    189     RTCCALIB_V1IDX_A21L             EFUSE_BLK2        238              6
    190     RTCCALIB_V1IDX_A22L             EFUSE_BLK2        244              6
    191     RTCCALIB_V1IDX_A23L             EFUSE_BLK2        250              6
    192     USER_DATA                       EFUSE_BLK3         0              256
    193     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48
    194     KEY0                            EFUSE_BLK4         0              256
    195     KEY1                            EFUSE_BLK5         0              256
    196     KEY2                            EFUSE_BLK6         0              256
    197     KEY3                            EFUSE_BLK7         0              256
    198     KEY4                            EFUSE_BLK8         0              256
    199     KEY5                            EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [32 38] [40 47] [49 51] [56 60] [68 70] [80 111] [116 117] [124 130] [132 154] [160 161]
    EFUSE_BLK1
    [0 122] [124 134]
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
