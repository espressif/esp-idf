
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 192
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS_EFUSE_RD_DISABLE         EFUSE_BLK0         0               1    
    2       WR_DIS_FLASH_CRYPT_CNT          EFUSE_BLK0         2               1    
    3       WR_DIS_BLK1                     EFUSE_BLK0         7               1    
    4       WR_DIS_BLK2                     EFUSE_BLK0         8               1    
    5       WR_DIS_BLK3                     EFUSE_BLK0         9               1    
    6       RD_DIS_BLK1                     EFUSE_BLK0         16              1    
    7       RD_DIS_BLK2                     EFUSE_BLK0         17              1    
    8       RD_DIS_BLK3                     EFUSE_BLK0         18              1    
    9       FLASH_CRYPT_CNT                 EFUSE_BLK0         20              7    
    10      UART_DOWNLOAD_DIS               EFUSE_BLK0         27              1    
    11      MAC_FACTORY                     EFUSE_BLK0         32              8    
    12      MAC_FACTORY                     EFUSE_BLK0         40              8    
    13      MAC_FACTORY                     EFUSE_BLK0         48              8    
    14      MAC_FACTORY                     EFUSE_BLK0         56              8    
    15      MAC_FACTORY                     EFUSE_BLK0         64              8    
    16      MAC_FACTORY                     EFUSE_BLK0         72              8    
    17      MAC_FACTORY_CRC                 EFUSE_BLK0         80              8    
    18      CHIP_VER_DIS_APP_CPU            EFUSE_BLK0         96              1    
    19      CHIP_VER_DIS_BT                 EFUSE_BLK0         97              1    
    20      CHIP_VER_PKG                    EFUSE_BLK0         98              1    
    21      CHIP_VER_PKG                    EFUSE_BLK0        105              3    
    22      CHIP_CPU_FREQ_LOW               EFUSE_BLK0        108              1    
    23      CHIP_CPU_FREQ_RATED             EFUSE_BLK0        109              1    
    24      CHIP_VER_REV1                   EFUSE_BLK0        111              1    
    25      ADC_VREF_AND_SDIO_DREF          EFUSE_BLK0        136              6    
    26      XPD_SDIO_REG                    EFUSE_BLK0        142              1    
    27      SDIO_TIEH                       EFUSE_BLK0        143              1    
    28      SDIO_FORCE                      EFUSE_BLK0        144              1    
    29      CHIP_VER_REV2                   EFUSE_BLK0        180              1    
    30      ENCRYPT_CONFIG                  EFUSE_BLK0        188              4    
    31      CONSOLE_DEBUG_DISABLE           EFUSE_BLK0        194              1    
    32      ABS_DONE_0                      EFUSE_BLK0        196              1    
    33      ABS_DONE_1                      EFUSE_BLK0        197              1    
    34      DISABLE_JTAG                    EFUSE_BLK0        198              1    
    35      DISABLE_DL_ENCRYPT              EFUSE_BLK0        199              1    
    36      DISABLE_DL_DECRYPT              EFUSE_BLK0        200              1    
    37      DISABLE_DL_CACHE                EFUSE_BLK0        201              1    
    38      ENCRYPT_FLASH_KEY               EFUSE_BLK1         0              192   
    39      SECURE_BOOT_KEY                 EFUSE_BLK2         0              192   
    40      MAC_CUSTOM_CRC                  EFUSE_BLK3         0               8    
    41      MAC_CUSTOM                      EFUSE_BLK3         8               48   
    42      ADC1_TP_LOW                     EFUSE_BLK3         96              7    
    43      ADC1_TP_HIGH                    EFUSE_BLK3        103              9    
    44      ADC2_TP_LOW                     EFUSE_BLK3        112              7    
    45      ADC2_TP_HIGH                    EFUSE_BLK3        119              9    
    46      SECURE_VERSION                  EFUSE_BLK3        128              32   
    47      MAC_CUSTOM_VER                  EFUSE_BLK3        184              8    

    Used bits in efuse table:
    EFUSE_BLK0 
    [0 0] [2 2] [7 9] [16 18] [20 27] [32 87] [96 98] [105 109] [111 111] [136 144] [180 180] [188 191] [194 194] [196 201] 

    EFUSE_BLK1 
    [0 191] 

    EFUSE_BLK2 
    [0 191] 

    EFUSE_BLK3 
    [0 55] [96 159] [184 191] 

    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)
