.. code-block:: none

    idf.py -p /dev/ttyUSB0  efuse-summary

    Executing action: efuse-summary
    (...)

    === Run "summary" command ===
    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Config fuses:
    WR_DIS (BLOCK0)                                    Disable programming of individual eFuses           = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK4-10                     = 0 R/W (0b0000000)
    DIS_ICACHE (BLOCK0)                                Represents whether cache is disabled. 1: Disabled  = False R/W (0b0)
                                                        0: Enabled.
    DIS_DIRECT_BOOT (BLOCK0)                           Represents whether direct boot mode is disabled or = False R/W (0b0)
                                                        enabled. 1. Disable 0: Enable
    UART_PRINT_CONTROL (BLOCK0)                        Represents the types of UART printing              = 0 R/W (0b00)
    HYS_EN_PAD (BLOCK0)                                Set bits to enable hysteresis function of PAD0~27  = False R/W (0b0)
    DIS_WIFI6 (BLOCK0)                                 Represents whether the WIFI6 feature is enable or  = False R/W (0b0)
                                                        disabled. 1: WIFI6 is disable; 0: WIFI6 is enabled
    ECC_FORCE_CONST_TIME (BLOCK0)                      Represents whether to force ecc to use const-time  = False R/W (0b0)
                                                        calculation mode. 1: Enable. 0: Disable
    BOOTLOADER_ANTI_ROLLBACK_EN (BLOCK0)               Represents whether the ani-rollback check for the  = False R/W (0b0)
                                                        2nd stage bootloader is enabled.1: Enabled0: Disab
                                                        led
    BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM (BLOCK0)    Represents whether the ani-rollback SECURE_VERSION = False R/W (0b0)
                                                        will be updated from the ROM bootloader.1: Enable
                                                        0: Disable
    REPEAT_DATA4 (BLOCK0)                              Reserved                                           = 0 R/W (0x000000)
    PSRAM_CAP (BLOCK1)                                 PSRAM capacity                                     = 0 R/W (0b000)
    PSRAM_VENDOR (BLOCK1)                              PSRAM vendor                                       = 0 R/W (0b00)
    TEMP (BLOCK1)                                      Temperature                                        = 0 R/W (0b00)
    BLOCK_USR_DATA (BLOCK3)                            User data
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK10)                          System data part 2 (reserved)
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TPUW (BLOCK0)                                Represents the flash waiting time after power-up;  = 0 R/W (0x0)
                                                        in unit of ms. When the value less than 15; the wa
                                                        iting time is programmed value. Otherwise; the wai
                                                        ting time is 2 times the programmed value
    FORCE_SEND_RESUME (BLOCK0)                         Represents whether ROM code is forced to send a re = False R/W (0b0)
                                                        sume command during SPI boot
    RECOVERY_BOOTLOADER_FLASH_SECTOR (BLOCK0)          Represents the starting flash sector (flash sector = 1008 R/W (0x3f0)
                                                        size is 0x1000) of the recovery bootloader used b
                                                        y the ROM bootloader If the primary bootloader fai
                                                        ls. 0 and 0xFFF - this feature is disabled
    FLASH_CAP (BLOCK1)                                 Flash capacity                                     = 0 R/W (0b000)
    FLASH_VENDOR (BLOCK1)                              Flash vendor                                       = 0 R/W (0b000)

    Identity fuses:
    WAFER_VERSION_MINOR (BLOCK1)                       Minor chip version                                 = 0 R/W (0x0)
    WAFER_VERSION_MAJOR (BLOCK1)                       Major chip version                                 = 1 R/W (0b01)
    DISABLE_WAFER_VERSION_MAJOR (BLOCK1)               Disables check of wafer version major              = False R/W (0b0)
    DISABLE_BLK_VERSION_MAJOR (BLOCK1)                 Disables check of blk version major                = False R/W (0b0)
    BLK_VERSION_MINOR (BLOCK1)                         BLK_VERSION_MINOR of BLOCK2                        = 0 R/W (0b000)
    BLK_VERSION_MAJOR (BLOCK1)                         BLK_VERSION_MAJOR of BLOCK2                        = 0 R/W (0b00)
    PKG_VERSION (BLOCK1)                               Package version                                    = 0 R/W (0b000)
    OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Jtag fuses:
    JTAG_SEL_ENABLE (BLOCK0)                           Represents whether the selection between usb_to_jt = False R/W (0b0)
                                                        ag and pad_to_jtag through strapping gpio15 when b
                                                        oth EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are
                                                        equal to 0 is enabled or disabled. 1: enabled 0: d
                                                        isabled
    DIS_PAD_JTAG (BLOCK0)                              Represents whether JTAG is disabled in the hard wa = False R/W (0b0)
                                                        y(permanently). 1: disabled 0: enabled

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
    = 30:ed:a0:e5:25:20 (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC
    = 00:00:00:00:00:00 (OK) R/W

    Security fuses:
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Represents whether the function that forces chip i = False R/W (0b0)
                                                        nto download mode is disabled or enabled. 1: disab
                                                        led 0: enabled
    SPI_DOWNLOAD_MSPI_DIS (BLOCK0)                     Represents whether SPI0 controller during boot_mod = False R/W (0b0)
                                                        e_download is disabled or enabled. 1: disabled 0:
                                                        enabled
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Represents whether flash encrypt function is disab = False R/W (0b0)
                                                        led or enabled(except in SPI boot mode). 1: disabled
                                                        0: enabled
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
    SECURE_BOOT_EN (BLOCK0)                            Represents whether secure boot is enabled or disab = False R/W (0b0)
                                                        led. 1. Enable 0: Disable
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Represents whether revoking aggressive secure boot = False R/W (0b0)
                                                        is enabled or disabled. 1. Enable 0: Disable
    DIS_DOWNLOAD_MODE (BLOCK0)                         Represents whether Download mode is disable or ena = False R/W (0b0)
                                                        ble. 1. Disable 0: Enable
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Represents whether security download is enabled or = False R/W (0b0)
                                                        disabled. 1: Enable 0: Disable
    SECURE_VERSION (BLOCK0)                            Represents the version used by ESP-IDF anti-rollba = 0 R/W (0x0000)
                                                        ck feature
    SECURE_BOOT_DISABLE_FAST_WAKE (BLOCK0)             Represents whether FAST_VERIFY_ON_WAKE is disable  = False R/W (0b0)
                                                        or enable when Secure Boot is enable
    XTS_DPA_CLK_ENABLE (BLOCK0)                        Represents whether xts-aes anti-dpa attack clock i = False R/W (0b0)
                                                        s enabled. 1. Enable. 0: Disable.
    XTS_DPA_PSEUDO_LEVEL (BLOCK0)                      Represents the pseudo round level of xts-aes anti- = 0 R/W (0b00)
                                                        dpa attack. 3: High. 2: Moderate 1. Low 0: Disabled
    ECDSA_DISABLE_P192 (BLOCK0)                        Represents whether to disable P192 curve in ECDSA. = False R/W (0b0)
                                                        1: Disabled. 0: Not disabled
    BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION (BLOCK0)   Represents the anti-rollback secure version of the = 0 R/W (0x0)
                                                        2nd stage bootloader used by the ROM bootloader
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
    DIS_USB_JTAG (BLOCK0)                              Represents whether the function of usb switch to j = False R/W (0b0)
                                                        tag is disabled or enabled. 1: disabled 0: enabled
    USB_EXCHG_PINS (BLOCK0)                            Represents whether the D+ and D- pins of USB_SERIA = False R/W (0b0)
                                                        L_JTAG PHY is exchanged. 1: exchanged 0: not excha
                                                        nged
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             Represents whether print from USB-Serial-JTAG is d = False R/W (0b0)
                                                        isabled or enabled. 1. Disable 0: Enable
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         Represents whether the USB-Serial-JTAG download fu = False R/W (0b0)
                                                        nction is disabled or enabled. 1: Disable 0: Enable

    Vdd fuses:
    VDD_SPI_AS_GPIO (BLOCK0)                           Represents whether vdd spi pin is functioned as gp = False R/W (0b0)
                                                        io. 1: functioned 0: not functioned

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK0)                             lp wdt timeout threshold at startup = initial time = 0 R/W (0b00)
                                                        out value * (2 ^ (EFUSE_WDT_DELAY_SEL + 1))
