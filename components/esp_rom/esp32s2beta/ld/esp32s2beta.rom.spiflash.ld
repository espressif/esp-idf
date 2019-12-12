/*
	SPI flash driver function, compatibility names.
*/


PROVIDE ( cache_ibus_mmu_set_rom = cache_ibus_mmu_set );
PROVIDE ( Cache_Read_Disable_rom = Cache_Read_Disable );
PROVIDE ( Cache_Read_Enable_rom = Cache_Read_Enable );
PROVIDE ( cache_dbus_mmu_set_rom = cache_dbus_mmu_set );
PROVIDE ( g_rom_spiflash_dummy_len_plus = dummy_len_plus);
PROVIDE ( g_ticks_per_us_pro = g_ticks_per_us );
PROVIDE ( g_rom_flashchip = SPI_flashchip_data );
PROVIDE ( g_rom_spiflash_chip = SPI_flashchip_data );
PROVIDE ( esp_rom_spiflash_config_param = SPIParamCfg );
PROVIDE ( esp_rom_spiflash_read_statushigh = SPI_read_status_high );
PROVIDE ( esp_rom_spiflash_read_user_cmd = SPI_user_command_read );
PROVIDE ( esp_rom_spiflash_write = SPIWrite );
PROVIDE ( esp_rom_spiflash_write_encrypted_disable = SPI_Write_Encrypt_Disable );
PROVIDE ( esp_rom_spiflash_write_encrypted_enable = SPI_Write_Encrypt_Enable );
PROVIDE ( esp_rom_spiflash_config_clk = SPIClkConfig );
PROVIDE ( esp_rom_spiflash_select_qio_pins = SelectSpiQIO );
PROVIDE ( esp_rom_spiflash_unlock = SPIUnlock );
PROVIDE ( esp_rom_spiflash_erase_sector = SPIEraseSector );
PROVIDE ( esp_rom_spiflash_erase_block = SPIEraseBlock );
PROVIDE ( esp_rom_spiflash_wait_idle = SPI_Wait_Idle );
PROVIDE ( esp_rom_spiflash_config_readmode = SPIReadModeCnfig );
PROVIDE ( esp_rom_spiflash_erase_block = SPIEraseBlock );
PROVIDE ( esp_rom_spiflash_write_encrypted = SPI_Encrypt_Write );