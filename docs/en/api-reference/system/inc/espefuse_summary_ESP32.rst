.. code-block:: none

    idf.py efuse-summary

    Executing action: efuse-summary
    (...)

    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Calibration fuses:
    ADC_VREF (BLOCK0)                                  True ADC reference voltage                         = 1121 R/W (0b00011)

    Config fuses:
    WR_DIS (BLOCK0)                                    Efuse write disable mask                           = 0 R/W (0x0000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK1-3                      = 0 R/W (0x0)
    DISABLE_APP_CPU (BLOCK0)                           Disables APP CPU                                   = False R/W (0b0)
    DISABLE_BT (BLOCK0)                                Disables Bluetooth                                 = False R/W (0b0)
    DIS_CACHE (BLOCK0)                                 Disables cache                                     = False R/W (0b0)
    CHIP_CPU_FREQ_LOW (BLOCK0)                         If set alongside EFUSE_RD_CHIP_CPU_FREQ_RATED; the = False R/W (0b0)
                                                        ESP32's max CPU frequency is rated for 160MHz. 24
                                                       0MHz otherwise
    CHIP_CPU_FREQ_RATED (BLOCK0)                       If set; the ESP32's maximum CPU frequency has been = True R/W (0b1)
                                                        rated
    BLK3_PART_RESERVE (BLOCK0)                         BLOCK3 partially served for ADC calibration data   = False R/W (0b0)
    CLK8M_FREQ (BLOCK0)                                8MHz clock freq override                           = 51 R/W (0x33)
    VOL_LEVEL_HP_INV (BLOCK0)                          This field stores the voltage level for CPU to run = 0 R/W (0b00)
                                                        at 240 MHz; or for flash/PSRAM to run at 80 MHz.0
                                                       x0: level 7; 0x1: level 6; 0x2: level 5; 0x3:i
                                                       level 4. (RO)
    CODING_SCHEME (BLOCK0)                             Efuse variable block length scheme
       = NONE (BLK1-3 len=256 bits) R/W (0b00)
    CONSOLE_DEBUG_DISABLE (BLOCK0)                     Disable ROM BASIC interpreter fallback             = True R/W (0b1)
    DISABLE_SDIO_HOST (BLOCK0)                                                                            = False R/W (0b0)
    DISABLE_DL_CACHE (BLOCK0)                          Disable flash cache in UART bootloader             = False R/W (0b0)

    Flash fuses:
    FLASH_CRYPT_CNT (BLOCK0)                           Flash encryption is enabled if this field has an o = 0 R/W (0b0000000)
                                                       dd number of bits set
    FLASH_CRYPT_CONFIG (BLOCK0)                        Flash encryption config (key tweak bits)           = 0 R/W (0x0)

    Identity fuses:
    CHIP_PACKAGE_4BIT (BLOCK0)                         Chip package identifier #4bit                      = False R/W (0b0)
    CHIP_PACKAGE (BLOCK0)                              Chip package identifier                            = 1 R/W (0b001)
    CHIP_VER_REV1 (BLOCK0)                             bit is set to 1 for rev1 silicon                   = True R/W (0b1)
    CHIP_VER_REV2 (BLOCK0)                                                                                = True R/W (0b1)
    WAFER_VERSION_MINOR (BLOCK0)                                                                          = 0 R/W (0b00)
    WAFER_VERSION_MAJOR (BLOCK0)                       calc WAFER VERSION MAJOR from CHIP_VER_REV1 and CH = 3 R/W (0b011)
                                                       IP_VER_REV2 and apb_ctl_date (read only)
    PKG_VERSION (BLOCK0)                               calc Chip package = CHIP_PACKAGE_4BIT << 3 + CHIP_ = 1 R/W (0x1)
                                                       PACKAGE (read only)

    Jtag fuses:
    JTAG_DISABLE (BLOCK0)                              Disable JTAG                                       = False R/W (0b0)

    Mac fuses:
    MAC (BLOCK0)                                       MAC address
       = 94:b9:7e:5a:6e:58 (CRC 0xe2 OK) R/W
    MAC_CRC (BLOCK0)                                   CRC8 for MAC address                               = 226 R/W (0xe2)
    MAC_VERSION (BLOCK3)                               Version of the MAC field                           = 0 R/W (0x00)

    Security fuses:
    UART_DOWNLOAD_DIS (BLOCK0)                         Disable UART download mode. Valid for ESP32 V3 and = False R/W (0b0)
                                                        newer; only
    ABS_DONE_0 (BLOCK0)                                Secure boot V1 is enabled for bootloader image     = False R/W (0b0)
    ABS_DONE_1 (BLOCK0)                                Secure boot V2 is enabled for bootloader image     = False R/W (0b0)
    DISABLE_DL_ENCRYPT (BLOCK0)                        Disable flash encryption in UART bootloader        = False R/W (0b0)
    DISABLE_DL_DECRYPT (BLOCK0)                        Disable flash decryption in UART bootloader        = False R/W (0b0)
    KEY_STATUS (BLOCK0)                                Usage of efuse block 3 (reserved)                  = False R/W (0b0)
    SECURE_VERSION (BLOCK3)                            Secure version for anti-rollback                   = 0 R/W (0x00000000)
    BLOCK1 (BLOCK1)                                    Flash encryption key
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK2 (BLOCK2)                                    Security boot key
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK3 (BLOCK3)                                    Variable Block 3
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Spi Pad fuses:
    SPI_PAD_CONFIG_HD (BLOCK0)                         read for SPI_pad_config_hd                         = 0 R/W (0b00000)
    SPI_PAD_CONFIG_CLK (BLOCK0)                        Override SD_CLK pad (GPIO6/SPICLK)                 = 0 R/W (0b00000)
    SPI_PAD_CONFIG_Q (BLOCK0)                          Override SD_DATA_0 pad (GPIO7/SPIQ)                = 0 R/W (0b00000)
    SPI_PAD_CONFIG_D (BLOCK0)                          Override SD_DATA_1 pad (GPIO8/SPID)                = 0 R/W (0b00000)
    SPI_PAD_CONFIG_CS0 (BLOCK0)                        Override SD_CMD pad (GPIO11/SPICS0)                = 0 R/W (0b00000)

    Vdd fuses:
    XPD_SDIO_REG (BLOCK0)                              read for XPD_SDIO_REG                              = False R/W (0b0)
    XPD_SDIO_TIEH (BLOCK0)                             If XPD_SDIO_FORCE & XPD_SDIO_REG                   = 1.8V R/W (0b0)
    XPD_SDIO_FORCE (BLOCK0)                            Ignore MTDI pin (GPIO12) for VDD_SDIO on reset     = False R/W (0b0)

    Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V)
