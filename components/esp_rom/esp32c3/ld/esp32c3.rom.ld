/* ROM function interface esp32c3.rom.ld for esp32c3
 *
 *
 * Generated from ./interface-esp32c3.yml md5sum 93b28a9e1fe42d212018eb4336849208
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
rtc_boot_control = 0x40000028;
rtc_select_apb_bridge = 0x4000002c;
rtc_unhold_all_pads = 0x40000030;
set_rtc_memory_crc = 0x40000034;
cacl_rtc_memory_crc = 0x40000038;
ets_is_print_boot = 0x4000003c;
ets_printf = 0x40000040;
ets_install_putc1 = 0x40000044;
ets_install_uart_printf = 0x40000048;
ets_install_putc2 = 0x4000004c;
PROVIDE( ets_delay_us = 0x40000050 );
ets_get_stack_info = 0x40000054;
ets_install_lock = 0x40000058;
ets_backup_dma_copy = 0x4000005c;
ets_apb_backup_init_lock_func = 0x40000060;
UartRxString = 0x40000064;
uart_tx_one_char = 0x40000068;
uart_tx_one_char2 = 0x4000006c;
uart_rx_one_char = 0x40000070;
uart_rx_one_char_block = 0x40000074;
uart_rx_readbuff = 0x40000078;
uartAttach = 0x4000007c;
uart_tx_flush = 0x40000080;
uart_tx_wait_idle = 0x40000084;
uart_div_modify = 0x40000088;
multofup = 0x4000008c;
software_reset = 0x40000090;
software_reset_cpu = 0x40000094;
assist_debug_clock_enable = 0x40000098;
assist_debug_record_enable = 0x4000009c;
clear_super_wdt_reset_flag = 0x400000a0;
disable_default_watchdog = 0x400000a4;
send_packet = 0x400000a8;
recv_packet = 0x400000ac;
GetUartDevice = 0x400000b0;
UartDwnLdProc = 0x400000b4;
Uart_Init = 0x400000b8;
ets_set_user_start = 0x400000bc;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x3ff1fffc;
ets_ops_table_ptr = 0x3fcdfffc;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x400000c0;
mz_crc32 = 0x400000c4;
mz_free = 0x400000c8;
tdefl_compress = 0x400000cc;
tdefl_compress_buffer = 0x400000d0;
tdefl_compress_mem_to_heap = 0x400000d4;
tdefl_compress_mem_to_mem = 0x400000d8;
tdefl_compress_mem_to_output = 0x400000dc;
tdefl_get_adler32 = 0x400000e0;
tdefl_get_prev_return_status = 0x400000e4;
tdefl_init = 0x400000e8;
tdefl_write_image_to_png_file_in_memory = 0x400000ec;
tdefl_write_image_to_png_file_in_memory_ex = 0x400000f0;
tinfl_decompress = 0x400000f4;
tinfl_decompress_mem_to_callback = 0x400000f8;
tinfl_decompress_mem_to_heap = 0x400000fc;
tinfl_decompress_mem_to_mem = 0x40000100;


/***************************************
 Group tjpgd
 ***************************************/

/* Functions */
PROVIDE( jd_prepare = 0x40000104 );
PROVIDE( jd_decomp = 0x40000108 );


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
PROVIDE( esp_rom_spiflash_wait_idle = 0x4000010c );
PROVIDE( esp_rom_spiflash_write_encrypted = 0x40000110 );
PROVIDE( esp_rom_spiflash_write_encrypted_dest = 0x40000114 );
PROVIDE( esp_rom_spiflash_write_encrypted_enable = 0x40000118 );
PROVIDE( esp_rom_spiflash_write_encrypted_disable = 0x4000011c );
PROVIDE( esp_rom_spiflash_erase_chip = 0x40000120 );
PROVIDE( esp_rom_spiflash_erase_block = 0x40000124 );
PROVIDE( esp_rom_spiflash_erase_sector = 0x40000128 );
PROVIDE( esp_rom_spiflash_write = 0x4000012c );
PROVIDE( esp_rom_spiflash_read = 0x40000130 );
PROVIDE( esp_rom_spiflash_config_param = 0x40000134 );
PROVIDE( esp_rom_spiflash_read_user_cmd = 0x40000138 );
PROVIDE( esp_rom_spiflash_select_qio_pins = 0x4000013c );
PROVIDE( esp_rom_spiflash_unlock = 0x40000140 );
PROVIDE( esp_rom_spi_flash_auto_sus_res = 0x40000144 );
PROVIDE( esp_rom_spi_flash_send_resume = 0x40000148 );
PROVIDE( esp_rom_spi_flash_update_id = 0x4000014c );
PROVIDE( esp_rom_spiflash_config_clk = 0x40000150 );
PROVIDE( esp_rom_spiflash_config_readmode = 0x40000154 );
PROVIDE( esp_rom_spiflash_read_status = 0x40000158 );
PROVIDE( esp_rom_spiflash_read_statushigh = 0x4000015c );
PROVIDE( esp_rom_spiflash_write_status = 0x40000160 );
PROVIDE( esp_rom_spiflash_attach = 0x40000164 );
PROVIDE( spi_flash_get_chip_size = 0x40000168 );
PROVIDE( spi_flash_guard_set = 0x4000016c );
PROVIDE( spi_flash_guard_get = 0x40000170 );
PROVIDE( spi_flash_write_config_set = 0x40000174 );
PROVIDE( spi_flash_write_config_get = 0x40000178 );
PROVIDE( spi_flash_safe_write_address_func_set = 0x4000017c );
PROVIDE( spi_flash_unlock = 0x40000180 );
PROVIDE( spi_flash_erase_range = 0x40000184 );
PROVIDE( spi_flash_erase_sector = 0x40000188 );
PROVIDE( spi_flash_write = 0x4000018c );
PROVIDE( spi_flash_read = 0x40000190 );
PROVIDE( spi_flash_write_encrypted = 0x40000194 );
PROVIDE( spi_flash_read_encrypted = 0x40000198 );
PROVIDE( spi_flash_mmap_os_func_set = 0x4000019c );
PROVIDE( spi_flash_mmap_page_num_init = 0x400001a0 );
PROVIDE( spi_flash_mmap = 0x400001a4 );
PROVIDE( spi_flash_mmap_pages = 0x400001a8 );
PROVIDE( spi_flash_munmap = 0x400001ac );
PROVIDE( spi_flash_mmap_dump = 0x400001b0 );
PROVIDE( spi_flash_check_and_flush_cache = 0x400001b4 );
PROVIDE( spi_flash_mmap_get_free_pages = 0x400001b8 );
PROVIDE( spi_flash_cache2phys = 0x400001bc );
PROVIDE( spi_flash_phys2cache = 0x400001c0 );
PROVIDE( spi_flash_disable_cache = 0x400001c4 );
PROVIDE( spi_flash_restore_cache = 0x400001c8 );
PROVIDE( spi_flash_cache_enabled = 0x400001cc );
PROVIDE( spi_flash_enable_cache = 0x400001d0 );
PROVIDE( spi_cache_mode_switch = 0x400001d4 );
PROVIDE( spi_common_set_dummy_output = 0x400001d8 );
PROVIDE( spi_common_set_flash_cs_timing = 0x400001dc );
PROVIDE( esp_enable_cache_flash_wrap = 0x400001e0 );
PROVIDE( SPIEraseArea = 0x400001e4 );
PROVIDE( SPILock = 0x400001e8 );
PROVIDE( SPIMasterReadModeCnfig = 0x400001ec );
PROVIDE( SPI_Common_Command = 0x400001f0 );
PROVIDE( SPI_WakeUp = 0x400001f4 );
PROVIDE( SPI_block_erase = 0x400001f8 );
PROVIDE( SPI_chip_erase = 0x400001fc );
PROVIDE( SPI_init = 0x40000200 );
PROVIDE( SPI_page_program = 0x40000204 );
PROVIDE( SPI_read_data = 0x40000208 );
PROVIDE( SPI_sector_erase = 0x4000020c );
PROVIDE( SPI_write_enable = 0x40000210 );
PROVIDE( SelectSpiFunction = 0x40000214 );
PROVIDE( SetSpiDrvs = 0x40000218 );
PROVIDE( Wait_SPI_Idle = 0x4000021c );
PROVIDE( spi_dummy_len_fix = 0x40000220 );
PROVIDE( Disable_QMode = 0x40000224 );
PROVIDE( Enable_QMode = 0x40000228 );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_spiflash_legacy_funcs = 0x3fcdfff4 );
PROVIDE( rom_spiflash_legacy_data = 0x3fcdfff0 );
PROVIDE( g_flash_guard_ops = 0x3fcdfff8 );


/***************************************
 Group spi_flash_hal
 ***************************************/

/* Functions */
PROVIDE( spi_flash_hal_poll_cmd_done = 0x4000022c );
PROVIDE( spi_flash_hal_device_config = 0x40000230 );
PROVIDE( spi_flash_hal_configure_host_io_mode = 0x40000234 );
PROVIDE( spi_flash_hal_common_command = 0x40000238 );
PROVIDE( spi_flash_hal_read = 0x4000023c );
PROVIDE( spi_flash_hal_erase_chip = 0x40000240 );
PROVIDE( spi_flash_hal_erase_sector = 0x40000244 );
PROVIDE( spi_flash_hal_erase_block = 0x40000248 );
PROVIDE( spi_flash_hal_program_page = 0x4000024c );
PROVIDE( spi_flash_hal_set_write_protect = 0x40000250 );
PROVIDE( spi_flash_hal_host_idle = 0x40000254 );


/***************************************
 Group spi_flash_chips
 ***************************************/

/* Functions */
PROVIDE( spi_flash_chip_generic_probe = 0x40000258 );
PROVIDE( spi_flash_chip_generic_detect_size = 0x4000025c );
PROVIDE( spi_flash_chip_generic_write = 0x40000260 );
PROVIDE( spi_flash_chip_generic_write_encrypted = 0x40000264 );
PROVIDE( spi_flash_chip_generic_set_write_protect = 0x40000268 );
PROVIDE( spi_flash_common_write_status_16b_wrsr = 0x4000026c );
PROVIDE( spi_flash_chip_generic_reset = 0x40000270 );
PROVIDE( spi_flash_chip_generic_erase_chip = 0x40000274 );
PROVIDE( spi_flash_chip_generic_erase_sector = 0x40000278 );
PROVIDE( spi_flash_chip_generic_erase_block = 0x4000027c );
PROVIDE( spi_flash_chip_generic_page_program = 0x40000280 );
PROVIDE( spi_flash_chip_generic_get_write_protect = 0x40000284 );
PROVIDE( spi_flash_common_read_status_16b_rdsr_rdsr2 = 0x40000288 );
PROVIDE( spi_flash_chip_generic_read_reg = 0x4000028c );
PROVIDE( spi_flash_chip_generic_yield = 0x40000290 );
PROVIDE( spi_flash_generic_wait_host_idle = 0x40000294 );
PROVIDE( spi_flash_chip_generic_wait_idle = 0x40000298 );
PROVIDE( spi_flash_chip_generic_config_host_io_mode = 0x4000029c );
PROVIDE( spi_flash_chip_generic_read = 0x400002a0 );
PROVIDE( spi_flash_common_read_status_8b_rdsr2 = 0x400002a4 );
PROVIDE( spi_flash_chip_generic_get_io_mode = 0x400002a8 );
PROVIDE( spi_flash_common_read_status_8b_rdsr = 0x400002ac );
PROVIDE( spi_flash_common_write_status_8b_wrsr = 0x400002b0 );
PROVIDE( spi_flash_common_write_status_8b_wrsr2 = 0x400002b4 );
PROVIDE( spi_flash_common_set_io_mode = 0x400002b8 );
PROVIDE( spi_flash_chip_generic_set_io_mode = 0x400002bc );
PROVIDE( spi_flash_chip_gd_get_io_mode = 0x400002c0 );
PROVIDE( spi_flash_chip_gd_probe = 0x400002c4 );
PROVIDE( spi_flash_chip_gd_set_io_mode = 0x400002c8 );
/* Data (.data, .bss, .rodata) */
PROVIDE( spi_flash_chip_generic_config_data = 0x3fcdffec );


/***************************************
 Group memspi_host
 ***************************************/

/* Functions */
PROVIDE( memspi_host_read_id_hs = 0x400002cc );
PROVIDE( memspi_host_read_status_hs = 0x400002d0 );
PROVIDE( memspi_host_flush_cache = 0x400002d4 );
PROVIDE( memspi_host_erase_chip = 0x400002d8 );
PROVIDE( memspi_host_erase_sector = 0x400002dc );
PROVIDE( memspi_host_erase_block = 0x400002e0 );
PROVIDE( memspi_host_program_page = 0x400002e4 );
PROVIDE( memspi_host_read = 0x400002e8 );
PROVIDE( memspi_host_set_write_protect = 0x400002ec );
PROVIDE( memspi_host_set_max_read_len = 0x400002f0 );
PROVIDE( memspi_host_read_data_slicer = 0x400002f4 );
PROVIDE( memspi_host_write_data_slicer = 0x400002f8 );


/***************************************
 Group esp_flash
 ***************************************/

/* Functions */
PROVIDE( esp_flash_chip_driver_initialized = 0x400002fc );
PROVIDE( esp_flash_read_id = 0x40000300 );
PROVIDE( esp_flash_get_size = 0x40000304 );
PROVIDE( esp_flash_erase_chip = 0x40000308 );
PROVIDE( rom_esp_flash_erase_region = 0x4000030c );
PROVIDE( esp_flash_get_chip_write_protect = 0x40000310 );
PROVIDE( esp_flash_set_chip_write_protect = 0x40000314 );
PROVIDE( esp_flash_get_protectable_regions = 0x40000318 );
PROVIDE( esp_flash_get_protected_region = 0x4000031c );
PROVIDE( esp_flash_set_protected_region = 0x40000320 );
PROVIDE( esp_flash_read = 0x40000324 );
PROVIDE( esp_flash_write = 0x40000328 );
PROVIDE( esp_flash_write_encrypted = 0x4000032c );
PROVIDE( esp_flash_read_encrypted = 0x40000330 );
PROVIDE( esp_flash_get_io_mode = 0x40000334 );
PROVIDE( esp_flash_set_io_mode = 0x40000338 );
PROVIDE( spi_flash_boot_attach = 0x4000033c );
PROVIDE( spi_flash_dump_counters = 0x40000340 );
PROVIDE( spi_flash_get_counters = 0x40000344 );
PROVIDE( spi_flash_op_counters_config = 0x40000348 );
PROVIDE( spi_flash_reset_counters = 0x4000034c );
/* Data (.data, .bss, .rodata) */
PROVIDE( esp_flash_default_chip = 0x3fcdffe8 );
PROVIDE( esp_flash_api_funcs = 0x3fcdffe4 );


/***************************************
 Group cache
 ***************************************/

/* Functions */
PROVIDE( Cache_Get_ICache_Line_Size = 0x400004b0 );
PROVIDE( Cache_Get_Mode = 0x400004b4 );
PROVIDE( Cache_Address_Through_IBus = 0x400004b8 );
PROVIDE( Cache_Address_Through_DBus = 0x400004bc );
PROVIDE( Cache_Set_Default_Mode = 0x400004c0 );
PROVIDE( Cache_Enable_Defalut_ICache_Mode = 0x400004c4 );
PROVIDE( ROM_Boot_Cache_Init = 0x400004c8 );
PROVIDE( Cache_Invalidate_ICache_Items = 0x400004cc );
PROVIDE( Cache_Op_Addr = 0x400004d0 );
PROVIDE( Cache_Invalidate_Addr = 0x400004d4 );
PROVIDE( Cache_Invalidate_ICache_All = 0x400004d8 );
PROVIDE( Cache_Mask_All = 0x400004dc );
PROVIDE( Cache_UnMask_Dram0 = 0x400004e0 );
PROVIDE( Cache_Suspend_ICache_Autoload = 0x400004e4 );
PROVIDE( Cache_Resume_ICache_Autoload = 0x400004e8 );
PROVIDE( Cache_Start_ICache_Preload = 0x400004ec );
PROVIDE( Cache_ICache_Preload_Done = 0x400004f0 );
PROVIDE( Cache_End_ICache_Preload = 0x400004f4 );
PROVIDE( Cache_Config_ICache_Autoload = 0x400004f8 );
PROVIDE( Cache_Enable_ICache_Autoload = 0x400004fc );
PROVIDE( Cache_Disable_ICache_Autoload = 0x40000500 );
PROVIDE( Cache_Enable_ICache_PreLock = 0x40000504 );
PROVIDE( Cache_Disable_ICache_PreLock = 0x40000508 );
PROVIDE( Cache_Lock_ICache_Items = 0x4000050c );
PROVIDE( Cache_Unlock_ICache_Items = 0x40000510 );
PROVIDE( Cache_Lock_Addr = 0x40000514 );
PROVIDE( Cache_Unlock_Addr = 0x40000518 );
PROVIDE( Cache_Disable_ICache = 0x4000051c );
PROVIDE( Cache_Enable_ICache = 0x40000520 );
PROVIDE( Cache_Suspend_ICache = 0x40000524 );
PROVIDE( Cache_Resume_ICache = 0x40000528 );
PROVIDE( Cache_Freeze_ICache_Enable = 0x4000052c );
PROVIDE( Cache_Freeze_ICache_Disable = 0x40000530 );
PROVIDE( Cache_Pms_Lock = 0x40000534 );
PROVIDE( Cache_Ibus_Pms_Set_Addr = 0x40000538 );
PROVIDE( Cache_Ibus_Pms_Set_Attr = 0x4000053c );
PROVIDE( Cache_Dbus_Pms_Set_Addr = 0x40000540 );
PROVIDE( Cache_Dbus_Pms_Set_Attr = 0x40000544 );
PROVIDE( Cache_Set_IDROM_MMU_Size = 0x40000548 );
PROVIDE( Cache_Get_IROM_MMU_End = 0x4000054c );
PROVIDE( Cache_Get_DROM_MMU_End = 0x40000550 );
PROVIDE( Cache_Owner_Init = 0x40000554 );
PROVIDE( Cache_Occupy_ICache_MEMORY = 0x40000558 );
PROVIDE( Cache_MMU_Init = 0x4000055c );
PROVIDE( Cache_Ibus_MMU_Set = 0x40000560 );
PROVIDE( Cache_Dbus_MMU_Set = 0x40000564 );
PROVIDE( Cache_Count_Flash_Pages = 0x40000568 );
PROVIDE( Cache_Travel_Tag_Memory = 0x4000056c );
PROVIDE( Cache_Get_Virtual_Addr = 0x40000570 );
PROVIDE( Cache_Get_Memory_BaseAddr = 0x40000574 );
PROVIDE( Cache_Get_Memory_Addr = 0x40000578 );
PROVIDE( Cache_Get_Memory_value = 0x4000057c );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_cache_op_cb = 0x3fcdffd8 );
PROVIDE( rom_cache_internal_table_ptr = 0x3fcdffd4 );


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_get_apb_freq = 0x40000580;
ets_get_cpu_frequency = 0x40000584;
ets_update_cpu_frequency = 0x40000588;
ets_get_printf_channel = 0x4000058c;
ets_get_xtal_div = 0x40000590;
ets_set_xtal_div = 0x40000594;
ets_get_xtal_freq = 0x40000598;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_input_get = 0x4000059c;
gpio_matrix_in = 0x400005a0;
gpio_matrix_out = 0x400005a4;
/* gpio_output_disable = 0x400005a8; */
/* gpio_output_enable = 0x400005ac; */
gpio_output_set = 0x400005b0;
gpio_pad_hold = 0x400005b4;
gpio_pad_input_disable = 0x400005b8;
gpio_pad_input_enable = 0x400005bc;
gpio_pad_pulldown = 0x400005c0;
gpio_pad_pullup = 0x400005c4;
gpio_pad_select_gpio = 0x400005c8;
gpio_pad_set_drv = 0x400005cc;
gpio_pad_unhold = 0x400005d0;
gpio_pin_wakeup_disable = 0x400005d4;
gpio_pin_wakeup_enable = 0x400005d8;
gpio_bypass_matrix_in = 0x400005dc;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x400005e0;
esprv_intc_int_set_threshold = 0x400005e4;
esprv_intc_int_enable = 0x400005e8;
esprv_intc_int_disable = 0x400005ec;
esprv_intc_int_set_type = 0x400005f0;
intr_matrix_set = 0x400005f4;
ets_intr_lock = 0x400005f8;
ets_intr_unlock = 0x400005fc;
PROVIDE( intr_handler_set = 0x40000600 );
ets_isr_attach = 0x40000604;
ets_isr_mask = 0x40000608;
ets_isr_unmask = 0x4000060c;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
md5_vector = 0x40000610;
MD5Init = 0x40000614;
MD5Update = 0x40000618;
MD5Final = 0x4000061c;
hmac_md5_vector = 0x40000620;
hmac_md5 = 0x40000624;
crc32_le = 0x40000628;
crc32_be = 0x4000062c;
crc16_le = 0x40000630;
crc16_be = 0x40000634;
crc8_le = 0x40000638;
crc8_be = 0x4000063c;
esp_crc8 = 0x40000640;
ets_sha_enable = 0x40000644;
ets_sha_disable = 0x40000648;
ets_sha_get_state = 0x4000064c;
ets_sha_init = 0x40000650;
ets_sha_process = 0x40000654;
ets_sha_starts = 0x40000658;
ets_sha_update = 0x4000065c;
ets_sha_finish = 0x40000660;
ets_sha_clone = 0x40000664;
ets_hmac_enable = 0x40000668;
ets_hmac_disable = 0x4000066c;
ets_hmac_calculate_message = 0x40000670;
ets_hmac_calculate_downstream = 0x40000674;
ets_hmac_invalidate_downstream = 0x40000678;
ets_jtag_enable_temporarily = 0x4000067c;
ets_aes_enable = 0x40000680;
ets_aes_disable = 0x40000684;
ets_aes_setkey = 0x40000688;
ets_aes_block = 0x4000068c;
ets_bigint_enable = 0x40000690;
ets_bigint_disable = 0x40000694;
ets_bigint_multiply = 0x40000698;
ets_bigint_modmult = 0x4000069c;
ets_bigint_modexp = 0x400006a0;
ets_bigint_wait_finish = 0x400006a4;
ets_bigint_getz = 0x400006a8;
ets_ds_enable = 0x400006ac;
ets_ds_disable = 0x400006b0;
ets_ds_start_sign = 0x400006b4;
ets_ds_is_busy = 0x400006b8;
ets_ds_finish_sign = 0x400006bc;
ets_ds_encrypt_params = 0x400006c0;
ets_aes_setkey_dec = 0x400006c4;
ets_aes_setkey_enc = 0x400006c8;
ets_mgf1_sha256 = 0x400006cc;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x400006d0;
ets_efuse_program = 0x400006d4;
ets_efuse_clear_program_registers = 0x400006d8;
ets_efuse_write_key = 0x400006dc;
ets_efuse_get_read_register_address = 0x400006e0;
ets_efuse_get_key_purpose = 0x400006e4;
ets_efuse_key_block_unused = 0x400006e8;
ets_efuse_find_unused_key_block = 0x400006ec;
ets_efuse_rs_calculate = 0x400006f0;
ets_efuse_count_unused_key_blocks = 0x400006f4;
ets_efuse_secure_boot_enabled = 0x400006f8;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x400006fc;
ets_efuse_cache_encryption_enabled = 0x40000700;
ets_efuse_download_modes_disabled = 0x40000704;
ets_efuse_find_purpose = 0x40000708;
ets_efuse_flash_opi_5pads_power_sel_vddspi = 0x4000070c;
ets_efuse_force_send_resume = 0x40000710;
ets_efuse_get_flash_delay_us = 0x40000714;
ets_efuse_get_mac = 0x40000718;
ets_efuse_get_spiconfig = 0x4000071c;
ets_efuse_usb_print_is_disabled = 0x40000720;
/*ets_efuse_get_uart_print_channel = 0x40000724;*/
ets_efuse_usb_serial_jtag_print_is_disabled = 0x40000724;
ets_efuse_get_uart_print_control = 0x40000728;
ets_efuse_get_wp_pad = 0x4000072c;
ets_efuse_legacy_spi_boot_mode_disabled = 0x40000730;
ets_efuse_security_download_modes_enabled = 0x40000734;
ets_efuse_set_timing = 0x40000738;
ets_efuse_jtag_disabled = 0x4000073c;
ets_efuse_usb_download_mode_disabled = 0x40000740;
ets_efuse_usb_module_disabled = 0x40000744;
ets_efuse_usb_device_disabled = 0x40000748;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x4000074c;
ets_rsa_pss_verify = 0x40000750;
ets_secure_boot_verify_bootloader_with_keys = 0x40000754;
ets_secure_boot_verify_signature = 0x40000758;
ets_secure_boot_read_key_digests = 0x4000075c;
ets_secure_boot_revoke_public_key_digest = 0x40000760;


/***************************************
 Group usb_uart
 ***************************************/

/* Functions */
PROVIDE( usb_uart_rx_one_char = 0x400008cc );
PROVIDE( usb_uart_rx_one_char_block = 0x400008d0 );
PROVIDE( usb_uart_tx_flush = 0x400008d4 );
PROVIDE( usb_uart_tx_one_char = 0x400008d8 );
/* Data (.data, .bss, .rodata) */
PROVIDE( g_uart_print = 0x3fcdffd1 );
PROVIDE( g_usb_print = 0x3fcdffd0 );


/***************************************
 Group bluetooth
 ***************************************/
/* Data (.data, .bss, .rodata) */
bt_rf_coex_cfg_p = 0x3fcdffcc;
bt_rf_coex_hooks_p = 0x3fcdffc8;
btdm_env_p = 0x3fcdffc4;
g_rw_controller_task_handle = 0x3fcdffc0;
g_rw_init_sem = 0x3fcdffbc;
g_rw_schd_queue = 0x3fcdffb8;
lld_init_env = 0x3fcdffb4;
lld_rpa_renew_env = 0x3fcdffb0;
lld_scan_env = 0x3fcdffac;
lld_scan_sync_env = 0x3fcdffa8;
lld_test_env = 0x3fcdffa4;
p_ble_util_buf_env = 0x3fcdffa0;
p_lld_env = 0x3fcdff9c;
p_llm_env = 0x3fcdff98;
r_h4tl_eif_p = 0x3fcdff94;
r_hli_funcs_p = 0x3fcdff90;
r_ip_funcs_p = 0x3fcdff8c;
r_modules_funcs_p = 0x3fcdff88;
r_osi_funcs_p = 0x3fcdff84;
r_plf_funcs_p = 0x3fcdff80;
vhci_env_p = 0x3fcdff7c;
aa_gen = 0x3fcdff78;
aes_env = 0x3fcdff6c;
bt_rf_coex_cfg_cb = 0x3fcdff1c;
btdm_pwr_state = 0x3fcdff18;
btdm_slp_err = 0x3fcdff14;
ecc_env = 0x3fcdff0c;
esp_handler = 0x3fcdff04;
esp_vendor_cmd = 0x3fcdfefc;
g_adv_delay_dis = 0x3fcdfef8;
g_conflict_elt = 0x3fcdfef4;
g_eif_api = 0x3fcdfee4;
g_event_empty = 0x3fcdfed8;
g_llc_state = 0x3fcdfecc;
g_llm_state = 0x3fcdfec8;
g_max_evt_env = 0x3fcdfec4;
g_misc_state = 0x3fcdfec0;
g_rma_rule_db = 0x3fcdfea4;
g_rtp_rule_db = 0x3fcdfe88;
g_scan_forever = 0x3fcdfe85;
g_time_msb = 0x3fcdfe84;
h4tl_env = 0x3fcdfe5c;
hci_env = 0x3fcdfe38;
hci_ext_host = 0x3fcdfe34;
hci_fc_env = 0x3fcdfe2c;
hci_tl_env = 0x3fcdfe00;
ke_env = 0x3fcdfdd0;
ke_event_env = 0x3fcdfd90;
ke_task_env = 0x3fcdfd14;
llc_env = 0x3fcdfcec;
lld_adv_env = 0x3fcdfcc4;
lld_con_env = 0x3fcdfc9c;
lld_exp_sync_pos_tab = 0x3fcdfc94;
lld_per_adv_env = 0x3fcdfc6c;
lld_sync_env = 0x3fcdfc44;
llm_le_adv_flow_env = 0x3fcdfc38;
rw_sleep_enable = 0x3fcdfc34;
rwble_env = 0x3fcdfc2c;
rwip_env = 0x3fcdfc10;
rwip_param = 0x3fcdfc04;
rwip_prog_delay = 0x3fcdfc00;
rwip_rf = 0x3fcdfbc8;
sch_alarm_env = 0x3fcdfbc0;
sch_arb_env = 0x3fcdfbac;
sch_plan_env = 0x3fcdfba4;
sch_prog_env = 0x3fcdfaa0;
sch_slice_env = 0x3fcdfa40;
sch_slice_params = 0x3fcdfa38;
timer_env = 0x3fcdfa30;
unloaded_area = 0x3fcdfa2c;
vshci_state = 0x3fcdfa28;
TASK_DESC_LLC = 0x3fcdfa1c;
TASK_DESC_LLM = 0x3fcdfa10;
TASK_DESC_VSHCI = 0x3fcdfa04;
co_default_bdaddr = 0x3fcdf9fc;
dbg_assert_block = 0x3fcdf9f8;
g_bt_plf_log_level = 0x3fcdf9f4;
hci_cmd_desc_tab_vs_esp = 0x3fcdf9d0;
hci_command_handler_tab_esp = 0x3fcdf9b8;
privacy_en = 0x3fcdf9b4;
sdk_cfg_priv_opts = 0x3fcdf96c;
BasePoint_x_256 = 0x3ff1ffdc;
BasePoint_y_256 = 0x3ff1ffbc;
DebugE256PublicKey_x = 0x3ff1ff9c;
DebugE256PublicKey_y = 0x3ff1ff7c;
DebugE256SecretKey = 0x3ff1ff5c;
ECC_4Win_Look_up_table = 0x3ff1f7a0;
LLM_AA_CT1 = 0x3ff1f79c;
LLM_AA_CT2 = 0x3ff1f798;
RF_TX_PW_CONV_TBL = 0x3ff1f790;
TASK_DESC_MISC = 0x3ff1f784;
adv_evt_prop2type = 0x3ff1f768;
adv_evt_type2prop = 0x3ff1f760;
aes_cmac_zero = 0x3ff1f750;
aes_k2_salt = 0x3ff1f740;
aes_k3_id64 = 0x3ff1f738;
aes_k3_salt = 0x3ff1f728;
aes_k4_id6 = 0x3ff1f724;
aes_k4_salt = 0x3ff1f714;
bigHexP256 = 0x3ff1f6e8;
byte_tx_time = 0x3ff1f6e0;
co_null_bdaddr = 0x3ff1f6d8;
co_phy_mask_to_rate = 0x3ff1f6d0;
co_phy_mask_to_value = 0x3ff1f6c8;
co_phy_to_rate = 0x3ff1f6c4;
co_phy_value_to_mask = 0x3ff1f6c0;
co_rate_to_byte_dur_us = 0x3ff1f6b8;
co_rate_to_phy = 0x3ff1f6b0;
co_rate_to_phy_mask = 0x3ff1f6ac;
co_sca2ppm = 0x3ff1f69c;
coef_B = 0x3ff1f670;
connect_req_dur_tab = 0x3ff1f668;
ecc_Jacobian_InfinityPoint256 = 0x3ff1f5e4;
em_base_reg_lut = 0x3ff1f518;
fixed_tx_time = 0x3ff1f510;
h4tl_msgtype2hdrlen = 0x3ff1f508;
hci_cmd_desc_root_tab = 0x3ff1f4d8;
hci_cmd_desc_tab_ctrl_bb = 0x3ff1f46c;
hci_cmd_desc_tab_info_par = 0x3ff1f43c;
hci_cmd_desc_tab_le = 0x3ff1f0a0;
hci_cmd_desc_tab_lk_ctrl = 0x3ff1f088;
hci_cmd_desc_tab_stat_par = 0x3ff1f07c;
hci_cmd_desc_tab_vs = 0x3ff1f040;
hci_evt_desc_tab = 0x3ff1eff8;
hci_evt_le_desc_tab = 0x3ff1ef58;
hci_evt_le_desc_tab_esp = 0x3ff1ef50;
hci_rsvd_evt_msk = 0x3ff1ef48;
lld_aux_phy_to_rate = 0x3ff1ef44;
lld_init_max_aux_dur_tab = 0x3ff1ef3c;
lld_scan_map_legacy_pdu_to_evt_type = 0x3ff1ef34;
lld_scan_max_aux_dur_tab = 0x3ff1ef2c;
lld_sync_max_aux_dur_tab = 0x3ff1ef24;
llm_local_le_feats = 0x3ff1ef1c;
llm_local_le_states = 0x3ff1ef14;
llm_local_supp_cmds = 0x3ff1eeec;
maxSecretKey_256 = 0x3ff1eecc;
max_data_tx_time = 0x3ff1eec4;
one_bits = 0x3ff1eeb4;
rwip_coex_cfg = 0x3ff1eeac;
rwip_priority = 0x3ff1ee94;
veryBigHexP256 = 0x3ff1ee48;

/***************************************
 Group rom_pp
 ***************************************/

/* Functions */
esp_pp_rom_version_get = 0x400015b0;
RC_GetBlockAckTime = 0x400015b4;
ebuf_list_remove = 0x400015b8;
/*esf_buf_alloc = 0x400015bc;*/
GetAccess = 0x400015c8;
hal_mac_is_low_rate_enabled = 0x400015cc;
hal_mac_tx_get_blockack = 0x400015d0;
/* hal_mac_tx_set_ppdu = 0x400015d4;*/
ic_get_trc = 0x400015d8;
/* ic_mac_deinit = 0x400015dc; */
ic_mac_init = 0x400015e0;
ic_interface_enabled = 0x400015e4;
is_lmac_idle = 0x400015e8;
/*lmacAdjustTimestamp = 0x400015ec;*/
lmacDiscardAgedMSDU = 0x400015f0;
/*lmacDiscardMSDU = 0x400015f4;*/
/*lmacEndFrameExchangeSequence = 0x400015f8;*/
lmacIsIdle = 0x400015fc;
lmacIsLongFrame = 0x40001600;
/*lmacMSDUAged = 0x40001604;*/
lmacPostTxComplete = 0x40001608;
lmacProcessAllTxTimeout = 0x4000160c;
lmacProcessCollisions = 0x40001610;
lmacProcessRxSucData = 0x40001614;
lmacReachLongLimit = 0x40001618;
lmacReachShortLimit = 0x4000161c;
lmacRecycleMPDU = 0x40001620;
lmacRxDone = 0x40001624;
/*lmacSetTxFrame = 0x40001628;*/
/*lmacTxFrame = 0x40001630;*/
mac_tx_set_duration = 0x40001634;
/* mac_tx_set_htsig = 0x40001638;*/
mac_tx_set_plcp0 = 0x4000163c;
/* mac_tx_set_plcp1 = 0x40001640;*/
mac_tx_set_plcp2 = 0x40001644;
/* pm_check_state = 0x40001648; */
pm_disable_dream_timer = 0x4000164c;
pm_disable_sleep_delay_timer = 0x40001650;
/*pm_dream = 0x40001654;*/
pm_mac_wakeup = 0x40001658;
pm_mac_sleep = 0x4000165c;
pm_enable_active_timer = 0x40001660;
pm_enable_sleep_delay_timer = 0x40001664;
pm_local_tsf_process = 0x40001668;
pm_set_beacon_filter = 0x4000166c;
/*pm_is_in_wifi_slice_threshold = 0x40001670;*/
pm_is_waked = 0x40001674;
/*pm_keep_alive = 0x40001678;*/
/* pm_on_beacon_rx = 0x4000167c; */
pm_on_data_rx = 0x40001680;
pm_on_tbtt = 0x40001684;
/* pm_parse_beacon = 0x40001688;*/
/* pm_process_tim = 0x4000168c; */
/*pm_rx_beacon_process = 0x40001690;*/
/* pm_rx_data_process = 0x40001694; */
/*pm_sleep = 0x40001698;*/
pm_sleep_for = 0x4000169c;
/* pm_tbtt_process = 0x400016a0; */
ppAMPDU2Normal = 0x400016a4;
/*ppAssembleAMPDU = 0x400016a8;*/
ppCalFrameTimes = 0x400016ac;
ppCalSubFrameLength = 0x400016b0;
/*ppCalTxAMPDULength = 0x400016b4;*/
ppCheckTxAMPDUlength = 0x400016b8;
ppDequeueRxq_Locked = 0x400016bc;
ppDequeueTxQ = 0x400016c0;
ppEmptyDelimiterLength = 0x400016c4;
ppEnqueueRxq = 0x400016c8;
ppEnqueueTxDone = 0x400016cc;
ppGetTxQFirstAvail_Locked = 0x400016d0;
ppGetTxframe = 0x400016d4;
ppProcessRxPktHdr = 0x400016e0;
/*ppProcessTxQ = 0x400016e4;*/
ppRecordBarRRC = 0x400016e8;
lmacRequestTxopQueue = 0x400016ec;
lmacReleaseTxopQueue = 0x400016f0;
ppRecycleAmpdu = 0x400016f4;
ppRecycleRxPkt = 0x400016f8;
ppResortTxAMPDU = 0x400016fc;
ppResumeTxAMPDU = 0x40001700;
/* ppRxFragmentProc = 0x40001704; */
/* ppRxPkt = 0x40001708; */
ppRxProtoProc = 0x4000170c;
ppSearchTxQueue = 0x40001710;
ppSearchTxframe = 0x40001714;
ppSelectNextQueue = 0x40001718;
ppSubFromAMPDU = 0x4000171c;
ppTask = 0x40001720;
ppTxPkt = 0x40001724;
ppTxProtoProc = 0x40001728;
ppTxqUpdateBitmap = 0x4000172c;
/*pp_coex_tx_request = 0x40001730;*/
pp_hdrsize = 0x40001734;
pp_post = 0x40001738;
pp_process_hmac_waiting_txq = 0x4000173c;
rcGetAmpduSched = 0x40001740;
rcUpdateRxDone = 0x40001744;
rc_get_trc = 0x40001748;
rc_get_trc_by_index = 0x4000174c;
rcAmpduLowerRate = 0x40001750;
rcampduuprate = 0x40001754;
rcClearCurAMPDUSched = 0x40001758;
rcClearCurSched = 0x4000175c;
rcClearCurStat = 0x40001760;
rcLowerSched = 0x40001768;
rcSetTxAmpduLimit = 0x4000176c;
/* rcTxUpdatePer = 0x40001770;*/
rcUpdateAckSnr = 0x40001774;
/*rcUpdateRate = 0x40001778;*/
/* rcUpdateTxDone = 0x4000177c; */
/* rcUpdateTxDoneAmpdu2 = 0x40001780;*/
rcUpSched = 0x40001784;
rssi_margin = 0x40001788;
rx11NRate2AMPDULimit = 0x4000178c;
TRC_AMPDU_PER_DOWN_THRESHOLD = 0x40001790;
TRC_AMPDU_PER_UP_THRESHOLD = 0x40001794;
trc_calc_duration = 0x40001798;
trc_isTxAmpduOperational = 0x4000179c;
trc_onAmpduOp = 0x400017a0;
TRC_PER_IS_GOOD = 0x400017a4;
trc_SetTxAmpduState = 0x400017a8;
trc_tid_isTxAmpduOperational = 0x400017ac;
trcAmpduSetState = 0x400017b0;
/*wDev_AppendRxBlocks = 0x400017b8;*/
wDev_DiscardFrame = 0x400017bc;
wDev_GetNoiseFloor = 0x400017c0;
wDev_IndicateAmpdu = 0x400017c4;
/*wDev_IndicateFrame = 0x400017c8;*/
wdev_bank_store = 0x400017cc;
wdev_bank_load = 0x400017d0;
wdev_mac_reg_load = 0x400017d4;
wdev_mac_reg_store = 0x400017d8;
wdev_mac_special_reg_load = 0x400017dc;
wdev_mac_special_reg_store = 0x400017e0;
wdev_mac_wakeup = 0x400017e4;
wdev_mac_sleep = 0x400017e8;
hal_mac_is_dma_enable = 0x400017ec;
/*wDev_ProcessFiq = 0x400017f0;*/
/*wDev_ProcessRxSucData = 0x400017f4;*/
wdevProcessRxSucDataAll = 0x400017f8;
wdev_csi_len_align = 0x400017fc;
ppDequeueTxDone_Locked = 0x40001800;
/*pm_tx_data_done_process = 0x40001808;*/
config_is_cache_tx_buf_enabled = 0x4000180c;
//ppMapWaitTxq = 0x40001810;
ppProcessWaitingQueue = 0x40001814;
ppDisableQueue = 0x40001818;
pm_allow_tx = 0x4000181c;
/* Data (.data, .bss, .rodata) */
our_instances_ptr = 0x3ff1ee44;
pTxRx = 0x3fcdf968;
lmacConfMib_ptr = 0x3fcdf964;
our_wait_eb = 0x3fcdf960;
our_tx_eb = 0x3fcdf95c;
pp_wdev_funcs = 0x3fcdf958;
g_osi_funcs_p = 0x3fcdf954;
wDevCtrl_ptr = 0x3fcdf950;
g_wdev_last_desc_reset_ptr = 0x3ff1ee40;
wDevMacSleep_ptr = 0x3fcdf94c;
g_lmac_cnt_ptr = 0x3fcdf948;
our_controls_ptr = 0x3ff1ee3c;
pp_sig_cnt_ptr = 0x3fcdf944;
g_eb_list_desc_ptr = 0x3fcdf940;
s_fragment_ptr = 0x3fcdf93c;
if_ctrl_ptr = 0x3fcdf938;
g_intr_lock_mux = 0x3fcdf934;
g_wifi_global_lock = 0x3fcdf930;
s_wifi_queue = 0x3fcdf92c;
pp_task_hdl = 0x3fcdf928;
s_pp_task_create_sem = 0x3fcdf924;
s_pp_task_del_sem = 0x3fcdf920;
g_wifi_menuconfig_ptr = 0x3fcdf91c;
xphyQueue = 0x3fcdf918;
ap_no_lr_ptr = 0x3fcdf914;
rc11BSchedTbl_ptr = 0x3fcdf910;
rc11NSchedTbl_ptr = 0x3fcdf90c;
rcLoRaSchedTbl_ptr = 0x3fcdf908;
BasicOFDMSched_ptr = 0x3fcdf904;
trc_ctl_ptr = 0x3fcdf900;
g_pm_cnt_ptr = 0x3fcdf8fc;
g_pm_ptr = 0x3fcdf8f8;
g_pm_cfg_ptr = 0x3fcdf8f4;
g_esp_mesh_quick_funcs_ptr = 0x3fcdf8f0;
g_txop_queue_status_ptr = 0x3fcdf8ec;
g_mac_sleep_en_ptr = 0x3fcdf8e8;
g_mesh_is_root_ptr = 0x3fcdf8e4;
g_mesh_topology_ptr = 0x3fcdf8e0;
g_mesh_init_ps_type_ptr = 0x3fcdf8dc;
g_mesh_is_started_ptr = 0x3fcdf8d8;
g_config_func = 0x3fcdf8d4;
g_net80211_tx_func = 0x3fcdf8d0;
g_timer_func = 0x3fcdf8cc;
s_michael_mic_failure_cb = 0x3fcdf8c8;
wifi_sta_rx_probe_req = 0x3fcdf8c4;
g_tx_done_cb_func = 0x3fcdf8c0;
g_per_conn_trc = 0x3fcdf874;
s_encap_amsdu_func = 0x3fcdf870;


/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x40001820;
ampdu_dispatch = 0x40001824;
ampdu_dispatch_all = 0x40001828;
ampdu_dispatch_as_many_as_possible = 0x4000182c;
ampdu_dispatch_movement = 0x40001830;
ampdu_dispatch_upto = 0x40001834;
chm_is_at_home_channel = 0x40001838;
cnx_node_is_existing = 0x4000183c;
cnx_node_search = 0x40001840;
ic_ebuf_recycle_rx = 0x40001844;
ic_ebuf_recycle_tx = 0x40001848;
ic_reset_rx_ba = 0x4000184c;
ieee80211_align_eb = 0x40001850;
/*ieee80211_ampdu_reorder = 0x40001854;*/
ieee80211_ampdu_start_age_timer = 0x40001858;
/*ieee80211_encap_esfbuf = 0x4000185c;*/
ieee80211_is_tx_allowed = 0x40001860;
ieee80211_output_pending_eb = 0x40001864;
/*ieee80211_output_process = 0x40001868;*/
ieee80211_set_tx_desc = 0x4000186c;
rom_sta_input = 0x40001870;
wifi_get_macaddr = 0x40001874;
wifi_rf_phy_disable = 0x40001878;
wifi_rf_phy_enable = 0x4000187c;
ic_ebuf_alloc = 0x40001880;
/*ieee80211_classify = 0x40001884;*/
ieee80211_copy_eb_header = 0x40001888;
ieee80211_recycle_cache_eb = 0x4000188c;
ieee80211_search_node = 0x40001890;
roundup2 = 0x40001894;
ieee80211_crypto_encap = 0x40001898;
/* ieee80211_crypto_decap = 0x4000189c; */
/* ieee80211_decap = 0x400018a0; */
ieee80211_set_tx_pti = 0x400018a4;
wifi_is_started = 0x400018a8;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x3fcdf86c;
g_scan = 0x3fcdf868;
g_chm = 0x3fcdf864;
g_ic_ptr = 0x3fcdf860;
g_hmac_cnt_ptr = 0x3fcdf85c;
g_tx_cacheq_ptr = 0x3fcdf858;
s_netstack_free = 0x3fcdf854;
mesh_rxcb = 0x3fcdf850;
sta_rxcb = 0x3fcdf84c;


/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
esp_coex_rom_version_get = 0x400018ac;
coex_bt_release = 0x400018b0;
coex_bt_request = 0x400018b4;
coex_core_ble_conn_dyn_prio_get = 0x400018b8;
/*coex_core_event_duration_get = 0x400018bc;*/
coex_core_pti_get = 0x400018c0;
coex_core_release = 0x400018c4;
coex_core_request = 0x400018c8;
coex_core_status_get = 0x400018cc;
/*coex_core_timer_idx_get = 0x400018d0;*/
coex_event_duration_get = 0x400018d4;
coex_hw_timer_disable = 0x400018d8;
coex_hw_timer_enable = 0x400018dc;
coex_hw_timer_set = 0x400018e0;
coex_schm_interval_set = 0x400018e4;
coex_schm_lock = 0x400018e8;
coex_schm_unlock = 0x400018ec;
/*coex_status_get = 0x400018f0;*/
coex_wifi_release = 0x400018f4;
esp_coex_ble_conn_dynamic_prio_get = 0x400018f8;
/* Data (.data, .bss, .rodata) */
coex_env_ptr = 0x3fcdf848;
coex_pti_tab_ptr = 0x3fcdf844;
coex_schm_env_ptr = 0x3fcdf840;
coexist_funcs = 0x3fcdf83c;
g_coa_funcs_p = 0x3fcdf838;
g_coex_param_ptr = 0x3fcdf834;


/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
phy_get_romfuncs = 0x400018fc;
rom_abs_temp = 0x40001900;
rom_bb_bss_cbw40_dig = 0x40001904;
rom_bb_wdg_test_en = 0x40001908;
rom_bb_wdt_get_status = 0x4000190c;
rom_bb_wdt_int_enable = 0x40001910;
rom_bb_wdt_rst_enable = 0x40001914;
rom_bb_wdt_timeout_clear = 0x40001918;
rom_cbw2040_cfg = 0x4000191c;
rom_check_noise_floor = 0x40001920;
rom_chip_i2c_readReg = 0x40001924;
rom_chip_i2c_writeReg = 0x40001928;
rom_correct_rf_ana_gain = 0x4000192c;
rom_dc_iq_est = 0x40001930;
rom_disable_agc = 0x40001934;
rom_en_pwdet = 0x40001938;
rom_enable_agc = 0x4000193c;
rom_get_bbgain_db = 0x40001940;
rom_get_data_sat = 0x40001944;
rom_get_i2c_read_mask = 0x40001948;
rom_get_pwctrl_correct = 0x4000194c;
rom_get_rf_gain_qdb = 0x40001950;
rom_i2c_readReg = 0x40001954;
rom_i2c_readReg_Mask = 0x40001958;
rom_i2c_writeReg = 0x4000195c;
rom_i2c_writeReg_Mask = 0x40001960;
/* rom_index_to_txbbgain = 0x40001964; */
rom_iq_est_disable = 0x40001968;
rom_iq_est_enable = 0x4000196c;
rom_linear_to_db = 0x40001970;
rom_loopback_mode_en = 0x40001974;
rom_mhz2ieee = 0x40001978;
rom_noise_floor_auto_set = 0x4000197c;
rom_pbus_debugmode = 0x40001980;
rom_pbus_force_mode = 0x40001984;
rom_pbus_force_test = 0x40001988;
rom_pbus_rd = 0x4000198c;
rom_pbus_rd_addr = 0x40001990;
rom_pbus_rd_shift = 0x40001994;
rom_pbus_set_dco = 0x40001998;
rom_pbus_set_rxgain = 0x4000199c;
rom_pbus_workmode = 0x400019a0;
rom_pbus_xpd_rx_off = 0x400019a4;
rom_pbus_xpd_rx_on = 0x400019a8;
rom_pbus_xpd_tx_off = 0x400019ac;
/* rom_pbus_xpd_tx_on = 0x400019b0; */
rom_phy_byte_to_word = 0x400019b4;
rom_phy_disable_cca = 0x400019b8;
rom_phy_enable_cca = 0x400019bc;
rom_phy_get_noisefloor = 0x400019c0;
rom_phy_get_rx_freq = 0x400019c4;
rom_phy_set_bbfreq_init = 0x400019c8;
rom_pow_usr = 0x400019cc;
rom_pwdet_sar2_init = 0x400019d0;
rom_read_hw_noisefloor = 0x400019d4;
rom_read_sar_dout = 0x400019d8;
rom_set_cal_rxdc = 0x400019dc;
rom_set_chan_cal_interp = 0x400019e0;
rom_set_loopback_gain = 0x400019e4;
rom_set_noise_floor = 0x400019e8;
rom_set_rxclk_en = 0x400019ec;
/* rom_set_tx_dig_gain = 0x400019f0; */
/* rom_set_txcap_reg = 0x400019f4; */
rom_set_txclk_en = 0x400019f8;
rom_spur_cal = 0x400019fc;
rom_spur_reg_write_one_tone = 0x40001a00;
rom_target_power_add_backoff = 0x40001a04;
rom_tx_pwctrl_bg_init = 0x40001a08;
/* rom_txbbgain_to_index = 0x40001a0c; */
rom_wifi_11g_rate_chg = 0x40001a10;
rom_write_gain_mem = 0x40001a14;
chip726_phyrom_version = 0x40001a18;
rom_disable_wifi_agc = 0x40001a1c;
rom_enable_wifi_agc = 0x40001a20;
rom_set_tx_gain_table = 0x40001a24;
rom_bt_index_to_bb = 0x40001a28;
rom_bt_bb_to_index = 0x40001a2c;
rom_wr_bt_tx_atten = 0x40001a30;
rom_wr_bt_tx_gain_mem = 0x40001a34;
rom_spur_coef_cfg = 0x40001a38;
rom_bb_bss_cbw40 = 0x40001a3c;
rom_set_cca = 0x40001a40;
rom_tx_paon_set = 0x40001a44;
rom_i2cmst_reg_init = 0x40001a48;
rom_iq_corr_enable = 0x40001a4c;
rom_fe_reg_init = 0x40001a50;
/* rom_agc_reg_init = 0x40001a54; */
/* rom_bb_reg_init = 0x40001a58; */
rom_mac_enable_bb = 0x40001a5c;
rom_bb_wdg_cfg = 0x40001a60;
rom_force_txon = 0x40001a64;
rom_fe_txrx_reset = 0x40001a68;
rom_set_rx_comp = 0x40001a6c;
/* rom_set_pbus_reg = 0x40001a70; */
rom_write_chan_freq = 0x40001a74;
/* rom_phy_xpd_rf = 0x40001a78; */
rom_set_xpd_sar = 0x40001a7c;
rom_write_dac_gain2 = 0x40001a80;
rom_rtc_sar2_init = 0x40001a84;
rom_get_target_power_offset = 0x40001a88;
/* rom_write_txrate_power_offset = 0x40001a8c; */
rom_get_rate_fcc_index = 0x40001a90;
rom_get_rate_target_power = 0x40001a94;
rom_write_wifi_dig_gain = 0x40001a98;
rom_bt_correct_rf_ana_gain = 0x40001a9c;
rom_pkdet_vol_start = 0x40001aa0;
rom_read_sar2_code = 0x40001aa4;
rom_get_sar2_vol = 0x40001aa8;
rom_get_pll_vol = 0x40001aac;
rom_get_phy_target_power = 0x40001ab0;
/* rom_temp_to_power = 0x40001ab4; */
rom_phy_track_pll_cap = 0x40001ab8;
rom_phy_pwdet_always_en = 0x40001abc;
rom_phy_pwdet_onetime_en = 0x40001ac0;
rom_get_i2c_mst0_mask = 0x40001ac4;
rom_get_i2c_hostid = 0x40001ac8;
rom_enter_critical_phy = 0x40001acc;
rom_exit_critical_phy = 0x40001ad0;
rom_chip_i2c_readReg_org = 0x40001ad4;
rom_i2c_paral_set_mst0 = 0x40001ad8;
rom_i2c_paral_set_read = 0x40001adc;
rom_i2c_paral_read = 0x40001ae0;
rom_i2c_paral_write = 0x40001ae4;
rom_i2c_paral_write_num = 0x40001ae8;
rom_i2c_paral_write_mask = 0x40001aec;
rom_bb_bss_cbw40_ana = 0x40001af0;
rom_chan_to_freq = 0x40001af4;
/* rom_open_i2c_xpd = 0x40001af8; */
rom_dac_rate_set = 0x40001afc;
/* rom_tsens_read_init = 0x40001b00; */
/* rom_tsens_code_read = 0x40001b04; */
rom_tsens_index_to_dac = 0x40001b08;
rom_tsens_index_to_offset = 0x40001b0c;
/* rom_tsens_dac_cal = 0x40001b10; */
rom_code_to_temp = 0x40001b14;
rom_write_pll_cap_mem = 0x40001b18;
rom_pll_correct_dcap = 0x40001b1c;
rom_phy_en_hw_set_freq = 0x40001b20;
rom_phy_dis_hw_set_freq = 0x40001b24;
/* rom_pll_vol_cal = 0x40001b28; */
