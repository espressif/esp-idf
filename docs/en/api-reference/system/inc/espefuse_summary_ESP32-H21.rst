.. code-block:: none

    idf.py -p PORT efuse-summary

    Executing action: efuse-summary
    (...)

    === Run "summary" command ===
    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Config fuses:
    WR_DIS (BLOCK0)                                    Disable programming of individual eFuses           = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK4-10                     = 0 R/W (0b0000000)
    DIS_ICACHE (BLOCK0)                                Represents whether icache is disabled or enabled.  = False R/W (0b0)
                                                       1: disabled. 0: enabled
    POWERGLITCH_EN (BLOCK0)                            Represents whether power glitch function is        = False R/W (0b0)
                                                       enabled. 1: enabled. 0: disabled
    DIS_TWAI (BLOCK0)                                  Represents whether TWAI function is disabled or    = False R/W (0b0)
                                                       enabled. 1: disabled. 0: enabled
    RPT4_RESERVED0_2 (BLOCK0)                          Reserved                                           = 0 R/W (0b00)
    RPT4_RESERVED0_1 (BLOCK0)                          Reserved                                           = False R/W (0b0)
    DIS_DIRECT_BOOT (BLOCK0)                           Represents whether direct boot mode is disabled or = False R/W (0b0)
                                                        enabled. 1: disabled. 0: enabled
    UART_PRINT_CONTROL (BLOCK0)                        Set the default UARTboot message output mode       = Enable R/W (0b00)
    HYS_EN_PAD0 (BLOCK0)                               Set bits to enable hysteresis function of PAD0~5   = 0 R/W (0b000000)
    HYS_EN_PAD1 (BLOCK0)                               Set bits to enable hysteresis function of PAD6~27  = 0 R/W (0b0000000000000000000000)
    RXIQ_0 (BLOCK1)                                    Stores RF Calibration data. RXIQ data 0            = 0 R/W (0b0000000)
    RXIQ_1 (BLOCK1)                                    Stores RF Calibration data. RXIQ data 1            = 0 R/W (0b0000000)
    BLOCK_USR_DATA (BLOCK3)                            User data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK10)                          System data part 2 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TPUW (BLOCK0)                                Represents the flash waiting time after power-up;  = 0 R/W (0x0)
                                                       in unit of ms. When the value less than 15; the
                                                       waiting time is the programmed value. Otherwise; the
                                                        waiting time is 2 times the programmed value
    FORCE_SEND_RESUME (BLOCK0)                         Represents whether ROM code is forced to send a    = False R/W (0b0)
                                                       resume command during SPI boot. 1: forced. 0:not
                                                       forced
    FLASH_CAP (BLOCK1)                                 Stores the flash cap                               = 0 R/W (0b000)
    FLASH_TEMP (BLOCK1)                                Stores the flash temp                              = 0 R/W (0b00)
    FLASH_VENDOR (BLOCK1)                              Stores the flash vendor                            = 0 R/W (0b000)

    Identity fuses:
    RXIQ_VERSION (BLOCK1)                              Stores RF Calibration data. RXIQ version           = 0 R/W (0b000)
    WAFER_VERSION_MINOR (BLOCK1)                       Stores the wafer version minor                     = 0 R/W (0b000)
    WAFER_VERSION_MAJOR (BLOCK1)                       Stores the wafer version major                     = 0 R/W (0b00)
    DISABLE_WAFER_VERSION_MAJOR (BLOCK1)               Disables check of wafer version major              = False R/W (0b0)
    PKG_VERSION (BLOCK1)                               Package version                                    = 0 R/W (0b000)
    OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLK_VERSION_MINOR (BLOCK2)                         BLK_VERSION_MINOR of BLOCK2. 1: RF Calibration     = 0 R/W (0b000)
                                                       data in BLOCK1
    BLK_VERSION_MAJOR (BLOCK2)                         BLK_VERSION_MAJOR of BLOCK2                        = 0 R/W (0b00)
    DISABLE_BLK_VERSION_MAJOR (BLOCK2)                 Disables check of blk version major                = False R/W (0b0)

    Jtag fuses:
    JTAG_SEL_ENABLE (BLOCK0)                           Set this bit to enable selection between           = False R/W (0b0)
                                                       usb_to_jtag and pad_to_jtag through strapping
                                                       gpio25 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are
                                                       equal to 0
    SOFT_DIS_JTAG (BLOCK0)                             Represents whether JTAG is disabled in soft way.   = 0 R/W (0b000)
                                                       Odd number: disabled. Even number: enabled
    DIS_PAD_JTAG (BLOCK0)                              Represents whether JTAG is disabled in the         = False R/W (0b0)
                                                       hard way(permanently). 1: disabled. 0: enabled

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
       = 00:00:00:00:00:00 (OK) R/W
    MAC_EXT (BLOCK1)                                   Stores the extended bits of MAC address            = 00:00 (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC
       = 00:00:00:00:00:00 (OK) R/W
    MAC_EUI64 (BLOCK1)                                 calc MAC_EUI64 = MAC[0]:MAC[1]:MAC[2]:MAC_EXT[0]:M
       = 00:00:00:00:00:00:00:00 (OK) R/W
                                                       AC_EXT[1]:MAC[3]:MAC[4]:MAC[5]

    Security fuses:
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Represents whether the function that forces chip   = False R/W (0b0)
                                                       into download mode is disabled or enabled. 1:
                                                       disabled. 0: enabled
    SPI_DOWNLOAD_MSPI_DIS (BLOCK0)                     Represents whether SPI0 controller during          = False R/W (0b0)
                                                       boot_mode_download is disabled or enabled. 1: disabled. 0:
                                                        enabled
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Represents whether flash encrypt function is       = False R/W (0b0)
                                                       disabled or enabled(except in SPI boot mode). 1:
                                                       disabled. 0: enabled
    SPI_BOOT_CRYPT_CNT (BLOCK0)                        Enables flash encryption when 1 or 3 bits are set  = Disable R/W (0b000)
                                                       and disables otherwise
    SECURE_BOOT_KEY_REVOKE0 (BLOCK0)                   Revoke 1st secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE1 (BLOCK0)                   Revoke 2nd secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE2 (BLOCK0)                   Revoke 3rd secure boot key                         = False R/W (0b0)
    KEY_PURPOSE_0 (BLOCK0)                             Represents the purpose of Key0                     = USER R/W (0x0)
    KEY_PURPOSE_1 (BLOCK0)                             Represents the purpose of Key1                     = USER R/W (0x0)
    KEY_PURPOSE_2 (BLOCK0)                             Represents the purpose of Key2                     = USER R/W (0x0)
    KEY_PURPOSE_3 (BLOCK0)                             Represents the purpose of Key3                     = USER R/W (0x0)
    KEY_PURPOSE_4 (BLOCK0)                             Represents the purpose of Key4                     = USER R/W (0x0)
    KEY_PURPOSE_5 (BLOCK0)                             Represents the purpose of Key5                     = USER R/W (0x0)
    SEC_DPA_LEVEL (BLOCK0)                             Represents the spa secure level by configuring the = 0 R/W (0b00)
                                                        clock random divide mode
    ECDSA_FORCE_USE_HARDWARE_K (BLOCK0)                Represents whether hardware random number k is     = False R/W (0b0)
                                                       forced used in ESDCA. 1: force used. 0: not force
                                                       used
    CRYPT_DPA_ENABLE (BLOCK0)                          Represents whether anti-dpa attack is enabled.     = False R/W (0b0)
                                                       1:enabled. 0: disabled
    SECURE_BOOT_EN (BLOCK0)                            Represents whether secure boot is enabled or       = False R/W (0b0)
                                                       disabled. 1: enabled. 0: disabled
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Represents whether revoking aggressive secure boot = False R/W (0b0)
                                                        is enabled or disabled. 1: enabled. 0: disabled
    DIS_DOWNLOAD_MODE (BLOCK0)                         Represents whether Download mode is disabled or    = False R/W (0b0)
                                                       enabled. 1: disabled. 0: enabled
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Represents whether security download is enabled or = False R/W (0b0)
                                                        disabled. 1: enabled. 0: disabled
    SECURE_VERSION (BLOCK0)                            Represents the version used by ESP-IDF             = 0 R/W (0x0000)
                                                       anti-rollback feature
    SECURE_BOOT_DISABLE_FAST_WAKE (BLOCK0)             Represents whether FAST VERIFY ON WAKE is disabled = False R/W (0b0)
                                                        or enabled when Secure Boot is enabled. 1:
                                                       disabled. 0: enabled
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

    Usb fuses:
    DIS_USB_JTAG (BLOCK0)                              Represents whether the function of usb switch to   = False R/W (0b0)
                                                       jtag is disabled or enabled. 1: disabled. 0:
                                                       enabled
    USB_EXCHG_PINS (BLOCK0)                            Represents whether the D+ and D- pins is exchanged = False R/W (0b0)
                                                       1: exchanged. 0: not exchanged
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             Set this bit to disable USB-Serial-JTAG print      = False R/W (0b0)
                                                       during rom boot
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         Represents whether the USB-Serial-JTAG download    = False R/W (0b0)
                                                       function is disabled or enabled. 1: disabled. 0:
                                                       enabled

    Vdd fuses:
    VDD_SPI_AS_GPIO (BLOCK0)                           Represents whether vdd spi pin is functioned as    = False R/W (0b0)
                                                       gpio. 1: functioned. 0: not functioned

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK0)                             Represents whether RTC watchdog timeout threshold  = 0 R/W (0b00)
                                                       is selected at startup. 1: selected. 0: not
                                                       selected
