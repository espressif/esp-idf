***********************
Chip Series Comparison
***********************

The comparison below covers key features of chips supported by ESP-IDF. For the full list of features please refer to respective datasheets in Section `Related Documents`_.

.. list-table:: Chip Series Comparison
   :widths: 20 40 40
   :header-rows: 1

   * - Feature
     - ESP32 Series
     - ESP32-S2 Series
   * - Launch year
     - 2016
     - 2020
   * - Variants
     - ESP32-D0WDQ6, ESP32-D0WD, ESP32-D2WD, ESP32-S0WD, ESP32-U4WDH, ESP32-PICO
     - ESP32-S2, ESP32-S2FH2, ESP32-S2FH4
   * - Core
     - Xtensa® dual-core 32-bit LX6 with 600 MIPS (in total); 200 MIPS for ESP32-U4WDH/ESP32-S0WD (single-core variants); 400 MIPS for ESP32-D2WD
     - Xtensa® single-core 32-bit LX7 with 300 MIPS
   * - Wi-Fi protocols
     - 802.11 b/g/n, 2.4 GHz
     - 802.11 b/g/n, 2.4 GHz
   * - Bluetooth®
     - Bluetooth 5.0 and Bluetooth LE
     - ✖️
   * - Typical frequency
     - 240 MHz (160 MHz for ESP32-S0WD, ESP32-D2WD, and ESP32-U4WDH)
     - 240 MHz
   * - SRAM
     - 520 KB
     - 320 KB
   * - ROM
     - 448 KB of ROM for booting and core functions
     - 128 KB of ROM for booting and core functions
   * - Flash
     - Up to 16 MB device, address 11 MB + 248 KB each time
     - Up to 1 GB device, address 11.5 MB each time
   * - PSRAM
     - Up to 16 MB device, address 4 MB each time
     - Up to 1 GB device, address 11.5 MB each time
   * - Cache
     - ✔️ 
     - ✔️ More functionalities, more modes
   * - **Peripherals**
     - 
     - 
   * - ADC
     - Two 12-bit, 18 channels
     - Two 13-bit, 20 channels
   * - Temperature sensor
     - ✖️
     - 1
   * - Touch sensor
     - 10
     - 14
   * - Hall sensor
     - 1
     - ✖️
   * - GPIO
     - 34
     - 43
   * - SPI
     - 4
     - 4 with more modes
   * - LCD interface
     - 1
     - 1
   * - UART
     - 3
     - 2 :sup:`1`
   * - I2C
     - 2
     - 2
   * - I2S
     - 2
     - 1 :sup:`1`
   * - Camera interface
     - 1
     - 1
   * - RMT
     - 8 channels
     - 4 channels :sup:`1`
   * - Pulse counter
     - 8 channels
     - 4 channels :sup:`1`
   * - LED PWM
     - 16 channels
     - 8 channels :sup:`1`
   * - USB OTG
     - ✖️
     - 1
   * - TWAI
     - 1
     - 1
   * - SD/SDIO/MMC host controller
     - 1
     - ✖️
   * - SDIO slave controller
     - 1
     - ✖️
   * - Ethernet MAC
     - 1
     - ✖️
   * - ULP
     - ULP FSM
     - PicoRV32 core with 8 KB SRAM, ULP FSM with more instructions
   * - **Security**
     - 
     - 
   * - Secure boot
     - ✔️
     - ✔️ Faster and safer
   * - Flash encryption
     - ✔️
     - ✔️ More functionalities (support for PSRAM encryption)
   * - OTP
     - 1024-bit
     - 4096-bit
   * - AES
     - ✔️
     - ✔️ DMA support for ECB, CBC, OFB, CTR, CFB8, CFB128 and GCM
   * - HASH
     - SHA-1, SHA-2
     - SHA-1, SHA-2; DMA support
   * - RSA
     - Up to 4096 bits
     - Up to 4096 bits, improved acceleration options
   * - RNG
     - ✔️
     - ✔️
   * - HMAC
     - ✖️
     - ✔️
   * - Digital signature
     - ✖️
     - ✔️
   * - XTS
     - ✖️
     - ✔️
   * - **Other**
     - 
     - 
   * - Deep-sleep (ULP sensor-monitored pattern)
     - 100 μA (when ADC work with a duty cycle of 1%)
     - 22 μA (when touch sensors work with a duty cycle of 1%)
   * - Size
     - QFN 5*5, 6*6
     - QFN 7*7 

**Note** 1: Reduced chip area


Related Documents
=================

- `ESP32 Datasheet (PDF) <https://espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_
- ESP32-PICO Datasheets (PDF)

    - `ESP32-PICO-D4 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_
    - `ESP32-PICO-V3 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3_datasheet_en.pdf>`_
    - `ESP32-PICO-V3-02 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3-02_datasheet_en.pdf>`_

- `ESP32-S2 Datasheet (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf>`_
