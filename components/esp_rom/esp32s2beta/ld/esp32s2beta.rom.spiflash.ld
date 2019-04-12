/*
	Address table for SPI driver functions in ESP32 ROM.
	These functions are only linked from ROM when SPI_FLASH_ROM_DRIVER_PATCH is not set in configuration.
*/

/*
PROVIDE ( esp_rom_spiflash_write_encrypted = 0x4000ae24 );
PROVIDE ( esp_rom_spiflash_erase_area = 0x4000b0a0 );
PROVIDE ( esp_rom_spiflash_erase_block = 0x4000ac20 );
PROVIDE ( esp_rom_spiflash_erase_chip = 0x4000ac00 );
PROVIDE ( esp_rom_spiflash_erase_sector = 0x4000ac80 );
PROVIDE ( esp_rom_spiflash_lock = 0x4000aa30 );
PROVIDE ( esp_rom_spiflash_read = 0x4000ae84 );
*/
/* SPIMasterReadModeCnfig */
/*
PROVIDE ( esp_rom_spiflash_config_readmode = 0x4000ab2c );
PROVIDE ( esp_rom_spiflash_read_status = 0x40009db8 );
PROVIDE ( esp_rom_spiflash_read_statushigh = 0x40009ffc );
PROVIDE ( esp_rom_spiflash_write = 0x4000ace0 );
PROVIDE ( esp_rom_spiflash_enable_write = 0x4000a2b8 );
PROVIDE ( esp_rom_spiflash_write_status = 0x4000a01c );
*/

/* always using patched versions of these functions 
PROVIDE ( esp_rom_spiflash_wait_idle = 0x40009e64 );
PROVIDE ( esp_rom_spiflash_unlock = 0x400????? );
*/
