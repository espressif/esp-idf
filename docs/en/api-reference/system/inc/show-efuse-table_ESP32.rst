
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 192
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               16
    2       WR_DIS.RD_DIS                   EFUSE_BLK0         0               1
    3       WR_DIS.WR_DIS                   EFUSE_BLK0         1               1
    4       WR_DIS.FLASH_CRYPT_CNT          EFUSE_BLK0         2               1
    5       WR_DIS.UART_DOWNLOAD_DIS        EFUSE_BLK0         2               1
    6       WR_DIS.MAC                      EFUSE_BLK0         3               1
    7       WR_DIS.MAC_CRC                  EFUSE_BLK0         3               1
    8       WR_DIS.DISABLE_APP_CPU          EFUSE_BLK0         3               1
    9       WR_DIS.DISABLE_BT               EFUSE_BLK0         3               1
    10      WR_DIS.DIS_CACHE                EFUSE_BLK0         3               1
    11      WR_DIS.VOL_LEVEL_HP_INV         EFUSE_BLK0         3               1
    12      WR_DIS.CLK8M_FREQ               EFUSE_BLK0         4               1
    13      WR_DIS.ADC_VREF                 EFUSE_BLK0         4               1
    14      WR_DIS.XPD_SDIO_REG             EFUSE_BLK0         5               1
    15      WR_DIS.XPD_SDIO_TIEH            EFUSE_BLK0         5               1
    16      WR_DIS.XPD_SDIO_FORCE           EFUSE_BLK0         5               1
    17      WR_DIS.SPI_PAD_CONFIG_CLK       EFUSE_BLK0         6               1
    18      WR_DIS.SPI_PAD_CONFIG_Q         EFUSE_BLK0         6               1
    19      WR_DIS.SPI_PAD_CONFIG_D         EFUSE_BLK0         6               1
    20      WR_DIS.SPI_PAD_CONFIG_CS0       EFUSE_BLK0         6               1
    21      WR_DIS.BLOCK1                   EFUSE_BLK0         7               1
    22      WR_DIS.BLOCK2                   EFUSE_BLK0         8               1
    23      WR_DIS.BLOCK3                   EFUSE_BLK0         9               1
    24      WR_DIS.CUSTOM_MAC_CRC           EFUSE_BLK0         9               1
    25      WR_DIS.CUSTOM_MAC               EFUSE_BLK0         9               1
    26      WR_DIS.ADC1_TP_LOW              EFUSE_BLK0         9               1
    27      WR_DIS.ADC1_TP_HIGH             EFUSE_BLK0         9               1
    28      WR_DIS.ADC2_TP_LOW              EFUSE_BLK0         9               1
    29      WR_DIS.ADC2_TP_HIGH             EFUSE_BLK0         9               1
    30      WR_DIS.SECURE_VERSION           EFUSE_BLK0         9               1
    31      WR_DIS.MAC_VERSION              EFUSE_BLK0         9               1
    32      WR_DIS.BLK3_PART_RESERVE        EFUSE_BLK0         10              1
    33      WR_DIS.FLASH_CRYPT_CONFIG       EFUSE_BLK0         10              1
    34      WR_DIS.CODING_SCHEME            EFUSE_BLK0         10              1
    35      WR_DIS.KEY_STATUS               EFUSE_BLK0         10              1
    36      WR_DIS.ABS_DONE_0               EFUSE_BLK0         12              1
    37      WR_DIS.ABS_DONE_1               EFUSE_BLK0         13              1
    38      WR_DIS.JTAG_DISABLE             EFUSE_BLK0         14              1
    39      WR_DIS.CONSOLE_DEBUG_DISABLE    EFUSE_BLK0         15              1
    40      WR_DIS.DISABLE_DL_ENCRYPT       EFUSE_BLK0         15              1
    41      WR_DIS.DISABLE_DL_DECRYPT       EFUSE_BLK0         15              1
    42      WR_DIS.DISABLE_DL_CACHE         EFUSE_BLK0         15              1
    43      RD_DIS                          EFUSE_BLK0         16              4
    44      RD_DIS.BLOCK1                   EFUSE_BLK0         16              1
    45      RD_DIS.BLOCK2                   EFUSE_BLK0         17              1
    46      RD_DIS.BLOCK3                   EFUSE_BLK0         18              1
    47      RD_DIS.CUSTOM_MAC_CRC           EFUSE_BLK0         18              1
    48      RD_DIS.CUSTOM_MAC               EFUSE_BLK0         18              1
    49      RD_DIS.ADC1_TP_LOW              EFUSE_BLK0         18              1
    50      RD_DIS.ADC1_TP_HIGH             EFUSE_BLK0         18              1
    51      RD_DIS.ADC2_TP_LOW              EFUSE_BLK0         18              1
    52      RD_DIS.ADC2_TP_HIGH             EFUSE_BLK0         18              1
    53      RD_DIS.SECURE_VERSION           EFUSE_BLK0         18              1
    54      RD_DIS.MAC_VERSION              EFUSE_BLK0         18              1
    55      RD_DIS.BLK3_PART_RESERVE        EFUSE_BLK0         19              1
    56      RD_DIS.FLASH_CRYPT_CONFIG       EFUSE_BLK0         19              1
    57      RD_DIS.CODING_SCHEME            EFUSE_BLK0         19              1
    58      RD_DIS.KEY_STATUS               EFUSE_BLK0         19              1
    59      FLASH_CRYPT_CNT                 EFUSE_BLK0         20              7
    60      UART_DOWNLOAD_DIS               EFUSE_BLK0         27              1
    61      MAC                             EFUSE_BLK0         32              8
    62      MAC                             EFUSE_BLK0         40              8
    63      MAC                             EFUSE_BLK0         48              8
    64      MAC                             EFUSE_BLK0         56              8
    65      MAC                             EFUSE_BLK0         64              8
    66      MAC                             EFUSE_BLK0         72              8
    67      MAC_CRC                         EFUSE_BLK0         80              8
    68      DISABLE_APP_CPU                 EFUSE_BLK0         96              1
    69      DISABLE_BT                      EFUSE_BLK0         97              1
    70      CHIP_PACKAGE_4BIT               EFUSE_BLK0         98              1
    71      DIS_CACHE                       EFUSE_BLK0         99              1
    72      SPI_PAD_CONFIG_HD               EFUSE_BLK0        100              5
    73      CHIP_PACKAGE                    EFUSE_BLK0        105              3
    74      CHIP_CPU_FREQ_LOW               EFUSE_BLK0        108              1
    75      CHIP_CPU_FREQ_RATED             EFUSE_BLK0        109              1
    76      BLK3_PART_RESERVE               EFUSE_BLK0        110              1
    77      CHIP_VER_REV1                   EFUSE_BLK0        111              1
    78      CLK8M_FREQ                      EFUSE_BLK0        128              8
    79      ADC_VREF                        EFUSE_BLK0        136              5
    80      XPD_SDIO_REG                    EFUSE_BLK0        142              1
    81      XPD_SDIO_TIEH                   EFUSE_BLK0        143              1
    82      XPD_SDIO_FORCE                  EFUSE_BLK0        144              1
    83      SPI_PAD_CONFIG_CLK              EFUSE_BLK0        160              5
    84      SPI_PAD_CONFIG_Q                EFUSE_BLK0        165              5
    85      SPI_PAD_CONFIG_D                EFUSE_BLK0        170              5
    86      SPI_PAD_CONFIG_CS0              EFUSE_BLK0        175              5
    87      CHIP_VER_REV2                   EFUSE_BLK0        180              1
    88      VOL_LEVEL_HP_INV                EFUSE_BLK0        182              2
    89      WAFER_VERSION_MINOR             EFUSE_BLK0        184              2
    90      FLASH_CRYPT_CONFIG              EFUSE_BLK0        188              4
    91      CODING_SCHEME                   EFUSE_BLK0        192              2
    92      CONSOLE_DEBUG_DISABLE           EFUSE_BLK0        194              1
    93      DISABLE_SDIO_HOST               EFUSE_BLK0        195              1
    94      ABS_DONE_0                      EFUSE_BLK0        196              1
    95      ABS_DONE_1                      EFUSE_BLK0        197              1
    96      JTAG_DISABLE                    EFUSE_BLK0        198              1
    97      DISABLE_DL_ENCRYPT              EFUSE_BLK0        199              1
    98      DISABLE_DL_DECRYPT              EFUSE_BLK0        200              1
    99      DISABLE_DL_CACHE                EFUSE_BLK0        201              1
    100     KEY_STATUS                      EFUSE_BLK0        202              1
    101     BLOCK1                          EFUSE_BLK1         0              192
    102     BLOCK2                          EFUSE_BLK2         0              192
    103     CUSTOM_MAC_CRC                  EFUSE_BLK3         0               8
    104     MAC_CUSTOM                      EFUSE_BLK3         8               48
    105     ADC1_TP_LOW                     EFUSE_BLK3         96              7
    106     ADC1_TP_HIGH                    EFUSE_BLK3        103              9
    107     ADC2_TP_LOW                     EFUSE_BLK3        112              7
    108     ADC2_TP_HIGH                    EFUSE_BLK3        119              9
    109     SECURE_VERSION                  EFUSE_BLK3        128              32
    110     MAC_VERSION                     EFUSE_BLK3        184              8

    Used bits in efuse table:
    EFUSE_BLK0
    [0 15] [0 2] [2 3] ... [19 19] [19 27] [32 87] [96 111] [128 140] [142 144] [160 180] [182 185] [188 202]
    EFUSE_BLK1
    [0 191]
    EFUSE_BLK2
    [0 191]
    EFUSE_BLK3
    [0 55] [96 159] [184 191]
    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)
