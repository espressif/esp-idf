.. code-block:: none

    idf.py efuse-summary

    Executing action: efuse-summary
    (...)

    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Config fuses:
    WR_DIS (BLOCK0)                                    Disable programming of individual eFuses           = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK4-10                     = 0 R/W (0b0000000)
    RESERVED_0_39 (BLOCK0)                             Enable usb device exchange pins of D+ and D-       = 0 R/W (0b000)
    DIS_TWAI (BLOCK0)                                  Represents whether TWAI function is disabled or en = False R/W (0b0)
                                                       abled. 1: disabled. 0: enabled
    EFUSE_DIS_WIFI6 (BLOCK0)                           USB intphy of usb device signle-end input high thr = False R/W (0b0)
                                                       eshold; 1.76V to 2V. Step by 80mV
    KM_HUK_GEN_STATE (BLOCK0)                          Set this bit to control validation of HUK generate = 0 R/W (0b00000)
                                                        mode. Odd of 1 is invalid; even of 1 is valid
    KM_RND_SWITCH_CYCLE (BLOCK0)                       Set bits to control key manager random number swit = False R/W (0b0)
                                                       ch cycle. 0: control by register. 1: 8 km clk cycl
                                                       es. 2: 16 km cycles. 3: 32 km cycles
    KM_DISABLE_DEPLOY_MODE (BLOCK0)                    Set bits to control key manager random number swit = 0 R/W (0b00000)
                                                       ch cycle. 0: control by register. 1: 8 km clk cycl
                                                       es. 2: 16 km cycles. 3: 32 km cycles
    KM_DEPLOY_ONLY_ONCE (BLOCK0)                       Set each bit to control whether corresponding key  = 0 R/W (0b00000)
                                                       can only be deployed once. 1 is true; 0 is false.
                                                       Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds
    ECC_FORCE_CONST_TIME (BLOCK0)                      Represents the spa secure level by configuring the = False R/W (0b0)
                                                        clock random divide mode
    DIS_DIRECT_BOOT (BLOCK0)                           Represents whether direct boot mode is disabled or = False R/W (0b0)
                                                        enabled. 1: disabled. 0: enabled
    UART_PRINT_CONTROL (BLOCK0)                        Represents the type of UART printing. 00: force en = 0 R/W (0b00)
                                                       able printing. 01: enable printing when GPIO8 is r
                                                       eset at low level. 10: enable printing when GPIO8
                                                       is reset at high level. 11: force disable printing
    HYS_EN_PAD (BLOCK0)                                Represents whether the hysteresis function of      = False R/W (0b0)
                                                       corresponding PAD is enabled. 1: enabled. 0:disabled
    DCDC_VSET_EN (BLOCK0)                              Select dcdc vset use efuse_dcdc_vset               = False R/W (0b0)
    DIS_SWD (BLOCK0)                                   Set this bit to disable super-watchdog             = False R/W (0b0)
    BOOTLOADER_ANTI_ROLLBACK_EN (BLOCK0)               HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    BOOTLOADER_ANTI_ROLLBACK_UPDATE (BLOCK0)           HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    RMA_ENA (BLOCK0)                                   HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0b00)
    RMA_SESSION_COUNTER (BLOCK0)                       HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0b000)
    RMA_NONCE_ENA (BLOCK0)                             HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0b00)
    RMA_CHIP_INFO_SOURCE (BLOCK0)                      HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    RMA_DISABLE_FAST_VEF (BLOCK0)                      HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    PVT_0_GLITCH_EN (BLOCK0)                           HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    PVT_0_GLITCH_MODE (BLOCK0)                         HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0b00)
    PVT_1_GLITCH_EN (BLOCK0)                           HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    PVT_1_GLITCH_MODE (BLOCK0)                         HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0b00)
    POWER_GLITCH_EN (BLOCK0)                           HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0x0)
    XTS_SHADOW_EN (BLOCK0)                             HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    SPI_BOOT_CRYPT_SCRAMBLER_EN (BLOCK0)               HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    PSRAM_CAP (BLOCK1)                                 PSRAM capacity                                     = 0 R/W (0b000)
    TEMP (BLOCK1)                                      Operating temperature of the ESP chip              = 0 R/W (0b00)
    PSRAM_VENDOR (BLOCK1)                              PSRAM vendor                                       = 0 R/W (0b00)
    BLOCK_USR_DATA (BLOCK3)                            User data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK9)                           System data part 2 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TYPE (BLOCK0)                                The type of interfaced flash. 0: four data lines;  = False R/W (0b0)
                                                       1: eight data lines
    FLASH_TPUW (BLOCK0)                                Represents the flash waiting time after power-up;  = 0 R/W (0x0)
                                                       in unit of ms. When the value less than 15; the wa
                                                       iting time is the programmed value. Otherwise; the
                                                        waiting time is 2 times the programmed value
    FORCE_SEND_RESUME (BLOCK0)                         Represents whether ROM code is forced to send a re = False R/W (0b0)
                                                       sume command during SPI boot. 1: forced. 0:not for
                                                       ced
    RECOVERY_BOOTLOADER_FLASH_SECTOR (BLOCK0)          HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0x000)
    PMU_FLASH_POWEL_SEL (BLOCK0)                       HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)
    PMU_FLASH_POWEL_SEL_EN (BLOCK0)                    HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)

    Identity fuses:
    BOOTLOADER_ANTI_ROLLBACK_VERSION (BLOCK0)          HP system power source select. 0:LDO. 1: DCDC      = 0 R/W (0x0)
    WAFER_VERSION_MINOR (BLOCK1)                       Minor chip version                                 = 0 R/W (0x0)
    WAFER_VERSION_MAJOR (BLOCK1)                       Major chip version                                 = 0 R/W (0b00)
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
                                                       equal to 0 is enabled or disabled. 1: enabled. 0:
                                                       disabled
    SOFT_DIS_JTAG (BLOCK0)                             Represents whether JTAG is disabled in soft way. O = 0 R/W (0b000)
                                                       dd number: disabled. Even number: enabled
    DIS_PAD_JTAG (BLOCK0)                              Represents whether JTAG is disabled in the hard wa = False R/W (0b0)
                                                       y(permanently). 1: disabled. 0: enabled
    RE_ENABLE_JTAG_SOURCE (BLOCK0)                     HP system power source select. 0:LDO. 1: DCDC      = False R/W (0b0)

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
       = 00:00:00:00:00:00 (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC
       = 00:00:00:00:00:00 (OK) R/W

    Security fuses:
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Represents whether the function that forces chip i = False R/W (0b0)
                                                       nto download mode is disabled or enabled. 1: disab
                                                       led. 0: enabled
    SPI_DOWNLOAD_MSPI_DIS (BLOCK0)                     Set this bit to disable accessing MSPI flash/MSPI  = False R/W (0b0)
                                                       ram by SYS AXI matrix during boot_mode_download
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Represents whether flash encrypt function is disab = False R/W (0b0)
                                                       led or enabled(except in SPI boot mode). 1: disable
                                                       ed. 0: enabled
    FORCE_USE_KEY_MANAGER_KEY (BLOCK0)                 Set each bit to control whether corresponding key  = 0 R/W (0b00000)
                                                       must come from key manager.. 1 is true; 0 is false
                                                       . Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds
    FORCE_DISABLE_SW_INIT_KEY (BLOCK0)                 Set this bit to disable software written init key; = False R/W (0b0)
                                                        and force use efuse_init_key
    KM_XTS_KEY_LENGTH_256 (BLOCK0)                     Set this bit to configure flash encryption use xts = False R/W (0b0)
                                                       -128 key; else use xts-256 key
    SPI_BOOT_CRYPT_CNT (BLOCK0)                        Enables flash encryption when 1 or 3 bits are set  = Disable R/W (0b000)
                                                       and disables otherwise
    SECURE_BOOT_KEY_REVOKE0 (BLOCK0)                   Revoke 1st secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE1 (BLOCK0)                   Revoke 2nd secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE2 (BLOCK0)                   Revoke 3rd secure boot key                         = False R/W (0b0)
    KEY_PURPOSE_0 (BLOCK0)                             Represents the purpose of Key0                     = USER R/W (0b00000)
    KEY_PURPOSE_1 (BLOCK0)                             Represents the purpose of Key1                     = USER R/W (0b00000)
    KEY_PURPOSE_2 (BLOCK0)                             Represents the purpose of Key2                     = USER R/W (0b00000)
    KEY_PURPOSE_3 (BLOCK0)                             Represents the purpose of Key3                     = USER R/W (0b00000)
    KEY_PURPOSE_4 (BLOCK0)                             Represents the purpose of Key4                     = USER R/W (0b00000)
    ECDSA_ENABLE_SOFT_K (BLOCK0)                       Represents whether hardware random number k is for = False R/W (0b0)
                                                       ced used in ESDCA. 1: force used. 0: not force use
                                                       d
    SEC_DPA_LEVEL (BLOCK0)                             Represents the spa secure level by configuring the = 0 R/W (0b00)
                                                        clock random divide mode
    XTS_DPA_CLK_ENABLE (BLOCK0)                        Represents the spa secure level by configuring the = False R/W (0b0)
                                                        clock random divide mode
    XTS_DPA_PSEUDO_LEVEL (BLOCK0)                      Represents whether anti-dpa attack is enabled. 1:e = 0 R/W (0b00)
                                                       nabled. 0: disabled
    SECURE_BOOT_EN (BLOCK0)                            Represents whether secure boot is enabled or disab = False R/W (0b0)
                                                       led. 1: enabled. 0: disabled
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Represents whether revoking aggressive secure boot = False R/W (0b0)
                                                        is enabled or disabled. 1: enabled. 0: disabled
    DIS_DOWNLOAD_MODE (BLOCK0)                         Represents whether Download mode is disabled or en = False R/W (0b0)
                                                       abled. 1: disabled. 0: enabled
    LOCK_KM_KEY (BLOCK0)                               TBD                                                = False R/W (0b0)
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Represents whether security download is enabled or = False R/W (0b0)
                                                        disabled. 1: enabled. 0: disabled
    SECURE_VERSION (BLOCK0)                            Represents the version used by ESP-IDF anti-rollba = 0 R/W (0x0000)
                                                       ck feature
    SECURE_BOOT_DISABLE_FAST_WAKE (BLOCK0)             Represents whether FAST VERIFY ON WAKE is disabled = False R/W (0b0)
                                                        or enabled when Secure Boot is enabled. 1: disable
                                                       ed. 0: enabled
    SECURE_BOOT_SHA384_EN (BLOCK0)                     Represents whether secure boot is enabled or disab = False R/W (0b0)
                                                       led. 1: enabled. 0: disabled
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

    Usb fuses:
    DIS_USB_JTAG (BLOCK0)                              Represents whether the function of usb switch to j = False R/W (0b0)
                                                       tag is disabled or enabled. 1: disabled. 0: enable
                                                       d
    DIS_USB_OTG_DOWNLOAD_MODE (BLOCK0)                 Set this bit to disable download via USB-OTG       = False R/W (0b0)
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             Represents whether print from USB-Serial-JTAG is d = False R/W (0b0)
                                                       isabled or enabled. 1: disabled. 0: enabled
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         Represents whether the USB-Serial-JTAG download fu = False R/W (0b0)
                                                       nction is disabled or enabled. 1: disabled. 0: ena
                                                       bled

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK0)                             Represents whether RTC watchdog timeout threshold  = 0 R/W (0b0)
                                                       is selected at startup. 1: selected. 0: not select
                                                       ed
    DIS_WDT (BLOCK0)                                   Set this bit to disable watch dog                  = False R/W (0b0)
