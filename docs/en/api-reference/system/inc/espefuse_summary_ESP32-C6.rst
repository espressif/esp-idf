.. code-block:: none

    espefuse.py -p PORT summary

    Connecting....
    Detecting chip type... ESP32-C6

    === Run "summary" command ===
    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Config fuses:
    DIS_ICACHE (BLOCK0)                                Disables ICache                                    = False R/W (0b0)
    DIS_DOWNLOAD_ICACHE (BLOCK0)                       Disables Icache when SoC is in Download mode       = False R/W (0b0)
    DIS_FORCE_DOWNLOAD (BLOCK0)                        Disables forcing chip into Download mode           = False R/W (0b0)
    DIS_SPI_DOWNLOAD_MSPI (BLOCK0)                     SPI0 controller is disabled in boot_mode_download  = False R/W (0b0)
    DIS_CAN (BLOCK0)                                   Disables the TWAI Controller hardware              = False R/W (0b0)
    VDD_SPI_AS_GPIO (BLOCK0)                           Set this bit to vdd spi pin function as gpio       = False R/W (0b0)
    BTLC_GPIO_ENABLE (BLOCK0)                          Enable btlc gpio                                   = 0 R/W (0b00)
    POWERGLITCH_EN (BLOCK0)                            Set this bit to enable power glitch function       = False R/W (0b0)
    POWER_GLITCH_DSENSE (BLOCK0)                       Sample delay configuration of power glitch         = 0 R/W (0b00)
    DIS_DIRECT_BOOT (BLOCK0)                           Disables direct boot mode                          = False R/W (0b0)
    UART_PRINT_CHANNEL (BLOCK0)                        Selects the default UART for printing boot msg     = UART0 R/W (0b0)
    UART_PRINT_CONTROL (BLOCK0)                        Sets the default UART boot message output mode     = Enabled R/W (0b00)
    FORCE_SEND_RESUME (BLOCK0)                         Force ROM code to send a resume command during SPI = False R/W (0b0)
                                                        bootduring SPI boot                              
    DISABLE_WAFER_VERSION_MAJOR (BLOCK0)               Disables check of wafer version major              = False R/W (0b0)
    DISABLE_BLK_VERSION_MAJOR (BLOCK0)                 Disables check of blk version major                = False R/W (0b0)
    BLOCK_USR_DATA (BLOCK3)                            User data                                         
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 

    Efuse fuses:
    WR_DIS (BLOCK0)                                    Disables programming of individual eFuses          = 0 R/W (0x00000000)
    RD_DIS (BLOCK0)                                    Disables software reading from BLOCK4-10           = 0 R/W (0b0000000)

    Flash Config fuses:
    FLASH_TPUW (BLOCK0)                                Configures flash startup delay after SoC power-up, = 0 R/W (0x0)
                                                        unit is (ms/2). When the value is 15, delay is 7.
                                                    5 ms                                              
    FLASH_ECC_MODE (BLOCK0)                            Set this bit to set flsah ecc mode.               
    = flash ecc 16to18 byte mode R/W (0b0)
    FLASH_TYPE (BLOCK0)                                Selects SPI flash type                             = 4 data lines R/W (0b0)
    FLASH_PAGE_SIZE (BLOCK0)                           Flash page size                                    = 0 R/W (0b00)
    FLASH_ECC_EN (BLOCK0)                              Enable ECC for flash boot                          = False R/W (0b0)

    Identity fuses:
    SECURE_VERSION (BLOCK0)                            Secure version (used by ESP-IDF anti-rollback feat = 0 R/W (0x0000)
                                                    ure)                                              
    MAC (BLOCK1)                                       Factory MAC Address                               
    = 60:55:f9:f6:03:24 (OK) R/W 
    WAFER_VERSION_MINOR_LO (BLOCK1)                    WAFER_VERSION_MINOR least significant bits         = 0 R/W (0b000)
    PKG_VERSION (BLOCK1)                               Package version                                    = 0 R/W (0b000)
    BLK_VERSION_MINOR (BLOCK1)                         BLOCK version minor                                = 0 R/W (0b000)
    WAFER_VERSION_MINOR_HI (BLOCK1)                    WAFER_VERSION_MINOR most significant bits          = 0 R/W (0b0)
    WAFER_VERSION_MAJOR (BLOCK1)                       WAFER_VERSION_MAJOR                                = 0 R/W (0b00)
    OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID                        
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLK_VERSION_MAJOR (BLOCK2)                         BLOCK version major                                = No calibration R/W (0b00)
    CUSTOM_MAC (BLOCK3)                                Custom MAC Address                                
    = 00:00:00:00:00:00 (OK) R/W 
    WAFER_VERSION_MINOR (BLOCK0)                       calc WAFER VERSION MINOR = WAFER_VERSION_MINOR_HI  = 0 R/W (0x0)
                                                    << 3 + WAFER_VERSION_MINOR_LO (read only)         

    Jtag Config fuses:
    JTAG_SEL_ENABLE (BLOCK0)                           Set this bit to enable selection between usb_to_jt = False R/W (0b0)
                                                    ag and pad_to_jtag through strapping gpio10 when b
                                                    oth reg_dis_usb_jtag and reg_dis_pad_jtag are equa
                                                    l to 0.                                           
    SOFT_DIS_JTAG (BLOCK0)                             Software disables JTAG. When software disabled, JT = 0 R/W (0b000)
                                                    AG can be activated temporarily by HMAC peripheral
    DIS_PAD_JTAG (BLOCK0)                              Permanently disable JTAG access via pads. USB JTAG = False R/W (0b0)
                                                        is controlled separately.                        

    Security fuses:
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Disables flash encryption when in download boot mo = False R/W (0b0)
                                                    des                                               
    SPI_BOOT_CRYPT_CNT (BLOCK0)                        Enables encryption and decryption, when an SPI boo = Disable R/W (0b000)
                                                    t mode is set. Enabled when 1 or 3 bits are set,di
                                                    sabled otherwise                                  
    SECURE_BOOT_KEY_REVOKE0 (BLOCK0)                   If set, revokes use of secure boot key digest 0    = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE1 (BLOCK0)                   If set, revokes use of secure boot key digest 1    = False R/W (0b0)
    SECURE_BOOT_KEY_REVOKE2 (BLOCK0)                   If set, revokes use of secure boot key digest 2    = False R/W (0b0)
    KEY_PURPOSE_0 (BLOCK0)                             KEY0 purpose                                       = USER R/W (0x0)
    KEY_PURPOSE_1 (BLOCK0)                             KEY1 purpose                                       = USER R/W (0x0)
    KEY_PURPOSE_2 (BLOCK0)                             KEY2 purpose                                       = USER R/W (0x0)
    KEY_PURPOSE_3 (BLOCK0)                             KEY3 purpose                                       = USER R/W (0x0)
    KEY_PURPOSE_4 (BLOCK0)                             KEY4 purpose                                       = USER R/W (0x0)
    KEY_PURPOSE_5 (BLOCK0)                             KEY5 purpose                                       = USER R/W (0x0)
    SECURE_BOOT_EN (BLOCK0)                            Enables secure boot                                = False R/W (0b0)
    SECURE_BOOT_AGGRESSIVE_REVOKE (BLOCK0)             Enables aggressive secure boot key revocation mode = False R/W (0b0)
    DIS_DOWNLOAD_MODE (BLOCK0)                         Disables all Download boot modes                   = False R/W (0b0)
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Enables secure UART download mode (read/write flas = False R/W (0b0)
                                                    h only)       
    SECURE_BOOT_DISABLE_FAST_WAKE (BLOCK0)             FAST VERIFY ON WAKE is disabled or enabled when    = False R/W (0b0)
                                                    Secure Boot is enabled       
    SEC_DPA_LEVEL (BLOCK0)                             Configures the clock random divide mode to         = 0 R/W (0b00)
                                                    determine the DPA security level
    CRYPT_DPA_ENABLE (BLOCK0)                          Defense against DPA attack is enabled              = True R/W (0b1)
    BLOCK_KEY0 (BLOCK4)
    Purpose: USER
                Encryption key0 or user data                      
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK_KEY1 (BLOCK5)
    Purpose: USER
                Encryption key1 or user data                      
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK_KEY2 (BLOCK6)
    Purpose: USER
                Encryption key2 or user data                      
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK_KEY3 (BLOCK7)
    Purpose: USER
                Encryption key3 or user data                      
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK_KEY4 (BLOCK8)
    Purpose: USER
                Encryption key4 or user data                      
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK_KEY5 (BLOCK9)
    Purpose: USER
                Encryption key5 or user data                      
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK_SYS_DATA2 (BLOCK10)                          System data (part 2)                              
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 

    Spi_Pad_Config fuses:
    SPI_PAD_CONFIG_CLK (BLOCK1)                        SPI CLK pad                                        = 0 R/W (0b000000)
    SPI_PAD_CONFIG_Q (BLOCK1)                          SPI Q (D1) pad                                     = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D (BLOCK1)                          SPI D (D0) pad                                     = 0 R/W (0b000000)
    SPI_PAD_CONFIG_CS (BLOCK1)                         SPI CS pad                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_HD (BLOCK1)                         SPI HD (D3) pad                                    = 0 R/W (0b000000)
    SPI_PAD_CONFIG_WP (BLOCK1)                         SPI WP (D2) pad                                    = 0 R/W (0b000000)
    SPI_PAD_CONFIG_DQS (BLOCK1)                        SPI DQS pad                                        = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D4 (BLOCK1)                         SPI D4 pad                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D5 (BLOCK1)                         SPI D5 pad                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D6 (BLOCK1)                         SPI D6 pad                                         = 0 R/W (0b000000)
    SPI_PAD_CONFIG_D7 (BLOCK1)                         SPI D7 pad                                         = 0 R/W (0b000000)

    Usb Config fuses:
    DIS_USB_JTAG (BLOCK0)                              Disables USB JTAG. JTAG access via pads is control = False R/W (0b0)
                                                    led separately                                    
    DIS_USB_DEVICE (BLOCK0)                            Disables USB DEVICE                                = False R/W (0b0)
    DIS_USB (BLOCK0)                                   Disables the USB OTG hardware                      = False R/W (0b0)
    USB_EXCHG_PINS (BLOCK0)                            Exchanges USB D+ and D- pins                       = False R/W (0b0)
    DIS_USB_SERIAL_JTAG_ROM_PRINT (BLOCK0)             USB-Serial-JTAG during ROM boot is disabled        = False R/W (0b0)
    DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE (BLOCK0)         USB-Serial-JTAG download function is disabled      = False R/W (0b0)

    Vdd_Spi Config fuses:
    PIN_POWER_SELECTION (BLOCK0)                       GPIO33-GPIO37 power supply selection in ROM code   = VDD3P3_CPU R/W (0b0)

    Wdt Config fuses:
    WDT_DELAY_SEL (BLOCK0)                             Selects RTC WDT timeout threshold at startup       = False R/W (0b0)
