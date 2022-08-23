
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
    28      RD_DIS                          EFUSE_BLK0         32              7    
    29      RD_DIS.KEY0                     EFUSE_BLK0         32              1    
    30      RD_DIS.KEY1                     EFUSE_BLK0         33              1    
    31      RD_DIS.KEY2                     EFUSE_BLK0         34              1    
    32      RD_DIS.KEY3                     EFUSE_BLK0         35              1    
    33      RD_DIS.KEY4                     EFUSE_BLK0         36              1    
    34      RD_DIS.KEY5                     EFUSE_BLK0         37              1    
    35      RD_DIS.SYS_DATA_PART2           EFUSE_BLK0         38              1    
    36      DIS_ICACHE                      EFUSE_BLK0         40              1    
    37      DIS_USB_JTAG                    EFUSE_BLK0         41              1    
    38      DIS_DOWNLOAD_ICACHE             EFUSE_BLK0         42              1    
    39      DIS_USB_DEVICE                  EFUSE_BLK0         43              1    
    40      DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1    
    41      DIS_CAN                         EFUSE_BLK0         46              1    
    42      JTAG_SEL_ENABLE                 EFUSE_BLK0         47              1    
    43      SOFT_DIS_JTAG                   EFUSE_BLK0         48              3    
    44      DIS_PAD_JTAG                    EFUSE_BLK0         51              1    
    45      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         52              1    
    46      USB_DREFH                       EFUSE_BLK0         53              2    
    47      USB_DREFL                       EFUSE_BLK0         55              2    
    48      USB_EXCHG_PINS                  EFUSE_BLK0         57              1    
    49      VDD_SPI_AS_GPIO                 EFUSE_BLK0         58              1    
    50      BTLC_GPIO_ENABLE                EFUSE_BLK0         59              2    
    51      POWERGLITCH_EN                  EFUSE_BLK0         61              1    
    52      POWER_GLITCH_DSENSE             EFUSE_BLK0         62              2    
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
    64      SECURE_BOOT_EN                  EFUSE_BLK0        116              1    
    65      SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1    
    66      FLASH_TPUW                      EFUSE_BLK0        124              4    
    67      DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1    
    68      DIS_DIRECT_BOOT                 EFUSE_BLK0        129              1    
    69      DIS_USB_SERIAL_JTAG_ROM_PRINT   EFUSE_BLK0        130              1    
    70      DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0        132              1    
    71      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1    
    72      UART_PRINT_CONTROL              EFUSE_BLK0        134              2    
    73      FORCE_SEND_RESUME               EFUSE_BLK0        141              1    
    74      SECURE_VERSION                  EFUSE_BLK0        142              16   
    75      ERR_RST_ENABLE                  EFUSE_BLK0        159              1    
    76      MAC_FACTORY                     EFUSE_BLK1         0               8    
    77      MAC_FACTORY                     EFUSE_BLK1         8               8    
    78      MAC_FACTORY                     EFUSE_BLK1         16              8    
    79      MAC_FACTORY                     EFUSE_BLK1         24              8    
    80      MAC_FACTORY                     EFUSE_BLK1         32              8    
    81      MAC_FACTORY                     EFUSE_BLK1         40              8    
    82      SPI_PAD_CONFIG_CLK              EFUSE_BLK1         48              6    
    83      SPI_PAD_CONFIG_Q_D1             EFUSE_BLK1         54              6    
    84      SPI_PAD_CONFIG_D_D0             EFUSE_BLK1         60              6    
    85      SPI_PAD_CONFIG_CS               EFUSE_BLK1         66              6    
    86      SPI_PAD_CONFIG_HD_D3            EFUSE_BLK1         72              6    
    87      SPI_PAD_CONFIG_WP_D2            EFUSE_BLK1         78              6    
    88      SPI_PAD_CONFIG_DQS              EFUSE_BLK1         84              6    
    89      SPI_PAD_CONFIG_D4               EFUSE_BLK1         90              6    
    90      SPI_PAD_CONFIG_D5               EFUSE_BLK1         96              6    
    91      SPI_PAD_CONFIG_D6               EFUSE_BLK1        102              6    
    92      SPI_PAD_CONFIG_D7               EFUSE_BLK1        108              6    
    93      WAFER_VERSION                   EFUSE_BLK1        114              3    
    94      PKG_VERSION                     EFUSE_BLK1        117              3    
    95      BLOCK1_VERSION                  EFUSE_BLK1        120              3    
    96      K_RTC_LDO                       EFUSE_BLK1        135              7    
    97      K_DIG_LDO                       EFUSE_BLK1        142              7    
    98      V_RTC_DBIAS20                   EFUSE_BLK1        149              8    
    99      V_DIG_DBIAS20                   EFUSE_BLK1        157              8    
    100     DIG_DBIAS_HVT                   EFUSE_BLK1        165              5    
    101     THRES_HVT                       EFUSE_BLK1        170              10   
    102     SYS_DATA_PART2                  EFUSE_BLK10        0              256   
    103     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128   
    104     BLOCK2_VERSION                  EFUSE_BLK2        128              3    
    105     TEMP_CALIB                      EFUSE_BLK2        131              9    
    106     OCODE                           EFUSE_BLK2        140              8    
    107     ADC1_INIT_CODE_ATTEN0           EFUSE_BLK2        148              10   
    108     ADC1_INIT_CODE_ATTEN1           EFUSE_BLK2        158              10   
    109     ADC1_INIT_CODE_ATTEN2           EFUSE_BLK2        168              10   
    110     ADC1_INIT_CODE_ATTEN3           EFUSE_BLK2        178              10   
    111     ADC1_CAL_VOL_ATTEN0             EFUSE_BLK2        188              10   
    112     ADC1_CAL_VOL_ATTEN1             EFUSE_BLK2        198              10   
    113     ADC1_CAL_VOL_ATTEN2             EFUSE_BLK2        208              10   
    114     ADC1_CAL_VOL_ATTEN3             EFUSE_BLK2        218              10   
    115     USER_DATA                       EFUSE_BLK3         0              256   
    116     USER_DATA.MAC_CUSTOM            EFUSE_BLK3        200              48   
    117     KEY0                            EFUSE_BLK4         0              256   
    118     KEY1                            EFUSE_BLK5         0              256   
    119     KEY2                            EFUSE_BLK6         0              256   
    120     KEY3                            EFUSE_BLK7         0              256   
    121     KEY4                            EFUSE_BLK8         0              256   
    122     KEY5                            EFUSE_BLK9         0              256   

    Used bits in efuse table:
    EFUSE_BLK0 
    [0 31] [0 0] [2 13] [15 16] [18 18] [20 29] [32 38] [32 38] [40 44] [46 63] [80 111] [116 117] [124 130] [132 135] [141 157] [159 159] 

    EFUSE_BLK1 
    [0 122] [135 179] 

    EFUSE_BLK10 
    [0 255] 

    EFUSE_BLK2 
    [0 227] 

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
    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32c3/esp_efuse_table.csv ...
    Verifying efuse table...