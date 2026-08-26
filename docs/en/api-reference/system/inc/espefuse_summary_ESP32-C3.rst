.. code-block:: none

    idf.py efuse-summary

    Executing action: efuse-summary
    (...)

    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Calibration fuses:
    K_RTC_LDO (BLOCK1)                                 BLOCK1 K_RTC_LDO                                   = -36 R/W (0b1001001)
    K_DIG_LDO (BLOCK1)                                 BLOCK1 K_DIG_LDO                                   = -64 R/W (0b1010000)
    V_RTC_DBIAS20 (BLOCK1)                             BLOCK1 voltage of rtc dbias20                      = -40 R/W (0x8a)
    V_DIG_DBIAS20 (BLOCK1)                             BLOCK1 voltage of digital dbias20                  = -76 R/W (0x93)
    DIG_DBIAS_HVT (BLOCK1)                             BLOCK1 digital dbias when hvt                      = -28 R/W (0b10111)
    THRES_HVT (BLOCK1)                                 BLOCK1 pvt threshold when hvt                      = 2000 R/W (0b0111110100)
    TEMP_CALIB (BLOCK2)                                Temperature calibration data                       = -7.2 R/W (0b101001000)
    OCODE (BLOCK2)                                     ADC OCode                                          = 78 R/W (0x4e)
    ADC1_INIT_CODE_ATTEN0 (BLOCK2)                     ADC1 init code at atten0                           = 1560 R/W (0b0110000110)
    ADC1_INIT_CODE_ATTEN1 (BLOCK2)                     ADC1 init code at atten1                           = -108 R/W (0b1000011011)
    ADC1_INIT_CODE_ATTEN2 (BLOCK2)                     ADC1 init code at atten2                           = -232 R/W (0b1000111010)
    ADC1_INIT_CODE_ATTEN3 (BLOCK2)                     ADC1 init code at atten3                           = -696 R/W (0b1010101110)
    ADC1_CAL_VOL_ATTEN0 (BLOCK2)                       ADC1 calibration voltage at atten0                 = -212 R/W (0b1000110101)
    ADC1_CAL_VOL_ATTEN1 (BLOCK2)                       ADC1 calibration voltage at atten1                 = 52 R/W (0b0000001101)
    ADC1_CAL_VOL_ATTEN2 (BLOCK2)                       ADC1 calibration voltage at atten2                 = -152 R/W (0b1000100110)
    ADC1_CAL_VOL_ATTEN3 (BLOCK2)                       ADC1 calibration voltage at atten3                 = -284 R/W (0b1001000111)

    Config fuses:
    WR_DIS (BLOCK0)                                    Disable programming of individual eFuses           = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK4-10                     = 0 R/W (0b0000000)
    DIS_ICACHE (BLOCK0)                                Set this bit to disable Icache                     = False R/W (0b0)
    DIS_TWAI (BLOCK0)                                  Set this bit to disable CAN function               = False R/W (0b0)
    DIS_DIRECT_BOOT (BLOCK0)                           Disable direct boot mode                           = False R/W (0b0)
    UART_PRINT_CONTROL (BLOCK0)                        Set the default UARTboot message output mode       = Enable R/W (0b00)
    ERR_RST_ENABLE (BLOCK0)                            Use BLOCK0 to check error record registers         = with check R/W (0b1)
    BLOCK_USR_DATA (BLOCK3)                            User data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK10)                          System data part 2 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TPUW (BLOCK0)                                Configures flash waiting time after power-up; in u = 0 R/W (0x0)
                                                       nit of ms. If the value is less than 15; the waiti
                                                       ng time is the configurable value; Otherwise; the
                                                       waiting time is twice the configurable value
    FORCE_SEND_RESUME (BLOCK0)                         Set this bit to force ROM code to send a resume co = False R/W (0b0)
                                                       mmand during SPI boot

    Identity fuses:
    DISABLE_WAFER_VERSION_MAJOR (BLOCK0)               Disables check of wafer version major              = False R/W (0b0)
    DISABLE_BLK_VERSION_MAJOR (BLOCK0)                 Disables check of blk version major                = False R/W (0b0)
    WAFER_VERSION_MINOR_LO (BLOCK1)                    WAFER_VERSION_MINOR least significant bits         = 3 R/W (0b011)
    PKG_VERSION (BLOCK1)                               Package version                                    = 0 R/W (0b000)
    BLK_VERSION_MINOR (BLOCK1)                         BLK_VERSION_MINOR                                  = 2 R/W (0b010)
    WAFER_VERSION_MINOR_HI (BLOCK1)                    WAFER_VERSION_MINOR most significant bit           = False R/W (0b0)
    WAFER_VERSION_MAJOR (BLOCK1)                       WAFER_VERSION_MAJOR                                = 0 R/W (0b00)
    OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID
       = 25 60 04 96 c3 fd 41 6f be ed 2c 51 1d e3 7e 21 R/W
    BLK_VERSION_MAJOR (BLOCK2)                         BLK_VERSION_MAJOR of BLOCK2                        = With calibration R/W (0b01)
    WAFER_VERSION_MINOR (BLOCK0)                       calc WAFER VERSION MINOR = WAFER_VERSION_MINOR_HI  = 3 R/W (0x3)
                                                       << 3 + WAFER_VERSION_MINOR_LO (read only)

    Jtag fuses:
    SOFT_DIS_JTAG (BLOCK0)                             Set these bits to disable JTAG in the soft way (od = 0 R/W (0b000)
                                                       d number 1 means disable ). JTAG can be enabled in
                                                        HMAC module
    DIS_PAD_JTAG (BLOCK0)                              Set this bit to disable JTAG in the hard way. JTAG = False R/W (0b0)
                                                        is disabled permanently

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
       = 58:cf:79:0f:96:8c (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC address
       = 00:00:00:00:00:00 (OK) R/W

    Security fuses:
    DIS_DOWNLOAD_ICACHE (BLOCK0)                       Set this bit to disable Icache in download mode (b = False R/W (0b0)
                                                       oot_mode[3:0] is 0; 1; 2; 3; 6; 7)
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Set this bit to disable the function that forces c = False R/W (0b0)
                                                       hip into download mode
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Set this bit to disable flash encryption when in   = False R/W (0b0)
                                                       download boot modes
    SPI_BOOT_CRYPT_CNT (BLOCK0)                        Enables flash encryption when 1 or 3 bits are set  = Disable R/W (0b000)
                                                       and disables otherwise
    SECURE_BOOT_KEY_REVOKE0 (BLOCK0)                   Revoke 1st secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE1 (BLOCK0)                   Revoke 2nd secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE2 (BLOCK0)                   Revoke 3rd secure boot key                         = False R/W (0b0)
    KEY_PURPOSE_0 (BLOCK0)                             Purpose of Key0                                    = USER R/W (0x0)
    KEY_PURPOSE_1 (BLOCK0)                             Purpose of Key1                                    = USER R/W (0x0)
    KEY_PURPOSE_2 (BLOCK0)                             Purpose of Key2                                    = USER R/W (0x0)
    KEY_PURPOSE_3 (BLOCK0)                             Purpose of Key3                                    = USER R/W (0x0)
    KEY_PURPOSE_4 (BLOCK0)                             Purpose of Key4                                    = USER R/W (0x0)
    KEY_PURPOSE_5 (BLOCK0)                             Purpose of Key5                                    = USER R/W (0x0)
    SECURE_BOOT_EN (BLOCK0)                            Set this bit to enable secure boot                 = False R/W (0b0)
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Set this bit to enable revoking aggressive secure  = False R/W (0b0)
                                                       boot
    DIS_DOWNLOAD_MODE (BLOCK0)                         Set this bit to disable download mode (boot_mode[3 = False R/W (0b0)
                                                       :0] = 0; 1; 2; 3; 6; 7)
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Set this bit to enable secure UART download mode   = False R/W (0b0)
    SECURE_VERSION (BLOCK0)                            Secure version (used by ESP-IDF anti-rollback      = 0 R/W (0x0000)
                                                       feature)
    BLOCK_KEY0 (BLOCK4)
      Purpose: USER
                   Key0 or user data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_KEY1 (BLOCK5)
      Purpose: USER
                   Key1 or user data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_KEY2 (BLOCK6)
      Purpose: USER
                   Key2 or user data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_KEY3 (BLOCK7)
      Purpose: USER
                   Key3 or user data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_KEY4 (BLOCK8)
      Purpose: USER
                   Key4 or user data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_KEY5 (BLOCK9)
      Purpose: USER
                   Key5 or user data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Spi Pad fuses:
    SPI_PAD_CONFIG_CLK (BLOCK1)                        SPI PAD CLK                                        = 0 R/W (0b000000)
    SPI_PAD_CONFIG_Q (BLOCK1)                          SPI PAD Q(D1)                                      = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D (BLOCK1)                          SPI PAD D(D0)                                      = 0 R/W (0b000000)
    SPI_PAD_CONFIG_CS (BLOCK1)                         SPI PAD CS                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_HD (BLOCK1)                         SPI PAD HD(D3)                                     = 0 R/W (0b000000)
    SPI_PAD_CONFIG_WP (BLOCK1)                         SPI PAD WP(D2)                                     = 0 R/W (0b000000)
    SPI_PAD_CONFIG_DQS (BLOCK1)                        SPI PAD DQS                                        = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D4 (BLOCK1)                         SPI PAD D4                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D5 (BLOCK1)                         SPI PAD D5                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D6 (BLOCK1)                         SPI PAD D6                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D7 (BLOCK1)                         SPI PAD D7                                         = 0 R/W (0b000000)

    Usb fuses:
    DIS_USB_JTAG (BLOCK0)                              Set this bit to disable function of usb switch to  = False R/W (0b0)
                                                       jtag in module of usb device
    DIS_USB_SERIAL_JTAG (BLOCK0)                       USB-Serial-JTAG                                    = Enable R/W (0b0)
    USB_EXCHG_PINS (BLOCK0)                            Set this bit to exchange USB D+ and D- pins        = False R/W (0b0)
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             USB printing                                       = Enable R/W (0b0)
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         Disable UART download mode through USB-Serial-JTAG = False R/W (0b0)

    Vdd fuses:
    VDD_SPI_AS_GPIO (BLOCK0)                           Set this bit to vdd spi pin function as gpio       = False R/W (0b0)

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK0)                             RTC watchdog timeout threshold; in unit of slow cl = 40000 R/W (0b00)
                                                       ock cycle
