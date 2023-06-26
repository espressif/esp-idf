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

PROVIDE ( esp_rom_efuse_mac_address_crc8       = esp_crc8 );
PROVIDE ( esp_rom_efuse_get_flash_gpio_info    = ets_efuse_get_spiconfig );
PROVIDE ( esp_rom_efuse_is_secure_boot_enabled = ets_efuse_secure_boot_enabled );
PROVIDE ( esp_rom_efuse_get_flash_wp_gpio      = ets_efuse_get_wp_pad );

PROVIDE ( esp_rom_uart_flush_tx       = uart_tx_flush );
PROVIDE ( esp_rom_uart_tx_one_char    = uart_tx_one_char );
PROVIDE ( esp_rom_uart_tx_wait_idle   = uart_tx_wait_idle );
PROVIDE ( esp_rom_uart_rx_one_char    = uart_rx_one_char );
PROVIDE ( esp_rom_uart_rx_string      = UartRxString );

PROVIDE ( esp_rom_md5_init   = MD5Init );
PROVIDE ( esp_rom_md5_update = MD5Update );
PROVIDE ( esp_rom_md5_final  = MD5Final );

PROVIDE ( esp_rom_software_reset_system = software_reset );
PROVIDE ( esp_rom_software_reset_cpu    = software_reset_cpu );

PROVIDE ( esp_rom_printf   = ets_printf );
PROVIDE ( esp_rom_delay_us = ets_delay_us );
PROVIDE ( esp_rom_get_reset_reason = rtc_get_reset_reason );
PROVIDE ( esp_rom_route_intr_matrix = intr_matrix_set );
PROVIDE ( esp_rom_get_cpu_ticks_per_us = ets_get_cpu_frequency );
PROVIDE ( esp_rom_set_cpu_ticks_per_us = ets_update_cpu_frequency );

PROVIDE ( esp_rom_spiflash_clear_bp = esp_rom_spiflash_unlock );
PROVIDE ( esp_rom_spiflash_write_enable = SPI_write_enable );
PROVIDE ( esp_rom_spiflash_erase_area = SPIEraseArea );

PROVIDE ( esp_rom_spiflash_fix_dummylen = spi_dummy_len_fix );
PROVIDE ( esp_rom_spiflash_set_drvs = SetSpiDrvs);
PROVIDE ( esp_rom_spiflash_select_padsfunc = SelectSpiFunction );
PROVIDE ( esp_rom_spiflash_common_cmd = SPI_Common_Command );

PROVIDE ( esp_rom_regi2c_read = rom_i2c_readReg );
PROVIDE ( esp_rom_regi2c_read_mask = rom_i2c_readReg_Mask );
PROVIDE ( esp_rom_regi2c_write = rom_i2c_writeReg );
PROVIDE ( esp_rom_regi2c_write_mask = rom_i2c_writeReg_Mask );
