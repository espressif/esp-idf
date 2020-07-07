/**
 * ROM APIs
 */

PROVIDE ( esp_rom_crc32_le = crc32_le );
PROVIDE ( esp_rom_crc16_le = crc16_le );
PROVIDE ( esp_rom_crc8_le  = crc8_le );
PROVIDE ( esp_rom_crc32_be = crc32_be );
PROVIDE ( esp_rom_crc16_be = crc16_be );
PROVIDE ( esp_rom_crc8_be  = crc8_be );

PROVIDE ( esp_rom_gpio_pad_select_gpio    = gpio_pad_select_gpio );
PROVIDE ( esp_rom_gpio_pad_pullup_only    = gpio_pad_pullup );
PROVIDE ( esp_rom_gpio_pad_set_drv        = gpio_pad_set_drv );
PROVIDE ( esp_rom_gpio_pad_unhold         = gpio_pad_unhold );
PROVIDE ( esp_rom_gpio_connect_in_signal  = gpio_matrix_in );
PROVIDE ( esp_rom_gpio_connect_out_signal = gpio_matrix_out );
