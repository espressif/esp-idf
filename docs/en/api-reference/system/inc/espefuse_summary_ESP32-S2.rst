.. code-block:: none

    idf.py efuse-summary

    Executing action: efuse-summary
    "ninja efuse-summary"...

    (...)

    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Config fuses:
    WR_DIS (BLOCK0)                                    Disable programming of individual eFuses           = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disable reading from BlOCK4-10                     = 0 R/W (0b0000000)
    DIS_ICACHE (BLOCK0)                                Set this bit to disable Icache                     = False R/W (0b0)
    DIS_DCACHE (BLOCK0)                                Set this bit to disable Dcache                     = False R/W (0b0)
    DIS_TWAI (BLOCK0)                                  Set this bit to disable the TWAI Controller functi = False R/W (0b0)
                                                       on
    DIS_BOOT_REMAP (BLOCK0)                            Disables capability to Remap RAM to ROM address sp = False R/W (0b0)
                                                       ace
    DIS_LEGACY_SPI_BOOT (BLOCK0)                       Set this bit to disable Legacy SPI boot mode       = False R/W (0b0)
    UART_PRINT_CHANNEL (BLOCK0)                        Selects the default UART for printing boot message = UART0 R/W (0b0)
                                                       s
    UART_PRINT_CONTROL (BLOCK0)                        Set the default UART boot message output mode      = Enable R/W (0b00)
    PIN_POWER_SELECTION (BLOCK0)                       Set default power supply for GPIO33-GPIO37; set wh = VDD3P3_CPU R/W (0b0)
                                                       en SPI flash is initialized
    BLOCK_USR_DATA (BLOCK3)                            User data
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
    BLOCK_SYS_DATA2 (BLOCK10)                          System data part 2 (reserved)
       = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_TPUW (BLOCK0)                                Configures flash startup delay after SoC power-up; = 0 R/W (0x0)
                                                        in unit of (ms/2). When the value is 15; delay is
                                                        7.5 ms
    FLASH_TYPE (BLOCK0)                                SPI flash type                                     = 4 data lines R/W (0b0)
    FORCE_SEND_RESUME (BLOCK0)                         If set; forces ROM code to send an SPI flash resum = False R/W (0b0)
                                                       e command during SPI boot
    FLASH_VERSION (BLOCK1)                             Flash version                                      = 2 R/W (0x2)

    Identity fuses:
    BLOCK0_VERSION (BLOCK0)                            BLOCK0 efuse version                               = 0 R/W (0b00)
    DISABLE_WAFER_VERSION_MAJOR (BLOCK0)               Disables check of wafer version major              = False R/W (0b0)
    DISABLE_BLK_VERSION_MAJOR (BLOCK0)                 Disables check of blk version major                = False R/W (0b0)
    WAFER_VERSION_MAJOR (BLOCK1)                       WAFER_VERSION_MAJOR                                = 1 R/W (0b01)
    WAFER_VERSION_MINOR_HI (BLOCK1)                    WAFER_VERSION_MINOR most significant bit           = False R/W (0b0)
    BLK_VERSION_MAJOR (BLOCK1)                         BLK_VERSION_MAJOR                                  = 0 R/W (0b00)
    PSRAM_VERSION (BLOCK1)                             PSRAM version                                      = 1 R/W (0x1)
    PKG_VERSION (BLOCK1)                               Package version                                    = 0 R/W (0x0)
    WAFER_VERSION_MINOR_LO (BLOCK1)                    WAFER_VERSION_MINOR least significant bits         = 0 R/W (0b000)
    OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID
       = ea 0e c6 f1 01 f2 38 82 e9 98 5b 59 81 fe 00 02 R/W
    BLK_VERSION_MINOR (BLOCK2)                         BLK_VERSION_MINOR of BLOCK2                        = ADC calib V2 R/W (0b010)
    WAFER_VERSION_MINOR (BLOCK0)                       calc WAFER VERSION MINOR = WAFER_VERSION_MINOR_HI  = 0 R/W (0x0)
                                                       << 3 + WAFER_VERSION_MINOR_LO (read only)

    Jtag fuses:
    SOFT_DIS_JTAG (BLOCK0)                             Software disables JTAG. When software disabled; JT = False R/W (0b0)
                                                       AG can be activated temporarily by HMAC peripheral
    HARD_DIS_JTAG (BLOCK0)                             Hardware disables JTAG permanently                 = False R/W (0b0)

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
       = 58:cf:79:b3:b9:54 (OK) R/W
    CUSTOM_MAC (BLOCK3)                                Custom MAC
       = 00:00:00:00:00:00 (OK) R/W

    Security fuses:
    DIS_DOWNLOAD_ICACHE (BLOCK0)                       Disables Icache when SoC is in Download mode       = False R/W (0b0)
    DIS_DOWNLOAD_DCACHE (BLOCK0)                       Disables Dcache when SoC is in Download mode       = False R/W (0b0)
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Set this bit to disable the function that forces c = False R/W (0b0)
                                                       hip into download mode
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Disables flash encryption when in download boot mo = False R/W (0b0)
                                                       des
    SPI_BOOT_CRYPT_CNT (BLOCK0)                        Enables flash encryption when 1 or 3 bits are set  = Disable R/W (0b000)
                                                       and disabled otherwise
    SECURE_BOOT_KEY_REVOKE0 (BLOCK0)                   Revoke 1st secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE1 (BLOCK0)                   Revoke 2nd secure boot key                         = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE2 (BLOCK0)                   Revoke 3rd secure boot key                         = False R/W (0b0)
    KEY_PURPOSE_0 (BLOCK0)                             Purpose of KEY0                                    = USER R/W (0x0)
    KEY_PURPOSE_1 (BLOCK0)                             Purpose of KEY1                                    = USER R/W (0x0)
    KEY_PURPOSE_2 (BLOCK0)                             Purpose of KEY2                                    = USER R/W (0x0)
    KEY_PURPOSE_3 (BLOCK0)                             Purpose of KEY3                                    = USER R/W (0x0)
    KEY_PURPOSE_4 (BLOCK0)                             Purpose of KEY4                                    = USER R/W (0x0)
    KEY_PURPOSE_5 (BLOCK0)                             Purpose of KEY5                                    = USER R/W (0x0)
    SECURE_BOOT_EN (BLOCK0)                            Set this bit to enable secure boot                 = False R/W (0b0)
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Set this bit to enable aggressive secure boot key  = False R/W (0b0)
                                                       revocation mode
    DIS_DOWNLOAD_MODE (BLOCK0)                         Set this bit to disable all download boot modes    = False R/W (0b0)
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Set this bit to enable secure UART download mode ( = False R/W (0b0)
                                                       read/write flash only)
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
    SPI_PAD_CONFIG_CLK (BLOCK1)                        SPI_PAD_configure CLK                              = 0 R/W (0b000000)
    SPI_PAD_CONFIG_Q (BLOCK1)                          SPI_PAD_configure Q(D1)                            = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D (BLOCK1)                          SPI_PAD_configure D(D0)                            = 0 R/W (0b000000)
    SPI_PAD_CONFIG_CS (BLOCK1)                         SPI_PAD_configure CS                               = 0 R/W (0b000000)
    SPI_PAD_CONFIG_HD (BLOCK1)                         SPI_PAD_configure HD(D3)                           = 0 R/W (0b000000)
    SPI_PAD_CONFIG_WP (BLOCK1)                         SPI_PAD_configure WP(D2)                           = 0 R/W (0b000000)
    SPI_PAD_CONFIG_DQS (BLOCK1)                        SPI_PAD_configure DQS                              = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D4 (BLOCK1)                         SPI_PAD_configure D4                               = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D5 (BLOCK1)                         SPI_PAD_configure D5                               = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D6 (BLOCK1)                         SPI_PAD_configure D6                               = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D7 (BLOCK1)                         SPI_PAD_configure D7                               = 0 R/W (0b000000)

    Usb fuses:
    DIS_USB (BLOCK0)                                   Set this bit to disable USB OTG function           = False R/W (0b0)
    USB_EXCHG_PINS (BLOCK0)                            Set this bit to exchange USB D+ and D- pins        = False R/W (0b0)
    USB_EXT_PHY_ENABLE (BLOCK0)                        Set this bit to enable external USB PHY            = False R/W (0b0)
    USB_FORCE_NOPERSIST (BLOCK0)                       If set; forces USB BVALID to 1                     = False R/W (0b0)
    DIS_USB_DOWNLOAD_MODE (BLOCK0)                     Set this bit to disable use of USB OTG in UART dow = False R/W (0b0)
                                                       nload boot mode

    Vdd fuses:
    VDD_SPI_XPD (BLOCK0)                               If VDD_SPI_FORCE is 1; this value determines if th = False R/W (0b0)
                                                       e VDD_SPI regulator is powered on
    VDD_SPI_TIEH (BLOCK0)                              If VDD_SPI_FORCE is 1; determines VDD_SPI voltage
       = VDD_SPI connects to 1.8 V LDO R/W (0b0)
    VDD_SPI_FORCE (BLOCK0)                             Set this bit to use XPD_VDD_PSI_REG and VDD_SPI_TI = False R/W (0b0)
                                                       EH to configure VDD_SPI LDO

    Wdt fuses:
    WDT_DELAY_SEL (BLOCK0)                             RTC watchdog timeout threshold; in unit of slow cl = 40000 R/W (0b00)
                                                       ock cycle

    Flash voltage (VDD_SPI) determined by GPIO45 on reset (GPIO45=High: VDD_SPI pin is powered from internal 1.8V LDO
    GPIO45=Low or NC: VDD_SPI pin is powered directly from VDD3P3_RTC_IO via resistor Rspi. Typically this voltage is 3.3 V).
