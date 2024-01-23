
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32c2/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                              efuse_block     bit_start       bit_count
    1       WR_DIS                                  EFUSE_BLK0         0               8
    2       WR_DIS.RD_DIS                           EFUSE_BLK0         0               1
    3       WR_DIS.WDT_DELAY_SEL                    EFUSE_BLK0         1               1
    4       WR_DIS.DIS_PAD_JTAG                     EFUSE_BLK0         1               1
    5       WR_DIS.DIS_DOWNLOAD_ICACHE              EFUSE_BLK0         1               1
    6       WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0         2               1
    7       WR_DIS.SPI_BOOT_CRYPT_CNT               EFUSE_BLK0         2               1
    8       WR_DIS.XTS_KEY_LENGTH_256               EFUSE_BLK0         2               1
    9       WR_DIS.SECURE_BOOT_EN                   EFUSE_BLK0         2               1
    10      WR_DIS.UART_PRINT_CONTROL               EFUSE_BLK0         3               1
    11      WR_DIS.FORCE_SEND_RESUME                EFUSE_BLK0         3               1
    12      WR_DIS.DIS_DOWNLOAD_MODE                EFUSE_BLK0         3               1
    13      WR_DIS.DIS_DIRECT_BOOT                  EFUSE_BLK0         3               1
    14      WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0         3               1
    15      WR_DIS.FLASH_TPUW                       EFUSE_BLK0         3               1
    16      WR_DIS.SECURE_VERSION                   EFUSE_BLK0         4               1
    17      WR_DIS.CUSTOM_MAC_USED                  EFUSE_BLK0         4               1
    18      WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0         4               1
    19      WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0         4               1
    20      WR_DIS.CUSTOM_MAC                       EFUSE_BLK0         5               1
    21      WR_DIS.MAC                              EFUSE_BLK0         6               1
    22      WR_DIS.WAFER_VERSION_MINOR              EFUSE_BLK0         6               1
    23      WR_DIS.WAFER_VERSION_MAJOR              EFUSE_BLK0         6               1
    24      WR_DIS.PKG_VERSION                      EFUSE_BLK0         6               1
    25      WR_DIS.BLK_VERSION_MINOR                EFUSE_BLK0         6               1
    26      WR_DIS.BLK_VERSION_MAJOR                EFUSE_BLK0         6               1
    27      WR_DIS.OCODE                            EFUSE_BLK0         6               1
    28      WR_DIS.TEMP_CALIB                       EFUSE_BLK0         6               1
    29      WR_DIS.ADC1_INIT_CODE_ATTEN0            EFUSE_BLK0         6               1
    30      WR_DIS.ADC1_INIT_CODE_ATTEN3            EFUSE_BLK0         6               1
    31      WR_DIS.ADC1_CAL_VOL_ATTEN0              EFUSE_BLK0         6               1
    32      WR_DIS.ADC1_CAL_VOL_ATTEN3              EFUSE_BLK0         6               1
    33      WR_DIS.DIG_DBIAS_HVT                    EFUSE_BLK0         6               1
    34      WR_DIS.DIG_LDO_SLP_DBIAS2               EFUSE_BLK0         6               1
    35      WR_DIS.DIG_LDO_SLP_DBIAS26              EFUSE_BLK0         6               1
    36      WR_DIS.DIG_LDO_ACT_DBIAS26              EFUSE_BLK0         6               1
    37      WR_DIS.DIG_LDO_ACT_STEPD10              EFUSE_BLK0         6               1
    38      WR_DIS.RTC_LDO_SLP_DBIAS13              EFUSE_BLK0         6               1
    39      WR_DIS.RTC_LDO_SLP_DBIAS29              EFUSE_BLK0         6               1
    40      WR_DIS.RTC_LDO_SLP_DBIAS31              EFUSE_BLK0         6               1
    41      WR_DIS.RTC_LDO_ACT_DBIAS31              EFUSE_BLK0         6               1
    42      WR_DIS.RTC_LDO_ACT_DBIAS13              EFUSE_BLK0         6               1
    43      WR_DIS.ADC_CALIBRATION_3                EFUSE_BLK0         6               1
    44      WR_DIS.BLOCK_KEY0                       EFUSE_BLK0         7               1
    45      RD_DIS                                  EFUSE_BLK0         32              2
    46      RD_DIS.KEY0                             EFUSE_BLK0         32              2
    47      RD_DIS.KEY0.LOW                         EFUSE_BLK0         32              1
    48      RD_DIS.KEY0.HI                          EFUSE_BLK0         33              1
    49      WDT_DELAY_SEL                           EFUSE_BLK0         34              2
    50      DIS_PAD_JTAG                            EFUSE_BLK0         36              1
    51      DIS_DOWNLOAD_ICACHE                     EFUSE_BLK0         37              1
    52      DIS_DOWNLOAD_MANUAL_ENCRYPT             EFUSE_BLK0         38              1
    53      SPI_BOOT_CRYPT_CNT                      EFUSE_BLK0         39              3
    54      XTS_KEY_LENGTH_256                      EFUSE_BLK0         42              1
    55      UART_PRINT_CONTROL                      EFUSE_BLK0         43              2
    56      FORCE_SEND_RESUME                       EFUSE_BLK0         45              1
    57      DIS_DOWNLOAD_MODE                       EFUSE_BLK0         46              1
    58      DIS_DIRECT_BOOT                         EFUSE_BLK0         47              1
    59      ENABLE_SECURITY_DOWNLOAD                EFUSE_BLK0         48              1
    60      FLASH_TPUW                              EFUSE_BLK0         49              4
    61      SECURE_BOOT_EN                          EFUSE_BLK0         53              1
    62      SECURE_VERSION                          EFUSE_BLK0         54              4
    63      CUSTOM_MAC_USED                         EFUSE_BLK0         58              1
    64      DISABLE_WAFER_VERSION_MAJOR             EFUSE_BLK0         59              1
    65      DISABLE_BLK_VERSION_MAJOR               EFUSE_BLK0         60              1
    66      USER_DATA                               EFUSE_BLK1         0               88
    67      USER_DATA.MAC_CUSTOM                    EFUSE_BLK1         0               48
    68      MAC                                     EFUSE_BLK2         0               8
    69      MAC                                     EFUSE_BLK2         8               8
    70      MAC                                     EFUSE_BLK2         16              8
    71      MAC                                     EFUSE_BLK2         24              8
    72      MAC                                     EFUSE_BLK2         32              8
    73      MAC                                     EFUSE_BLK2         40              8
    74      WAFER_VERSION_MINOR                     EFUSE_BLK2         48              4
    75      WAFER_VERSION_MAJOR                     EFUSE_BLK2         52              2
    76      PKG_VERSION                             EFUSE_BLK2         54              3
    77      BLK_VERSION_MINOR                       EFUSE_BLK2         57              3
    78      BLK_VERSION_MAJOR                       EFUSE_BLK2         60              2
    79      OCODE                                   EFUSE_BLK2         62              7
    80      TEMP_CALIB                              EFUSE_BLK2         69              9
    81      ADC1_INIT_CODE_ATTEN0                   EFUSE_BLK2         78              8
    82      ADC1_INIT_CODE_ATTEN3                   EFUSE_BLK2         86              5
    83      ADC1_CAL_VOL_ATTEN0                     EFUSE_BLK2         91              8
    84      ADC1_CAL_VOL_ATTEN3                     EFUSE_BLK2         99              6
    85      DIG_DBIAS_HVT                           EFUSE_BLK2        105              5
    86      DIG_LDO_SLP_DBIAS2                      EFUSE_BLK2        110              7
    87      DIG_LDO_SLP_DBIAS26                     EFUSE_BLK2        117              8
    88      DIG_LDO_ACT_DBIAS26                     EFUSE_BLK2        125              6
    89      DIG_LDO_ACT_STEPD10                     EFUSE_BLK2        131              4
    90      RTC_LDO_SLP_DBIAS13                     EFUSE_BLK2        135              7
    91      RTC_LDO_SLP_DBIAS29                     EFUSE_BLK2        142              9
    92      RTC_LDO_SLP_DBIAS31                     EFUSE_BLK2        151              6
    93      RTC_LDO_ACT_DBIAS31                     EFUSE_BLK2        157              6
    94      RTC_LDO_ACT_DBIAS13                     EFUSE_BLK2        163              8
    95      ADC_CALIBRATION_3                       EFUSE_BLK2        192              11
    96      KEY0                                    EFUSE_BLK3         0              256
    97      KEY0.FE_256BIT                          EFUSE_BLK3         0              256
    98      KEY0.FE_128BIT                          EFUSE_BLK3         0              128
    99      KEY0.SB_128BIT                          EFUSE_BLK3        128             128

    Used bits in efuse table:
    EFUSE_BLK0
    [0 7] [0 1] [1 1] [1 2] [2 2] ... [6 6] [6 6] [6 6] [6 6] [6 6] [6 6] [6 6] [6 6] [6 6] [6 7] [32 33] [32 33] [32 60]
    EFUSE_BLK1
    [0 87] [0 47]
    EFUSE_BLK2
    [0 170] [192 202]
    EFUSE_BLK3
    [0 255] [0 255] [0 255]
    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)
