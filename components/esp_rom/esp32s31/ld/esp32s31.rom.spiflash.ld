/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.spiflash.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group esp_flash
 ***************************************/

/* Functions */
esp_flash_chip_driver_initialized = 0x2f800214;
esp_flash_read_id = 0x2f800218;
esp_flash_get_size = 0x2f80021c;
esp_flash_erase_chip = 0x2f800220;
esp_flash_erase_region = 0x2f800224;
esp_flash_get_chip_write_protect = 0x2f800228;
esp_flash_set_chip_write_protect = 0x2f80022c;
esp_flash_get_protectable_regions = 0x2f800230;
esp_flash_get_protected_region = 0x2f800234;
esp_flash_set_protected_region = 0x2f800238;
esp_flash_read = 0x2f80023c;
esp_flash_write = 0x2f800240;
esp_flash_write_encrypted = 0x2f800244;
esp_flash_read_encrypted = 0x2f800248;
esp_flash_get_io_mode = 0x2f80024c;
esp_flash_set_io_mode = 0x2f800250;
spi_flash_boot_attach = 0x2f800254;
esp_flash_read_chip_id = 0x2f800258;
detect_spi_flash_chip = 0x2f80025c;
esp_flash_suspend_cmd_init = 0x2f800260;
/* Data (.data, .bss, .rodata) */
esp_flash_default_chip = 0x2f07ffdc;
esp_flash_api_funcs = 0x2f07ffd8;


/***************************************
 Group spi_flash_chips
 ***************************************/

/* Functions */
spi_flash_chip_generic_probe = 0x2f800264;
spi_flash_chip_generic_detect_size = 0x2f800268;
spi_flash_chip_generic_write = 0x2f80026c;
spi_flash_chip_generic_write_encrypted = 0x2f800270;
spi_flash_chip_generic_set_write_protect = 0x2f800274;
spi_flash_common_write_status_16b_wrsr = 0x2f800278;
spi_flash_chip_generic_reset = 0x2f80027c;
spi_flash_chip_generic_erase_chip = 0x2f800280;
spi_flash_chip_generic_erase_sector = 0x2f800284;
spi_flash_chip_generic_erase_block = 0x2f800288;
spi_flash_chip_generic_page_program = 0x2f80028c;
spi_flash_chip_generic_get_write_protect = 0x2f800290;
spi_flash_common_read_status_16b_rdsr_rdsr2 = 0x2f800294;
spi_flash_chip_generic_read_reg = 0x2f800298;
spi_flash_chip_generic_yield = 0x2f80029c;
spi_flash_generic_wait_host_idle = 0x2f8002a0;
spi_flash_chip_generic_wait_idle = 0x2f8002a4;
spi_flash_chip_generic_config_host_io_mode = 0x2f8002a8;
spi_flash_chip_generic_read = 0x2f8002ac;
spi_flash_common_read_status_8b_rdsr2 = 0x2f8002b0;
spi_flash_chip_generic_get_io_mode = 0x2f8002b4;
spi_flash_common_read_status_8b_rdsr = 0x2f8002b8;
spi_flash_common_write_status_8b_wrsr = 0x2f8002bc;
spi_flash_common_write_status_8b_wrsr2 = 0x2f8002c0;
spi_flash_common_set_io_mode = 0x2f8002c4;
spi_flash_chip_generic_set_io_mode = 0x2f8002c8;
spi_flash_chip_generic_read_unique_id = 0x2f8002cc;
spi_flash_chip_generic_get_caps = 0x2f8002d0;
spi_flash_chip_generic_suspend_cmd_conf = 0x2f8002d4;
spi_flash_chip_gd_get_io_mode = 0x2f8002d8;
spi_flash_chip_gd_probe = 0x2f8002dc;
spi_flash_chip_gd_set_io_mode = 0x2f8002e0;
/* Data (.data, .bss, .rodata) */
spi_flash_chip_generic_config_data = 0x2f07ffd4;
spi_flash_encryption = 0x2f07ffd0;


/***************************************
 Group memspi_host
 ***************************************/

/* Functions */
memspi_host_read_id_hs = 0x2f8002e4;
memspi_host_read_status_hs = 0x2f8002e8;
memspi_host_flush_cache = 0x2f8002ec;
memspi_host_erase_chip = 0x2f8002f0;
memspi_host_erase_sector = 0x2f8002f4;
memspi_host_erase_block = 0x2f8002f8;
memspi_host_program_page = 0x2f8002fc;
memspi_host_read = 0x2f800300;
memspi_host_set_write_protect = 0x2f800304;
memspi_host_set_max_read_len = 0x2f800308;
memspi_host_read_data_slicer = 0x2f80030c;
memspi_host_write_data_slicer = 0x2f800310;


/***************************************
 Group hal_spiflash
 ***************************************/

/* Functions */
spi_flash_hal_poll_cmd_done = 0x2f800314;
spi_flash_hal_device_config = 0x2f800318;
spi_flash_hal_configure_host_io_mode = 0x2f80031c;
spi_flash_hal_common_command = 0x2f800320;
spi_flash_hal_read = 0x2f800324;
spi_flash_hal_erase_chip = 0x2f800328;
spi_flash_hal_erase_sector = 0x2f80032c;
spi_flash_hal_erase_block = 0x2f800330;
spi_flash_hal_program_page = 0x2f800334;
spi_flash_hal_set_write_protect = 0x2f800338;
spi_flash_hal_host_idle = 0x2f80033c;
spi_flash_hal_check_status = 0x2f800340;
spi_flash_hal_setup_read_suspend = 0x2f800344;
spi_flash_hal_resume = 0x2f800348;
spi_flash_hal_suspend = 0x2f80034c;
spi_flash_encryption_hal_enable = 0x2f800350;
spi_flash_encryption_hal_disable = 0x2f800354;
spi_flash_encryption_hal_prepare = 0x2f800358;
spi_flash_encryption_hal_done = 0x2f80035c;
spi_flash_encryption_hal_destroy = 0x2f800360;
spi_flash_encryption_hal_check = 0x2f800364;
