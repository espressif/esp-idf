
.. code-block:: none

    $ ./efuse_table_gen.py -t IDF_TARGET_PATH_NAME {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Parsing efuse CSV input file esp32c6/esp_efuse_table.csv ...
    Verifying efuse table...
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32   
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1    
    3       WR_DIS.SWAP_UART_SDIO_EN        EFUSE_BLK0         1               1    
    4       WR_DIS.GROUP_1                  EFUSE_BLK0         2               1    
    5       WR_DIS.GROUP_2                  EFUSE_BLK0         3               1    
    6       WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1    
    7       WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1    
    8       WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1    
    9       WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1    
    10      WR_DIS.KEY0_PURPOSE             EFUSE_BLK0         8               1    
    11      WR_DIS.KEY1_PURPOSE             EFUSE_BLK0         9               1    
    12      WR_DIS.KEY2_PURPOSE             EFUSE_BLK0         10              1    
    13      WR_DIS.KEY3_PURPOSE             EFUSE_BLK0         11              1    
    14      WR_DIS.KEY4_PURPOSE             EFUSE_BLK0         12              1    
    15      WR_DIS.KEY5_PURPOSE             EFUSE_BLK0         13              1    
    16      WR_DIS.SEC_DPA_LEVEL            EFUSE_BLK0         14              1    
    17      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1    
    18      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1    
    19      WR_DIS.GROUP_3                  EFUSE_BLK0         18              1    
    20      WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE    EFUSE_BLK0         19              1    
    21      WR_DIS.BLK1                     EFUSE_BLK0         20              1    
    22      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1    
    23      WR_DIS.USER_DATA                EFUSE_BLK0         22              1    
    24      WR_DIS.KEY0                     EFUSE_BLK0         23              1    
    25      WR_DIS.KEY1                     EFUSE_BLK0         24              1    
    26      WR_DIS.KEY2                     EFUSE_BLK0         25              1    
    27      WR_DIS.KEY3                     EFUSE_BLK0         26              1    
    28      WR_DIS.KEY4                     EFUSE_BLK0         27              1    
    29      WR_DIS.KEY5                     EFUSE_BLK0         28              1    
    30      WR_DIS.SYS_DATA_PART2           EFUSE_BLK0         29              1    
    31      RD_DIS                          EFUSE_BLK0         32              7    
    32      RD_DIS.KEY0                     EFUSE_BLK0         32              1    
    33      RD_DIS.KEY1                     EFUSE_BLK0         33              1    
    34      RD_DIS.KEY2                     EFUSE_BLK0         34              1    
    35      RD_DIS.KEY3                     EFUSE_BLK0         35              1    
    36      RD_DIS.KEY4                     EFUSE_BLK0         36              1    
    37      RD_DIS.KEY5                     EFUSE_BLK0         37              1    
    38      RD_DIS.SYS_DATA_PART2           EFUSE_BLK0         38              1    
    39      SWAP_UART_SDIO_EN               EFUSE_BLK0         39              1    
    40      DIS_ICACHE                      EFUSE_BLK0         40              1    
    41      DIS_USB_JTAG                    EFUSE_BLK0         41              1    
    42      DIS_DOWNLOAD_ICACHE             EFUSE_BLK0         42              1    
    43      DIS_USB_SERIAL_JTAG             EFUSE_BLK0         43              1    
    44      DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1    
    45      DIS_SPI_DOWNLOAD_MSPI           EFUSE_BLK0         45              1    
    46      DIS_TWAI                        EFUSE_BLK0         46              1    
    47      JTAG_SEL_ENABLE                 EFUSE_BLK0         47              1    
    48      SOFT_DIS_JTAG                   EFUSE_BLK0         48              3    
    49      DIS_PAD_JTAG                    EFUSE_BLK0         51              1    
    50      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         52              1    
    51      USB_EXCHG_PINS                  EFUSE_BLK0         57              1    
    52      VDD_SPI_AS_GPIO                 EFUSE_BLK0         58              1    
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
    65      CRYPT_DPA_ENABLE                EFUSE_BLK0        115              1    
    66      SECURE_BOOT_EN                  EFUSE_BLK0        116              1    
    67      SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1    
    68      FLASH_TPUW                      EFUSE_BLK0        124              4    
    69      DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1    
    70      DIS_DIRECT_BOOT                 EFUSE_BLK0        129              1    
    71      DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        130              1    
    72      DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1    
    73      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1    
    74      UART_PRINT_CONTROL              EFUSE_BLK0        134              2    
    75      FORCE_SEND_RESUME               EFUSE_BLK0        141              1    
    76      SECURE_VERSION                  EFUSE_BLK0        142              16   
    77      SECURE_BOOT_DISABLE_FAST_WAKE   EFUSE_BLK0        158              1    
    78      DISABLE_WAFER_VERSION_MAJOR     EFUSE_BLK0        160              1    
    79      DISABLE_BLK_VERSION_MAJOR       EFUSE_BLK0        161              1    
    80      MAC_FACTORY                     EFUSE_BLK1         0               8    
    81      MAC_FACTORY                     EFUSE_BLK1         8               8    
    82      MAC_FACTORY                     EFUSE_BLK1         16              8    
    83      MAC_FACTORY                     EFUSE_BLK1         24              8    
    84      MAC_FACTORY                     EFUSE_BLK1         32              8    
    85      MAC_FACTORY                     EFUSE_BLK1         40              8    
    86      SPI_PAD_CONFIG_CLK              EFUSE_BLK1         48              6    
    87      SPI_PAD_CONFIG_Q_D1             EFUSE_BLK1         54              6    
    88      SPI_PAD_CONFIG_D_D0             EFUSE_BLK1         60              6    
    89      SPI_PAD_CONFIG_CS               EFUSE_BLK1         66              6    
    90      SPI_PAD_CONFIG_HD_D3            EFUSE_BLK1         72              6    
    91      SPI_PAD_CONFIG_WP_D2            EFUSE_BLK1         78              6    
    92      SPI_PAD_CONFIG_DQS              EFUSE_BLK1         84              6    
    93      SPI_PAD_CONFIG_D4               EFUSE_BLK1         90              6    
    94      SPI_PAD_CONFIG_D5               EFUSE_BLK1         96              6    
    95      SPI_PAD_CONFIG_D6               EFUSE_BLK1        102              6    
    96      SPI_PAD_CONFIG_D7               EFUSE_BLK1        108              6    
    97      WAFER_VERSION_MINOR             EFUSE_BLK1        114              3    
    98      PKG_VERSION                     EFUSE_BLK1        117              3    
    99      BLK_VERSION_MINOR               EFUSE_BLK1        120              3    
    100     K_RTC_LDO                       EFUSE_BLK1        135              7    
    101     K_DIG_LDO                       EFUSE_BLK1        142              7    
    102     V_RTC_DBIAS20                   EFUSE_BLK1        149              8    
    103     V_DIG_DBIAS20                   EFUSE_BLK1        157              8    
    104     DIG_DBIAS_HVT                   EFUSE_BLK1        165              5    
    105     THRES_HVT                       EFUSE_BLK1        170              10   
    106     WAFER_VERSION_MINOR             EFUSE_BLK1        183              1    
    107     WAFER_VERSION_MAJOR             EFUSE_BLK1        184              2    
    108     SYS_DATA_PART2                  EFUSE_BLK10        0              256   
    109     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128   
    110     BLK_VERSION_MAJOR               EFUSE_BLK2        128              2    
    111     TEMP_CALIB                      EFUSE_BLK2        131              9    
    112     OCODE                           EFUSE_BLK2        140              8    
    113     ADC1_INIT_CODE_ATTEN0           EFUSE_BLK2        148              10   
    114     ADC1_INIT_CODE_ATTEN1           EFUSE_BLK2        158              10   
    115     ADC1_INIT_CODE_ATTEN2           EFUSE_BLK2        168              10   
    116     ADC1_INIT_CODE_ATTEN3           EFUSE_BLK2        178              10   
    117     ADC1_CAL_VOL_ATTEN0             EFUSE_BLK2        188              10   
    118     ADC1_CAL_VOL_ATTEN1             EFUSE_BLK2        198              10   
    119     ADC1_CAL_VOL_ATTEN2             EFUSE_BLK2        208              10   
    120     ADC1_CAL_VOL_ATTEN3             EFUSE_BLK2        218              10   
    121     USER_DATA                       EFUSE_BLK3         0              256   
    122     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48   
    123     KEY0                            EFUSE_BLK4         0              256   
    124     KEY1                            EFUSE_BLK5         0              256   
    125     KEY2                            EFUSE_BLK6         0              256   
    126     KEY3                            EFUSE_BLK7         0              256   
    127     KEY4                            EFUSE_BLK8         0              256   
    128     KEY5                            EFUSE_BLK9         0              256   

    Used bits in efuse table:
    EFUSE_BLK0 
    [0 31] [0 16] [18 29] [32 38] [32 52] [57 58] [80 113] [115 117] [124 130] [132 135] [141 158] [160 161] 

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
