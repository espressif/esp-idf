/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.spiflash.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 2476337377df636dda217b0b3c1a63db
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group spi_flash_cache
 ***************************************/

/* Functions */
spi_flash_disable_cache = 0x400001f0;
spi_flash_restore_cache = 0x400001f4;
spi_flash_cache_enabled = 0x400001f8;
spi_flash_enable_cache = 0x400001fc;
esp_enable_cache_flash_wrap = 0x40000200;


/***************************************
 Group spi_flash_mmap
 ***************************************/

/* Functions */
spi_flash_mmap_os_func_set = 0x40000204;
spi_flash_mmap_page_num_init = 0x40000208;
spi_flash_mmap = 0x4000020c;
spi_flash_mmap_pages = 0x40000210;
spi_flash_munmap = 0x40000214;
spi_flash_mmap_dump = 0x40000218;
spi_flash_check_and_flush_cache = 0x4000021c;
spi_flash_mmap_get_free_pages = 0x40000220;
spi_flash_cache2phys = 0x40000224;
spi_flash_phys2cache = 0x40000228;


/***************************************
 Group esp_flash
 ***************************************/

/* Functions */
esp_flash_chip_driver_initialized = 0x4000022c;
esp_flash_read_id = 0x40000230;
esp_flash_get_size = 0x40000234;
esp_flash_erase_chip = 0x40000238;
esp_flash_erase_region = 0x4000023c;
esp_flash_get_chip_write_protect = 0x40000240;
esp_flash_set_chip_write_protect = 0x40000244;
esp_flash_get_protectable_regions = 0x40000248;
esp_flash_get_protected_region = 0x4000024c;
esp_flash_set_protected_region = 0x40000250;
esp_flash_read = 0x40000254;
esp_flash_write = 0x40000258;
esp_flash_write_encrypted = 0x4000025c;
esp_flash_read_encrypted = 0x40000260;
esp_flash_get_io_mode = 0x40000264;
esp_flash_set_io_mode = 0x40000268;
spi_flash_boot_attach = 0x4000026c;
esp_flash_read_chip_id = 0x40000270;
detect_spi_flash_chip = 0x40000274;
esp_flash_suspend_cmd_init = 0x40000278;
/* Data (.data, .bss, .rodata) */
esp_flash_default_chip = 0x4087ffe8;
esp_flash_api_funcs = 0x4087ffe4;


/***************************************
 Group spi_flash_chips
 ***************************************/

/* Functions */
spi_flash_chip_generic_probe = 0x4000027c;
spi_flash_chip_generic_detect_size = 0x40000280;
spi_flash_chip_generic_write = 0x40000284;
spi_flash_chip_generic_write_encrypted = 0x40000288;
spi_flash_chip_generic_set_write_protect = 0x4000028c;
spi_flash_common_write_status_16b_wrsr = 0x40000290;
spi_flash_chip_generic_reset = 0x40000294;
spi_flash_chip_generic_erase_chip = 0x40000298;
spi_flash_chip_generic_erase_sector = 0x4000029c;
spi_flash_chip_generic_erase_block = 0x400002a0;
spi_flash_chip_generic_page_program = 0x400002a4;
spi_flash_chip_generic_get_write_protect = 0x400002a8;
spi_flash_common_read_status_16b_rdsr_rdsr2 = 0x400002ac;
spi_flash_chip_generic_read_reg = 0x400002b0;
spi_flash_chip_generic_yield = 0x400002b4;
spi_flash_generic_wait_host_idle = 0x400002b8;
spi_flash_chip_generic_wait_idle = 0x400002bc;
spi_flash_chip_generic_config_host_io_mode = 0x400002c0;
spi_flash_chip_generic_read = 0x400002c4;
spi_flash_common_read_status_8b_rdsr2 = 0x400002c8;
spi_flash_chip_generic_get_io_mode = 0x400002cc;
spi_flash_common_read_status_8b_rdsr = 0x400002d0;
spi_flash_common_write_status_8b_wrsr = 0x400002d4;
spi_flash_common_write_status_8b_wrsr2 = 0x400002d8;
spi_flash_common_set_io_mode = 0x400002dc;
spi_flash_chip_generic_set_io_mode = 0x400002e0;
spi_flash_chip_generic_read_unique_id = 0x400002e4;
spi_flash_chip_generic_get_caps = 0x400002e8;
spi_flash_chip_generic_suspend_cmd_conf = 0x400002ec;
spi_flash_chip_gd_get_io_mode = 0x400002f0;
spi_flash_chip_gd_probe = 0x400002f4;
spi_flash_chip_gd_set_io_mode = 0x400002f8;
/* Data (.data, .bss, .rodata) */
spi_flash_chip_generic_config_data = 0x4087ffe0;
spi_flash_encryption = 0x4087ffdc;


/***************************************
 Group memspi_host
 ***************************************/

/* Functions */
memspi_host_read_id_hs = 0x400002fc;
memspi_host_read_status_hs = 0x40000300;
memspi_host_flush_cache = 0x40000304;
memspi_host_erase_chip = 0x40000308;
memspi_host_erase_sector = 0x4000030c;
memspi_host_erase_block = 0x40000310;
memspi_host_program_page = 0x40000314;
memspi_host_read = 0x40000318;
memspi_host_set_write_protect = 0x4000031c;
memspi_host_set_max_read_len = 0x40000320;
memspi_host_read_data_slicer = 0x40000324;
memspi_host_write_data_slicer = 0x40000328;


/***************************************
 Group hal_spiflash
 ***************************************/

/* Functions */
spi_flash_hal_poll_cmd_done = 0x4000032c;
spi_flash_hal_device_config = 0x40000330;
spi_flash_hal_configure_host_io_mode = 0x40000334;
spi_flash_hal_common_command = 0x40000338;
spi_flash_hal_read = 0x4000033c;
spi_flash_hal_erase_chip = 0x40000340;
spi_flash_hal_erase_sector = 0x40000344;
spi_flash_hal_erase_block = 0x40000348;
spi_flash_hal_program_page = 0x4000034c;
spi_flash_hal_set_write_protect = 0x40000350;
spi_flash_hal_host_idle = 0x40000354;
spi_flash_hal_check_status = 0x40000358;
spi_flash_hal_setup_read_suspend = 0x4000035c;
spi_flash_hal_setup_auto_suspend_mode = 0x40000360;
spi_flash_hal_setup_auto_resume_mode = 0x40000364;
spi_flash_hal_disable_auto_suspend_mode = 0x40000368;
spi_flash_hal_disable_auto_resume_mode = 0x4000036c;
spi_flash_hal_resume = 0x40000370;
spi_flash_hal_suspend = 0x40000374;
spi_flash_encryption_hal_enable = 0x40000378;
spi_flash_encryption_hal_disable = 0x4000037c;
spi_flash_encryption_hal_prepare = 0x40000380;
spi_flash_encryption_hal_done = 0x40000384;
spi_flash_encryption_hal_destroy = 0x40000388;
spi_flash_encryption_hal_check = 0x4000038c;
