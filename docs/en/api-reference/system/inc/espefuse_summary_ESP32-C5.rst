.. code-block:: none

    espefuse.py -p PORT summary

    espefuse.py v4.8.dev4

    === Run "summary" command ===
    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Config fuses:
    WR_DIS (BLOCK0)                                    Disable programming of individual eFuses           = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK4-10                     = 0 R/W (0b0000000)
    DIS_ICACHE (BLOCK0)                                Represents whether icache is disabled or enabled.\ = False R/W (0b0)
                                                       \ 1: disabled\\ 0: enabled\\
    DIS_TWAI (BLOCK0)                                  Represents whether TWAI function is disabled or en = False R/W (0b0)
                                                       abled.\\ 1: disabled\\ 0: enabled\\
    KM_DISABLE_DEPLOY_MODE (BLOCK0)                    Represents whether the deploy mode of key manager  = 0 R/W (0x0)
                                                       is disable or not. \\ 1: disabled \\ 0: enabled.\\
    KM_RND_SWITCH_CYCLE (BLOCK0)                       Set the bits to control key manager random number  = 0 R/W (0b00)
                                                       switch cycle. 0: control by register. 1: 8 km clk
                                                       cycles. 2: 16 km cycles. 3: 32 km cycles
    KM_DEPLOY_ONLY_ONCE (BLOCK0)                       Set each bit to control whether corresponding key  = 0 R/W (0x0)
                                                       can only be deployed once. 1 is true; 0 is false.
                                                       bit 0: ecsda; bit 1: xts; bit2: hmac; bit3: ds
    DIS_DIRECT_BOOT (BLOCK0)                           Represents whether direct boot mode is disabled or = False R/W (0b0)
                                                        enabled.\\ 1: disabled\\ 0: enabled
    UART_PRINT_CONTROL (BLOCK0)                        Set the default UARTboot message output mode       = Enable R/W (0b00)
    HYS_EN_PAD (BLOCK0)                                Represents whether the hysteresis function of      = False R/W (0b0)
                                                       corresponding PAD is enabled.\\ 1: enabled\\ 0:disabled
    HUK_GEN_STATE (BLOCK0)                             Set the bits to control validation of HUK generate = 0 R/W (0b000000000)
                                                        mode.\\ Odd of 1 is invalid.\\ Even of 1 is valid
    XTAL_48M_SEL (BLOCK0)                              Represents whether XTAL frequency is 48MHz or not. = 0 R/W (0b000)
                                                        If not; 40MHz XTAL will be used. If this field co
                                                       ntains Odd number bit 1: Enable 48MHz XTAL\ Even n
                                                       umber bit 1: Enable 40MHz XTAL
    XTAL_48M_SEL_MODE (BLOCK0)                         Specify the XTAL frequency selection is decided by = False R/W (0b0)
                                                        eFuse or strapping-PAD-state. 1: eFuse\\ 0: strap
                                                       ping-PAD-state
    ECC_FORCE_CONST_TIME (BLOCK0)                      Represents whether to force ecc to use const-time  = False R/W (0b0)
                                                       calculation mode. \\ 1: Enable. \\ 0: Disable
    BLOCK_SYS_DATA1 (BLOCK2)                           System data part 1 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_USR_DATA (BLOCK3)                            User data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK10)                          System data part 2 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TPUW (BLOCK0)                                Represents the flash waiting time after power-up;  = 0 R/W (0x0)
                                                       in unit of ms. When the value less than 15; the wa
                                                       iting time is the programmed value. Otherwise; the
                                                        waiting time is 2 times the programmed value
    FORCE_SEND_RESUME (BLOCK0)                         Represents whether ROM code is forced to send a re = False R/W (0b0)
                                                       sume command during SPI boot.\\ 1: forced\\ 0:not
                                                       forced\\

    Jtag fuses:
    JTAG_SEL_ENABLE (BLOCK0)                           Represents whether the selection between usb_to_jt = False R/W (0b0)
                                                       ag and pad_to_jtag through strapping gpio15 when b
                                                       oth EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are
                                                       equal to 0 is enabled or disabled.\\ 1: enabled\\
                                                       0: disabled\\
    SOFT_DIS_JTAG (BLOCK0)                             Represents whether JTAG is disabled in soft way.\\ = 0 R/W (0b000)
                                                        Odd number: disabled\\ Even number: enabled\\
    DIS_PAD_JTAG (BLOCK0)                              Represents whether JTAG is disabled in the hard wa = False R/W (0b0)
                                                       y(permanently).\\ 1: disabled\\ 0: enabled\\

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
       = 00:00:00:00:00:00 (OK) R/W
    MAC_EXT (BLOCK1)                                   Represents the extended bits of MAC address        = 00:00 (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC
       = 00:00:00:00:00:00 (OK) R/W
    MAC_EUI64 (BLOCK1)                                 calc MAC_EUI64 = MAC[0]:MAC[1]:MAC[2]:MAC_EXT[0]:M
       = 00:00:00:00:00:00:00:00 (OK) R/W
                                                       AC_EXT[1]:MAC[3]:MAC[4]:MAC[5]

    Security fuses:
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Represents whether the function that forces chip i = False R/W (0b0)
                                                       nto download mode is disabled or enabled.\\ 1: dis
                                                       abled\\ 0: enabled\\
    SPI_DOWNLOAD_MSPI_DIS (BLOCK0)                     Represents whether SPI0 controller during boot_mod = False R/W (0b0)
                                                       e_download is disabled or enabled.\\ 1: disabled\\
                                                        0: enabled\\
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Represents whether flash encrypt function is disab = False R/W (0b0)
                                                       led or enabled(except in SPI boot mode).\\ 1: disa
                                                       bled\\ 0: enabled\\
    FORCE_USE_KEY_MANAGER_KEY (BLOCK0)                 Set each bit to control whether corresponding key  = 0 R/W (0x0)
                                                       must come from key manager. 1 is true; 0 is false.
                                                        bit 0: ecsda; bit 1: xts; bit2: hmac; bit3: ds
    FORCE_DISABLE_SW_INIT_KEY (BLOCK0)                 Set this bit to disable software written init key; = False R/W (0b0)
                                                        and force use efuse_init_key
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
                                                       led.\\ 1: enabled\\ 0: disabled\\
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Represents whether revoking aggressive secure boot = False R/W (0b0)
                                                        is enabled or disabled.\\ 1: enabled.\\ 0: disabled
    KM_XTS_KEY_LENGTH_256 (BLOCK0)                     Set this bitto configure flash encryption use xts- = False R/W (0b0)
                                                       128 key. else use xts-256 key
    DIS_DOWNLOAD_MODE (BLOCK0)                         Represents whether Download mode is disabled or en = False R/W (0b0)
                                                       abled.\\ 1: disabled\\ 0: enabled\\
    LOCK_KM_KEY (BLOCK0)                               Represetns whether to lock the efuse xts key.\\ 1. = False R/W (0b0)
                                                        Lock\\ 0: Unlock\\
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Represents whether security download is enabled or = False R/W (0b0)
                                                        disabled.\\ 1: enabled\\ 0: disabled\\
    SECURE_VERSION (BLOCK0)                            Represents the version used by ESP-IDF anti-rollba = 0 R/W (0x0000)
                                                       ck feature
    SECURE_BOOT_DISABLE_FAST_WAKE (BLOCK0)             Represents whether FAST VERIFY ON WAKE is disabled = False R/W (0b0)
                                                        or enabled when Secure Boot is enabled.\\ 1: disa
                                                       bled\\ 0: enabled\\
    XTS_DPA_PSEUDO_LEVEL (BLOCK0)                      Represents the pseudo round level of xts-aes anti- = 0 R/W (0b00)
                                                       dpa attack.\\ 3: High.\\ 2: Moderate 1. Low\\ 0: D
                                                       isabled\\
    XTS_DPA_CLK_ENABLE (BLOCK0)                        Represents whether xts-aes anti-dpa attack clock i = False R/W (0b0)
                                                       s enabled.\\ 1. Enable.\\ 0: Disable.\\
    ECDSA_DISABLE_P192 (BLOCK0)                        Represents whether to disable P192 curve in ECDSA. = False R/W (0b0)
                                                       \\ 1: Disabled.\\ 0: Not disable
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
                                                       tag is disabled or enabled.\\ 1: disabled\\ 0: ena
                                                       bled\\
    USB_EXCHG_PINS (BLOCK0)                            Represents whether the D+ and D- pins is exchanged = False R/W (0b0)
                                                       .\\ 1: exchanged\\ 0: not exchanged\\
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             Represents whether print from USB-Serial-JTAG is d = False R/W (0b0)
                                                       isabled or enabled.\\ 1: disabled\\ 0: enabled\\
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         Represents whether the USB-Serial-JTAG download fu = False R/W (0b0)
                                                       nction is disabled or enabled.\\ 1: Disable\\ 0: E
                                                       nable\\

    Vdd fuses:
    VDD_SPI_AS_GPIO (BLOCK0)                           Represents whether vdd spi pin is functioned as gp = False R/W (0b0)
                                                       io.\\ 1: functioned\\ 0: not functioned\\

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK0)                             Represents the threshold level of the RTC watchdog = 0 R/W (0b00)
                                                        STG0 timeout.\\ 0: Original threshold configurati
                                                       on value of STG0 *2 \\1: Original threshold config
                                                       uration value of STG0 *4 \\2: Original threshold c
                                                       onfiguration value of STG0 *8 \\3: Original thresh
                                                       old configuration value of STG0 *16 \\
