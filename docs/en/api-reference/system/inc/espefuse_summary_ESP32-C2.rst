.. code-block:: none

    espefuse.py -p PORT summary

    Connecting...................
    Detecting chip type... ESP32-C2
    espefuse.py v4.1

    === Run "summary" command ===
    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Adc_Calib fuses:
    ADC_CALIBRATION_0 (BLOCK2)                                                                            = 0 R/W (0b0000000000000000000000)
    ADC_CALIBRATION_1 (BLOCK2)                                                                            = 0 R/W (0x00000000)
    ADC_CALIBRATION_2 (BLOCK2)                                                                            = 0 R/W (0x00000000)

    Config fuses:
    UART_PRINT_CONTROL (BLOCK0)                        Set UART boot message output mode                  = Force print R/W (0b00)
    FORCE_SEND_RESUME (BLOCK0)                         Force ROM code to send a resume cmd during SPI boo = False R/W (0b0)
                                                       t                                                 
    DIS_DIRECT_BOOT (BLOCK0)                           Disable direct_boot mode                           = False R/W (0b0)

    Efuse fuses:
    WR_DIS (BLOCK0)                                    Disables programming of individual eFuses          = 0 R/W (0x00)
    RD_DIS (BLOCK0)                                    Disables software reading from BLOCK3              = 0 R/W (0b00)

    Flash Config fuses:
    FLASH_TPUW (BLOCK0)                                Configures flash startup delay after SoC power-up, = 0 R/W (0x0)
                                                        unit is (ms/2). When the value is 15, delay is 7.
                                                       5 ms                                              

    Identity fuses:
    SECURE_VERSION (BLOCK0)                            Secure version (anti-rollback feature)             = 0 R/W (0x0)
    CUSTOM_MAC_USED (BLOCK0)                           Enable CUSTOM_MAC programming                      = False R/W (0b0)
    CUSTOM_MAC (BLOCK1)                                Custom MAC addr                                   
    = 00:00:00:00:00:00 (OK) R/W 
    MAC (BLOCK2)                                       Factory MAC Address                               
    = 94:b5:55:80:00:d0 (OK) R/W 
    WAFER_VERSION (BLOCK2)                             WAFER version                                      = (revision 0) R/W (0b000)
    PKG_VERSION (BLOCK2)                               Package version                                    = ESP32-C2 R/W (0b000)
    BLOCK2_VERSION (BLOCK2)                            Version of BLOCK2                                  = No calibration R/W (0b000)

    Jtag Config fuses:
    DIS_PAD_JTAG (BLOCK0)                              Permanently disable JTAG access via padsUSB JTAG i = False R/W (0b0)
                                                       s controlled separately                           

    Ldo fuses:
    LDO_VOL_BIAS_CONFIG_LOW (BLOCK2)                                                                      = 0 R/W (0b000)
    LDO_VOL_BIAS_CONFIG_HIGH (BLOCK2)                                                                     = 0 R/W (0b000000000000000000000000000)

    Pvt fuses:
    PVT_LOW (BLOCK2)                                                                                      = 0 R/W (0b00000)
    PVT_HIGH (BLOCK2)                                                                                     = 0 R/W (0b0000000000)

    Rf fuses:
    RF_REF_I_BIAS_CONFIG (BLOCK2)                                                                         = 0 R/W (0b000)

    Security fuses:
    DIS_DOWNLOAD_ICACHE (BLOCK0)                       Disables iCache in download mode                   = False R/W (0b0)
    DIS_DOWNLOAD_MANUAL_ENCRYPT (BLOCK0)               Disables flash encryption in Download boot modes   = False R/W (0b0)
    SPI_BOOT_CRYPT_CNT (BLOCK0)                        Enables encryption and decryption, when an SPI boo = Disable R/W (0b000)
                                                       t mode is set. Enabled when 1 or 3 bits are set,dis
                                                       abled otherwise                                   
    XTS_KEY_LENGTH_256 (BLOCK0)                        Flash encryption key length                        = 128 bits key R/W (0b0)
    DIS_DOWNLOAD_MODE (BLOCK0)                         Disables all Download boot modes                   = False R/W (0b0)
    ENABLE_SECURITY_DOWNLOAD (BLOCK0)                  Enables secure UART download mode (read/write flas = False R/W (0b0)
                                                       h only)                                           
    SECURE_BOOT_EN (BLOCK0)                            Configures secure boot                             = Flase R/W (0b0)
    BLOCK_KEY0 (BLOCK3)                                BLOCK_KEY0 - 256-bits. 256-bit key of Flash Encryp
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
                                                    tion                                              
    BLOCK_KEY0_LOW_128 (BLOCK3)                        BLOCK_KEY0 - lower 128-bits. 128-bit key of Flash 
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
                                                    Encryption                                        
    BLOCK_KEY0_HI_128 (BLOCK3)                         BLOCK_KEY0 - higher 128-bits. 128-bits key of Secu
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
                                                    re Boot.                                          

    Wdt Config fuses:
    WDT_DELAY_SEL (BLOCK0)                             RTC WDT timeout threshold                          = 0 R/W (0b00)


To get a dump for all eFuse registers.

.. code-block:: none

    espefuse.py -p PORT dump

    Connecting..............
    Detecting chip type... ESP32-C2
    BLOCK0          (BLOCK0          ) [0 ] read_regs: 00000000 00000000
    BLOCK1          (BLOCK1          ) [1 ] read_regs: 00000000 00000000 00000000
    BLOCK2          (BLOCK2          ) [2 ] read_regs: 558000d0 000094b5 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY0      (BLOCK3          ) [3 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

    BLOCK0          (BLOCK0          ) [0 ] err__regs: 00000000 00000000
    EFUSE_RD_RS_ERR_REG         0x00000000
    espefuse.py v4.1

    === Run "dump" command ===
