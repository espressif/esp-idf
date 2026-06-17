.. code-block:: none

    $ ./efuse_table_gen.py -t {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info

    Max number of bits in BLK 256
    Sorted efuse table:
    #     field_name                         efuse_block     bit_start     bit_count
    1     WR_DIS                             EFUSE_BLK0        0            32
    2     WR_DIS.RD_DIS                      EFUSE_BLK0        0            1
    3     WR_DIS.KM_RND_SWITCH_CYCLE         EFUSE_BLK0        1            1
    4     WR_DIS.KM_DISABLE_DEPLOY_MODE      EFUSE_BLK0        1            1
    5     WR_DIS.KM_DEPLOY_ONLY_ONCE         EFUSE_BLK0        1            1
    6     WR_DIS.FORCE_USE_KEY_MANAGER_KEY     EFUSE_BLK0        1            1
    7     WR_DIS.FORCE_DISABLE_SW_INIT_KEY     EFUSE_BLK0        1            1
    8     WR_DIS.KM_XTS_KEY_LENGTH_256       EFUSE_BLK0        1            1
    9     WR_DIS.LOCK_KM_KEY                 EFUSE_BLK0        1            1
    10     WR_DIS.DIS_USB_JTAG                EFUSE_BLK0        2            1
    11     WR_DIS.DIS_FORCE_DOWNLOAD          EFUSE_BLK0        2            1
    12     WR_DIS.SPI_DOWNLOAD_MSPI_DIS       EFUSE_BLK0        2            1
    13     WR_DIS.DIS_TWAI                    EFUSE_BLK0        2            1
    14     WR_DIS.JTAG_SEL_ENABLE             EFUSE_BLK0        2            1
    15     WR_DIS.DIS_PAD_JTAG                EFUSE_BLK0        2            1
    16     WR_DIS.DIS_DOWNLOAD_MANUAL_ENCRYPT     EFUSE_BLK0        2            1
    17     WR_DIS.WDT_DELAY_SEL               EFUSE_BLK0        2            1
    18     WR_DIS.SECURE_BOOT_DISABLE_FAST_WAKE     EFUSE_BLK0        2            1
    19     WR_DIS.DIS_WDT                     EFUSE_BLK0        2            1
    20     WR_DIS.DIS_SWD                     EFUSE_BLK0        2            1
    21     WR_DIS.PMU_FLASH_POWER_SEL         EFUSE_BLK0        2            1
    22     WR_DIS.PMU_FLASH_POWER_SEL_EN      EFUSE_BLK0        2            1
    23     WR_DIS.POWER_GLITCH_EN             EFUSE_BLK0        2            1
    24     WR_DIS.PVT_0_GLITCH_EN             EFUSE_BLK0        3            1
    25     WR_DIS.PVT_0_GLITCH_MODE           EFUSE_BLK0        3            1
    26     WR_DIS.PVT_1_GLITCH_EN             EFUSE_BLK0        3            1
    27     WR_DIS.PVT_1_GLITCH_MODE           EFUSE_BLK0        3            1
    28     WR_DIS.SPI_BOOT_CRYPT_CNT          EFUSE_BLK0        4            1
    29     WR_DIS.SECURE_BOOT_KEY_REVOKE0     EFUSE_BLK0        5            1
    30     WR_DIS.SECURE_BOOT_KEY_REVOKE1     EFUSE_BLK0        6            1
    31     WR_DIS.SECURE_BOOT_KEY_REVOKE2     EFUSE_BLK0        7            1
    32     WR_DIS.KEY_PURPOSE_0               EFUSE_BLK0        8            1
    33     WR_DIS.KEY_PURPOSE_1               EFUSE_BLK0        9            1
    34     WR_DIS.KEY_PURPOSE_2               EFUSE_BLK0        10           1
    35     WR_DIS.KEY_PURPOSE_3               EFUSE_BLK0        11           1
    36     WR_DIS.KEY_PURPOSE_4               EFUSE_BLK0        12           1
    37     WR_DIS.DIS_SM_CRYPT                EFUSE_BLK0        14           1
    38     WR_DIS.ECC_FORCE_CONST_TIME        EFUSE_BLK0        14           1
    39     WR_DIS.ECDSA_DISABLE_SOFT_K        EFUSE_BLK0        14           1
    40     WR_DIS.SEC_DPA_LEVEL               EFUSE_BLK0        14           1
    41     WR_DIS.XTS_DPA_CLK_ENABLE          EFUSE_BLK0        14           1
    42     WR_DIS.XTS_DPA_PSEUDO_LEVEL        EFUSE_BLK0        14           1
    43     WR_DIS.RMA_NONCE_ENA               EFUSE_BLK0        14           1
    44     WR_DIS.RMA_CHIP_INFO_SOURCE        EFUSE_BLK0        14           1
    45     WR_DIS.RMA_DISABLE_FAST_VEF        EFUSE_BLK0        14           1
    46     WR_DIS.ENA_XTS_SHADOW              EFUSE_BLK0        14           1
    47     WR_DIS.ENA_SPI_BOOT_CRYPT_SCRAMBLER     EFUSE_BLK0        14           1
    48     WR_DIS.RE_ENABLE_JTAG_SOURCE       EFUSE_BLK0        14           1
    49     WR_DIS.SECURE_BOOT_EN              EFUSE_BLK0        15           1
    50     WR_DIS.SECURE_BOOT_AGGRESSIVE_REVOKE     EFUSE_BLK0        16           1
    51     WR_DIS.FLASH_TYPE                  EFUSE_BLK0        18           1
    52     WR_DIS.DIS_USB_OTG_DOWNLOAD_MODE     EFUSE_BLK0        18           1
    53     WR_DIS.FLASH_TPUW                  EFUSE_BLK0        18           1
    54     WR_DIS.DIS_DOWNLOAD_MODE           EFUSE_BLK0        18           1
    55     WR_DIS.DIS_DIRECT_BOOT             EFUSE_BLK0        18           1
    56     WR_DIS.DIS_USB_SERIAL_JTAG_ROM_PRINT     EFUSE_BLK0        18           1
    57     WR_DIS.DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE     EFUSE_BLK0        18           1
    58     WR_DIS.ENABLE_SECURITY_DOWNLOAD     EFUSE_BLK0        18           1
    59     WR_DIS.UART_PRINT_CONTROL          EFUSE_BLK0        18           1
    60     WR_DIS.FORCE_SEND_RESUME           EFUSE_BLK0        18           1
    61     WR_DIS.SECURE_VERSION              EFUSE_BLK0        18           1
    62     WR_DIS.HUK_GEN_STATE               EFUSE_BLK0        19           1
    63     WR_DIS.BLK1                        EFUSE_BLK0        20           1
    64     WR_DIS.MAC                         EFUSE_BLK0        20           1
    65     WR_DIS.MAC_EXT                     EFUSE_BLK0        20           1
    66     WR_DIS.WAFER_VERSION_MINOR         EFUSE_BLK0        20           1
    67     WR_DIS.WAFER_VERSION_MAJOR         EFUSE_BLK0        20           1
    68     WR_DIS.DISABLE_WAFER_VERSION_MAJOR     EFUSE_BLK0        20           1
    69     WR_DIS.DISABLE_BLK_VERSION_MAJOR     EFUSE_BLK0        20           1
    70     WR_DIS.BLK_VERSION_MINOR           EFUSE_BLK0        20           1
    71     WR_DIS.BLK_VERSION_MAJOR           EFUSE_BLK0        20           1
    72     WR_DIS.PSRAM_CAP                   EFUSE_BLK0        20           1
    73     WR_DIS.TEMP                        EFUSE_BLK0        20           1
    74     WR_DIS.PSRAM_VENDOR                EFUSE_BLK0        20           1
    75     WR_DIS.PKG_VERSION                 EFUSE_BLK0        20           1
    76     WR_DIS.SYS_DATA_PART1              EFUSE_BLK0        21           1
    77     WR_DIS.OPTIONAL_UNIQUE_ID          EFUSE_BLK0        21           1
    78     WR_DIS.BLOCK_USR_DATA              EFUSE_BLK0        22           1
    79     WR_DIS.CUSTOM_MAC                  EFUSE_BLK0        22           1
    80     WR_DIS.BLOCK_KEY0                  EFUSE_BLK0        23           1
    81     WR_DIS.BLOCK_KEY1                  EFUSE_BLK0        24           1
    82     WR_DIS.BLOCK_KEY2                  EFUSE_BLK0        25           1
    83     WR_DIS.BLOCK_KEY3                  EFUSE_BLK0        26           1
    84     WR_DIS.BLOCK_KEY4                  EFUSE_BLK0        27           1
    85     WR_DIS.BLOCK_SYS_DATA2             EFUSE_BLK0        28           1
    86     WR_DIS.USB_DEVICE_EXCHG_PINS       EFUSE_BLK0        28           1
    87     WR_DIS.SOFT_DIS_JTAG               EFUSE_BLK0        31           1
    88     RD_DIS                             EFUSE_BLK0        32           7
    89     RD_DIS.BLOCK_KEY0                  EFUSE_BLK0        32           1
    90     RD_DIS.BLOCK_KEY1                  EFUSE_BLK0        33           1
    91     RD_DIS.BLOCK_KEY2                  EFUSE_BLK0        34           1
    92     RD_DIS.BLOCK_KEY3                  EFUSE_BLK0        35           1
    93     RD_DIS.BLOCK_KEY4                  EFUSE_BLK0        36           1
    94     DIS_USB_JTAG                       EFUSE_BLK0        42           1
    95     DIS_FORCE_DOWNLOAD                 EFUSE_BLK0        44           1
    96     SPI_DOWNLOAD_MSPI_DIS              EFUSE_BLK0        45           1
    97     DIS_TWAI                           EFUSE_BLK0        46           1
    98     JTAG_SEL_ENABLE                    EFUSE_BLK0        47           1
    99     SOFT_DIS_JTAG                      EFUSE_BLK0        48           3
    100     DIS_PAD_JTAG                       EFUSE_BLK0        51           1
    101     DIS_DOWNLOAD_MANUAL_ENCRYPT        EFUSE_BLK0        52           1
    102     HUK_GEN_STATE                      EFUSE_BLK0        55           5
    103     KM_RND_SWITCH_CYCLE                EFUSE_BLK0        64           1
    104     KM_DISABLE_DEPLOY_MODE             EFUSE_BLK0        66           5
    105     KM_DEPLOY_ONLY_ONCE                EFUSE_BLK0        71           5
    106     FORCE_USE_KEY_MANAGER_KEY          EFUSE_BLK0        76           5
    107     FORCE_DISABLE_SW_INIT_KEY          EFUSE_BLK0        81           1
    108     KM_XTS_KEY_LENGTH_256              EFUSE_BLK0        82           1
    109     WDT_DELAY_SEL                      EFUSE_BLK0        83           1
    110     DIS_SM_CRYPT                       EFUSE_BLK0        84           1
    111     SPI_BOOT_CRYPT_CNT                 EFUSE_BLK0        85           3
    112     SECURE_BOOT_KEY_REVOKE0            EFUSE_BLK0        88           1
    113     SECURE_BOOT_KEY_REVOKE1            EFUSE_BLK0        89           1
    114     SECURE_BOOT_KEY_REVOKE2            EFUSE_BLK0        90           1
    115     KEY_PURPOSE_0                      EFUSE_BLK0        96           5
    116     KEY_PURPOSE_1                      EFUSE_BLK0       101           5
    117     KEY_PURPOSE_2                      EFUSE_BLK0       106           5
    118     KEY_PURPOSE_3                      EFUSE_BLK0       111           5
    119     KEY_PURPOSE_4                      EFUSE_BLK0       116           5
    120     ECC_FORCE_CONST_TIME               EFUSE_BLK0       121           1
    121     ECDSA_DISABLE_SOFT_K               EFUSE_BLK0       122           1
    122     SEC_DPA_LEVEL                      EFUSE_BLK0       123           2
    123     XTS_DPA_CLK_ENABLE                 EFUSE_BLK0       125           1
    124     XTS_DPA_PSEUDO_LEVEL               EFUSE_BLK0       128           2
    125     SECURE_BOOT_EN                     EFUSE_BLK0       130           1
    126     SECURE_BOOT_AGGRESSIVE_REVOKE      EFUSE_BLK0       131           1
    127     FLASH_TYPE                         EFUSE_BLK0       133           1
    128     DIS_USB_OTG_DOWNLOAD_MODE          EFUSE_BLK0       137           1
    129     FLASH_TPUW                         EFUSE_BLK0       140           4
    130     DIS_DOWNLOAD_MODE                  EFUSE_BLK0       144           1
    131     DIS_DIRECT_BOOT                    EFUSE_BLK0       145           1
    132     DIS_USB_SERIAL_JTAG_ROM_PRINT      EFUSE_BLK0       146           1
    133     LOCK_KM_KEY                        EFUSE_BLK0       147           1
    134     DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE     EFUSE_BLK0       148           1
    135     ENABLE_SECURITY_DOWNLOAD           EFUSE_BLK0       149           1
    136     UART_PRINT_CONTROL                 EFUSE_BLK0       150           2
    137     FORCE_SEND_RESUME                  EFUSE_BLK0       152           1
    138     SECURE_VERSION                     EFUSE_BLK0       160           16
    139     SECURE_BOOT_DISABLE_FAST_WAKE      EFUSE_BLK0       176           1
    140     HYS_EN_PAD                         EFUSE_BLK0       177           1
    141     DCDC_VSET_EN                       EFUSE_BLK0       194           1
    142     DIS_WDT                            EFUSE_BLK0       195           1
    143     DIS_SWD                            EFUSE_BLK0       196           1
    144     SECURE_BOOT_SHA384_EN              EFUSE_BLK0       203           1
    145     BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION     EFUSE_BLK0       204           4
    146     BOOTLOADER_ANTI_ROLLBACK_EN        EFUSE_BLK0       208           1
    147     BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM     EFUSE_BLK0       209           1
    148     RECOVERY_BOOTLOADER_FLASH_SECTOR     EFUSE_BLK0       210           12
    149     RMA_ENA                            EFUSE_BLK0       222           2
    150     RMA_SESSION_COUNTER                EFUSE_BLK0       224           3
    151     RMA_NONCE_ENA                      EFUSE_BLK0       227           2
    152     RMA_CHIP_INFO_SOURCE               EFUSE_BLK0       229           1
    153     RMA_DISABLE_FAST_VEF               EFUSE_BLK0       230           1
    154     PVT_0_GLITCH_EN                    EFUSE_BLK0       231           1
    155     PVT_0_GLITCH_MODE                  EFUSE_BLK0       232           2
    156     PVT_1_GLITCH_EN                    EFUSE_BLK0       234           1
    157     PVT_1_GLITCH_MODE                  EFUSE_BLK0       235           2
    158     PMU_FLASH_POWER_SEL                EFUSE_BLK0       237           1
    159     PMU_FLASH_POWER_SEL_EN             EFUSE_BLK0       238           1
    160     POWER_GLITCH_EN                    EFUSE_BLK0       239           4
    161     ENA_XTS_SHADOW                     EFUSE_BLK0       243           1
    162     ENA_SPI_BOOT_CRYPT_SCRAMBLER       EFUSE_BLK0       244           1
    163     RE_ENABLE_JTAG_SOURCE              EFUSE_BLK0       245           1
    164     MAC                                EFUSE_BLK1        0            8
    165     MAC                                EFUSE_BLK1        8            8
    166     MAC                                EFUSE_BLK1        16           8
    167     MAC                                EFUSE_BLK1        24           8
    168     MAC                                EFUSE_BLK1        32           8
    169     MAC                                EFUSE_BLK1        40           8
    170     MAC_EXT                            EFUSE_BLK1        48           8
    171     MAC_EXT                            EFUSE_BLK1        56           8
    172     WAFER_VERSION_MINOR                EFUSE_BLK1       114           4
    173     WAFER_VERSION_MAJOR                EFUSE_BLK1       118           2
    174     DISABLE_WAFER_VERSION_MAJOR        EFUSE_BLK1       120           1
    175     DISABLE_BLK_VERSION_MAJOR          EFUSE_BLK1       121           1
    176     BLK_VERSION_MINOR                  EFUSE_BLK1       122           3
    177     BLK_VERSION_MAJOR                  EFUSE_BLK1       125           2
    178     PSRAM_CAP                          EFUSE_BLK1       127           3
    179     TEMP                               EFUSE_BLK1       130           2
    180     PSRAM_VENDOR                       EFUSE_BLK1       132           2
    181     PKG_VERSION                        EFUSE_BLK1       134           2
    182     OPTIONAL_UNIQUE_ID                 EFUSE_BLK2        0           128
    183     USER_DATA                          EFUSE_BLK3        0           256
    184     USER_DATA.MAC_CUSTOM               EFUSE_BLK3       200           48
    185     KEY0                               EFUSE_BLK4        0           256
    186     KEY1                               EFUSE_BLK5        0           256
    187     KEY2                               EFUSE_BLK6        0           256
    188     KEY3                               EFUSE_BLK7        0           256
    189     KEY4                               EFUSE_BLK8        0           256
    190     SYS_DATA_PART2                     EFUSE_BLK9        0            32
    191     USB_DEVICE_EXCHG_PINS              EFUSE_BLK9       192           1

    Used bits in efuse table:
    EFUSE_BLK0
    [0 31] [0 1] [1 1] [1 1] ... [140 152] [160 177] [194 196] [203 245]

    EFUSE_BLK1
    [0 63] [114 135]

    EFUSE_BLK2
    [0 127]

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
    [0 31] [192 192]

    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)
