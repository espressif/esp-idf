
.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Parsing efuse CSV input file $IDF_PATH/components/efuse/esp32c2/esp_efuse_table.csv ...
    Verifying efuse table...
    Max number of bits in BLK 256
    Sorted efuse table:
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS                          EFUSE_BLK0         0               8    
    2       WR_DIS.KEY0_RD_DIS              EFUSE_BLK0         0               1    
    3       WR_DIS.GROUP_1                  EFUSE_BLK0         1               1    
    4       WR_DIS.GROUP_2                  EFUSE_BLK0         2               1    
    5       WR_DIS.SPI_BOOT_CRYPT_CNT       EFUSE_BLK0         2               1    
    6       WR_DIS.GROUP_3                  EFUSE_BLK0         3               1    
    7       WR_DIS.BLK0_RESERVED            EFUSE_BLK0         4               1    
    8       WR_DIS.SYS_DATA_PART0           EFUSE_BLK0         5               1    
    9       WR_DIS.SYS_DATA_PART1           EFUSE_BLK0         6               1    
    10      WR_DIS.KEY0                     EFUSE_BLK0         7               1    
    11      RD_DIS                          EFUSE_BLK0         32              2    
    12      RD_DIS.KEY0                     EFUSE_BLK0         32              2    
    13      RD_DIS.KEY0.LOW                 EFUSE_BLK0         32              1    
    14      RD_DIS.KEY0.HI                  EFUSE_BLK0         33              1    
    15      WDT_DELAY_SEL                   EFUSE_BLK0         34              2    
    16      DIS_PAD_JTAG                    EFUSE_BLK0         36              1    
    17      DIS_DOWNLOAD_ICACHE             EFUSE_BLK0         37              1    
    18      DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0         38              1    
    19      SPI_BOOT_CRYPT_CNT              EFUSE_BLK0         39              3    
    20      XTS_KEY_LENGTH_256              EFUSE_BLK0         42              1    
    21      UART_PRINT_CONTROL              EFUSE_BLK0         43              2    
    22      FORCE_SEND_RESUME               EFUSE_BLK0         45              1    
    23      DIS_DOWNLOAD_MODE               EFUSE_BLK0         46              1    
    24      DIS_DIRECT_BOOT                 EFUSE_BLK0         47              1    
    25      ENABLE_SECURITY_DOWNLOAD        EFUSE_BLK0         48              1    
    26      FLASH_TPUW                      EFUSE_BLK0         49              4    
    27      SECURE_BOOT_EN                  EFUSE_BLK0         53              1    
    28      SECURE_VERSION                  EFUSE_BLK0         54              4    
    29      USER_DATA                       EFUSE_BLK1         0               88   
    30      USER_DATA.MAC_CUSTOM            EFUSE_BLK1         0               48   
    31      MAC_FACTORY                     EFUSE_BLK2         0               8    
    32      MAC_FACTORY                     EFUSE_BLK2         8               8    
    33      MAC_FACTORY                     EFUSE_BLK2         16              8    
    34      MAC_FACTORY                     EFUSE_BLK2         24              8    
    35      MAC_FACTORY                     EFUSE_BLK2         32              8    
    36      MAC_FACTORY                     EFUSE_BLK2         40              8    
    37      WAFER_VERSION                   EFUSE_BLK2         48              3    
    38      PKG_VERSION                     EFUSE_BLK2         51              3    
    39      BLOCK2_VERSION                  EFUSE_BLK2         54              3    
    40      RF_REF_I_BIAS_CONFIG            EFUSE_BLK2         57              4    
    41      LDO_VOL_BIAS_CONFIG_LOW         EFUSE_BLK2         61              3    
    42      LDO_VOL_BIAS_CONFIG_HIGH        EFUSE_BLK2         64              27   
    43      PVT_LOW                         EFUSE_BLK2         91              5    
    44      PVT_HIGH                        EFUSE_BLK2         96              10   
    45      ADC_CALIBRATION_0               EFUSE_BLK2        106              22   
    46      ADC_CALIBRATION_1               EFUSE_BLK2        128              32   
    47      ADC_CALIBRATION_2               EFUSE_BLK2        160              32   
    48      KEY0                            EFUSE_BLK3         0              256   
    49      KEY0.FE_256BIT                  EFUSE_BLK3         0              256   
    50      KEY0.FE_128BIT                  EFUSE_BLK3         0              128   
    51      KEY0.SB_128BIT                  EFUSE_BLK3        128             128   

    Used bits in efuse table:
    EFUSE_BLK0 
    [0 7] [0 2] [2 7] [32 33] [32 33] [32 57] 

    EFUSE_BLK1 
    [0 87] [0 47] 

    EFUSE_BLK2 
    [0 191] 

    EFUSE_BLK3 
    [0 255] [0 255] [0 255] 

    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)
