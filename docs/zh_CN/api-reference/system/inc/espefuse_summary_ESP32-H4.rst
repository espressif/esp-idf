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
    DIS_TWAI (BLOCK0)                                  Represents whether TWAI function is disabled or en = False R/W (0b0)
                                                       abled. 1: disabled 0: enabled
    PVT_GLITCH_EN (BLOCK0)                             Represents whether to enable PVT power glitch moni = False R/W (0b0)
                                                       tor function.1:Enable. 0:Disable
    PVT_GLITCH_MODE (BLOCK0)                           Use to configure glitch mode                       = 0 R/W (0b00)
    DIS_CORE1 (BLOCK0)                                 Represents whether the CPU-Core1 is disabled.  1:  = False R/W (0b0)
                                                       Disabled.  0: Not disable
    ECC_FORCE_CONST_TIME (BLOCK0)                      Represents whether to force ecc to use const-time  = False R/W (0b0)
                                                       calculation mode.  1: Enable.  0: Disable
    KM_DISABLE_DEPLOY_MODE (BLOCK0)                    Represents whether the new key deployment of key m = 0 R/W (0b00000)
                                                       anager is disabled. Bit0: Represents whether the n
                                                       ew ECDSA key deployment is disabled0: Enabled1: Di
                                                       sabledBit1: Represents whether the new XTS-AES (fl
                                                       ash and PSRAM) key deployment is disabled0: Enable
                                                       d1: DisabledBit2: Represents whether the new HMAC
                                                       key deployment is disabled0: Enabled1: DisabledBit
                                                       3: Represents whether the new DS key deployment is
                                                        disabled0: Enabled1: Disabled
    KM_RND_SWITCH_CYCLE (BLOCK0)                       Represents the cycle at which the Key Manager swit = 0 R/W (0b00)
                                                       ches random numbers.0: Controlled by the \hyperref
                                                       [fielddesc:KEYMNGRNDSWITCHCYCLE]{KEYMNG\_RND\_SWIT
                                                       CH\_CYCLE} register. For more information; please
                                                       refer to Chapter \ref{mod:keymng} \textit{\nameref
                                                       {mod:keymng}}1: 8 Key Manager clock cycles2: 16 Ke
                                                       y Manager clock cycles3: 32 Key Manager clock cycl
                                                       es
    KM_DEPLOY_ONLY_ONCE (BLOCK0)                       Represents whether the corresponding key can be de = 0 R/W (0b00000)
                                                       ployed only once.Bit0: Represents whether the ECDS
                                                       A key can be deployed only once0: The key can be d
                                                       eployed multiple times1: The key can be deployed o
                                                       nly onceBit1: Represents whether the XTS-AES (flas
                                                       h and PSRAM) key can be deployed only once0: The k
                                                       ey can be deployed multiple times1: The key can be
                                                        deployed only onceBit2: Represents whether the HM
                                                       AC key can be deployed only once0: The key can be
                                                       deployed multiple times1: The key can be deployed
                                                       only onceBit3: Represents whether the DS key can b
                                                       e deployed only once0: The key can be deployed
                                                       multiple times1: The key can be deployed only once
    DIS_DIRECT_BOOT (BLOCK0)                           Represents whether direct boot mode is disabled or = False R/W (0b0)
                                                        enabled. 1: disabled 0: enabled
    UART_PRINT_CONTROL (BLOCK0)                        Represents the type of UART printing. 00: force en = 0 R/W (0b00)
                                                       able printing 01: enable printing when GPIO8 is re
                                                       set at low level 10: enable printing when GPIO8 is
                                                        reset at high level 11: force disable printing
    HUK_GEN_STATE (BLOCK0)                             Represents whether the HUK generate mode is valid. = 0 R/W (0b00000)
                                                       Odd count of bits with a value of 1: InvalidEven c
                                                       ount of bits with a value of 1: Valid
    DCDC_CCM_EN (BLOCK0)                               Represents whether change DCDC to CCM mode         = False R/W (0b0)
    PVT_LIMIT (BLOCK1)                                 Power glitch monitor threthold                     = 0 R/W (0x0000)
    PVT_CELL_SELECT (BLOCK1)                           Power glitch monitor PVT cell select               = 0 R/W (0b0000000)
    PVT_PUMP_LIMIT (BLOCK1)                            Use to configure voltage monitor limit for charge  = 0 R/W (0x00)
                                                       pump
    PUMP_DRV (BLOCK1)                                  Use to configure charge pump voltage gain          = 0 R/W (0x0)
    HYS_EN_PAD (BLOCK1)                                Represents whether the hysteresis function of      = False R/W (0b0)
                                                       corresponding PAD is enabled. 1: enabled 0:disabled
    PVT_GLITCH_CHARGE_RESET (BLOCK1)                   Represents whether to trigger reset or charge pump = False R/W (0b0)
                                                        when PVT power glitch happened.1:Trigger charge p
                                                       ump. 0:Trigger reset
    PSRAM_CAP (BLOCK1)                                 Psram capacity                                     = 0 R/W (0b000)
    PSRAM_VENDOR (BLOCK1)                              Psram vendor                                       = 0 R/W (0b00)
    TEMP (BLOCK1)                                      Temp (die embedded inside)                         = 0 R/W (0b00)
    ADJUST_1V2 (BLOCK1)                                SPI LDO adjust of 1.2v                             = 0 R/W (0x0)
    ADJUST_1V8 (BLOCK1)                                SPI LDO adjust of 1.8v                             = 0 R/W (0x0)
    ACTIVE_DCDC_1V25 (BLOCK1)                          DCDC-DCDC DBIAS of 1.25v                           = 0 R/W (0x0)
    ACTIVE_DCDC_1V35 (BLOCK1)                          DCDC-DCDC DBIAS of 1.35v                           = 0 R/W (0x0)
    SLP_DCDC (BLOCK1)                                  DCDC DBIAS in sleep                                = 0 R/W (0b00000)
    VDD_3V4_DOUT (BLOCK2)                              ADC dout of vdd 3.4v                               = 0 R/W (0b0000000000)
    INITCODE_DIFF_1P8_3P3 (BLOCK2)                     Initcode diff between IO LDO 1.8v and 3.3v         = 0 R/W (0b00000)
    HI_DOUT_DIFF_1P8_3P3 (BLOCK2)                      HI dout diff between IO LDO 1.8v and 3.3v          = 0 R/W (0b00000)
    BLOCK_USR_DATA (BLOCK3)                            User data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK10)                          System data part 2 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TPUW (BLOCK0)                                Represents the flash waiting time after power-up;  = 0 R/W (0b000)
                                                       in unit of ms. When the value less than 15; the wa
                                                       iting time is the programmed value. Otherwise; the
                                                        waiting time is 2 times the programmed value
    FORCE_SEND_RESUME (BLOCK0)                         Represents whether ROM code is forced to send a re = False R/W (0b0)
                                                       sume command during SPI boot. 1: forced 0:not forc
                                                       ed
    FLASH_LDO_EFUSE_SEL (BLOCK0)                       Represents whether to select efuse control flash l = False R/W (0b0)
                                                       do default voltage.  1 : efuse 0 : strapping
    FLASH_LDO_POWER_SEL (BLOCK1)                       Represents which flash ldo be select: 1: FLASH LDO = False R/W (0b0)
                                                        1P2 0 : FLASH LDO 1P8
    FLASH_CAP (BLOCK1)                                 Flash capacity                                     = 0 R/W (0b000)
    FLASH_VENDOR (BLOCK1)                              Flash vendor                                       = 0 R/W (0b000)

    Identity fuses:
    WAFER_VERSION_MINOR (BLOCK1)                       Minor chip version                                 = 1 R/W (0x1)
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
                                                       equal to 0 is enabled or disabled. 1: enabled 0: d
                                                       isabled
    DIS_PAD_JTAG (BLOCK0)                              Represents whether JTAG is disabled in the hard wa = False R/W (0b0)
                                                       y(permanently). 1: disabled 0: enabled
    SOFT_DIS_JTAG (BLOCK0)                             Represents whether JTAG is disabled in soft way. O = 0 R/W (0b000)
                                                       dd number: disabled Even number: enabled

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
       = 30:ed:a0:ed:57:cc (OK) R/W
    MAC_EXT (BLOCK1)                                   Represents the extended bits of MAC address        = 00:00 (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC
       = 00:00:00:00:00:00 (OK) R/W
    MAC_EUI64 (BLOCK1)                                 calc MAC_EUI64 = MAC[0]:MAC[1]:MAC[2]:MAC_EXT[0]:M
       = 30:ed:a0:00:00:ed:57:cc (OK) R/W
                                                       AC_EXT[1]:MAC[3]:MAC[4]:MAC[5]

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
    KEY_PURPOSE_0 (BLOCK0)                             Represents the purpose of Key0                     = USER R/W (0b00000)
    KEY_PURPOSE_1 (BLOCK0)                             Represents the purpose of Key1                     = USER R/W (0b00000)
    KEY_PURPOSE_2 (BLOCK0)                             Represents the purpose of Key2                     = USER R/W (0b00000)
    KEY_PURPOSE_3 (BLOCK0)                             Represents the purpose of Key3                     = USER R/W (0b00000)
    KEY_PURPOSE_4 (BLOCK0)                             Represents the purpose of Key4                     = USER R/W (0b00000)
    KEY_PURPOSE_5 (BLOCK0)                             Represents the purpose of Key5                     = USER R/W (0b00000)
    SEC_DPA_LEVEL (BLOCK0)                             Represents the spa secure level by configuring the = 0 R/W (0b00)
                                                        clock random divide mode
    XTS_DPA_PSEUDO_LEVEL (BLOCK0)                      Represents the pseudo round level of xts-aes anti- = 0 R/W (0b00)
                                                       dpa attack. 3: High. 2: Moderate 1. Low 0: Disabled
    XTS_DPA_CLK_ENABLE (BLOCK0)                        Represents whether xts-aes anti-dpa attack clock i = False R/W (0b0)
                                                       s enabled. 1. Enable. 0: Disable.
    SECURE_BOOT_SHA384_EN (BLOCK0)                     Represents if the chip supports Secure Boot using  = False R/W (0b0)
                                                       SHA-384
    SECURE_BOOT_EN (BLOCK0)                            Represents whether secure boot is enabled or disab = False R/W (0b0)
                                                       led. 1: enabled 0: disabled
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Represents whether revoking aggressive secure boot = False R/W (0b0)
                                                        is enabled or disabled. 1: enabled. 0: disabled
    FORCE_USE_KEY_MANAGER_KEY (BLOCK0)                 Represents whether the corresponding key must come = 0 R/W (0b00000)
                                                        from Key Manager. Bit0: Represents whether the EC
                                                       DSA key must come from Key Manager.0: The key does
                                                        not need to come from Key Manager1: The key must
                                                       come from Key ManagerBit1: Represents whether the
                                                       XTS-AES (flash and PSRAM) key must come from Key M
                                                       anager.0: The key does not need to come from Key M
                                                       anager1: The key must come from Key ManagerBit2: R
                                                       epresents whether the HMAC key must come from Key
                                                       Manager.0: The key does not need to come from Key
                                                       Manager1: The key must come from Key ManagerBit3:
                                                       Represents whether the DS key must come from Key M
                                                       anager.0: The key does not need to come from Key M
                                                       anager1: The key must come from Key Manager
    FORCE_DISABLE_SW_INIT_KEY (BLOCK0)                 Represents whether to disable the use of the initi = False R/W (0b0)
                                                       alization key written by software and instead forc
                                                       e use efuse\_init\_key.0: Enable1: Disable
    KM_XTS_KEY_LENGTH_256 (BLOCK0)                     Represents which key flash encryption uses.0: XTS- = False R/W (0b0)
                                                       AES-256 key1: XTS-AES-128 key
    LOCK_KM_KEY (BLOCK0)                               Represents whether the keys in the Key Manager are = False R/W (0b0)
                                                        locked after deployment.0: Not locked1: Locked
    DIS_DOWNLOAD_MODE (BLOCK0)                         Represents whether Download mode is disabled or en = False R/W (0b0)
                                                       abled. 1: disabled 0: enabled
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Represents whether security download is enabled or = False R/W (0b0)
                                                        disabled. 1: enabled 0: disabled
    SECURE_VERSION (BLOCK0)                            Represents the version used by ESP-IDF anti-rollba = 0 R/W (0x0000)
                                                       ck feature
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
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             Represents whether print from USB-Serial-JTAG is d = False R/W (0b0)
                                                       isabled or enabled. 1: disabled 0: enabled
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         Represents whether the USB-Serial-JTAG download fu = False R/W (0b0)
                                                       nction is disabled or enabled. 1: Disable 0: Enabl
                                                       e
    USB_EXCHG_PINS (BLOCK0)                            Represents whether the D+ and D- pins of USB_SERIA = False R/W (0b0)
                                                       L_JTAG PHY is exchanged. 1: exchanged 0: not excha
                                                       nged
    USB_OTG_FS_EXCHG_PINS (BLOCK0)                     Represents whether the D+ and D- pins of USB_OTG_F = False R/W (0b0)
                                                       S PHY is exchanged. 1: exchanged 0: not exchanged
    USB_PHY_SEL (BLOCK0)                               Represents whether to exchange the USB_SERIAL_JTAG = False R/W (0b0)
                                                        PHY with USB_OTG_FS PHY.  1: exchanged.  0: not e
                                                       xchanged

    Vdd fuses:
    VDD_SPI_LDO_ADJUST (BLOCK1)                        Represents configuration of FLASH LDO mode and     = 0 R/W (0x00)
                                                       voltage.

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK1)                             Represents the threshold level of the RTC watchdog = 0 R/W (0b00)
                                                        STG0 timeout. 0: Original threshold configuration
                                                        value of STG0 *2 1: Original threshold configurat
                                                       ion value of STG0 *4 2: Original threshold configu
                                                       ration value of STG0 *8 3: Original threshold conf
                                                       iguration value of STG0 *16
