.. code-block:: none

    espefuse.py -p PORT summary

    Connecting........__
    Detecting chip type... ESP32
    espefuse.py v3.1-dev
    EFUSE_NAME (Block)                       Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Calibration fuses:
    BLK3_PART_RESERVE (BLOCK0):              BLOCK3 partially served for ADC calibration data   = True R/W (0b1)
    ADC_VREF (BLOCK0):                       Voltage reference calibration                      = 1114 R/W (0b00010)
    ADC1_TP_LOW (BLOCK3):                    ADC1 150mV reading                                 = 346 R/W (0b0010001)
    ADC1_TP_HIGH (BLOCK3):                   ADC1 850mV reading                                 = 3285 R/W (0b000000101)
    ADC2_TP_LOW (BLOCK3):                    ADC2 150mV reading                                 = 449 R/W (0b0000111)
    ADC2_TP_HIGH (BLOCK3):                   ADC2 850mV reading                                 = 3362 R/W (0b111110101)

    Config fuses:
    XPD_SDIO_FORCE (BLOCK0):                 Ignore MTDI pin (GPIO12) for VDD_SDIO on reset     = False R/W (0b0)
    XPD_SDIO_REG (BLOCK0):                   If XPD_SDIO_FORCE, enable VDD_SDIO reg on reset    = False R/W (0b0)
    XPD_SDIO_TIEH (BLOCK0):                  If XPD_SDIO_FORCE & XPD_SDIO_REG                   = 1.8V R/W (0b0)
    CLK8M_FREQ (BLOCK0):                     8MHz clock freq override                           = 53 R/W (0x35)
    SPI_PAD_CONFIG_CLK (BLOCK0):             Override SD_CLK pad (GPIO6/SPICLK)                 = 0 R/W (0b00000)
    SPI_PAD_CONFIG_Q (BLOCK0):               Override SD_DATA_0 pad (GPIO7/SPIQ)                = 0 R/W (0b00000)
    SPI_PAD_CONFIG_D (BLOCK0):               Override SD_DATA_1 pad (GPIO8/SPID)                = 0 R/W (0b00000)
    SPI_PAD_CONFIG_HD (BLOCK0):              Override SD_DATA_2 pad (GPIO9/SPIHD)               = 0 R/W (0b00000)
    SPI_PAD_CONFIG_CS0 (BLOCK0):             Override SD_CMD pad (GPIO11/SPICS0)                = 0 R/W (0b00000)
    DISABLE_SDIO_HOST (BLOCK0):              Disable SDIO host                                  = False R/W (0b0)

    Efuse fuses:
    WR_DIS (BLOCK0):                         Efuse write disable mask                           = 0 R/W (0x0000)
    RD_DIS (BLOCK0):                         Efuse read disable mask                            = 0 R/W (0x0)
    CODING_SCHEME (BLOCK0):                  Efuse variable block length scheme                
    = 3/4 (BLK1-3 len=192 bits) R/W (0b01)
    KEY_STATUS (BLOCK0):                     Usage of efuse block 3 (reserved)                  = False R/W (0b0)

    Identity fuses:
    MAC (BLOCK0):                            Factory MAC Address                               
    = 84:0d:8e:18:8e:44 (CRC 0xad OK) R/W 
    MAC_CRC (BLOCK0):                        CRC8 for factory MAC address                       = 173 R/W (0xad)
    CHIP_VER_REV1 (BLOCK0):                  Silicon Revision 1                                 = True R/W (0b1)
    CHIP_VER_REV2 (BLOCK0):                  Silicon Revision 2                                 = False R/W (0b0)
    CHIP_VERSION (BLOCK0):                   Reserved for future chip versions                  = 2 R/W (0b10)
    CHIP_PACKAGE (BLOCK0):                   Chip package identifier                            = 0 R/W (0b000)
    MAC_VERSION (BLOCK3):                    Version of the MAC field                           = 0 R/W (0x00)

    Security fuses:
    FLASH_CRYPT_CNT (BLOCK0):                Flash encryption mode counter                      = 0 R/W (0b0000000)
    UART_DOWNLOAD_DIS (BLOCK0):              Disable UART download mode (ESP32 rev3 only)       = False R/W (0b0)
    FLASH_CRYPT_CONFIG (BLOCK0):             Flash encryption config (key tweak bits)           = 0 R/W (0x0)
    CONSOLE_DEBUG_DISABLE (BLOCK0):          Disable ROM BASIC interpreter fallback             = True R/W (0b1)
    ABS_DONE_0 (BLOCK0):                     Secure boot V1 is enabled for bootloader image     = False R/W (0b0)
    ABS_DONE_1 (BLOCK0):                     Secure boot V2 is enabled for bootloader image     = False R/W (0b0)
    JTAG_DISABLE (BLOCK0):                   Disable JTAG                                       = False R/W (0b0)
    DISABLE_DL_ENCRYPT (BLOCK0):             Disable flash encryption in UART bootloader        = False R/W (0b0)
    DISABLE_DL_DECRYPT (BLOCK0):             Disable flash decryption in UART bootloader        = False R/W (0b0)
    DISABLE_DL_CACHE (BLOCK0):               Disable flash cache in UART bootloader             = False R/W (0b0)
    BLOCK1 (BLOCK1):                         Flash encryption key                              
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK2 (BLOCK2):                         Secure boot key                                   
    = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
    BLOCK3 (BLOCK3):                         Variable Block 3                                  
    = 00 00 00 00 00 00 00 00 00 00 00 00 91 02 87 fa 00 00 00 00 00 00 00 00 R/W 

    Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).

To get a dump for all eFuse registers.

.. code-block:: none

    espefuse.py -p PORT dump

    Connecting........_
    Detecting chip type... ESP32
    BLOCK0          (                ) [0 ] read_regs: 00000000 8e188e44 00ad840d 0000e000 00000235 00000000 00000005
    BLOCK1          (flash_encryption) [1 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK2          (secure_boot_v1 s) [2 ] read_regs: 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK3          (                ) [3 ] read_regs: 00000000 00000000 00000000 fa870291 00000000 00000000
    espefuse.py v3.1-dev
