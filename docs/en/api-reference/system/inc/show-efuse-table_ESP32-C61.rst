.. code-block:: none

    $ ./efuse_table_gen.py -t IDF_TARGET_PATH_NAME {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Parsing efuse CSV input file esp32c61/esp_efuse_table.csv ...
    Verifying efuse table...
    Sorted efuse table:
    #   field_name                              efuse_block     bit_start   bit_count
    1   WR_DIS                                  EFUSE_BLK0      0           32
    2   WR_DIS.RD_DIS                           EFUSE_BLK0      0           1
    3   WR_DIS.DIS_ICACHE                       EFUSE_BLK0      2           1
    4   WR_DIS.DIS_USB_JTAG                     EFUSE_BLK0      2           1
    5   WR_DIS.DIS_FORCE_DOWNLOAD               EFUSE_BLK0      2           1
    6   WR_DIS.SPI_DOWNLOAD_MSPI_DIS            EFUSE_BLK0      2           1
    7   WR_DIS.JTAG_SEL_ENABLE                  EFUSE_BLK0      2           1
    8   WR_DIS.DIS_PAD_JTAG                     EFUSE_BLK0      2           1
    9   WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT      EFUSE_BLK0      2           1
    10  WR_DIS.HYS_EN_PAD                       EFUSE_BLK0      2           1
    11  WR_DIS.DIS_WIFI6                        EFUSE_BLK0      2           1
    12  WR_DIS.WDT_DELAY_SEL                    EFUSE_BLK0      3           1
    13  WR_DIS.SPI_BOOT_CRYPT_CNT               EFUSE_BLK0      4           1
    14  WR_DIS.SECURE_BOOT_KEY_REVOKE0          EFUSE_BLK0      5           1
    15  WR_DIS.SECURE_BOOT_KEY_REVOKE1          EFUSE_BLK0      6           1
    16  WR_DIS.SECURE_BOOT_KEY_REVOKE2          EFUSE_BLK0      7           1
    17  WR_DIS.KEY_PURPOSE_0                    EFUSE_BLK0      8           1
    18  WR_DIS.KEY_PURPOSE_1                    EFUSE_BLK0      9           1
    19  WR_DIS.KEY_PURPOSE_2                    EFUSE_BLK0      10          1
    20  WR_DIS.KEY_PURPOSE_3                    EFUSE_BLK0      11          1
    21  WR_DIS.KEY_PURPOSE_4                    EFUSE_BLK0      12          1
    22  WR_DIS.KEY_PURPOSE_5                    EFUSE_BLK0      13          1
    23  WR_DIS.SEC_DPA_LEVEL                    EFUSE_BLK0      14          1
    24  WR_DIS.XTS_DPA_CLK_ENABLE               EFUSE_BLK0      14          1
    25  WR_DIS.XTS_DPA_PSEUDO_LEVEL             EFUSE_BLK0      14          1
    26  WR_DIS.ECDSA_DISABLE_P192               EFUSE_BLK0      14          1
    27  WR_DIS.ECC_FORCE_CONST_TIME             EFUSE_BLK0      14          1
    28  WR_DIS.SECURE_BOOT_EN                   EFUSE_BLK0      15          1
    29  WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE    EFUSE_BLK0      16          1
    30  WR_DIS.FLASH_TPUW                       EFUSE_BLK0      18          1
    31  WR_DIS.DIS_DOWNLOAD_MODE                EFUSE_BLK0      18          1
    32  WR_DIS.DIS_DIRECT_BOOT                  EFUSE_BLK0      18          1
    33  WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT    EFUSE_BLK0      18          1
    34  WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE EFUSE_BLK0     18          1
    35  WR_DIS.ENABLE_SECURITY_DOWNLOAD         EFUSE_BLK0      18          1
    36  WR_DIS.UART_PRINT_CONTROL               EFUSE_BLK0      18          1
    37  WR_DIS.FORCE_SEND_RESUME                EFUSE_BLK0      18          1
    38  WR_DIS.SECURE_VERSION                   EFUSE_BLK0      18          1
    39  WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE    EFUSE_BLK0      18          1
    40  WR_DIS.BLK1                             EFUSE_BLK0      20          1
    41  WR_DIS.MAC                              EFUSE_BLK0      20          1
    42  WR_DIS.WAFER_VERSION_MINOR              EFUSE_BLK0      20          1
    43  WR_DIS.WAFER_VERSION_MAJOR              EFUSE_BLK0      20          1
    44  WR_DIS.DISABLE_WAFER_VERSION_MAJOR      EFUSE_BLK0      20          1
    45  WR_DIS.DISABLE_BLK_VERSION_MAJOR        EFUSE_BLK0      20          1
    46  WR_DIS.BLK_VERSION_MINOR                EFUSE_BLK0      20          1
    47  WR_DIS.BLK_VERSION_MAJOR                EFUSE_BLK0      20          1
    48  WR_DIS.FLASH_CAP                        EFUSE_BLK0      20          1
    49  WR_DIS.FLASH_VENDOR                     EFUSE_BLK0      20          1
    50  WR_DIS.PSRAM_CAP                        EFUSE_BLK0      20          1
    51  WR_DIS.PSRAM_VENDOR                     EFUSE_BLK0      20          1
    52  WR_DIS.TEMP                             EFUSE_BLK0      20          1
    53  WR_DIS.PKG_VERSION                      EFUSE_BLK0      20          1
    54  WR_DIS.ACTIVE_HP_DBIAS                  EFUSE_BLK0      20          1
    55  WR_DIS.ACTIVE_LP_DBIAS                  EFUSE_BLK0      20          1
    56  WR_DIS.LSLP_HP_DBG                      EFUSE_BLK0      20          1
    57  WR_DIS.LSLP_HP_DBIAS                    EFUSE_BLK0      20          1
    58  WR_DIS.DSLP_LP_DBG                      EFUSE_BLK0      20          1
    59  WR_DIS.DSLP_LP_DBIAS                    EFUSE_BLK0      20          1
    60  WR_DIS.LP_HP_DBIAS_VOL_GAP              EFUSE_BLK0      20          1
    61  WR_DIS.SYS_DATA_PART1                   EFUSE_BLK0      21          1
    62  WR_DIS.OPTIONAL_UNIQUE_ID               EFUSE_BLK0      21          1
    63  WR_DIS.TEMPERATURE_SENSOR               EFUSE_BLK0      21          1
    64  WR_DIS.OCODE                            EFUSE_BLK0      21          1
    65  WR_DIS.ADC1_AVE_INIT_CODE_ATTEN0        EFUSE_BLK0      21          1
    66  WR_DIS.ADC1_AVE_INIT_CODE_ATTEN1        EFUSE_BLK0      21          1
    67  WR_DIS.ADC1_AVE_INIT_CODE_ATTEN2        EFUSE_BLK0      21          1
    68  WR_DIS.ADC1_AVE_INIT_CODE_ATTEN3        EFUSE_BLK0      21          1
    69  WR_DIS.ADC1_HI_DOUT_ATTEN0              EFUSE_BLK0      21          1
    70  WR_DIS.ADC1_HI_DOUT_ATTEN1              EFUSE_BLK0      21          1
    71  WR_DIS.ADC1_HI_DOUT_ATTEN2              EFUSE_BLK0      21          1
    72  WR_DIS.ADC1_HI_DOUT_ATTEN3              EFUSE_BLK0      21          1
    73  WR_DIS.ADC1_CH0_ATTEN0_INITCODE_DIFF    EFUSE_BLK0      21          1
    74  WR_DIS.ADC1_CH1_ATTEN0_INITCODE_DIFF    EFUSE_BLK0      21          1
    75  WR_DIS.ADC1_CH2_ATTEN0_INITCODE_DIFF    EFUSE_BLK0      21          1
    76  WR_DIS.ADC1_CH3_ATTEN0_INITCODE_DIFF    EFUSE_BLK0      21          1
    77  WR_DIS.BLOCK_USR_DATA                   EFUSE_BLK0      22          1
    78  WR_DIS.CUSTOM_MAC                       EFUSE_BLK0      22          1
    79  WR_DIS.BLOCK_KEY0                       EFUSE_BLK0      23          1
    80  WR_DIS.BLOCK_KEY1                       EFUSE_BLK0      24          1
    81  WR_DIS.BLOCK_KEY2                       EFUSE_BLK0      25          1
    82  WR_DIS.BLOCK_KEY3                       EFUSE_BLK0      26          1
    83  WR_DIS.BLOCK_KEY4                       EFUSE_BLK0      27          1
    84  WR_DIS.BLOCK_KEY5                       EFUSE_BLK0      28          1
    85  WR_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0      29          1
    86  WR_DIS.USB_EXCHG_PINS                   EFUSE_BLK0      30          1
    87  WR_DIS.VDD_SPI_AS_GPIO                  EFUSE_BLK0      30          1
    88  RD_DIS                                  EFUSE_BLK0      32          7
    89  RD_DIS.BLOCK_KEY0                       EFUSE_BLK0      32          1
    90  RD_DIS.BLOCK_KEY1                       EFUSE_BLK0      33          1
    91  RD_DIS.BLOCK_KEY2                       EFUSE_BLK0      34          1
    92  RD_DIS.BLOCK_KEY3                       EFUSE_BLK0      35          1
    93  RD_DIS.BLOCK_KEY4                       EFUSE_BLK0      36          1
    94  RD_DIS.BLOCK_KEY5                       EFUSE_BLK0      37          1
    95  RD_DIS.BLOCK_SYS_DATA2                  EFUSE_BLK0      38          1
    96  DIS_ICACHE                              EFUSE_BLK0      39          1
    97  DIS_USB_JTAG                            EFUSE_BLK0      40          1
    98  DIS_FORCE_DOWNLOAD                      EFUSE_BLK0      42          1
    99  SPI_DOWNLOAD_MSPI_DIS                   EFUSE_BLK0      43          1
    100 JTAG_SEL_ENABLE                         EFUSE_BLK0      44          1
    101 DIS_PAD_JTAG                            EFUSE_BLK0      45          1
    102 DIS_DOWNLOAD_MANUAL_ENCRYPT             EFUSE_BLK0      46          1
    103 USB_EXCHG_PINS                          EFUSE_BLK0      51          1
    104 VDD_SPI_AS_GPIO                         EFUSE_BLK0      52          1
    105 WDT_DELAY_SEL                           EFUSE_BLK0      53          2
    106 SPI_BOOT_CRYPT_CNT                      EFUSE_BLK0      55          3
    107 SECURE_BOOT_KEY_REVOKE0                 EFUSE_BLK0      58          1
    108 SECURE_BOOT_KEY_REVOKE1                 EFUSE_BLK0      59          1
    109 SECURE_BOOT_KEY_REVOKE2                 EFUSE_BLK0      60          1
    110 KEY_PURPOSE_0                           EFUSE_BLK0      64          4
    111 KEY_PURPOSE_1                           EFUSE_BLK0      68          4
    112 KEY_PURPOSE_2                           EFUSE_BLK0      72          4
    113 KEY_PURPOSE_3                           EFUSE_BLK0      76          4
    114 KEY_PURPOSE_4                           EFUSE_BLK0      80          4
    115 KEY_PURPOSE_5                           EFUSE_BLK0      84          4
    116 SEC_DPA_LEVEL                           EFUSE_BLK0      88          2
    117 SECURE_BOOT_EN                          EFUSE_BLK0      90          1
    118 SECURE_BOOT_AGGRESSIVE_REVOKE           EFUSE_BLK0      91          1
    119 FLASH_TPUW                              EFUSE_BLK0      92          4
    120 DIS_DOWNLOAD_MODE                       EFUSE_BLK0      96          1
    121 DIS_DIRECT_BOOT                         EFUSE_BLK0      97          1
    122 DIS_USB_SERIAL_JTAG_ROM_PRINT           EFUSE_BLK0      98          1
    123 DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE       EFUSE_BLK0      99          1
    124 ENABLE_SECURITY_DOWNLOAD                EFUSE_BLK0      100         1
    125 UART_PRINT_CONTROL                      EFUSE_BLK0      101         2
    126 FORCE_SEND_RESUME                       EFUSE_BLK0      103         1
    127 SECURE_VERSION                          EFUSE_BLK0      104         16
    128 SECURE_BOOT_DISABLE_FAST_WAKE           EFUSE_BLK0      120         1
    129 HYS_EN_PAD                              EFUSE_BLK0      121         1
    130 XTS_DPA_CLK_ENABLE                      EFUSE_BLK0      122         1
    131 XTS_DPA_PSEUDO_LEVEL                    EFUSE_BLK0      123         2
    132 DIS_WIFI6                               EFUSE_BLK0      125         1
    133 ECDSA_DISABLE_P192                      EFUSE_BLK0      126         1
    134 ECC_FORCE_CONST_TIME                    EFUSE_BLK0      127         1
    135 BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION EFUSE_BLK0      128         4
    136 BOOTLOADER_ANTI_ROLLBACK_EN             EFUSE_BLK0      132         1
    137 BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM  EFUSE_BLK0      133         1
    138 RECOVERY_BOOTLOADER_FLASH_SECTOR        EFUSE_BLK0      134         12
    139 REPEAT_DATA4                            EFUSE_BLK0      160         24
    140 MAC                                     EFUSE_BLK1      0           8
    141 MAC                                     EFUSE_BLK1      8           8
    142 MAC                                     EFUSE_BLK1      16          8
    143 MAC                                     EFUSE_BLK1      24          8
    144 MAC                                     EFUSE_BLK1      32          8
    145 MAC                                     EFUSE_BLK1      40          8
    146 WAFER_VERSION_MINOR                     EFUSE_BLK1      64          4
    147 WAFER_VERSION_MAJOR                     EFUSE_BLK1      68          2
    148 DISABLE_WAFER_VERSION_MAJOR             EFUSE_BLK1      70          1
    149 DISABLE_BLK_VERSION_MAJOR               EFUSE_BLK1      71          1
    150 BLK_VERSION_MINOR                       EFUSE_BLK1      72          3
    151 BLK_VERSION_MAJOR                       EFUSE_BLK1      75          2
    152 FLASH_CAP                               EFUSE_BLK1      77          3
    153 FLASH_VENDOR                            EFUSE_BLK1      80          3
    154 PSRAM_CAP                               EFUSE_BLK1      83          3
    155 PSRAM_VENDOR                            EFUSE_BLK1      86          2
    156 TEMP                                    EFUSE_BLK1      88          2
    157 PKG_VERSION                             EFUSE_BLK1      90          3
    158 ACTIVE_HP_DBIAS                         EFUSE_BLK1      93          4
    159 ACTIVE_LP_DBIAS                         EFUSE_BLK1      97          4
    160 LSLP_HP_DBG                             EFUSE_BLK1      101         2
    161 LSLP_HP_DBIAS                           EFUSE_BLK1      103         4
    162 DSLP_LP_DBG                             EFUSE_BLK1      107         4
    163 DSLP_LP_DBIAS                           EFUSE_BLK1      111         5
    164 LP_HP_DBIAS_VOL_GAP                     EFUSE_BLK1      116         5
    165 SYS_DATA_PART2                          EFUSE_BLK10     0           256
    166 OPTIONAL_UNIQUE_ID                      EFUSE_BLK2      0           128
    167 TEMPERATURE_SENSOR                      EFUSE_BLK2      128         9
    168 OCODE                                   EFUSE_BLK2      137         8
    169 ADC1_AVE_INIT_CODE_ATTEN0               EFUSE_BLK2      145         10
    170 ADC1_AVE_INIT_CODE_ATTEN1               EFUSE_BLK2      155         10
    171 ADC1_AVE_INIT_CODE_ATTEN2               EFUSE_BLK2      165         10
    172 ADC1_AVE_INIT_CODE_ATTEN3               EFUSE_BLK2      175         10
    173 ADC1_HI_DOUT_ATTEN0                     EFUSE_BLK2      185         10
    174 ADC1_HI_DOUT_ATTEN1                     EFUSE_BLK2      195         10
    175 ADC1_HI_DOUT_ATTEN2                     EFUSE_BLK2      205         10
    176 ADC1_HI_DOUT_ATTEN3                     EFUSE_BLK2      215         10
    177 ADC1_CH0_ATTEN0_INITCODE_DIFF           EFUSE_BLK2      225         4
    178 ADC1_CH1_ATTEN0_INITCODE_DIFF           EFUSE_BLK2      229         4
    179 ADC1_CH2_ATTEN0_INITCODE_DIFF           EFUSE_BLK2      233         4
    180 ADC1_CH3_ATTEN0_INITCODE_DIFF           EFUSE_BLK2      237         4
    181 USER_DATA                               EFUSE_BLK3      0           256
    182 USER_DATA.MAC_CUSTOM                    EFUSE_BLK3      200         48
    183 KEY0                                    EFUSE_BLK4      0           256
    184 KEY1                                    EFUSE_BLK5      0           256
    185 KEY2                                    EFUSE_BLK6      0           256
    186 KEY3                                    EFUSE_BLK7      0           256
    187 KEY4                                    EFUSE_BLK8      0           256
    188 KEY5                                    EFUSE_BLK9      0           256

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 0] [2 2] ... [64 145] [160 183]

    EFUSE_BLK1
    [0 47] [64 120]

    EFUSE_BLK10
    [0 255]

    EFUSE_BLK2
    [0 240]

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
