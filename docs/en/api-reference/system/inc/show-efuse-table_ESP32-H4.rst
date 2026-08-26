.. code-block:: none

    $ ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file esp32h4/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1
    3       WR_DIS.KM_DISABLE_DEPLOY_MODE   EFUSE_BLK0         1               1
    4       WR_DIS.KM_RND_SWITCH_CYCLE      EFUSE_BLK0         1               1
    5       WR_DIS.KM_DEPLOY_ONLY_ONCE      EFUSE_BLK0         1               1
    6       WR_DIS.FORCE_USE_KEY_MANAGER_KEY        EFUSE_BLK0         1               1
    7       WR_DIS.FORCE_DISABLE_SW_INIT_KEY        EFUSE_BLK0         1               1
    8       WR_DIS.KM_XTS_KEY_LENGTH_256    EFUSE_BLK0         1               1
    9       WR_DIS.LOCK_KM_KEY              EFUSE_BLK0         1               1
    10      WR_DIS.DIS_USB_JTAG             EFUSE_BLK0         2               1
    11      WR_DIS.DIS_FORCE_DOWNLOAD       EFUSE_BLK0         2               1
    12      WR_DIS.SPI_DOWNLOAD_MSPI_DIS    EFUSE_BLK0         2               1
    13      WR_DIS.DIS_TWAI                 EFUSE_BLK0         2               1
    14      WR_DIS.JTAG_SEL_ENABLE          EFUSE_BLK0         2               1
    15      WR_DIS.DIS_PAD_JTAG             EFUSE_BLK0         2               1
    16      WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    17      WR_DIS.PVT_GLITCH_EN            EFUSE_BLK0         2               1
    18      WR_DIS.PVT_GLITCH_MODE          EFUSE_BLK0         2               1
    19      WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1
    20      WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1
    21      WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1
    22      WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1
    23      WR_DIS.KEY_PURPOSE_0            EFUSE_BLK0         8               1
    24      WR_DIS.KEY_PURPOSE_1            EFUSE_BLK0         9               1
    25      WR_DIS.KEY_PURPOSE_2            EFUSE_BLK0         10              1
    26      WR_DIS.KEY_PURPOSE_3            EFUSE_BLK0         11              1
    27      WR_DIS.KEY_PURPOSE_4            EFUSE_BLK0         12              1
    28      WR_DIS.KEY_PURPOSE_5            EFUSE_BLK0         13              1
    29      WR_DIS.SEC_DPA_LEVEL            EFUSE_BLK0         14              1
    30      WR_DIS.XTS_DPA_PSEUDO_LEVEL     EFUSE_BLK0         14              1
    31      WR_DIS.XTS_DPA_CLK_ENABLE       EFUSE_BLK0         14              1
    32      WR_DIS.ECC_FORCE_CONST_TIME     EFUSE_BLK0         14              1
    33      WR_DIS.SECURE_BOOT_SHA384_EN    EFUSE_BLK0         14              1
    34      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1
    35      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1
    36      WR_DIS.FLASH_TPUW               EFUSE_BLK0         18              1
    37      WR_DIS.DIS_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    38      WR_DIS.DIS_DIRECT_BOOT          EFUSE_BLK0         18              1
    39      WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0         18              1
    40      WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE        EFUSE_BLK0         18              1
    41      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         18              1
    42      WR_DIS.UART_PRINT_CONTROL       EFUSE_BLK0         18              1
    43      WR_DIS.FORCE_SEND_RESUME        EFUSE_BLK0         18              1
    44      WR_DIS.SECURE_VERSION           EFUSE_BLK0         18              1
    45      WR_DIS.HUK_GEN_STATE            EFUSE_BLK0         19              1
    46      WR_DIS.BLK1                     EFUSE_BLK0         20              1
    47      WR_DIS.MAC                      EFUSE_BLK0         20              1
    48      WR_DIS.MAC_EXT                  EFUSE_BLK0         20              1
    49      WR_DIS.PVT_LIMIT                EFUSE_BLK0         20              1
    50      WR_DIS.PVT_CELL_SELECT          EFUSE_BLK0         20              1
    51      WR_DIS.PVT_PUMP_LIMIT           EFUSE_BLK0         20              1
    52      WR_DIS.PUMP_DRV                 EFUSE_BLK0         20              1
    53      WR_DIS.WDT_DELAY_SEL            EFUSE_BLK0         20              1
    54      WR_DIS.HYS_EN_PAD               EFUSE_BLK0         20              1
    55      WR_DIS.PVT_GLITCH_CHARGE_RESET  EFUSE_BLK0         20              1
    56      WR_DIS.VDD_SPI_LDO_ADJUST       EFUSE_BLK0         20              1
    57      WR_DIS.FLASH_LDO_POWER_SEL      EFUSE_BLK0         20              1
    58      WR_DIS.WAFER_VERSION_MINOR      EFUSE_BLK0         20              1
    59      WR_DIS.WAFER_VERSION_MAJOR      EFUSE_BLK0         20              1
    60      WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0         20              1
    61      WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0         20              1
    62      WR_DIS.BLK_VERSION_MINOR        EFUSE_BLK0         20              1
    63      WR_DIS.BLK_VERSION_MAJOR        EFUSE_BLK0         20              1
    64      WR_DIS.FLASH_CAP                EFUSE_BLK0         20              1
    65      WR_DIS.FLASH_VENDOR             EFUSE_BLK0         20              1
    66      WR_DIS.PSRAM_CAP                EFUSE_BLK0         20              1
    67      WR_DIS.PSRAM_VENDOR             EFUSE_BLK0         20              1
    68      WR_DIS.TEMP                     EFUSE_BLK0         20              1
    69      WR_DIS.PKG_VERSION              EFUSE_BLK0         20              1
    70      WR_DIS.PVT_DBIAS                EFUSE_BLK0         20              1
    71      WR_DIS.ADJUST_1V2               EFUSE_BLK0         20              1
    72      WR_DIS.ADJUST_1V8               EFUSE_BLK0         20              1
    73      WR_DIS.ACTIVE_DCDC_1V25         EFUSE_BLK0         20              1
    74      WR_DIS.ACTIVE_DCDC_1V35         EFUSE_BLK0         20              1
    75      WR_DIS.SLP_DCDC                 EFUSE_BLK0         20              1
    76      WR_DIS.LSLP_HP_DRVB             EFUSE_BLK0         20              1
    77      WR_DIS.DSLP_LP_DBIAS            EFUSE_BLK0         20              1
    78      WR_DIS.TEMP_CALIB               EFUSE_BLK0         20              1
    79      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1
    80      WR_DIS.OPTIONAL_UNIQUE_ID       EFUSE_BLK0         21              1
    81      WR_DIS.OCODE                    EFUSE_BLK0         21              1
    82      WR_DIS.DCDC_OCODE               EFUSE_BLK0         21              1
    83      WR_DIS.VDD_3V4_DOUT             EFUSE_BLK0         21              1
    84      WR_DIS.ADC1_AVE_INITCODE_ATTEN0         EFUSE_BLK0         21              1
    85      WR_DIS.ADC1_AVE_INITCODE_ATTEN1         EFUSE_BLK0         21              1
    86      WR_DIS.ADC1_AVE_INITCODE_ATTEN2         EFUSE_BLK0         21              1
    87      WR_DIS.ADC1_AVE_INITCODE_ATTEN3         EFUSE_BLK0         21              1
    88      WR_DIS.ADC1_HI_DOUT_ATTEN0      EFUSE_BLK0         21              1
    89      WR_DIS.ADC1_HI_DOUT_ATTEN1      EFUSE_BLK0         21              1
    90      WR_DIS.ADC1_HI_DOUT_ATTEN2      EFUSE_BLK0         21              1
    91      WR_DIS.ADC1_HI_DOUT_ATTEN3      EFUSE_BLK0         21              1
    92      WR_DIS.ADC1_CH0_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    93      WR_DIS.ADC1_CH1_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    94      WR_DIS.ADC1_CH2_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    95      WR_DIS.ADC1_CH3_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    96      WR_DIS.ADC1_CH4_ATTEN0_INITCODE_DIFF    EFUSE_BLK0         21              1
    97      WR_DIS.INITCODE_DIFF_1P8_3P3    EFUSE_BLK0         21              1
    98      WR_DIS.HI_DOUT_DIFF_1P8_3P3     EFUSE_BLK0         21              1
    99      WR_DIS.BLOCK_USR_DATA           EFUSE_BLK0         22              1
    100     WR_DIS.CUSTOM_MAC               EFUSE_BLK0         22              1
    101     WR_DIS.BLOCK_KEY0               EFUSE_BLK0         23              1
    102     WR_DIS.BLOCK_KEY1               EFUSE_BLK0         24              1
    103     WR_DIS.BLOCK_KEY2               EFUSE_BLK0         25              1
    104     WR_DIS.BLOCK_KEY3               EFUSE_BLK0         26              1
    105     WR_DIS.BLOCK_KEY4               EFUSE_BLK0         27              1
    106     WR_DIS.BLOCK_KEY5               EFUSE_BLK0         28              1
    107     WR_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         29              1
    108     WR_DIS.USB_EXCHG_PINS           EFUSE_BLK0         30              1
    109     WR_DIS.USB_OTG_FS_EXCHG_PINS    EFUSE_BLK0         30              1
    110     WR_DIS.USB_PHY_SEL              EFUSE_BLK0         30              1
    111     WR_DIS.SOFT_DIS_JTAG            EFUSE_BLK0         31              1
    112     RD_DIS                          EFUSE_BLK0         32              7
    113     RD_DIS.BLOCK_KEY0               EFUSE_BLK0         32              1
    114     RD_DIS.BLOCK_KEY1               EFUSE_BLK0         33              1
    115     RD_DIS.BLOCK_KEY2               EFUSE_BLK0         34              1
    116     RD_DIS.BLOCK_KEY3               EFUSE_BLK0         35              1
    117     RD_DIS.BLOCK_KEY4               EFUSE_BLK0         36              1
    118     RD_DIS.BLOCK_KEY5               EFUSE_BLK0         37              1
    119     RD_DIS.BLOCK_SYS_DATA2          EFUSE_BLK0         38              1
    120     DIS_USB_JTAG                    EFUSE_BLK0         39              1
    121     DIS_FORCE_DOWNLOAD              EFUSE_BLK0         41              1
    122     SPI_DOWNLOAD_MSPI_DIS           EFUSE_BLK0         42              1
    123     DIS_TWAI                        EFUSE_BLK0         43              1
    124     JTAG_SEL_ENABLE                 EFUSE_BLK0         44              1
    125     DIS_PAD_JTAG                    EFUSE_BLK0         45              1
    126     DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         46              1
    127     PVT_GLITCH_EN                   EFUSE_BLK0         50              1
    128     PVT_GLITCH_MODE                 EFUSE_BLK0         52              2
    129     DIS_CORE1                       EFUSE_BLK0         54              1
    130     SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         55              3
    131     SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         58              1
    132     SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         59              1
    133     SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         60              1
    134     KEY_PURPOSE_0                   EFUSE_BLK0         64              5
    135     KEY_PURPOSE_1                   EFUSE_BLK0         69              5
    136     KEY_PURPOSE_2                   EFUSE_BLK0         74              5
    137     KEY_PURPOSE_3                   EFUSE_BLK0         79              5
    138     KEY_PURPOSE_4                   EFUSE_BLK0         84              5
    139     KEY_PURPOSE_5                   EFUSE_BLK0         89              5
    140     SEC_DPA_LEVEL                   EFUSE_BLK0         94              2
    141     XTS_DPA_PSEUDO_LEVEL            EFUSE_BLK0         96              2
    142     XTS_DPA_CLK_ENABLE              EFUSE_BLK0         98              1
    143     ECC_FORCE_CONST_TIME            EFUSE_BLK0         99              1
    144     SECURE_BOOT_SHA384_EN           EFUSE_BLK0        100              1
    145     SECURE_BOOT_EN                  EFUSE_BLK0        101              1
    146     SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        102              1
    147     KM_DISABLE_DEPLOY_MODE          EFUSE_BLK0        103              5
    148     KM_RND_SWITCH_CYCLE             EFUSE_BLK0        108              2
    149     KM_DEPLOY_ONLY_ONCE             EFUSE_BLK0        110              5
    150     FORCE_USE_KEY_MANAGER_KEY       EFUSE_BLK0        115              5
    151     FORCE_DISABLE_SW_INIT_KEY       EFUSE_BLK0        120              1
    152     KM_XTS_KEY_LENGTH_256           EFUSE_BLK0        121              1
    153     LOCK_KM_KEY                     EFUSE_BLK0        122              1
    154     FLASH_TPUW                      EFUSE_BLK0        123              3
    155     DIS_DOWNLOAD_MODE               EFUSE_BLK0        127              1
    156     DIS_DIRECT_BOOT                 EFUSE_BLK0        128              1
    157     DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        129              1
    158     DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        130              1
    159     ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        131              1
    160     UART_PRINT_CONTROL              EFUSE_BLK0        132              2
    161     FORCE_SEND_RESUME               EFUSE_BLK0        134              1
    162     SECURE_VERSION                  EFUSE_BLK0        135              16
    163     HUK_GEN_STATE                   EFUSE_BLK0        151              5
    164     FLASH_LDO_EFUSE_SEL             EFUSE_BLK0        156              1
    165     USB_EXCHG_PINS                  EFUSE_BLK0        168              1
    166     USB_OTG_FS_EXCHG_PINS           EFUSE_BLK0        169              1
    167     USB_PHY_SEL                     EFUSE_BLK0        170              1
    168     SOFT_DIS_JTAG                   EFUSE_BLK0        171              3
    169     IO_LDO_ADJUST                   EFUSE_BLK0        174              8
    170     IO_LDO_1P8                      EFUSE_BLK0        182              1
    171     DCDC_CCM_EN                     EFUSE_BLK0        183              1
    172     MAC                             EFUSE_BLK1         0               8
    173     MAC                             EFUSE_BLK1         8               8
    174     MAC                             EFUSE_BLK1         16              8
    175     MAC                             EFUSE_BLK1         24              8
    176     MAC                             EFUSE_BLK1         32              8
    177     MAC                             EFUSE_BLK1         40              8
    178     MAC_EXT                         EFUSE_BLK1         48              8
    179     MAC_EXT                         EFUSE_BLK1         56              8
    180     PVT_LIMIT                       EFUSE_BLK1         64              16
    181     PVT_CELL_SELECT                 EFUSE_BLK1         80              7
    182     PVT_PUMP_LIMIT                  EFUSE_BLK1         87              8
    183     PUMP_DRV                        EFUSE_BLK1         96              4
    184     WDT_DELAY_SEL                   EFUSE_BLK1        100              2
    185     HYS_EN_PAD                      EFUSE_BLK1        102              1
    186     PVT_GLITCH_CHARGE_RESET         EFUSE_BLK1        103              1
    187     VDD_SPI_LDO_ADJUST              EFUSE_BLK1        105              8
    188     FLASH_LDO_POWER_SEL             EFUSE_BLK1        113              1
    189     WAFER_VERSION_MINOR             EFUSE_BLK1        114              4
    190     WAFER_VERSION_MAJOR             EFUSE_BLK1        118              2
    191     DISABLE_WAFER_VERSION_MAJOR     EFUSE_BLK1        120              1
    192     DISABLE_BLK_VERSION_MAJOR       EFUSE_BLK1        121              1
    193     BLK_VERSION_MINOR               EFUSE_BLK1        122              3
    194     BLK_VERSION_MAJOR               EFUSE_BLK1        125              2
    195     FLASH_CAP                       EFUSE_BLK1        127              3
    196     FLASH_VENDOR                    EFUSE_BLK1        130              3
    197     PSRAM_CAP                       EFUSE_BLK1        133              3
    198     PSRAM_VENDOR                    EFUSE_BLK1        136              2
    199     TEMP                            EFUSE_BLK1        138              2
    200     PKG_VERSION                     EFUSE_BLK1        140              3
    201     PVT_DBIAS                       EFUSE_BLK1        143              5
    202     ADJUST_1V2                      EFUSE_BLK1        148              4
    203     ADJUST_1V8                      EFUSE_BLK1        152              4
    204     ACTIVE_DCDC_1V25                EFUSE_BLK1        156              4
    205     ACTIVE_DCDC_1V35                EFUSE_BLK1        160              4
    206     SLP_DCDC                        EFUSE_BLK1        164              5
    207     LSLP_HP_DRVB                    EFUSE_BLK1        169              5
    208     DSLP_LP_DBIAS                   EFUSE_BLK1        174              2
    209     TEMP_CALIB                      EFUSE_BLK1        176              10
    210     SYS_DATA_PART2                  EFUSE_BLK10        0              256
    211     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128
    212     OCODE                           EFUSE_BLK2        128              8
    213     DCDC_OCODE                      EFUSE_BLK2        136              8
    214     VDD_3V4_DOUT                    EFUSE_BLK2        144              10
    215     ADC1_AVE_INITCODE_ATTEN0        EFUSE_BLK2        154              9
    216     ADC1_AVE_INITCODE_ATTEN1        EFUSE_BLK2        163              9
    217     ADC1_AVE_INITCODE_ATTEN2        EFUSE_BLK2        172              9
    218     ADC1_AVE_INITCODE_ATTEN3        EFUSE_BLK2        181              9
    219     ADC1_HI_DOUT_ATTEN0             EFUSE_BLK2        190              9
    220     ADC1_HI_DOUT_ATTEN1             EFUSE_BLK2        199              9
    221     ADC1_HI_DOUT_ATTEN2             EFUSE_BLK2        208              9
    222     ADC1_HI_DOUT_ATTEN3             EFUSE_BLK2        217              9
    223     ADC1_CH0_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        226              3
    224     ADC1_CH1_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        229              3
    225     ADC1_CH2_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        232              3
    226     ADC1_CH3_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        235              3
    227     ADC1_CH4_ATTEN0_INITCODE_DIFF   EFUSE_BLK2        238              3
    228     INITCODE_DIFF_1P8_3P3           EFUSE_BLK2        241              5
    229     HI_DOUT_DIFF_1P8_3P3            EFUSE_BLK2        246              5
    230     USER_DATA                       EFUSE_BLK3         0              256
    231     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48
    232     KEY0                            EFUSE_BLK4         0              256
    233     KEY1                            EFUSE_BLK5         0              256
    234     KEY2                            EFUSE_BLK6         0              256
    235     KEY3                            EFUSE_BLK7         0              256
    236     KEY4                            EFUSE_BLK8         0              256
    237     KEY5                            EFUSE_BLK9         0              256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 1] [1 1] ... [50 50] [52 60] [64 125] [127 156] [168 183]

    EFUSE_BLK1
    [0 94] [96 103] [105 185]

    EFUSE_BLK10
    [0 255]

    EFUSE_BLK2
    [0 250]

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
