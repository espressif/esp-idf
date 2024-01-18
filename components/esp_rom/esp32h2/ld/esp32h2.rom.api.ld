/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** ROM APIs
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

PROVIDE ( esp_rom_efuse_mac_address_crc8       = esp_crc8 );
PROVIDE ( esp_rom_efuse_is_secure_boot_enabled = ets_efuse_secure_boot_enabled );

PROVIDE ( esp_rom_uart_flush_tx       = uart_tx_flush );
PROVIDE ( esp_rom_uart_tx_one_char    = uart_tx_one_char2 );
PROVIDE ( esp_rom_uart_tx_wait_idle   = uart_tx_wait_idle );
PROVIDE ( esp_rom_uart_rx_one_char    = uart_rx_one_char );
PROVIDE ( esp_rom_uart_rx_string      = UartRxString );
PROVIDE ( esp_rom_uart_set_as_console = uart_tx_switch );
PROVIDE ( esp_rom_uart_putc           = ets_write_char_uart );

PROVIDE ( esp_rom_output_flush_tx       = uart_tx_flush );
PROVIDE ( esp_rom_output_tx_one_char    = uart_tx_one_char );
PROVIDE ( esp_rom_output_tx_wait_idle   = uart_tx_wait_idle );
PROVIDE ( esp_rom_output_rx_one_char    = uart_rx_one_char );
PROVIDE ( esp_rom_output_rx_string      = UartRxString );
PROVIDE ( esp_rom_output_set_as_console = uart_tx_switch );
PROVIDE ( esp_rom_output_putc           = ets_write_char_uart );

PROVIDE ( esp_rom_md5_init   = MD5Init );
PROVIDE ( esp_rom_md5_update = MD5Update );
PROVIDE ( esp_rom_md5_final  = MD5Final );

PROVIDE ( esp_rom_software_reset_system   = software_reset );
PROVIDE ( esp_rom_software_reset_cpu   = software_reset_cpu );

PROVIDE ( esp_rom_printf   = ets_printf );
PROVIDE ( esp_rom_install_uart_printf = ets_install_uart_printf );
PROVIDE ( esp_rom_delay_us = ets_delay_us );
PROVIDE ( esp_rom_get_reset_reason = rtc_get_reset_reason );
PROVIDE ( esp_rom_route_intr_matrix = intr_matrix_set );
PROVIDE ( esp_rom_get_cpu_ticks_per_us = ets_get_cpu_frequency );
PROVIDE ( esp_rom_set_cpu_ticks_per_us = ets_update_cpu_frequency );

PROVIDE ( esp_rom_spiflash_attach = spi_flash_attach );
PROVIDE ( esp_rom_spiflash_clear_bp = esp_rom_spiflash_unlock );
PROVIDE ( esp_rom_spiflash_write_enable = SPI_write_enable );
PROVIDE ( esp_rom_spiflash_erase_area = SPIEraseArea );

PROVIDE ( esp_rom_spiflash_fix_dummylen = spi_dummy_len_fix );
PROVIDE ( esp_rom_spiflash_set_drvs = SetSpiDrvs);
PROVIDE ( esp_rom_spiflash_select_padsfunc = SelectSpiFunction );
PROVIDE ( esp_rom_spiflash_common_cmd = SPI_Common_Command );
