/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface esp32b1z.rom.ld for esp32b1z
 *
 *
 * Generated from ./interface-esp32b1z.yml md5sum a8cce65aa1422e5781ad0d729ef0a0a6
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group common
 ***************************************/

/* Functions */
rtc_get_reset_reason = 0x40000018;
analog_super_wdt_reset_happened = 0x4000001c;
jtag_cpu_reset_happened = 0x40000020;
rtc_get_wakeup_cause = 0x40000024;
rtc_select_apb_bridge = 0x40000028;
rtc_unhold_all_pads = 0x4000002c;
ets_is_print_boot = 0x40000030;
ets_printf = 0x40000034;
ets_install_putc1 = 0x40000038;
ets_install_uart_printf = 0x4000003c;
ets_install_putc2 = 0x40000040;
PROVIDE( ets_delay_us = 0x40000044 );
ets_get_stack_info = 0x40000048;
ets_install_lock = 0x4000004c;
ets_backup_dma_copy = 0x40000050;
ets_apb_backup_init_lock_func = 0x40000054;
UartRxString = 0x40000058;
uart_tx_one_char = 0x4000005c;
uart_tx_one_char2 = 0x40000060;
uart_rx_one_char = 0x40000064;
uart_rx_one_char_block = 0x40000068;
uart_rx_readbuff = 0x4000006c;
uartAttach = 0x40000070;
uart_tx_flush = 0x40000074;
uart_tx_wait_idle = 0x40000078;
uart_div_modify = 0x4000007c;
multofup = 0x40000080;
software_reset = 0x40000084;
software_reset_cpu = 0x40000088;
assist_debug_clock_enable = 0x4000008c;
assist_debug_record_enable = 0x40000090;
clear_super_wdt_reset_flag = 0x40000094;
disable_default_watchdog = 0x40000098;
esp_rom_set_rtc_wake_addr = 0x4000009c;
esp_rom_get_rtc_wake_addr = 0x400000a0;
send_packet = 0x400000a4;
recv_packet = 0x400000a8;
GetUartDevice = 0x400000ac;
UartDwnLdProc = 0x400000b0;
Uart_Init = 0x400000b4;
ets_set_user_start = 0x400000b8;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x3ff1fffc;
ets_ops_table_ptr = 0x3fcdfffc;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x400000bc;
mz_crc32 = 0x400000c0;
mz_free = 0x400000c4;
tdefl_compress = 0x400000c8;
tdefl_compress_buffer = 0x400000cc;
tdefl_compress_mem_to_heap = 0x400000d0;
tdefl_compress_mem_to_mem = 0x400000d4;
tdefl_compress_mem_to_output = 0x400000d8;
tdefl_get_adler32 = 0x400000dc;
tdefl_get_prev_return_status = 0x400000e0;
tdefl_init = 0x400000e4;
tdefl_write_image_to_png_file_in_memory = 0x400000e8;
tdefl_write_image_to_png_file_in_memory_ex = 0x400000ec;
tinfl_decompress = 0x400000f0;
tinfl_decompress_mem_to_callback = 0x400000f4;
tinfl_decompress_mem_to_heap = 0x400000f8;
tinfl_decompress_mem_to_mem = 0x400000fc;


/***************************************
 Group tjpgd
 ***************************************/

/* Functions */
PROVIDE( jd_prepare = 0x40000100 );
PROVIDE( jd_decomp = 0x40000104 );


/***************************************
 Group esp-dsp
 ***************************************/

/* Data (.data, .bss, .rodata) */
dsps_fft2r_w_table_fc32_1024 = 0x3fcdfff8;


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
PROVIDE( esp_rom_spiflash_wait_idle = 0x40000108 );
PROVIDE( esp_rom_spiflash_write_encrypted = 0x4000010c );
PROVIDE( esp_rom_spiflash_write_encrypted_dest = 0x40000110 );
PROVIDE( esp_rom_spiflash_write_encrypted_enable = 0x40000114 );
PROVIDE( esp_rom_spiflash_write_encrypted_disable = 0x40000118 );
PROVIDE( esp_rom_spiflash_erase_chip = 0x4000011c );
PROVIDE( esp_rom_spiflash_erase_block = 0x40000120 );
PROVIDE( esp_rom_spiflash_erase_sector = 0x40000124 );
PROVIDE( esp_rom_spiflash_write = 0x40000128 );
PROVIDE( esp_rom_spiflash_read = 0x4000012c );
PROVIDE( esp_rom_spiflash_config_param = 0x40000130 );
PROVIDE( esp_rom_spiflash_read_user_cmd = 0x40000134 );
PROVIDE( esp_rom_spiflash_select_qio_pins = 0x40000138 );
PROVIDE( esp_rom_spiflash_unlock = 0x4000013c );
PROVIDE( esp_rom_spi_flash_auto_sus_res = 0x40000140 );
PROVIDE( esp_rom_spi_flash_send_resume = 0x40000144 );
PROVIDE( esp_rom_spi_flash_update_id = 0x40000148 );
PROVIDE( esp_rom_spiflash_config_clk = 0x4000014c );
PROVIDE( esp_rom_spiflash_config_readmode = 0x40000150 );
PROVIDE( esp_rom_spiflash_read_status = 0x40000154 );
PROVIDE( esp_rom_spiflash_read_statushigh = 0x40000158 );
PROVIDE( esp_rom_spiflash_write_status = 0x4000015c );
PROVIDE( spi_flash_attach = 0x40000160 );
PROVIDE( spi_flash_get_chip_size = 0x40000164 );
PROVIDE( spi_flash_guard_set = 0x40000168 );
PROVIDE( spi_flash_guard_get = 0x4000016c );
PROVIDE( spi_flash_write_config_set = 0x40000170 );
PROVIDE( spi_flash_write_config_get = 0x40000174 );
PROVIDE( spi_flash_safe_write_address_func_set = 0x40000178 );
PROVIDE( spi_flash_unlock = 0x4000017c );
PROVIDE( spi_flash_erase_range = 0x40000180 );
PROVIDE( spi_flash_erase_sector = 0x40000184 );
PROVIDE( spi_flash_write = 0x40000188 );
PROVIDE( spi_flash_read = 0x4000018c );
PROVIDE( spi_flash_write_encrypted = 0x40000190 );
PROVIDE( spi_flash_read_encrypted = 0x40000194 );
PROVIDE( spi_flash_mmap_os_func_set = 0x40000198 );
PROVIDE( spi_flash_mmap_page_num_init = 0x4000019c );
PROVIDE( spi_flash_mmap = 0x400001a0 );
PROVIDE( spi_flash_mmap_pages = 0x400001a4 );
PROVIDE( spi_flash_munmap = 0x400001a8 );
PROVIDE( spi_flash_mmap_dump = 0x400001ac );
PROVIDE( spi_flash_check_and_flush_cache = 0x400001b0 );
PROVIDE( spi_flash_mmap_get_free_pages = 0x400001b4 );
PROVIDE( spi_flash_cache2phys = 0x400001b8 );
PROVIDE( spi_flash_phys2cache = 0x400001bc );
PROVIDE( spi_flash_disable_cache = 0x400001c0 );
PROVIDE( spi_flash_restore_cache = 0x400001c4 );
PROVIDE( spi_flash_cache_enabled = 0x400001c8 );
PROVIDE( spi_flash_enable_cache = 0x400001cc );
PROVIDE( spi_cache_mode_switch = 0x400001d0 );
PROVIDE( spi_common_set_dummy_output = 0x400001d4 );
PROVIDE( spi_common_set_flash_cs_timing = 0x400001d8 );
PROVIDE( esp_enable_cache_flash_wrap = 0x400001dc );
PROVIDE( SPIEraseArea = 0x400001e0 );
PROVIDE( SPILock = 0x400001e4 );
PROVIDE( SPIMasterReadModeCnfig = 0x400001e8 );
PROVIDE( SPI_Common_Command = 0x400001ec );
PROVIDE( SPI_WakeUp = 0x400001f0 );
PROVIDE( SPI_block_erase = 0x400001f4 );
PROVIDE( SPI_chip_erase = 0x400001f8 );
PROVIDE( SPI_init = 0x400001fc );
PROVIDE( SPI_page_program = 0x40000200 );
PROVIDE( SPI_read_data = 0x40000204 );
PROVIDE( SPI_sector_erase = 0x40000208 );
PROVIDE( SPI_write_enable = 0x4000020c );
PROVIDE( SelectSpiFunction = 0x40000210 );
PROVIDE( SetSpiDrvs = 0x40000214 );
PROVIDE( Wait_SPI_Idle = 0x40000218 );
PROVIDE( spi_dummy_len_fix = 0x4000021c );
PROVIDE( Disable_QMode = 0x40000220 );
PROVIDE( Enable_QMode = 0x40000224 );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_spiflash_legacy_funcs = 0x3fcdfff0 );
PROVIDE( rom_spiflash_legacy_data = 0x3fcdffec );
PROVIDE( g_flash_guard_ops = 0x3fcdfff4 );


/***************************************
 Group hal_soc
 ***************************************/

/* Functions */
PROVIDE( spi_flash_hal_poll_cmd_done = 0x40000228 );
PROVIDE( spi_flash_hal_device_config = 0x4000022c );
PROVIDE( spi_flash_hal_configure_host_io_mode = 0x40000230 );
PROVIDE( spi_flash_hal_common_command = 0x40000234 );
PROVIDE( spi_flash_hal_read = 0x40000238 );
PROVIDE( spi_flash_hal_erase_chip = 0x4000023c );
PROVIDE( spi_flash_hal_erase_sector = 0x40000240 );
PROVIDE( spi_flash_hal_erase_block = 0x40000244 );
PROVIDE( spi_flash_hal_program_page = 0x40000248 );
PROVIDE( spi_flash_hal_set_write_protect = 0x4000024c );
PROVIDE( spi_flash_hal_host_idle = 0x40000250 );


/***************************************
 Group spi_flash_chips
 ***************************************/

/* Functions */
PROVIDE( spi_flash_chip_generic_probe = 0x40000254 );
PROVIDE( spi_flash_chip_generic_detect_size = 0x40000258 );
PROVIDE( spi_flash_chip_generic_write = 0x4000025c );
PROVIDE( spi_flash_chip_generic_write_encrypted = 0x40000260 );
PROVIDE( spi_flash_chip_generic_set_write_protect = 0x40000264 );
PROVIDE( spi_flash_common_write_status_16b_wrsr = 0x40000268 );
PROVIDE( spi_flash_chip_generic_reset = 0x4000026c );
PROVIDE( spi_flash_chip_generic_erase_chip = 0x40000270 );
PROVIDE( spi_flash_chip_generic_erase_sector = 0x40000274 );
PROVIDE( spi_flash_chip_generic_erase_block = 0x40000278 );
PROVIDE( spi_flash_chip_generic_page_program = 0x4000027c );
PROVIDE( spi_flash_chip_generic_get_write_protect = 0x40000280 );
PROVIDE( spi_flash_common_read_status_16b_rdsr_rdsr2 = 0x40000284 );
PROVIDE( spi_flash_chip_generic_read_reg = 0x40000288 );
PROVIDE( spi_flash_chip_generic_yield = 0x4000028c );
PROVIDE( spi_flash_generic_wait_host_idle = 0x40000290 );
PROVIDE( spi_flash_chip_generic_wait_idle = 0x40000294 );
PROVIDE( spi_flash_chip_generic_config_host_io_mode = 0x40000298 );
PROVIDE( spi_flash_chip_generic_read = 0x4000029c );
PROVIDE( spi_flash_common_read_status_8b_rdsr2 = 0x400002a0 );
PROVIDE( spi_flash_chip_generic_get_io_mode = 0x400002a4 );
PROVIDE( spi_flash_common_read_status_8b_rdsr = 0x400002a8 );
PROVIDE( spi_flash_common_write_status_8b_wrsr = 0x400002ac );
PROVIDE( spi_flash_common_write_status_8b_wrsr2 = 0x400002b0 );
PROVIDE( spi_flash_common_set_io_mode = 0x400002b4 );
PROVIDE( spi_flash_chip_generic_set_io_mode = 0x400002b8 );
PROVIDE( spi_flash_chip_gd_get_io_mode = 0x400002bc );
PROVIDE( spi_flash_chip_gd_probe = 0x400002c0 );
PROVIDE( spi_flash_chip_gd_set_io_mode = 0x400002c4 );
/* Data (.data, .bss, .rodata) */
PROVIDE( spi_flash_chip_generic_config_data = 0x3fcdffe8 );


/***************************************
 Group memspi_host
 ***************************************/

/* Functions */
PROVIDE( memspi_host_read_id_hs = 0x400002c8 );
PROVIDE( memspi_host_read_status_hs = 0x400002cc );
PROVIDE( memspi_host_flush_cache = 0x400002d0 );
PROVIDE( memspi_host_erase_chip = 0x400002d4 );
PROVIDE( memspi_host_erase_sector = 0x400002d8 );
PROVIDE( memspi_host_erase_block = 0x400002dc );
PROVIDE( memspi_host_program_page = 0x400002e0 );
PROVIDE( memspi_host_read = 0x400002e4 );
PROVIDE( memspi_host_set_write_protect = 0x400002e8 );
PROVIDE( memspi_host_set_max_read_len = 0x400002ec );
PROVIDE( memspi_host_read_data_slicer = 0x400002f0 );
PROVIDE( memspi_host_write_data_slicer = 0x400002f4 );


/***************************************
 Group esp_flash
 ***************************************/

/* Functions */
PROVIDE( esp_flash_chip_driver_initialized = 0x400002f8 );
PROVIDE( esp_flash_read_id = 0x400002fc );
PROVIDE( esp_flash_get_size = 0x40000300 );
PROVIDE( esp_flash_erase_chip = 0x40000304 );
PROVIDE( rom_esp_flash_erase_region = 0x40000308 );
PROVIDE( esp_flash_get_chip_write_protect = 0x4000030c );
PROVIDE( esp_flash_set_chip_write_protect = 0x40000310 );
PROVIDE( esp_flash_get_protectable_regions = 0x40000314 );
PROVIDE( esp_flash_get_protected_region = 0x40000318 );
PROVIDE( esp_flash_set_protected_region = 0x4000031c );
PROVIDE( esp_flash_read = 0x40000320 );
PROVIDE( esp_flash_write = 0x40000324 );
PROVIDE( esp_flash_write_encrypted = 0x40000328 );
PROVIDE( esp_flash_read_encrypted = 0x4000032c );
PROVIDE( esp_flash_get_io_mode = 0x40000330 );
PROVIDE( esp_flash_set_io_mode = 0x40000334 );
PROVIDE( spi_flash_boot_attach = 0x40000338 );
PROVIDE( spi_flash_dump_counters = 0x4000033c );
PROVIDE( spi_flash_get_counters = 0x40000340 );
PROVIDE( spi_flash_op_counters_config = 0x40000344 );
PROVIDE( spi_flash_reset_counters = 0x40000348 );
PROVIDE( esp_flash_read_chip_id = 0x4000034c );
PROVIDE( detect_spi_flash_chip = 0x40000350 );
PROVIDE( esp_rom_spiflash_write_disable = 0x40000354 );
/* Data (.data, .bss, .rodata) */
PROVIDE( esp_flash_default_chip = 0x3fcdffe4 );
PROVIDE( esp_flash_api_funcs = 0x3fcdffe0 );


/***************************************
 Group cache
 ***************************************/

/* Functions */
PROVIDE( Cache_Get_ICache_Line_Size = 0x400004b8 );
PROVIDE( Cache_Get_Mode = 0x400004bc );
PROVIDE( Cache_Address_Through_IBus = 0x400004c0 );
PROVIDE( Cache_Address_Through_DBus = 0x400004c4 );
PROVIDE( Cache_Set_Default_Mode = 0x400004c8 );
PROVIDE( Cache_Enable_Defalut_ICache_Mode = 0x400004cc );
PROVIDE( ROM_Boot_Cache_Init = 0x400004d0 );
PROVIDE( Cache_Invalidate_ICache_Items = 0x400004d4 );
PROVIDE( Cache_Op_Addr = 0x400004d8 );
PROVIDE( Cache_Invalidate_Addr = 0x400004dc );
PROVIDE( Cache_Invalidate_ICache_All = 0x400004e0 );
PROVIDE( Cache_Mask_All = 0x400004e4 );
PROVIDE( Cache_UnMask_Dram0 = 0x400004e8 );
PROVIDE( Cache_Suspend_ICache_Autoload = 0x400004ec );
PROVIDE( Cache_Resume_ICache_Autoload = 0x400004f0 );
PROVIDE( Cache_Start_ICache_Preload = 0x400004f4 );
PROVIDE( Cache_ICache_Preload_Done = 0x400004f8 );
PROVIDE( Cache_End_ICache_Preload = 0x400004fc );
PROVIDE( Cache_Config_ICache_Autoload = 0x40000500 );
PROVIDE( Cache_Enable_ICache_Autoload = 0x40000504 );
PROVIDE( Cache_Disable_ICache_Autoload = 0x40000508 );
PROVIDE( Cache_Enable_ICache_PreLock = 0x4000050c );
PROVIDE( Cache_Disable_ICache_PreLock = 0x40000510 );
PROVIDE( Cache_Lock_ICache_Items = 0x40000514 );
PROVIDE( Cache_Unlock_ICache_Items = 0x40000518 );
PROVIDE( Cache_Lock_Addr = 0x4000051c );
PROVIDE( Cache_Unlock_Addr = 0x40000520 );
PROVIDE( Cache_Disable_ICache = 0x40000524 );
PROVIDE( Cache_Enable_ICache = 0x40000528 );
PROVIDE( Cache_Suspend_ICache = 0x4000052c );
PROVIDE( Cache_Resume_ICache = 0x40000530 );
PROVIDE( Cache_Freeze_ICache_Enable = 0x40000534 );
PROVIDE( Cache_Freeze_ICache_Disable = 0x40000538 );
PROVIDE( Cache_Pms_Lock = 0x4000053c );
PROVIDE( Cache_Ibus_Pms_Set_Addr = 0x40000540 );
PROVIDE( Cache_Ibus_Pms_Set_Attr = 0x40000544 );
PROVIDE( Cache_Dbus_Pms_Set_Addr = 0x40000548 );
PROVIDE( Cache_Dbus_Pms_Set_Attr = 0x4000054c );
PROVIDE( Cache_Set_IDROM_MMU_Size = 0x40000550 );
PROVIDE( Cache_Get_IROM_MMU_End = 0x40000554 );
PROVIDE( Cache_Get_DROM_MMU_End = 0x40000558 );
PROVIDE( Cache_Owner_Init = 0x4000055c );
PROVIDE( Cache_Occupy_ICache_MEMORY = 0x40000560 );
PROVIDE( Cache_MMU_Init = 0x40000564 );
PROVIDE( Cache_Ibus_MMU_Set = 0x40000568 );
PROVIDE( Cache_Dbus_MMU_Set = 0x4000056c );
PROVIDE( Cache_Count_Flash_Pages = 0x40000570 );
PROVIDE( Cache_Travel_Tag_Memory = 0x40000574 );
PROVIDE( Cache_Get_Virtual_Addr = 0x40000578 );
PROVIDE( Cache_Get_Memory_BaseAddr = 0x4000057c );
PROVIDE( Cache_Get_Memory_Addr = 0x40000580 );
PROVIDE( Cache_Get_Memory_value = 0x40000584 );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_cache_op_cb = 0x3fcdffd4 );
PROVIDE( rom_cache_internal_table_ptr = 0x3fcdffd0 );


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_get_apb_freq = 0x40000588;
ets_get_cpu_frequency = 0x4000058c;
ets_update_cpu_frequency = 0x40000590;
ets_get_printf_channel = 0x40000594;
ets_get_xtal_div = 0x40000598;
ets_set_xtal_div = 0x4000059c;
ets_get_xtal_freq = 0x400005a0;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_input_get = 0x400005a4;
gpio_matrix_in = 0x400005a8;
gpio_matrix_out = 0x400005ac;
gpio_output_disable = 0x400005b0;
gpio_output_enable = 0x400005b4;
gpio_output_set = 0x400005b8;
gpio_pad_hold = 0x400005bc;
gpio_pad_input_disable = 0x400005c0;
gpio_pad_input_enable = 0x400005c4;
gpio_pad_pulldown = 0x400005c8;
gpio_pad_pullup = 0x400005cc;
gpio_pad_select_gpio = 0x400005d0;
gpio_pad_set_drv = 0x400005d4;
gpio_pad_unhold = 0x400005d8;
gpio_pin_wakeup_disable = 0x400005dc;
gpio_pin_wakeup_enable = 0x400005e0;
gpio_bypass_matrix_in = 0x400005e4;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x400005e8;
esprv_intc_int_set_threshold = 0x400005ec;
esprv_intc_int_enable = 0x400005f0;
esprv_intc_int_disable = 0x400005f4;
esprv_intc_int_set_type = 0x400005f8;
intr_matrix_set = 0x400005fc;
ets_intr_lock = 0x40000600;
ets_intr_unlock = 0x40000604;
PROVIDE( intr_handler_set = 0x40000608 );
ets_isr_attach = 0x4000060c;
ets_isr_mask = 0x40000610;
ets_isr_unmask = 0x40000614;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
md5_vector = 0x40000618;
MD5Init = 0x4000061c;
MD5Update = 0x40000620;
MD5Final = 0x40000624;
hmac_md5_vector = 0x40000628;
hmac_md5 = 0x4000062c;
crc32_le = 0x40000630;
crc32_be = 0x40000634;
crc16_le = 0x40000638;
crc16_be = 0x4000063c;
crc8_le = 0x40000640;
crc8_be = 0x40000644;
esp_crc8 = 0x40000648;
ets_sha_enable = 0x4000064c;
ets_sha_disable = 0x40000650;
ets_sha_get_state = 0x40000654;
ets_sha_init = 0x40000658;
ets_sha_process = 0x4000065c;
ets_sha_starts = 0x40000660;
ets_sha_update = 0x40000664;
ets_sha_finish = 0x40000668;
ets_sha_clone = 0x4000066c;
ets_hmac_enable = 0x40000670;
ets_hmac_disable = 0x40000674;
ets_hmac_calculate_message = 0x40000678;
ets_hmac_calculate_downstream = 0x4000067c;
ets_hmac_invalidate_downstream = 0x40000680;
ets_jtag_enable_temporarily = 0x40000684;
ets_aes_enable = 0x40000688;
ets_aes_disable = 0x4000068c;
ets_aes_setkey = 0x40000690;
ets_aes_block = 0x40000694;
ets_bigint_enable = 0x40000698;
ets_bigint_disable = 0x4000069c;
ets_bigint_multiply = 0x400006a0;
ets_bigint_modmult = 0x400006a4;
ets_bigint_modexp = 0x400006a8;
ets_bigint_wait_finish = 0x400006ac;
ets_bigint_getz = 0x400006b0;
ets_ds_enable = 0x400006b4;
ets_ds_disable = 0x400006b8;
ets_ds_start_sign = 0x400006bc;
ets_ds_is_busy = 0x400006c0;
ets_ds_finish_sign = 0x400006c4;
ets_ds_encrypt_params = 0x400006c8;
ets_aes_setkey_dec = 0x400006cc;
ets_aes_setkey_enc = 0x400006d0;
ets_mgf1_sha256 = 0x400006d4;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x400006d8;
ets_efuse_program = 0x400006dc;
ets_efuse_clear_program_registers = 0x400006e0;
ets_efuse_write_key = 0x400006e4;
ets_efuse_get_read_register_address = 0x400006e8;
ets_efuse_get_key_purpose = 0x400006ec;
ets_efuse_key_block_unused = 0x400006f0;
ets_efuse_find_unused_key_block = 0x400006f4;
ets_efuse_rs_calculate = 0x400006f8;
ets_efuse_count_unused_key_blocks = 0x400006fc;
ets_efuse_secure_boot_enabled = 0x40000700;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x40000704;
ets_efuse_cache_encryption_enabled = 0x40000708;
ets_efuse_download_modes_disabled = 0x4000070c;
ets_efuse_find_purpose = 0x40000710;
ets_efuse_flash_opi_5pads_power_sel_vddspi = 0x40000714;
ets_efuse_force_send_resume = 0x40000718;
ets_efuse_get_flash_delay_us = 0x4000071c;
ets_efuse_get_mac = 0x40000720;
ets_efuse_get_spiconfig = 0x40000724;
ets_efuse_usb_print_is_disabled = 0x40000728;
/*ets_efuse_get_uart_print_channel = 0x4000072c;*/
ets_efuse_usb_serial_jtag_print_is_disabled = 0x4000072c;
ets_efuse_get_uart_print_control = 0x40000730;
ets_efuse_get_wp_pad = 0x40000734;
ets_efuse_direct_boot_mode_disabled = 0x40000738;
ets_efuse_security_download_modes_enabled = 0x4000073c;
ets_efuse_set_timing = 0x40000740;
ets_efuse_jtag_disabled = 0x40000744;
ets_efuse_usb_download_mode_disabled = 0x40000748;
ets_efuse_usb_module_disabled = 0x4000074c;
ets_efuse_usb_device_disabled = 0x40000750;
ets_efuse_secure_boot_fast_wake_enabled = 0x40000754;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x40000758;
ets_rsa_pss_verify = 0x4000075c;
ets_secure_boot_verify_bootloader_with_keys = 0x40000760;
ets_secure_boot_verify_signature = 0x40000764;
ets_secure_boot_read_key_digests = 0x40000768;
ets_secure_boot_revoke_public_key_digest = 0x4000076c;


/***************************************
 Group usb_uart
 ***************************************/

/* Functions */
PROVIDE( usb_uart_device_rx_one_char = 0x400008d8 );
PROVIDE( usb_uart_device_rx_one_char_block = 0x400008dc );
PROVIDE( usb_uart_device_tx_flush = 0x400008e0 );
PROVIDE( usb_uart_device_tx_one_char = 0x400008e4 );
/* Data (.data, .bss, .rodata) */
PROVIDE( g_uart_print = 0x3fcdffcd );
PROVIDE( g_usb_print = 0x3fcdffcc );


/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
phy_get_romfuncs = 0x400008e8;
rom_abs_temp = 0x400008ec;
rom_bb_bss_cbw40_dig = 0x400008f0;
rom_bb_wdg_test_en = 0x400008f4;
rom_bb_wdt_get_status = 0x400008f8;
rom_bb_wdt_int_enable = 0x400008fc;
rom_bb_wdt_rst_enable = 0x40000900;
rom_bb_wdt_timeout_clear = 0x40000904;
rom_cbw2040_cfg = 0x40000908;
rom_check_noise_floor = 0x4000090c;
rom_chip_i2c_readReg = 0x40000910;
rom_chip_i2c_writeReg = 0x40000914;
rom_correct_rf_ana_gain = 0x40000918;
rom_dc_iq_est = 0x4000091c;
rom_disable_agc = 0x40000920;
rom_en_pwdet = 0x40000924;
rom_enable_agc = 0x40000928;
rom_get_bbgain_db = 0x4000092c;
rom_get_data_sat = 0x40000930;
rom_get_i2c_read_mask = 0x40000934;
rom_get_pwctrl_correct = 0x40000938;
rom_get_rf_gain_qdb = 0x4000093c;
rom_i2c_readReg = 0x40000940;
rom_i2c_readReg_Mask = 0x40000944;
rom_i2c_writeReg = 0x40000948;
rom_i2c_writeReg_Mask = 0x4000094c;
rom_index_to_txbbgain = 0x40000950;
rom_iq_est_disable = 0x40000954;
rom_iq_est_enable = 0x40000958;
rom_linear_to_db = 0x4000095c;
rom_loopback_mode_en = 0x40000960;
rom_mhz2ieee = 0x40000964;
rom_noise_floor_auto_set = 0x40000968;
rom_pbus_debugmode = 0x4000096c;
rom_pbus_force_mode = 0x40000970;
rom_pbus_force_test = 0x40000974;
rom_pbus_rd = 0x40000978;
rom_pbus_rd_addr = 0x4000097c;
rom_pbus_rd_shift = 0x40000980;
rom_pbus_set_dco = 0x40000984;
rom_pbus_set_rxgain = 0x40000988;
rom_pbus_workmode = 0x4000098c;
rom_pbus_xpd_rx_off = 0x40000990;
rom_pbus_xpd_rx_on = 0x40000994;
rom_pbus_xpd_tx_off = 0x40000998;
rom_pbus_xpd_tx_on = 0x4000099c;
rom_phy_byte_to_word = 0x400009a0;
rom_phy_disable_cca = 0x400009a4;
rom_phy_enable_cca = 0x400009a8;
rom_phy_get_noisefloor = 0x400009ac;
rom_phy_get_rx_freq = 0x400009b0;
rom_phy_set_bbfreq_init = 0x400009b4;
rom_pow_usr = 0x400009b8;
rom_pwdet_sar2_init = 0x400009bc;
rom_read_hw_noisefloor = 0x400009c0;
rom_read_sar_dout = 0x400009c4;
rom_set_cal_rxdc = 0x400009c8;
rom_set_chan_cal_interp = 0x400009cc;
rom_set_loopback_gain = 0x400009d0;
rom_set_noise_floor = 0x400009d4;
rom_set_rxclk_en = 0x400009d8;
rom_set_tx_dig_gain = 0x400009dc;
rom_set_txcap_reg = 0x400009e0;
rom_set_txclk_en = 0x400009e4;
rom_spur_cal = 0x400009e8;
rom_spur_reg_write_one_tone = 0x400009ec;
rom_target_power_add_backoff = 0x400009f0;
rom_tx_pwctrl_bg_init = 0x400009f4;
rom_txbbgain_to_index = 0x400009f8;
rom_wifi_11g_rate_chg = 0x400009fc;
rom_write_gain_mem = 0x40000a00;
chip726_phyrom_version = 0x40000a04;
rom_disable_wifi_agc = 0x40000a08;
rom_enable_wifi_agc = 0x40000a0c;
rom_set_tx_gain_table = 0x40000a10;
rom_bt_index_to_bb = 0x40000a14;
rom_bt_bb_to_index = 0x40000a18;
rom_wr_bt_tx_atten = 0x40000a1c;
rom_wr_bt_tx_gain_mem = 0x40000a20;
rom_spur_coef_cfg = 0x40000a24;
rom_bb_bss_cbw40 = 0x40000a28;
rom_set_cca = 0x40000a2c;
rom_tx_paon_set = 0x40000a30;
rom_i2cmst_reg_init = 0x40000a34;
rom_iq_corr_enable = 0x40000a38;
rom_fe_reg_init = 0x40000a3c;
rom_agc_reg_init = 0x40000a40;
rom_bb_reg_init = 0x40000a44;
rom_mac_enable_bb = 0x40000a48;
rom_bb_wdg_cfg = 0x40000a4c;
rom_force_txon = 0x40000a50;
rom_fe_txrx_reset = 0x40000a54;
rom_set_rx_comp = 0x40000a58;
rom_set_pbus_reg = 0x40000a5c;
rom_write_chan_freq = 0x40000a60;
rom_phy_xpd_rf = 0x40000a64;
rom_set_xpd_sar = 0x40000a68;
rom_write_dac_gain2 = 0x40000a6c;
rom_rtc_sar2_init = 0x40000a70;
rom_get_target_power_offset = 0x40000a74;
rom_write_txrate_power_offset = 0x40000a78;
rom_get_rate_fcc_index = 0x40000a7c;
rom_get_rate_target_power = 0x40000a80;
rom_write_wifi_dig_gain = 0x40000a84;
rom_bt_correct_rf_ana_gain = 0x40000a88;
rom_pkdet_vol_start = 0x40000a8c;
rom_read_sar2_code = 0x40000a90;
rom_get_sar2_vol = 0x40000a94;
rom_get_pll_vol = 0x40000a98;
rom_get_phy_target_power = 0x40000a9c;
rom_temp_to_power = 0x40000aa0;
rom_phy_track_pll_cap = 0x40000aa4;
rom_phy_pwdet_always_en = 0x40000aa8;
rom_phy_pwdet_onetime_en = 0x40000aac;
rom_get_i2c_mst0_mask = 0x40000ab0;
rom_get_i2c_hostid = 0x40000ab4;
rom_enter_critical_phy = 0x40000ab8;
rom_exit_critical_phy = 0x40000abc;
rom_chip_i2c_readReg_org = 0x40000ac0;
rom_i2c_paral_set_mst0 = 0x40000ac4;
rom_i2c_paral_set_read = 0x40000ac8;
rom_i2c_paral_read = 0x40000acc;
rom_i2c_paral_write = 0x40000ad0;
rom_i2c_paral_write_num = 0x40000ad4;
rom_i2c_paral_write_mask = 0x40000ad8;
rom_bb_bss_cbw40_ana = 0x40000adc;
rom_chan_to_freq = 0x40000ae0;
rom_open_i2c_xpd = 0x40000ae4;
rom_dac_rate_set = 0x40000ae8;
rom_tsens_read_init = 0x40000aec;
rom_tsens_code_read = 0x40000af0;
rom_tsens_index_to_dac = 0x40000af4;
rom_tsens_index_to_offset = 0x40000af8;
rom_tsens_dac_cal = 0x40000afc;
rom_code_to_temp = 0x40000b00;
rom_write_pll_cap_mem = 0x40000b04;
rom_pll_correct_dcap = 0x40000b08;
rom_phy_en_hw_set_freq = 0x40000b0c;
rom_phy_dis_hw_set_freq = 0x40000b10;
rom_pll_vol_cal = 0x40000b14;
rom_wrtie_pll_cap = 0x40000b18;
rom_set_tx_gain_mem = 0x40000b1c;
rom_bt_tx_dig_gain = 0x40000b20;
rom_bt_get_tx_gain = 0x40000b24;
rom_get_chan_target_power = 0x40000b28;
rom_get_tx_gain_value = 0x40000b2c;
rom_wifi_tx_dig_gain = 0x40000b30;
rom_wifi_get_tx_gain = 0x40000b34;
rom_fe_i2c_reg_renew = 0x40000b38;
rom_wifi_agc_sat_gain = 0x40000b3c;
rom_i2c_master_reset = 0x40000b40;
rom_bt_filter_reg = 0x40000b44;
rom_phy_bbpll_cal = 0x40000b48;
rom_i2c_sar2_init_code = 0x40000b4c;
rom_phy_param_addr = 0x40000b50;
rom_phy_reg_init = 0x40000b54;
rom_set_chan_reg = 0x40000b58;
rom_phy_wakeup_init = 0x40000b5c;
rom_phy_i2c_init1 = 0x40000b60;
rom_tsens_temp_read = 0x40000b64;
rom_bt_track_pll_cap = 0x40000b68;
rom_wifi_track_pll_cap = 0x40000b6c;
rom_wifi_set_tx_gain = 0x40000b70;
rom_txpwr_cal_track = 0x40000b74;
rom_tx_pwctrl_background = 0x40000b78;
rom_bt_set_tx_gain = 0x40000b7c;
rom_noise_check_loop = 0x40000b80;
rom_phy_close_rf = 0x40000b84;
rom_phy_xpd_tsens = 0x40000b88;
rom_phy_freq_mem_backup = 0x40000b8c;
rom_phy_ant_init = 0x40000b90;
rom_bt_track_tx_power = 0x40000b94;
rom_wifi_track_tx_power = 0x40000b98;
rom_phy_dig_reg_backup = 0x40000b9c;
chip726_phyrom_version_num = 0x40000ba0;
/* Data (.data, .bss, .rodata) */
phy_param_rom = 0x3fcdffc8;
