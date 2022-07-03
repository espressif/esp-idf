
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32s2/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               32   
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1    
    3       WR_DIS.DIS_RTC_RAM_BOOT         EFUSE_BLK0         1               1    
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
    16      WR_DIS.SECURE_BOOT_EN           EFUSE_BLK0         15              1    
    17      WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0         16              1    
    18      WR_DIS.GROUP_3                  EFUSE_BLK0         18              1    
    19      WR_DIS.BLK1                     EFUSE_BLK0         20              1    
    20      WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         21              1    
    21      WR_DIS.USER_DATA                EFUSE_BLK0         22              1    
    22      WR_DIS.KEY0                     EFUSE_BLK0         23              1    
    23      WR_DIS.KEY1                     EFUSE_BLK0         24              1    
    24      WR_DIS.KEY2                     EFUSE_BLK0         25              1    
    25      WR_DIS.KEY3                     EFUSE_BLK0         26              1    
    26      WR_DIS.KEY4                     EFUSE_BLK0         27              1    
    27      WR_DIS.KEY5                     EFUSE_BLK0         28              1    
    28      WR_DIS.SYS_DATA_PART2           EFUSE_BLK0         29              1    
    29      WR_DIS.USB_EXCHG_PINS           EFUSE_BLK0         30              1    
    30      RD_DIS                          EFUSE_BLK0         32              7    
    31      RD_DIS.KEY0                     EFUSE_BLK0         32              1    
    32      RD_DIS.KEY1                     EFUSE_BLK0         33              1    
    33      RD_DIS.KEY2                     EFUSE_BLK0         34              1    
    34      RD_DIS.KEY3                     EFUSE_BLK0         35              1    
    35      RD_DIS.KEY4                     EFUSE_BLK0         36              1    
    36      RD_DIS.KEY5                     EFUSE_BLK0         37              1    
    37      RD_DIS.SYS_DATA_PART2           EFUSE_BLK0         38              1    
    38      DIS_RTC_RAM_BOOT                EFUSE_BLK0         39              1    
    39      DIS_ICACHE                      EFUSE_BLK0         40              1    
    40      DIS_DCACHE                      EFUSE_BLK0         41              1    
    41      DIS_DOWNLOAD_ICACHE             EFUSE_BLK0         42              1    
    42      DIS_DOWNLOAD_DCACHE             EFUSE_BLK0         43              1    
    43      DIS_FORCE_DOWNLOAD              EFUSE_BLK0         44              1    
    44      DIS_USB                         EFUSE_BLK0         45              1    
    45      DIS_CAN                         EFUSE_BLK0         46              1    
    46      DIS_BOOT_REMAP                  EFUSE_BLK0         47              1    
    47      SOFT_DIS_JTAG                   EFUSE_BLK0         49              1    
    48      HARD_DIS_JTAG                   EFUSE_BLK0         50              1    
    49      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         51              1    
    50      USB_EXCHG_PINS                  EFUSE_BLK0         56              1    
    51      USB_EXT_PHY_ENABLE              EFUSE_BLK0         57              1    
    52      BLOCK0_VERSION                  EFUSE_BLK0         59              2    
    53      VDD_SPI_XPD                     EFUSE_BLK0         68              1    
    54      VDD_SPI_TIEH                    EFUSE_BLK0         69              1    
    55      VDD_SPI_FORCE                   EFUSE_BLK0         70              1    
    56      WDT_DELAY_SEL                   EFUSE_BLK0         80              2    
    57      SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         82              3    
    58      SECURE_BOOT_KEY_REVOKE0         EFUSE_BLK0         85              1    
    59      SECURE_BOOT_KEY_REVOKE1         EFUSE_BLK0         86              1    
    60      SECURE_BOOT_KEY_REVOKE2         EFUSE_BLK0         87              1    
    61      KEY_PURPOSE_0                   EFUSE_BLK0         88              4    
    62      KEY_PURPOSE_1                   EFUSE_BLK0         92              4    
    63      KEY_PURPOSE_2                   EFUSE_BLK0         96              4    
    64      KEY_PURPOSE_3                   EFUSE_BLK0        100              4    
    65      KEY_PURPOSE_4                   EFUSE_BLK0        104              4    
    66      KEY_PURPOSE_5                   EFUSE_BLK0        108              4    
    67      SECURE_BOOT_EN                  EFUSE_BLK0        116              1    
    68      SECURE_BOOT_AGGRESSIVE_REVOKE   EFUSE_BLK0        117              1    
    69      FLASH_TPUW                      EFUSE_BLK0        124              4    
    70      DIS_DOWNLOAD_MODE               EFUSE_BLK0        128              1    
    71      DIS_LEGACY_SPI_BOOT             EFUSE_BLK0        129              1    
    72      UART_PRINT_CHANNEL              EFUSE_BLK0        130              1    
    73      DIS_USB_DOWNLOAD_MODE           EFUSE_BLK0        132              1    
    74      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0        133              1    
    75      UART_PRINT_CONTROL              EFUSE_BLK0        134              2    
    76      PIN_POWER_SELECTION             EFUSE_BLK0        136              1    
    77      FLASH_TYPE                      EFUSE_BLK0        137              1    
    78      FORCE_SEND_RESUME               EFUSE_BLK0        138              1    
    79      SECURE_VERSION                  EFUSE_BLK0        139              16   
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
    97      WAFER_VERSION                   EFUSE_BLK1        114              3    
    98      FLASH_VERSION                   EFUSE_BLK1        117              4    
    99      BLOCK1_VERSION                  EFUSE_BLK1        121              3    
    100     PSRAM_VERSION                   EFUSE_BLK1        124              4    
    101     PKG_VERSION                     EFUSE_BLK1        128              4    
    102     SYS_DATA_PART2                  EFUSE_BLK10        0              256   
    103     OPTIONAL_UNIQUE_ID              EFUSE_BLK2         0              128   
    104     BLOCK2_VERSION                  EFUSE_BLK2        132              3    
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
    [0 31] [0 13] [15 16] [18 18] [20 30] [32 38] [32 47] [49 51] [56 57] [59 60] [68 70] [80 111] [116 117] [124 130] [132 154] 

    EFUSE_BLK1 
    [0 131] 

    EFUSE_BLK10 
    [0 255] 

    EFUSE_BLK2 
    [0 127] [132 134] 

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
