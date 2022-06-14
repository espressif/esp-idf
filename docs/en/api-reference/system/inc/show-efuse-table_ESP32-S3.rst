
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32   
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1    
    3       WR_DIS.GROUP_1                  EFUSE_BLK0         2               1    
    4       WR_DIS.GROUP_2                  EFUSE_BLK0         3               1    
    5       WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         4               1    
    6       WR_DIS.SECURE_BOOT_KEY_REVOKE0  EFUSE_BLK0         5               1    
    7       WR_DIS.SECURE_BOOT_KEY_REVOKE1  EFUSE_BLK0         6               1    
    8       WR_DIS.SECURE_BOOT_KEY_REVOKE2  EFUSE_BLK0         7               1    
    9       WR_DIS.KEY0_PURPOSE             EFUSE_BLK0         8               1    
    10      WR_DIS.KEY1_PURPOSE             EFUSE_BLK0         9               1    
    11      WR_DIS.KEY2_PURPOSE             EFUSE_BLK0         10              1    
    12      WR_DIS.KEY3_PURPOSE             EFUSE_BLK0         11              1    
    13      WR_DIS.KEY4_PURPOSE             EFUSE_BLK0         12              1    
    14      WR_DIS.KEY5_PURPOSE             EFUSE_BLK0         13              1    
    15      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1    
    16      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1    
    17      WR_DIS.GROUP_3                  EFUSE_BLK0         18              1    
    18      WR_DIS.BLK1                     EFUSE_BLK0         20              1    
    19      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1    
    20      WR_DIS.USER_DATA                EFUSE_BLK0         22              1    
    21      WR_DIS.KEY0                     EFUSE_BLK0         23              1    
    22      WR_DIS.KEY1                     EFUSE_BLK0         24              1    
    23      WR_DIS.KEY2                     EFUSE_BLK0         25              1    
    24      WR_DIS.KEY3                     EFUSE_BLK0         26              1    
    25      WR_DIS.KEY4                     EFUSE_BLK0         27              1    
    26      WR_DIS.KEY5                     EFUSE_BLK0         28              1    
    27      WR_DIS.SYS_DATA_PART2           EFUSE_BLK0         29              1    
    28      WR_DIS.USB_EXCHG_PINS           EFUSE_BLK0         30              1    
    29      RD_DIS                          EFUSE_BLK0         32              7    
    30      RD_DIS.KEY0                     EFUSE_BLK0         32              1    
    31      RD_DIS.KEY1                     EFUSE_BLK0         33              1    
    32      RD_DIS.KEY2                     EFUSE_BLK0         34              1    
    33      RD_DIS.KEY3                     EFUSE_BLK0         35              1    
    34      RD_DIS.KEY4                     EFUSE_BLK0         36              1    
    35      RD_DIS.KEY5                     EFUSE_BLK0         37              1    
    36      RD_DIS.SYS_DATA_PART2           EFUSE_BLK0         38              1    
    37      DIS_ICACHE                      EFUSE_BLK0         40              1    
    38      DIS_DCACHE                      EFUSE_BLK0         41              1    
    39      DIS_DOWNLOAD_ICACHE             EFUSE_BLK0         42              1    
    40      DIS_DOWNLOAD_DCACHE             EFUSE_BLK0         43              1    
    41      DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1    
    42      DIS_USB                         EFUSE_BLK0         45              1    
    43      DIS_CAN                         EFUSE_BLK0         46              1    
    44      DIS_APP_CPU                     EFUSE_BLK0         47              1    
    45      SOFT_DIS_JTAG                   EFUSE_BLK0         48              3    
    46      HARD_DIS_JTAG                   EFUSE_BLK0         51              1    
    47      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         52              1    
    48      USB_EXCHG_PINS                  EFUSE_BLK0         57              1    
    49      USB_EXT_PHY_ENABLE              EFUSE_BLK0         58              1    
    50      BTLC_GPIO_ENABLE                EFUSE_BLK0         59              2    
    51      VDD_SPI_XPD                     EFUSE_BLK0         68              1    
    52      VDD_SPI_TIEH                    EFUSE_BLK0         69              1    
    53      VDD_SPI_FORCE                   EFUSE_BLK0         70              1    
    54      WDT_DELAY_SEL                   EFUSE_BLK0         80              2    
    55      SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         82              3    
    56      SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         85              1    
    57      SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         86              1    
    58      SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         87              1    
    59      KEY_PURPOSE_0                   EFUSE_BLK0         88              4    
    60      KEY_PURPOSE_1                   EFUSE_BLK0         92              4    
    61      KEY_PURPOSE_2                   EFUSE_BLK0         96              4    
    62      KEY_PURPOSE_3                   EFUSE_BLK0        100              4    
    63      KEY_PURPOSE_4                   EFUSE_BLK0        104              4    
    64      KEY_PURPOSE_5                   EFUSE_BLK0        108              4    
    65      SECURE_BOOT_EN                  EFUSE_BLK0        116              1    
    66      SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1    
    67      DIS_USB_JTAG                    EFUSE_BLK0        118              1    
    68      DIS_USB_SERIAL_JTAG             EFUSE_BLK0        119              1    
    69      STRAP_JTAG_SEL                  EFUSE_BLK0        120              1    
    70      USB_PHY_SEL                     EFUSE_BLK0        121              1    
    71      FLASH_TPUW                      EFUSE_BLK0        124              4    
    72      DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1    
    73      DIS_DIRECT_BOOT                 EFUSE_BLK0        129              1    
    74      DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        130              1    
    75      FLASH_ECC_MODE                  EFUSE_BLK0        131              1    
    76      DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1    
    77      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1    
    78      UART_PRINT_CONTROL              EFUSE_BLK0        134              2    
    79      PIN_POWER_SELECTION             EFUSE_BLK0        136              1    
    80      FLASH_TYPE                      EFUSE_BLK0        137              1    
    81      FLASH_PAGE_SIZE                 EFUSE_BLK0        138              2    
    82      FLASH_ECC_EN                    EFUSE_BLK0        140              1    
    83      FORCE_SEND_RESUME               EFUSE_BLK0        141              1    
    84      SECURE_VERSION                  EFUSE_BLK0        142              16   
    85      DIS_USB_OTG_DOWNLOAD_MODE       EFUSE_BLK0        159              1    
    86      MAC_FACTORY                     EFUSE_BLK1         0               8    
    87      MAC_FACTORY                     EFUSE_BLK1         8               8    
    88      MAC_FACTORY                     EFUSE_BLK1         16              8    
    89      MAC_FACTORY                     EFUSE_BLK1         24              8    
    90      MAC_FACTORY                     EFUSE_BLK1         32              8    
    91      MAC_FACTORY                     EFUSE_BLK1         40              8    
    92      SPI_PAD_CONFIG_CLK              EFUSE_BLK1         48              6    
    93      SPI_PAD_CONFIG_Q_D1             EFUSE_BLK1         54              6    
    94      SPI_PAD_CONFIG_D_D0             EFUSE_BLK1         60              6    
    95      SPI_PAD_CONFIG_CS               EFUSE_BLK1         66              6    
    96      SPI_PAD_CONFIG_HD_D3            EFUSE_BLK1         72              6    
    97      SPI_PAD_CONFIG_WP_D2            EFUSE_BLK1         78              6    
    98      SPI_PAD_CONFIG_DQS              EFUSE_BLK1         84              6    
    99      SPI_PAD_CONFIG_D4               EFUSE_BLK1         90              6    
    100     SPI_PAD_CONFIG_D5               EFUSE_BLK1         96              6    
    101     SPI_PAD_CONFIG_D6               EFUSE_BLK1        102              6    
    102     SPI_PAD_CONFIG_D7               EFUSE_BLK1        108              6    
    103     WAFER_VERSION                   EFUSE_BLK1        114              3    
    104     PKG_VERSION                     EFUSE_BLK1        117              3    
    105     BLK_VER_MINOR                   EFUSE_BLK1        120              3    
    106     ADC2_CAL_VOL_ATTEN3             EFUSE_BLK1        186              6    
    107     SYS_DATA_PART2                  EFUSE_BLK10        0              256   
    108     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128   
    109     BLK_VER_MAJOR                   EFUSE_BLK2        128              2    
    110     TEMP_CALIB                      EFUSE_BLK2        132              9    
    111     OCODE                           EFUSE_BLK2        141              8    
    112     ADC1_INIT_CODE_ATTEN0           EFUSE_BLK2        149              8    
    113     ADC1_INIT_CODE_ATTEN1           EFUSE_BLK2        157              6    
    114     ADC1_INIT_CODE_ATTEN2           EFUSE_BLK2        163              6    
    115     ADC1_INIT_CODE_ATTEN3           EFUSE_BLK2        169              6    
    116     ADC2_INIT_CODE_ATTEN0           EFUSE_BLK2        175              8    
    117     ADC2_INIT_CODE_ATTEN1           EFUSE_BLK2        183              6    
    118     ADC2_INIT_CODE_ATTEN2           EFUSE_BLK2        189              6    
    119     ADC2_INIT_CODE_ATTEN3           EFUSE_BLK2        195              6    
    120     ADC1_CAL_VOL_ATTEN0             EFUSE_BLK2        201              8    
    121     ADC1_CAL_VOL_ATTEN1             EFUSE_BLK2        209              8    
    122     ADC1_CAL_VOL_ATTEN2             EFUSE_BLK2        217              8    
    123     ADC1_CAL_VOL_ATTEN3             EFUSE_BLK2        225              8    
    124     ADC2_CAL_VOL_ATTEN0             EFUSE_BLK2        233              8    
    125     ADC2_CAL_VOL_ATTEN1             EFUSE_BLK2        241              7    
    126     ADC2_CAL_VOL_ATTEN2             EFUSE_BLK2        248              7    
    127     USER_DATA                       EFUSE_BLK3         0              256   
    128     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48   
    129     KEY0                            EFUSE_BLK4         0              256   
    130     KEY1                            EFUSE_BLK5         0              256   
    131     KEY2                            EFUSE_BLK6         0              256   
    132     KEY3                            EFUSE_BLK7         0              256   
    133     KEY4                            EFUSE_BLK8         0              256   
    134     KEY5                            EFUSE_BLK9         0              256   

    Used bits in efuse table:
    EFUSE_BLK0 
    [0 31] [0 0] [2 13] [15 16] [18 18] [20 30] [32 38] [32 38] [40 52] [57 60] [68 70] [80 111] [116 121] [124 157] [159 159] 

    EFUSE_BLK1 
    [0 122] [186 191] 

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
    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32s3/esp_efuse_table.csv ...
    Verifying efuse table...
