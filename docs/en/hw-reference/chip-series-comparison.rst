***********************
Chip Series Comparison
***********************

:link_to_translation:`zh_CN:[中文]`

The comparison below covers key features of chips supported by ESP-IDF. For the full list of features please refer to respective datasheets in Section `Related Documents`_.

.. list-table:: Chip Series Comparison
   :widths: 20 40 40 40
   :header-rows: 1

   * - Feature
     - ESP32 Series
     - ESP32-S2 Series
     - ESP32-C3 Series
   * - Launch year
     - 2016
     - 2020
     - 2020
   * - Variants
     - See `ESP32 Datasheet (PDF) <https://espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_
     - See `ESP32-S2 Datasheet (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf>`_
     - See `ESP32-C3 Datasheet (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf>`_
   * - Core
     - Xtensa® dual-core 32-bit LX6 with 600 MIPS (in total); 200 MIPS for ESP32-U4WDH/ESP32-S0WD (single-core variants); 400 MIPS for ESP32-D2WD
     - Xtensa® single-core 32-bit LX7 with 300 MIPS
     - 32-bit single-core RISC-V
   * - Wi-Fi protocols
     - 802.11 b/g/n, 2.4 GHz
     - 802.11 b/g/n, 2.4 GHz
     - 802.11 b/g/n, 2.4 GHz
   * - Bluetooth®
     - Bluetooth v4.2 BR/EDR and Bluetooth Low Energy
     - ✖️
     - Bluetooth 5.0
   * - Typical frequency
     - 240 MHz (160 MHz for ESP32-S0WD, ESP32-D2WD, and ESP32-U4WDH)
     - 240 MHz
     - 160 MHz
   * - SRAM
     - 520 KB
     - 320 KB
     - 400 KB
   * - ROM
     - 448 KB for booting and core functions
     - 128 KB for booting and core functions
     - 384 KB for booting and core functions
   * - Embedded flash
     - 2 MB, 4 MB, or none, depending on variants
     - 2 MB, 4 MB, or none, depending on variants
     - 4 MB or none, depending on variants
   * - External flash
     - Up to 16 MB device, address 11 MB + 248 KB each time
     - Up to 1 GB device, address 11.5 MB each time
     - Up to 16 MB device, address 8 MB each time
   * - External RAM
     - Up to 8 MB device, address 4 MB each time
     - Up to 1 GB device, address 11.5 MB each time
     - ✖️
   * - Cache
     - ✔️ Two-way set associative
     - ✔️ Four-way set associative, independent instruction cache and data cache
     - ✔️ Eight-way set associative, 32-bit data/instruction bus width
   * - **Peripherals**
     - 
     -
     - 
   * - ADC
     - Two 12-bit, 18 channels
     - Two 13-bit, 20 channels
     - Two 12-bit SAR ADCs, at most 6 channels
   * - DAC
     - Two 8-bit channels
     - Two 8-bit channels
     - ✖️ 
   * - Timers
     - Four 64-bit general-purpose timers, and three watchdog timers
     - Four 64-bit general-purpose timers, and three watchdog timers
     - Two 54-bit general-purpose timers, and three watchdog timers       
   * - Temperature sensor
     - ✖️
     - 1
     - 1
   * - Touch sensor
     - 10
     - 14
     - ✖️
   * - Hall sensor
     - 1
     - ✖️
     - ✖️
   * - GPIO
     - 34
     - 43
     - 22
   * - SPI
     - 4
     - 4 with more modes, compared with ESP32
     - 3
   * - LCD interface
     - 1
     - 1
     - ✖️
   * - UART
     - 3
     - 2 :sup:`1`
     - 2 :sup:`1`
   * - I2C
     - 2
     - 2
     - 1
   * - I2S
     - 2, can be configured to operate with 8/16/32/40/48-bit resolution as an input or output channel.
     - 1, can be configured to operate with 8/16/24/32/48/64-bit resolution as an input or output channel.
     - 1, can be configured to operate with 8/16/24/32-bit resolution as an input or output channel.
   * - Camera interface
     - 1
     - 1
     - ✖️
   * - DMA
     - Dedicated DMA to UART, SPI, I2S, SDIO slave, SD/MMC host, EMAC, BT, and Wi-Fi
     - Dedicated DMA to UART, SPI, AES, SHA, I2S, and ADC Controller
     - General-purpose, 3 TX channels, 3 RX channels
   * - RMT
     - 8 channels
     - 4 channels :sup:`1`, can be configured to TX/RX channels
     - 4 channels :sup:`2`, 2 TX channels, 2 RX channels
   * - Pulse counter
     - 8 channels
     - 4 channels :sup:`1`
     - ✖️
   * - LED PWM
     - 16 channels
     - 8 channels :sup:`1`
     - 6 channels :sup:`2`
   * - MCPWM
     - 2, six PWM outputs
     - ✖️ 
     - ✖️ 
   * - USB OTG
     - ✖️
     - 1
     - ✖️           
   * - TWAI® controller (compatible with ISO 11898-1)
     - 1
     - 1
     - 1
   * - SD/SDIO/MMC host controller
     - 1
     - ✖️
     - ✖️
   * - SDIO slave controller
     - 1
     - ✖️
     - ✖️
   * - Ethernet MAC
     - 1
     - ✖️
     - ✖️
   * - ULP
     - ULP FSM
     - PicoRV32 core with 8 KB SRAM, ULP FSM with more instructions
     - ✖️
   * - Debug Assist
     - ✖️
     - ✖️
     - 1
   * - **Security**
     - 
     -
     - 
   * - Secure boot
     - ✔️
     - ✔️ Faster and safer, compared with ESP32
     - ✔️ Faster and safer, compared with ESP32
   * - Flash encryption
     - ✔️
     - ✔️ Support for PSRAM encryption. Safer, compared with ESP32
     - ✔️ Safer, compared with ESP32
   * - OTP
     - 1024-bit
     - 4096-bit
     - 4096-bit
   * - AES
     - ✔️ AES-128, AES-192, AES-256 (FIPS PUB 197)
     - ✔️ AES-128, AES-192, AES-256 (FIPS PUB 197)
     - ✔️ AES-128, AES-256 (FIPS PUB 197)
   * - HASH
     - SHA-1, SHA-256, SHA-384, SHA-512 (FIPS PUB 180-4)
     - SHA-1, SHA-224, SHA-256, SHA-384, SHA-512, SHA-512/224, SHA-512/256, SHA-512/t (FIPS PUB 180-4); DMA support
     - SHA-1, SHA-224, SHA-256 (FIPS PUB 180-4)
   * - RSA
     - Up to 4096 bits
     - Up to 4096 bits, improved acceleration options compared with ESP32
     - Up to 3072 bits
   * - RNG
     - ✔️
     - ✔️
     - ✔️
   * - HMAC
     - ✖️
     - ✔️
     - ✔️
   * - Digital signature
     - ✖️
     - ✔️
     - ✔️
   * - XTS
     - ✖️
     - ✔️ XTS-AES-128, XTS-AES-256
     - ✔️ XTS-AES-128
   * - **Other**
     - 
     -
     - 
   * - Deep-sleep (ULP sensor-monitored pattern)
     - 100 μA (when ADC work with a duty cycle of 1%)
     - 22 μA (when touch sensors work with a duty cycle of 1%)
     - No such pattern
   * - Size
     - QFN48 5*5, 6*6, depending on variants
     - QFN56 7*7 
     - QFN32 5*5

**Note** 1: Reduced chip area compared with ESP32


**Note** 2: Reduced chip area compared with ESP32 and ESP32-S2


**Note** 3: Die size: ESP32-C3 < ESP32-S2 < ESP32




Related Documents
=================

- `ESP32 Datasheet (PDF) <https://espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_
- ESP32-PICO Datasheets (PDF)

    - `ESP32-PICO-D4 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_
    - `ESP32-PICO-V3 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3_datasheet_en.pdf>`_
    - `ESP32-PICO-V3-02 <https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3-02_datasheet_en.pdf>`_

- `ESP32-S2 Datasheet (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf>`_
- `ESP32-C3 Datasheet (PDF) <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf>`_
- `ESP Product Selector <http://products.espressif.com:8000/#/>`_