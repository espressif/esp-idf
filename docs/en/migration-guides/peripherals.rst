Migrate Peripherals to ESP-IDF 5.0
==================================

Peripheral Clock Gating
-----------------------

As usual, peripheral clock gating is still handled by driver itself, users don't need to take care of the peripheral module clock gating.

However, for advanced users who implement their own drivers based on ``hal`` and ``soc`` components, the previous clock gating include path has been changed from ``driver/periph_ctrl.h`` to ``esp_private/periph_ctrl.h``.

SPI Flash Interface
-------------------

Version before v5.0, spi flash functions in rom can be included by ``esp32**/rom/spi_flash.h``. However, your code written for different chips may be filled with ROM headers of different versions. At the meantime not all the APIs can be used on all chips.

Therefore, the common APIs are extracted to ``esp_rom_spiflash.h``. Although it's not a breaking change, it is strongly recommended to only use the functions with prefix ``esp_rom_spiflash`` included by ``esp_rom_spiflash.h`` for better cross-compatibility.

To make the API clearer, we renamed the function ``esp_rom_spiflash_lock`` to ``esp_rom_spiflash_set_bp``. We renamed ``esp_rom_spiflash_unlock`` to ``esp_rom_spiflash_clear_bp``.

ADC
---

Previous `driver/adc2_wifi_private.h` has been moved to `esp_private/adc2_wifi.h`.

GPIO
----

The previous Kconfig option `RTCIO_SUPPORT_RTC_GPIO_DESC` has been removed, thus the ``rtc_gpio_desc`` array is unavailable. Please use ``rtc_io_desc`` array instead.
