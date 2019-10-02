/*
	Address table for SPI driver functions in ESP32 ROM.
	These functions are only linked from ROM when SPI_FLASH_ROM_DRIVER_PATCH is not set in configuration.
*/

PROVIDE ( esp_rom_spiflash_write_encrypted = 0x40062e78 );
PROVIDE ( esp_rom_spiflash_erase_area = 0x400631ac );
PROVIDE ( esp_rom_spiflash_erase_block = 0x40062c4c );
PROVIDE ( esp_rom_spiflash_erase_chip = 0x40062c14 );
PROVIDE ( esp_rom_spiflash_erase_sector = 0x40062ccc );
PROVIDE ( esp_rom_spiflash_attach = 0x40062a6c );
PROVIDE ( esp_rom_spiflash_lock = 0x400628f0 );
PROVIDE ( esp_rom_spiflash_read = 0x40062ed8 );
PROVIDE ( esp_rom_spiflash_config_readmode = 0x40062b64 ); /* SPIMasterReadModeCnfig */
PROVIDE ( esp_rom_spiflash_read_status = 0x4006226c );
PROVIDE ( esp_rom_spiflash_read_statushigh = 0x40062448 );
PROVIDE ( esp_rom_spiflash_write = 0x40062d50 );
PROVIDE ( esp_rom_spiflash_enable_write = 0x40062320 );
PROVIDE ( esp_rom_spiflash_write_status = 0x400622f0 );

/* always using patched versions of these functions 
PROVIDE ( esp_rom_spiflash_wait_idle = 0x400622c0 );
PROVIDE ( esp_rom_spiflash_unlock = 0x400????? );
*/
