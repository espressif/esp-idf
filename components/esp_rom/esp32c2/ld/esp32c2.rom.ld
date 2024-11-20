/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface esp32c2.rom.ld for esp32c2
 *
 *
 * Generated from ./interface-esp32c2.yml md5sum c679b6ed5e9f0a9c3e7b93e5e0f2a1a3
 *
 * Compatible with ROM where ECO version equal or greater to 1.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group common
 ***************************************/

/* Functions */
rtc_get_reset_reason = 0x40000018;
analog_super_wdt_reset_happened = 0x4000001c;
rtc_get_wakeup_cause = 0x40000020;
rtc_select_apb_bridge = 0x40000024;
rtc_unhold_all_pads = 0x40000028;
ets_is_print_boot = 0x4000002c;
ets_vprintf = 0x40000030;
ets_printf = 0x40000034;
ets_install_putc1 = 0x40000038;
ets_install_uart_printf = 0x4000003c;
ets_install_putc2 = 0x40000040;
ets_delay_us = 0x40000044;
ets_get_stack_info = 0x40000048;
ets_install_lock = 0x4000004c;
UartRxString = 0x40000050;
UartGetCmdLn = 0x40000054;
uart_tx_one_char = 0x40000058;
uart_tx_one_char2 = 0x4000005c;
uart_rx_one_char = 0x40000060;
uart_rx_one_char_block = 0x40000064;
uart_rx_readbuff = 0x40000068;
uartAttach = 0x4000006c;
uart_tx_flush = 0x40000070;
uart_tx_wait_idle = 0x40000074;
uart_div_modify = 0x40000078;
ets_write_char_uart = 0x4000007c;
uart_tx_switch = 0x40000080;
multofup = 0x40000084;
software_reset = 0x40000088;
software_reset_cpu = 0x4000008c;
assist_debug_clock_enable = 0x40000090;
assist_debug_record_enable = 0x40000094;
clear_super_wdt_reset_flag = 0x40000098;
disable_default_watchdog = 0x4000009c;
send_packet = 0x400000a0;
recv_packet = 0x400000a4;
GetUartDevice = 0x400000a8;
UartDwnLdProc = 0x400000ac;
GetSecurityInfoProc = 0x400000b0;
Uart_Init = 0x400000b4;
ets_set_user_start = 0x400000b8;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x3ff4fffc;
ets_ops_table_ptr = 0x3fcdfffc;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x400000bc;
mz_free = 0x400000c0;
tdefl_compress = 0x400000c4;
tdefl_compress_buffer = 0x400000c8;
tdefl_compress_mem_to_heap = 0x400000cc;
tdefl_compress_mem_to_mem = 0x400000d0;
tdefl_compress_mem_to_output = 0x400000d4;
tdefl_get_adler32 = 0x400000d8;
tdefl_get_prev_return_status = 0x400000dc;
tdefl_init = 0x400000e0;
tdefl_write_image_to_png_file_in_memory = 0x400000e4;
tdefl_write_image_to_png_file_in_memory_ex = 0x400000e8;
tinfl_decompress = 0x400000ec;
tinfl_decompress_mem_to_callback = 0x400000f0;
tinfl_decompress_mem_to_heap = 0x400000f4;
tinfl_decompress_mem_to_mem = 0x400000f8;


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
PROVIDE( esp_rom_spiflash_wait_idle = 0x400000fc );
PROVIDE( esp_rom_spiflash_write_encrypted = 0x40000100 );
PROVIDE( esp_rom_spiflash_write_encrypted_dest = 0x40000104 );
PROVIDE( esp_rom_spiflash_write_encrypted_enable = 0x40000108 );
PROVIDE( esp_rom_spiflash_write_encrypted_disable = 0x4000010c );
PROVIDE( esp_rom_spiflash_erase_chip = 0x40000110 );
PROVIDE( _esp_rom_spiflash_erase_sector = 0x40000114 );
PROVIDE( _esp_rom_spiflash_erase_block = 0x40000118 );
PROVIDE( _esp_rom_spiflash_write = 0x4000011c );
PROVIDE( _esp_rom_spiflash_read = 0x40000120 );
PROVIDE( _esp_rom_spiflash_unlock = 0x40000124 );
PROVIDE( _SPIEraseArea = 0x40000128 );
PROVIDE( _SPI_write_enable = 0x4000012c );
PROVIDE( esp_rom_spiflash_erase_sector = 0x40000130 );
PROVIDE( esp_rom_spiflash_erase_block = 0x40000134 );
PROVIDE( esp_rom_spiflash_write = 0x40000138 );
PROVIDE( esp_rom_spiflash_read = 0x4000013c );
PROVIDE( esp_rom_spiflash_unlock = 0x40000140 );
PROVIDE( SPIEraseArea = 0x40000144 );
PROVIDE( SPI_write_enable = 0x40000148 );
PROVIDE( esp_rom_spiflash_config_param = 0x4000014c );
PROVIDE( esp_rom_spiflash_read_user_cmd = 0x40000150 );
PROVIDE( esp_rom_spiflash_select_qio_pins = 0x40000154 );
PROVIDE( esp_rom_spi_flash_auto_sus_res = 0x40000158 );
PROVIDE( esp_rom_spi_flash_send_resume = 0x4000015c );
PROVIDE( esp_rom_spi_flash_update_id = 0x40000160 );
PROVIDE( esp_rom_spiflash_config_clk = 0x40000164 );
PROVIDE( esp_rom_spiflash_config_readmode = 0x40000168 );
PROVIDE( esp_rom_spiflash_read_status = 0x4000016c );
PROVIDE( esp_rom_spiflash_read_statushigh = 0x40000170 );
PROVIDE( esp_rom_spiflash_write_status = 0x40000174 );
PROVIDE( spi_flash_attach = 0x40000178 );
PROVIDE( spi_flash_get_chip_size = 0x4000017c );
PROVIDE( spi_flash_guard_set = 0x40000180 );
PROVIDE( spi_flash_guard_get = 0x40000184 );
PROVIDE( spi_flash_read_encrypted = 0x40000188 );
PROVIDE( spi_flash_mmap_os_func_set = 0x4000018c );
PROVIDE( spi_flash_mmap_page_num_init = 0x40000190 );
PROVIDE( spi_flash_mmap = 0x40000194 );
PROVIDE( spi_flash_mmap_pages = 0x40000198 );
PROVIDE( spi_flash_munmap = 0x4000019c );
PROVIDE( spi_flash_mmap_dump = 0x400001a0 );
PROVIDE( spi_flash_check_and_flush_cache = 0x400001a4 );
PROVIDE( spi_flash_mmap_get_free_pages = 0x400001a8 );
PROVIDE( spi_flash_cache2phys = 0x400001ac );
PROVIDE( spi_flash_phys2cache = 0x400001b0 );
PROVIDE( spi_flash_disable_cache = 0x400001b4 );
PROVIDE( spi_flash_restore_cache = 0x400001b8 );
PROVIDE( spi_flash_cache_enabled = 0x400001bc );
PROVIDE( spi_flash_enable_cache = 0x400001c0 );
PROVIDE( spi_cache_mode_switch = 0x400001c4 );
PROVIDE( spi_common_set_dummy_output = 0x400001c8 );
PROVIDE( spi_common_set_flash_cs_timing = 0x400001cc );
PROVIDE( esp_rom_spi_set_address_bit_len = 0x400001d0 );
PROVIDE( esp_enable_cache_flash_wrap = 0x400001d4 );
PROVIDE( SPILock = 0x400001d8 );
PROVIDE( SPIMasterReadModeCnfig = 0x400001dc );
PROVIDE( SPI_Common_Command = 0x400001e0 );
PROVIDE( SPI_WakeUp = 0x400001e4 );
PROVIDE( SPI_block_erase = 0x400001e8 );
PROVIDE( SPI_chip_erase = 0x400001ec );
PROVIDE( SPI_init = 0x400001f0 );
PROVIDE( SPI_page_program = 0x400001f4 );
PROVIDE( SPI_read_data = 0x400001f8 );
PROVIDE( SPI_sector_erase = 0x400001fc );
PROVIDE( SelectSpiFunction = 0x40000200 );
PROVIDE( SetSpiDrvs = 0x40000204 );
PROVIDE( Wait_SPI_Idle = 0x40000208 );
PROVIDE( spi_dummy_len_fix = 0x4000020c );
PROVIDE( Disable_QMode = 0x40000210 );
PROVIDE( Enable_QMode = 0x40000214 );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_spiflash_legacy_funcs = 0x3fcdfff4 );
PROVIDE( rom_spiflash_legacy_data = 0x3fcdfff0 );
PROVIDE( g_flash_guard_ops = 0x3fcdfff8 );


/***************************************
 Group hal_soc
 ***************************************/

/* Functions */
PROVIDE( spi_flash_hal_poll_cmd_done = 0x40000218 );
PROVIDE( spi_flash_hal_device_config = 0x4000021c );
PROVIDE( spi_flash_hal_configure_host_io_mode = 0x40000220 );
PROVIDE( spi_flash_hal_common_command = 0x40000224 );
PROVIDE( spi_flash_hal_read = 0x40000228 );
PROVIDE( spi_flash_hal_erase_chip = 0x4000022c );
PROVIDE( spi_flash_hal_erase_sector = 0x40000230 );
PROVIDE( spi_flash_hal_erase_block = 0x40000234 );
PROVIDE( spi_flash_hal_program_page = 0x40000238 );
PROVIDE( spi_flash_hal_set_write_protect = 0x4000023c );
PROVIDE( spi_flash_hal_host_idle = 0x40000240 );
PROVIDE( spi_flash_hal_check_status = 0x40000244 );
PROVIDE( spi_flash_hal_setup_read_suspend = 0x40000248 );
PROVIDE( spi_flash_hal_setup_auto_suspend_mode = 0x4000024c );
PROVIDE( spi_flash_hal_setup_auto_resume_mode = 0x40000250 );
PROVIDE( spi_flash_hal_disable_auto_suspend_mode = 0x40000254 );
PROVIDE( spi_flash_hal_disable_auto_resume_mode = 0x40000258 );
PROVIDE( spi_flash_hal_resume = 0x4000025c );
PROVIDE( spi_flash_hal_suspend = 0x40000260 );
PROVIDE( spi_flash_encryption_hal_enable = 0x40000264 );
PROVIDE( spi_flash_encryption_hal_disable = 0x40000268 );
PROVIDE( spi_flash_encryption_hal_prepare = 0x4000026c );
PROVIDE( spi_flash_encryption_hal_done = 0x40000270 );
PROVIDE( spi_flash_encryption_hal_destroy = 0x40000274 );
PROVIDE( spi_flash_encryption_hal_check = 0x40000278 );


/***************************************
 Group spi_flash_chips
 ***************************************/

/* Functions */
PROVIDE( spi_flash_chip_generic_probe = 0x40000380 );
PROVIDE( spi_flash_chip_generic_detect_size = 0x40000384 );
PROVIDE( spi_flash_chip_generic_write = 0x40000388 );
PROVIDE( spi_flash_chip_generic_write_encrypted = 0x4000038c );
PROVIDE( spi_flash_chip_generic_set_write_protect = 0x40000390 );
PROVIDE( spi_flash_common_write_status_16b_wrsr = 0x40000394 );
PROVIDE( spi_flash_chip_generic_reset = 0x40000398 );
PROVIDE( spi_flash_chip_generic_erase_chip = 0x4000039c );
PROVIDE( spi_flash_chip_generic_erase_sector = 0x400003a0 );
PROVIDE( spi_flash_chip_generic_erase_block = 0x400003a4 );
PROVIDE( spi_flash_chip_generic_page_program = 0x400003a8 );
PROVIDE( spi_flash_chip_generic_get_write_protect = 0x400003ac );
PROVIDE( spi_flash_common_read_status_16b_rdsr_rdsr2 = 0x400003b0 );
PROVIDE( spi_flash_chip_generic_read_reg = 0x400003b4 );
PROVIDE( spi_flash_chip_generic_yield = 0x400003b8 );
PROVIDE( spi_flash_generic_wait_host_idle = 0x400003bc );
PROVIDE( spi_flash_chip_generic_wait_idle = 0x400003c0 );
PROVIDE( spi_flash_chip_generic_config_host_io_mode = 0x400003c4 );
PROVIDE( spi_flash_chip_generic_read = 0x400003c8 );
PROVIDE( spi_flash_common_read_status_8b_rdsr2 = 0x400003cc );
PROVIDE( spi_flash_chip_generic_get_io_mode = 0x400003d0 );
PROVIDE( spi_flash_common_read_status_8b_rdsr = 0x400003d4 );
PROVIDE( spi_flash_common_write_status_8b_wrsr = 0x400003d8 );
PROVIDE( spi_flash_common_write_status_8b_wrsr2 = 0x400003dc );
PROVIDE( spi_flash_common_set_io_mode = 0x400003e0 );
PROVIDE( spi_flash_chip_generic_set_io_mode = 0x400003e4 );
PROVIDE( spi_flash_chip_generic_read_unique_id = 0x400003e8 );
PROVIDE( spi_flash_chip_generic_get_caps = 0x400003ec );
PROVIDE( spi_flash_chip_generic_suspend_cmd_conf = 0x400003f0 );
PROVIDE( spi_flash_chip_gd_get_io_mode = 0x400003f4 );
PROVIDE( spi_flash_chip_gd_probe = 0x400003f8 );
PROVIDE( spi_flash_chip_gd_set_io_mode = 0x400003fc );
/* Data (.data, .bss, .rodata) */
PROVIDE( spi_flash_chip_generic_config_data = 0x3fcdffe8 );
PROVIDE( spi_flash_encryption = 0x3fcdffe4 );


/***************************************
 Group memspi_host
 ***************************************/

/* Functions */
PROVIDE( memspi_host_read_id_hs = 0x40000400 );
PROVIDE( memspi_host_read_status_hs = 0x40000404 );
PROVIDE( memspi_host_flush_cache = 0x40000408 );
PROVIDE( memspi_host_erase_chip = 0x4000040c );
PROVIDE( memspi_host_erase_sector = 0x40000410 );
PROVIDE( memspi_host_erase_block = 0x40000414 );
PROVIDE( memspi_host_program_page = 0x40000418 );
PROVIDE( memspi_host_read = 0x4000041c );
PROVIDE( memspi_host_set_write_protect = 0x40000420 );
PROVIDE( memspi_host_set_max_read_len = 0x40000424 );
PROVIDE( memspi_host_read_data_slicer = 0x40000428 );
PROVIDE( memspi_host_write_data_slicer = 0x4000042c );


/***************************************
 Group esp_flash
 ***************************************/

/* Functions */
PROVIDE( esp_flash_chip_driver_initialized = 0x40000430 );
PROVIDE( esp_flash_read_id = 0x40000434 );
PROVIDE( esp_flash_get_size = 0x40000438 );
PROVIDE( esp_flash_erase_chip = 0x4000043c );
PROVIDE( esp_flash_erase_region = 0x40000440 );
PROVIDE( esp_flash_get_chip_write_protect = 0x40000444 );
PROVIDE( esp_flash_set_chip_write_protect = 0x40000448 );
PROVIDE( esp_flash_get_protectable_regions = 0x4000044c );
PROVIDE( esp_flash_get_protected_region = 0x40000450 );
PROVIDE( esp_flash_set_protected_region = 0x40000454 );
PROVIDE( esp_flash_read = 0x40000458 );
PROVIDE( esp_flash_write = 0x4000045c );
PROVIDE( esp_flash_write_encrypted = 0x40000460 );
PROVIDE( esp_flash_read_encrypted = 0x40000464 );
PROVIDE( esp_flash_get_io_mode = 0x40000468 );
PROVIDE( esp_flash_set_io_mode = 0x4000046c );
PROVIDE( spi_flash_boot_attach = 0x40000470 );
PROVIDE( esp_flash_read_chip_id = 0x40000474 );
PROVIDE( detect_spi_flash_chip = 0x40000478 );
PROVIDE( esp_rom_spiflash_write_disable = 0x4000047c );
PROVIDE( esp_flash_suspend_cmd_init = 0x40000480 );
/* Data (.data, .bss, .rodata) */
PROVIDE( esp_flash_default_chip = 0x3fcdffe0 );
PROVIDE( esp_flash_api_funcs = 0x3fcdffdc );


/***************************************
 Group cache
 ***************************************/

/* Functions */
PROVIDE( Cache_Get_ICache_Line_Size = 0x400006e0 );
PROVIDE( Cache_Get_Mode = 0x400006e4 );
PROVIDE( Cache_Address_Through_IBus = 0x400006e8 );
PROVIDE( Cache_Address_Through_DBus = 0x400006ec );
PROVIDE( Cache_Set_Default_Mode = 0x400006f0 );
PROVIDE( Cache_Enable_Defalut_ICache_Mode = 0x400006f4 );
PROVIDE( ROM_Boot_Cache_Init = 0x400006f8 );
PROVIDE( MMU_Set_Page_Mode = 0x400006fc );
PROVIDE( MMU_Get_Page_Mode = 0x40000700 );
PROVIDE( Cache_Invalidate_ICache_Items = 0x40000704 );
PROVIDE( Cache_Op_Addr = 0x40000708 );
PROVIDE( Cache_Invalidate_Addr = 0x4000070c );
PROVIDE( Cache_Invalidate_ICache_All = 0x40000710 );
PROVIDE( Cache_Mask_All = 0x40000714 );
PROVIDE( Cache_UnMask_Dram0 = 0x40000718 );
PROVIDE( Cache_Disable_ICache = 0x4000071c );
PROVIDE( Cache_Enable_ICache = 0x40000720 );
PROVIDE( Cache_Suspend_ICache = 0x40000724 );
PROVIDE( Cache_Resume_ICache = 0x40000728 );
PROVIDE( Cache_Freeze_ICache_Enable = 0x4000072c );
PROVIDE( Cache_Freeze_ICache_Disable = 0x40000730 );
PROVIDE( Cache_Set_IDROM_MMU_Size = 0x40000734 );
PROVIDE( Cache_Get_IROM_MMU_End = 0x40000738 );
PROVIDE( Cache_Get_DROM_MMU_End = 0x4000073c );
PROVIDE( Cache_Owner_Init = 0x40000740 );
PROVIDE( Cache_Occupy_ICache_MEMORY = 0x40000744 );
PROVIDE( Cache_MMU_Init = 0x40000748 );
PROVIDE( Cache_Ibus_MMU_Set = 0x4000074c );
PROVIDE( Cache_Dbus_MMU_Set = 0x40000750 );
PROVIDE( Cache_Count_Flash_Pages = 0x40000754 );
PROVIDE( Cache_Travel_Tag_Memory = 0x40000758 );
PROVIDE( Cache_Get_Virtual_Addr = 0x4000075c );
PROVIDE( Cache_Get_Memory_BaseAddr = 0x40000760 );
PROVIDE( Cache_Get_Memory_Addr = 0x40000764 );
PROVIDE( Cache_Get_Memory_value = 0x40000768 );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_cache_op_cb = 0x3fcdffd0 );
PROVIDE( rom_cache_internal_table_ptr = 0x3fcdffcc );


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_get_apb_freq = 0x4000076c;
ets_get_cpu_frequency = 0x40000770;
ets_update_cpu_frequency = 0x40000774;
ets_get_printf_channel = 0x40000778;
ets_get_xtal_div = 0x4000077c;
ets_set_xtal_div = 0x40000780;
ets_get_xtal_freq = 0x40000784;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_input_get = 0x40000788;
gpio_matrix_in = 0x4000078c;
gpio_matrix_out = 0x40000790;
/* gpio_output_disable = 0x40000794; */
/* gpio_output_enable = 0x40000798; */
gpio_output_set = 0x4000079c;
gpio_pad_hold = 0x400007a0;
gpio_pad_input_disable = 0x400007a4;
gpio_pad_input_enable = 0x400007a8;
gpio_pad_pulldown = 0x400007ac;
gpio_pad_pullup = 0x400007b0;
gpio_pad_select_gpio = 0x400007b4;
gpio_pad_set_drv = 0x400007b8;
gpio_pad_unhold = 0x400007bc;
gpio_pin_wakeup_disable = 0x400007c0;
gpio_pin_wakeup_enable = 0x400007c4;
gpio_bypass_matrix_in = 0x400007c8;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x400007cc;
esprv_intc_int_set_threshold = 0x400007d0;
esprv_intc_int_enable = 0x400007d4;
esprv_intc_int_disable = 0x400007d8;
esprv_intc_int_set_type = 0x400007dc;
PROVIDE( intr_handler_set = 0x400007e0 );
intr_matrix_set = 0x400007e4;
ets_intr_lock = 0x400007e8;
ets_intr_unlock = 0x400007ec;
ets_isr_attach = 0x400007f0;
ets_isr_mask = 0x400007f4;
ets_isr_unmask = 0x400007f8;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
crc32_le = 0x400007fc;
crc16_le = 0x40000800;
crc8_le = 0x40000804;
crc32_be = 0x40000808;
crc16_be = 0x4000080c;
crc8_be = 0x40000810;
esp_crc8 = 0x40000814;
ets_sha_enable = 0x40000818;
ets_sha_disable = 0x4000081c;
ets_sha_get_state = 0x40000820;
ets_sha_init = 0x40000824;
ets_sha_process = 0x40000828;
ets_sha_starts = 0x4000082c;
ets_sha_update = 0x40000830;
ets_sha_finish = 0x40000834;
ets_sha_clone = 0x40000838;
/* Data (.data, .bss, .rodata) */
crc32_le_table_ptr = 0x3ff4fff8;
crc16_le_table_ptr = 0x3ff4fff4;
crc8_le_table_ptr = 0x3ff4fff0;
crc32_be_table_ptr = 0x3ff4ffec;
crc16_be_table_ptr = 0x3ff4ffe8;
crc8_be_table_ptr = 0x3ff4ffe4;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x4000083c;
ets_efuse_program = 0x40000840;
ets_efuse_clear_program_registers = 0x40000844;
ets_efuse_write_key = 0x40000848;
ets_efuse_get_read_register_address = 0x4000084c;
ets_efuse_get_key_purpose = 0x40000850;
ets_efuse_key_block_unused = 0x40000854;
ets_efuse_find_unused_key_block = 0x40000858;
ets_efuse_rs_calculate = 0x4000085c;
ets_efuse_count_unused_key_blocks = 0x40000860;
ets_efuse_secure_boot_enabled = 0x40000864;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x40000868;
ets_efuse_cache_encryption_enabled = 0x4000086c;
ets_efuse_download_modes_disabled = 0x40000870;
ets_efuse_find_purpose = 0x40000874;
ets_efuse_force_send_resume = 0x40000878;
ets_efuse_get_flash_delay_us = 0x4000087c;
ets_efuse_get_mac = 0x40000880;
ets_efuse_get_uart_print_control = 0x40000884;
ets_efuse_direct_boot_mode_disabled = 0x40000888;
ets_efuse_security_download_modes_enabled = 0x4000088c;
ets_efuse_set_timing = 0x40000890;
ets_efuse_jtag_disabled = 0x40000894;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_ecdsa_verify = 0x40000898;
ets_secure_boot_verify_bootloader_with_keys = 0x4000089c;
ets_secure_boot_verify_signature = 0x400008a0;
ets_secure_boot_read_key_digests = 0x400008a4;


/***************************************
 Group usb_uart
 ***************************************/

/* Data (.data, .bss, .rodata) */
g_uart_print = 0x3fcdffc9;
g_usb_print = 0x3fcdffc8;


/***************************************
 Group rom_pp
 ***************************************/

/* Functions */
esp_pp_rom_version_get = 0x40001ad4;
RC_GetBlockAckTime = 0x40001ad8;
ebuf_list_remove = 0x40001adc;
/*esf_buf_alloc = 0x40001ae0;*/
/*esf_buf_alloc_dynamic = 0x40001ae4;*/
/*esf_buf_recycle = 0x40001ae8;*/
GetAccess = 0x40001aec;
hal_mac_is_low_rate_enabled = 0x40001af0;
hal_mac_tx_get_blockack = 0x40001af4;
/*hal_mac_tx_set_ppdu = 0x40001af8;*/
ic_get_trc = 0x40001afc;
/* ic_mac_deinit = 0x40001b00; */
ic_mac_init = 0x40001b04;
ic_interface_enabled = 0x40001b08;
is_lmac_idle = 0x40001b0c;
/*lmacAdjustTimestamp = 0x40001b10;*/
lmacDiscardAgedMSDU = 0x40001b14;
/*lmacDiscardMSDU = 0x40001b18;*/
/*lmacEndFrameExchangeSequence = 0x40001b1c;*/
lmacIsIdle = 0x40001b20;
lmacIsLongFrame = 0x40001b24;
/*lmacMSDUAged = 0x40001b28;*/
lmacPostTxComplete = 0x40001b2c;
lmacProcessAllTxTimeout = 0x40001b30;
lmacProcessCollisions = 0x40001b34;
lmacProcessRxSucData = 0x40001b38;
lmacReachLongLimit = 0x40001b3c;
lmacReachShortLimit = 0x40001b40;
lmacRecycleMPDU = 0x40001b44;
lmacRxDone = 0x40001b48;
/*lmacSetTxFrame = 0x40001b4c;*/
lmacTxDone = 0x40001b50;
lmacTxFrame = 0x40001b54;
mac_tx_set_duration = 0x40001b58;
mac_tx_set_htsig = 0x40001b5c;
mac_tx_set_plcp0 = 0x40001b60;
mac_tx_set_plcp1 = 0x40001b64;
mac_tx_set_plcp2 = 0x40001b68;
/* pm_check_state = 0x40001b6c; */
pm_disable_dream_timer = 0x40001b70;
pm_disable_sleep_delay_timer = 0x40001b74;
/*pm_dream = 0x40001b78;*/
pm_mac_wakeup = 0x40001b7c;
pm_mac_sleep = 0x40001b80;
pm_enable_active_timer = 0x40001b84;
pm_enable_sleep_delay_timer = 0x40001b88;
pm_local_tsf_process = 0x40001b8c;
pm_set_beacon_filter = 0x40001b90;
pm_is_in_wifi_slice_threshold = 0x40001b94;
pm_is_waked = 0x40001b98;
pm_keep_alive = 0x40001b9c;
/* pm_on_beacon_rx = 0x40001ba0; */
pm_on_data_rx = 0x40001ba4;
pm_on_tbtt = 0x40001ba8;
/* pm_parse_beacon = 0x40001bac;*/
/* pm_process_tim = 0x40001bb0; */
/*pm_rx_beacon_process = 0x40001bb4;*/
/* pm_rx_data_process = 0x40001bb8; */
/*pm_sleep = 0x40001bbc;*/
pm_sleep_for = 0x40001bc0;
/* pm_tbtt_process = 0x40001bc4; */
ppAMPDU2Normal = 0x40001bc8;
/*ppAssembleAMPDU = 0x40001bcc;*/
ppCalFrameTimes = 0x40001bd0;
ppCalSubFrameLength = 0x40001bd4;
/*ppCalTxAMPDULength = 0x40001bd8;*/
ppCheckTxAMPDUlength = 0x40001bdc;
ppDequeueRxq_Locked = 0x40001be0;
ppDequeueTxQ = 0x40001be4;
ppEmptyDelimiterLength = 0x40001be8;
ppEnqueueRxq = 0x40001bec;
ppEnqueueTxDone = 0x40001bf0;
ppGetTxQFirstAvail_Locked = 0x40001bf4;
ppGetTxframe = 0x40001bf8;
ppMapTxQueue = 0x40001bfc;
ppProcTxSecFrame = 0x40001c00;
ppProcessRxPktHdr = 0x40001c04;
/*ppProcessTxQ = 0x40001c08;*/
ppRecordBarRRC = 0x40001c0c;
lmacRequestTxopQueue = 0x40001c10;
lmacReleaseTxopQueue = 0x40001c14;
ppRecycleAmpdu = 0x40001c18;
ppRecycleRxPkt = 0x40001c1c;
ppResortTxAMPDU = 0x40001c20;
ppResumeTxAMPDU = 0x40001c24;
/*ppRxFragmentProc = 0x40001c28;*/
/* ppRxPkt = 0x40001c2c; */
ppRxProtoProc = 0x40001c30;
ppSearchTxQueue = 0x40001c34;
ppSearchTxframe = 0x40001c38;
ppSelectNextQueue = 0x40001c3c;
ppSubFromAMPDU = 0x40001c40;
ppTask = 0x40001c44;
ppTxPkt = 0x40001c48;
ppTxProtoProc = 0x40001c4c;
ppTxqUpdateBitmap = 0x40001c50;
/*pp_coex_tx_request = 0x40001c54;*/
pp_hdrsize = 0x40001c58;
pp_post = 0x40001c5c;
pp_process_hmac_waiting_txq = 0x40001c60;
rcGetAmpduSched = 0x40001c64;
rcUpdateRxDone = 0x40001c68;
rc_get_trc = 0x40001c6c;
rc_get_trc_by_index = 0x40001c70;
rcAmpduLowerRate = 0x40001c74;
rcampduuprate = 0x40001c78;
rcClearCurAMPDUSched = 0x40001c7c;
rcClearCurSched = 0x40001c80;
rcClearCurStat = 0x40001c84;
/*rcGetSched = 0x40001c88;*/
rcLowerSched = 0x40001c8c;
rcSetTxAmpduLimit = 0x40001c90;
/* rcTxUpdatePer = 0x40001c94;*/
rcUpdateAckSnr = 0x40001c98;
/*rcUpdateRate = 0x40001c9c;*/
/*rcUpdateTxDone = 0x40001ca0;*/
/*rcUpdateTxDoneAmpdu2 = 0x40001ca4;*/
rcUpSched = 0x40001ca8;
rssi_margin = 0x40001cac;
rx11NRate2AMPDULimit = 0x40001cb0;
TRC_AMPDU_PER_DOWN_THRESHOLD = 0x40001cb4;
TRC_AMPDU_PER_UP_THRESHOLD = 0x40001cb8;
trc_calc_duration = 0x40001cbc;
trc_isTxAmpduOperational = 0x40001cc0;
trc_onAmpduOp = 0x40001cc4;
TRC_PER_IS_GOOD = 0x40001cc8;
trc_SetTxAmpduState = 0x40001ccc;
trc_tid_isTxAmpduOperational = 0x40001cd0;
trcAmpduSetState = 0x40001cd4;
wDevCheckBlockError = 0x40001cd8;
/*wDev_AppendRxBlocks = 0x40001cdc;*/
wDev_DiscardFrame = 0x40001ce0;
wDev_GetNoiseFloor = 0x40001ce4;
wDev_IndicateAmpdu = 0x40001ce8;
/*wDev_IndicateFrame = 0x40001cec;*/
wdev_mac_reg_load = 0x40001cf0;
wdev_mac_reg_store = 0x40001cf4;
wdev_mac_special_reg_load = 0x40001cf8;
wdev_mac_special_reg_store = 0x40001cfc;
wdev_mac_wakeup = 0x40001d00;
wdev_mac_sleep = 0x40001d04;
/* wDev_ProcessFiq = 0x40001d08; */
/*wDev_ProcessRxSucData = 0x40001d0c;*/
wdevProcessRxSucDataAll = 0x40001d10;
wdev_csi_len_align = 0x40001d14;
ppDequeueTxDone_Locked = 0x40001d18;
/*ppProcTxDone = 0x40001d1c;*/
/*pm_tx_data_done_process = 0x40001d20;*/
config_is_cache_tx_buf_enabled = 0x40001d24;
//ppMapWaitTxq = 0x40001d28;
ppProcessWaitingQueue = 0x40001d2c;
ppDisableQueue = 0x40001d30;
pm_allow_tx = 0x40001d34;
wdev_is_data_in_rxlist = 0x40001d38;
ppProcTxCallback = 0x40001d3c;
pm_is_open = 0x40001d40;
pm_wake_up = 0x40001d44;
pm_wake_done = 0x40001d48;
pm_disable_disconnected_sleep_delay_timer = 0x40001d4c;
pm_enable_disconnected_sleep_delay_timer = 0x40001d50;
hal_mac_get_txq_state = 0x40001d54;
hal_mac_clr_txq_state = 0x40001d58;
hal_mac_tx_set_cca = 0x40001d5c;
hal_mac_set_txq_invalid = 0x40001d60;
hal_mac_txq_disable = 0x40001d64;
hal_mac_is_txq_enabled = 0x40001d68;
hal_mac_get_txq_pmd = 0x40001d6c;
/*lmacDiscardFrameExchangeSequence = 0x40001d70;*/
/*lmacDisableTransmit = 0x40001d74;*/
/*lmacProcessTxTimeout = 0x40001d78;*/
/*lmacProcessTxSuccess = 0x40001d7c;*/
lmacProcessCollision = 0x40001d80;
lmacProcessTxRtsError = 0x40001d84;
lmacProcessCtsTimeout = 0x40001d88;
/* lmacProcessTxComplete = 0x40001d8c;*/
lmacProcessAckTimeout = 0x40001d90;
lmacProcessTxError = 0x40001d94;
lmacProcessTxseckiderr = 0x40001d98;
rcReachRetryLimit = 0x40001d9c;
lmacProcessShortRetryFail = 0x40001da0;
lmacEndRetryAMPDUFail = 0x40001da4;
ppFillAMPDUBar = 0x40001da8;
rcGetRate = 0x40001dac;
ppReSendBar = 0x40001db0;
lmacProcessLongRetryFail = 0x40001db4;
lmacRetryTxFrame = 0x40001db8;
lmacProcessCollisions_task = 0x40001dbc;
/*lmacProcessTxopQComplete = 0x40001dc0;*/
lmacInitAc = 0x40001dc4;
/*lmacInit = 0x40001dc8;*/
mac_tx_set_txop_q = 0x40001dcc;
/*hal_init = 0x40001dd0;*/
hal_mac_rx_set_policy = 0x40001dd4;
hal_mac_set_bssid = 0x40001dd8;
mac_rx_policy_init = 0x40001ddc;
/*mac_txrx_init = 0x40001de0;*/
mac_rxbuf_init = 0x40001de4;
mac_last_rxbuf_init = 0x40001de8;
hal_attenna_init = 0x40001dec;
hal_timer_update_by_rtc = 0x40001df0;
hal_coex_pti_init = 0x40001df4;
/*lmac_stop_hw_txq = 0x40001df8;*/
ppDirectRecycleAmpdu = 0x40001dfc;
esp_wifi_internal_set_rts = 0x40001e00;
esp_wifi_internal_get_rts = 0x40001e04;
/*ppTxFragmentProc = 0x40001e08;*/
/*esf_buf_setup = 0x40001e0c;*/
hal_agreement_add_rx_ba = 0x40001e10;
hal_agreement_del_rx_ba = 0x40001e14;
/*hal_crypto_set_key_entry = 0x40001e18;*/
hal_crypto_get_key_entry = 0x40001e1c;
hal_crypto_clr_key_entry = 0x40001e20;
config_get_wifi_task_stack_size = 0x40001e24;
pp_create_task = 0x40001e28;
hal_set_sta_tsf_wakeup = 0x40001e2c;
hal_set_rx_beacon_pti = 0x40001e30;
/* pm_start = 0x40001e34; */
/* pm_stop = 0x40001e38; */
hal_disable_sta_tbtt = 0x40001e3c;
ppCalTxopDur = 0x40001e40;
wDev_IndicateCtrlFrame = 0x40001e44;
hal_enable_sta_tbtt = 0x40001e48;
/*hal_set_sta_tbtt = 0x40001e4c;*/
/* pm_update_next_tbtt = 0x40001e50;*/
/* pm_set_sleep_type = 0x40001e54; */
wDev_Rxbuf_Init = 0x40001e58;
wDev_Rxbuf_Deinit = 0x40001e5c;
ppCalTkipMic = 0x40001e60;
wDev_SnifferRxData = 0x40001e64;
hal_crypto_enable = 0x40001e68;
hal_crypto_disable = 0x40001e6c;
wDev_Insert_KeyEntry = 0x40001e70;
wDev_remove_KeyEntry = 0x40001e74;
rc_enable_trc = 0x40001e78;
rc_set_per_conn_fix_rate = 0x40001e7c;
wdev_csi_rx_process = 0x40001e80;
wDev_SnifferRxAmpdu = 0x40001e84;
hal_mac_tsf_reset = 0x40001e88;
dbg_lmac_statis_dump = 0x40001e8c;
dbg_lmac_rxtx_statis_dump = 0x40001e90;
dbg_lmac_hw_statis_dump = 0x40001e94;
dbg_lmac_diag_statis_dump = 0x40001e98;
dbg_lmac_ps_statis_dump = 0x40001e9c;
/*pp_timer_do_process = 0x40001ea0;*/
rcUpdateAMPDUParam = 0x40001ea4;
rcUpdatePhyMode = 0x40001ea8;
rcGetHighestRateIdx = 0x40001eac;
//pm_tx_null_data_done_process = 0x40001eb0;
//pm_tx_data_process = 0x40001eb4;
/* pm_attach = 0x40001eb8; */
/* pm_coex_schm_process = 0x40001ebc; */
ppInitTxq = 0x40001ec0;
pp_attach = 0x40001ec4;
pp_deattach = 0x40001ec8;
//pm_on_probe_resp_rx = 0x40001ecc;
hal_set_sta_tsf = 0x40001ed0;
ic_update_sta_tsf = 0x40001ed4;
ic_tx_pkt = 0x40001ed8;
//pm_send_probe_stop = 0x40001edc;
pm_send_probe_start = 0x40001ee0;
pm_on_coex_schm_process_restart = 0x40001ee4;
hal_mac_set_rxq_policy = 0x40001ee8;
hal_sniffer_enable = 0x40001eec;
hal_sniffer_disable = 0x40001ef0;
/*hal_sniffer_rx_set_promis = 0x40001ef4;*/
hal_sniffer_rx_clr_statistics = 0x40001ef8;
/*hal_sniffer_set_promis_misc_pkt = 0x40001efc;*/
tsf_hal_set_tsf_enable = 0x40001f00;
tsf_hal_set_tsf_disable = 0x40001f04;
tsf_hal_is_tsf_enabled = 0x40001f08;
tsf_hal_set_modem_wakeup_early_time = 0x40001f0c;
tsf_hal_get_counter_value = 0x40001f10;
tsf_hal_set_counter_value = 0x40001f14;
tsf_hal_get_time = 0x40001f18;
tsf_hal_set_time = 0x40001f1c;
tsf_hal_set_tbtt_enable = 0x40001f20;
tsf_hal_set_tbtt_disable = 0x40001f24;
tsf_hal_set_tbtt_intr_enable = 0x40001f28;
tsf_hal_set_tbtt_intr_disable = 0x40001f2c;
tsf_hal_set_tbtt_soc_wakeup_enable = 0x40001f30;
tsf_hal_set_tbtt_soc_wakeup_disable = 0x40001f34;
/*tsf_hal_set_tbtt_start_time = 0x40001f38;*/
tsf_hal_set_tbtt_early_time = 0x40001f3c;
tsf_hal_set_tbtt_interval = 0x40001f40;
tsf_hal_get_tbtt_interval = 0x40001f44;
tsf_hal_set_timer_enable = 0x40001f48;
tsf_hal_set_timer_disable = 0x40001f4c;
tsf_hal_set_timer_target = 0x40001f50;
tsf_hal_get_timer_target = 0x40001f54;
tsf_hal_set_timer_intr_enable = 0x40001f58;
tsf_hal_set_timer_intr_disable = 0x40001f5c;
tsf_hal_set_timer_soc_wakeup_enable = 0x40001f60;
tsf_hal_set_timer_soc_wakeup_disable = 0x40001f64;
pm_disconnected_wake = 0x40001f68;
pm_get_connectionless_status = 0x40001f6c;
pm_update_by_connectionless_status = 0x40001f70;
pm_connectionless_wake_interval_timeout_process = 0x40001f74;
pm_connectionless_wake_window_timeout_process = 0x40001f78;
/* Data (.data, .bss, .rodata) */
our_instances_ptr = 0x3ff4fbbc;
pTxRx = 0x3fcdfdec;
lmacConfMib_ptr = 0x3fcdfde8;
our_wait_eb = 0x3fcdfde4;
our_tx_eb = 0x3fcdfde0;
pp_wdev_funcs = 0x3fcdfddc;
g_osi_funcs_p = 0x3fcdfdd8;
wDevCtrl_ptr = 0x3fcdfdd4;
g_wdev_last_desc_reset_ptr = 0x3ff4fbb8;
wDevMacSleep_ptr = 0x3fcdfdd0;
g_lmac_cnt_ptr = 0x3fcdfdcc;
our_controls_ptr = 0x3ff4fbb4;
pp_sig_cnt_ptr = 0x3fcdfdc8;
g_eb_list_desc_ptr = 0x3fcdfdc4;
s_fragment_ptr = 0x3fcdfdc0;
if_ctrl_ptr = 0x3fcdfdbc;
g_intr_lock_mux = 0x3fcdfdb8;
g_wifi_global_lock = 0x3fcdfdb4;
s_wifi_queue = 0x3fcdfdb0;
pp_task_hdl = 0x3fcdfdac;
s_pp_task_create_sem = 0x3fcdfda8;
s_pp_task_del_sem = 0x3fcdfda4;
g_wifi_menuconfig_ptr = 0x3fcdfda0;
xphyQueue = 0x3fcdfd9c;
ap_no_lr_ptr = 0x3fcdfd98;
rc11BSchedTbl_ptr = 0x3fcdfd94;
rc11NSchedTbl_ptr = 0x3fcdfd90;
rcLoRaSchedTbl_ptr = 0x3fcdfd8c;
BasicOFDMSched_ptr = 0x3fcdfd88;
trc_ctl_ptr = 0x3fcdfd84;
g_pm_cnt_ptr = 0x3fcdfd80;
g_pm_ptr = 0x3fcdfd7c;
g_pm_cfg_ptr = 0x3fcdfd78;
g_esp_mesh_quick_funcs_ptr = 0x3fcdfd74;
g_txop_queue_status_ptr = 0x3fcdfd70;
g_mac_sleep_en_ptr = 0x3fcdfd6c;
g_mesh_is_root_ptr = 0x3fcdfd68;
g_mesh_topology_ptr = 0x3fcdfd64;
g_mesh_init_ps_type_ptr = 0x3fcdfd60;
g_mesh_is_started_ptr = 0x3fcdfd5c;
g_config_func = 0x3fcdfd58;
g_net80211_tx_func = 0x3fcdfd54;
g_timer_func = 0x3fcdfd50;
s_michael_mic_failure_cb = 0x3fcdfd4c;
wifi_sta_rx_probe_req = 0x3fcdfd48;
g_tx_done_cb_func = 0x3fcdfd44;
g_per_conn_trc = 0x3fcdfd28;
s_encap_amsdu_func = 0x3fcdfd24;
bars = 0x3fcdfc84;
eb_txdesc_space = 0x3fcdfbf4;
eb_space = 0x3fcdfb54;
g_pd_mac_in_light_sleep = 0x3fcdfb50;
s_fix_rate_mask = 0x3fcdfb4c;
s_fix_rate = 0x3fcdfb44;
g_wdev_csi_rx = 0x3fcdfb40;
g_wdev_csi_rx_ctx = 0x3fcdfb3c;
BcnSendTick = 0x3fcdfb38;
g_pp_timer_info_ptr = 0x3fcdfb34;
rcP2P11NSchedTbl_ptr = 0x3fcdfb30;
rcP2P11GSchedTbl_ptr = 0x3fcdfb2c;
rc11GSchedTbl_ptr = 0x3fcdfb28;


/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x40001f7c;
ampdu_dispatch = 0x40001f80;
ampdu_dispatch_all = 0x40001f84;
ampdu_dispatch_as_many_as_possible = 0x40001f88;
ampdu_dispatch_movement = 0x40001f8c;
ampdu_dispatch_upto = 0x40001f90;
chm_is_at_home_channel = 0x40001f94;
cnx_node_is_existing = 0x40001f98;
cnx_node_search = 0x40001f9c;
ic_ebuf_recycle_rx = 0x40001fa0;
ic_ebuf_recycle_tx = 0x40001fa4;
ic_reset_rx_ba = 0x40001fa8;
ieee80211_align_eb = 0x40001fac;
/*ieee80211_ampdu_reorder = 0x40001fb0;*/
ieee80211_ampdu_start_age_timer = 0x40001fb4;
/*ieee80211_encap_esfbuf = 0x40001fb8;*/
ieee80211_is_tx_allowed = 0x40001fbc;
ieee80211_output_pending_eb = 0x40001fc0;
/* ieee80211_output_process = 0x40001fc4; */
ieee80211_set_tx_desc = 0x40001fc8;
/*sta_input = 0x40001fcc;*/
wifi_get_macaddr = 0x40001fd0;
wifi_rf_phy_disable = 0x40001fd4;
wifi_rf_phy_enable = 0x40001fd8;
ic_ebuf_alloc = 0x40001fdc;
/*ieee80211_classify = 0x40001fe0;*/
ieee80211_copy_eb_header = 0x40001fe4;
ieee80211_recycle_cache_eb = 0x40001fe8;
ieee80211_search_node = 0x40001fec;
roundup2 = 0x40001ff0;
ieee80211_crypto_encap = 0x40001ff4;
/* ieee80211_crypto_decap = 0x40001ff8; */
ieee80211_decap = 0x40001ffc;
ieee80211_set_tx_pti = 0x40002000;
wifi_is_started = 0x40002004;
ieee80211_gettid = 0x40002008;
/* ieee80211_ccmp_decrypt = 0x4000200c; */
/* ieee80211_ccmp_encrypt = 0x40002010; */
ccmp_encap = 0x40002014;
ccmp_decap = 0x40002018;
tkip_encap = 0x4000201c;
tkip_decap = 0x40002020;
wep_encap = 0x40002024;
wep_decap = 0x40002028;
dbg_hmac_rxtx_statis_dump = 0x4000202c;
dbg_hmac_statis_dump = 0x40002030;
/* ieee80211_send_action_vendor_spec = 0x40002034; */
ieee80211_vnd_lora_ie_size = 0x40002048;
ieee80211_vnd_ie_size = 0x4000204c;
ieee80211_add_ssid = 0x40002050;
ieee80211_add_rates = 0x40002054;
/*ieee80211_add_xrates = 0x40002058;*/
ieee80211_is_ht_cipher = 0x4000205c;
ieee80211_setup_lr_rates = 0x40002068;
ieee80211_ht_node_init = 0x4000206c;
ieee80211_is_support_rate = 0x40002070;
ieee80211_setup_rates = 0x40002074;
ieee80211_is_lr_only = 0x40002078;
ieee80211_setup_phy_mode = 0x4000207c;
ieee80211_sta_is_connected = 0x40002080;
current_task_is_wifi_task = 0x40002084;
wifi_get_init_state = 0x40002088;
/* ieee80211_timer_process = 0x4000208c; */
/* cnx_coexist_timeout = 0x40002090; */
/* sta_recv_mgmt = 0x40002094;*/
ieee80211_send_setup = 0x40002098;
//ieee80211_send_probereq = 0x4000209c;
sta_auth_shared = 0x400020a4;
/* cnx_coexist_timeout_process = 0x400020ac; */
ieee80211_alloc_challenge = 0x400020b0;
cnx_assoc_timeout = 0x400020b4;
ieee80211_vnd_ie_set = 0x400020b8;
ieee80211_vnd_lora_ie_set = 0x400020bc;
ieee80211_add_wme_param = 0x400020c0;
ieee80211_add_dsparams = 0x400020c4;
ieee80211_add_csa = 0x400020c8;
/*ieee80211_add_extcap = 0x400020cc;*/
ieee80211_regdomain_get_country = 0x400020d0;
ieee80211_add_countryie = 0x400020d4;
ieee80211_amsdu_adjust_head = 0x400020dc;
ieee80211_amsdu_adjust_last_length = 0x400020e0;
ieee80211_amsdu_send_check = 0x400020e4;
ieee80211_amsdu_encap_check = 0x400020e8;
ieee80211_amsdu_length_check = 0x400020ec;
ieee80211_encap_amsdu = 0x400020f0;
ieee80211_output_raw_process = 0x400020f4;
ieee80211_raw_frame_sanity_check = 0x400020fc;
//ieee80211_crypto_aes_128_cmac_encrypt = 0x40002100;
ieee80211_alloc_tx_buf = 0x40002108;
/* ieee80211_output_do = 0x4000210c; */
/* ieee80211_send_nulldata = 0x40002110; */
/* ieee80211_setup_robust_mgmtframe = 0x40002114; */
ieee80211_encap_null_data = 0x4000211c;
/* ieee80211_send_deauth_no_bss = 0x40002120; */
ieee80211_alloc_deauth = 0x40002124;
ieee80211_send_proberesp = 0x40002128;
ieee80211_getcapinfo = 0x40002130;
/* sta_rx_csa = 0x40002134; */
/* sta_recv_sa_query_resp = 0x40002144; */
ieee80211_set_max_rate = 0x4000214c;
ic_set_sta = 0x40002150;
ieee80211_parse_wpa = 0x40002158;
ieee80211_add_assoc_req_ies = 0x40002160;
ieee80211_add_probe_req_ies = 0x40002164;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x3fcdfb24;
g_scan = 0x3fcdfb20;
g_chm = 0x3fcdfb1c;
g_ic_ptr = 0x3fcdfb18;
g_hmac_cnt_ptr = 0x3fcdfaf4;
g_tx_cacheq_ptr = 0x3fcdfb14;
s_netstack_free = 0x3fcdfb10;
mesh_rxcb = 0x3fcdfb0c;
sta_rxcb = 0x3fcdfb08;
ccmp_ptr = 0x3fcdfb04;
s_wifi_nvs_ptr = 0x3fcdfb00;
tkip_ptr = 0x3fcdfafc;
wep_ptr = 0x3fcdfaf8;
g_hmac_cnt_ptr = 0x3fcdfaf4;
g_misc_nvs = 0x3fcdfaf0;
s_wifi_init_state = 0x3fcdfac0;
s_wifi_task_hdl = 0x3fcdfaec;
in_rssi_adjust = 0x3fcdfae8;
rssi_saved = 0x3fcdfae0;
rssi_index = 0x3fcdfadc;
/* s_sa_query_retries = 0x3fcdfad8; */
/* s_sa_query_success = 0x3fcdfad5; */
g_sta_connected_flag = 0x3fcdfad4;
wpa_crypto_funcs_ptr = 0x3fcdfad0;
s_netstack_ref = 0x3fcdfacc;
sta_csa_timer_ptr = 0x3fcdfac8;
/* s_trans_id = 0x3fcdfac4; */


/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
esp_coex_rom_version_get = 0x40002168;
coex_bt_release = 0x4000216c;
coex_bt_request = 0x40002170;
coex_core_ble_conn_dyn_prio_get = 0x40002174;
/*coex_core_event_duration_get = 0x40002178;*/
coex_core_pti_get = 0x4000217c;
coex_core_release = 0x40002180;
coex_core_request = 0x40002184;
coex_core_status_get = 0x40002188;
/*coex_core_timer_idx_get = 0x4000218c;*/
coex_event_duration_get = 0x40002190;
coex_hw_timer_disable = 0x40002194;
coex_hw_timer_enable = 0x40002198;
coex_hw_timer_set = 0x4000219c;
coex_schm_interval_set = 0x400021a0;
coex_schm_lock = 0x400021a4;
coex_schm_unlock = 0x400021a8;
/*coex_status_get = 0x400021ac;*/
coex_wifi_release = 0x400021b0;
esp_coex_ble_conn_dynamic_prio_get = 0x400021b4;
/*coex_hw_timer_tick_get = 0x400021b8;*/
/* Data (.data, .bss, .rodata) */
coex_env_ptr = 0x3fcdfabc;
coex_pti_tab_ptr = 0x3fcdfab8;
coex_schm_env_ptr = 0x3fcdfab4;
coexist_funcs = 0x3fcdfab0;
g_coa_funcs_p = 0x3fcdfaac;
g_coex_param_ptr = 0x3fcdfaa8;


/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
phy_param_addr = 0x400021bc;
phy_get_romfuncs = 0x400021c0;
chip729_phyrom_version = 0x400021c4;
chip729_phyrom_version_num = 0x400021c8;
rom_get_rc_dout = 0x400021cc;
rc_cal = 0x400021d0;
phy_analog_delay_cal = 0x400021d4;
phy_rx_rifs_en = 0x400021d8;
phy_current_level_set = 0x400021dc;
phy_bbpll_en_usb = 0x400021e0;
phy_bt_power_track = 0x400021e4;
/* phy_xpd_tsens = 0x400021e8); Link this function in phy_lib, no longer link the rom one.*/
bb_wdt_rst_enable = 0x400021ec;
bb_wdt_int_enable = 0x400021f0;
bb_wdt_timeout_clear = 0x400021f4;
bb_wdt_get_status = 0x400021f8;
rom_enter_critical_phy = 0x400021fc;
rom_exit_critical_phy = 0x40002200;
rom_bb_bss_cbw40 = 0x40002204;
rom_set_chan_reg = 0x40002208;
abs_temp = 0x4000220c;
set_chan_cal_interp = 0x40002210;
loopback_mode_en = 0x40002214;
get_data_sat = 0x40002218;
phy_byte_to_word = 0x4000221c;
phy_get_rx_freq = 0x40002220;
i2c_master_reset = 0x40002224;
chan14_mic_enable = 0x40002228;
chan14_mic_cfg = 0x4000222c;
set_adc_rand = 0x40002230;
phy_set_most_tpw = 0x40002234;
phy_get_most_tpw = 0x40002238;
esp_tx_state_out = 0x4000223c;
phy_get_adc_rand = 0x40002240;
phy_internal_delay = 0x40002244;
phy_ftm_comp = 0x40002248;
phy_11p_set = 0x4000224c;
phy_freq_mem_backup = 0x40002250;
ant_dft_cfg = 0x40002254;
ant_wifitx_cfg = 0x40002258;
ant_wifirx_cfg = 0x4000225c;
ant_bttx_cfg = 0x40002260;
ant_btrx_cfg = 0x40002264;
phy_chan_dump_cfg = 0x40002268;
phy_enable_low_rate = 0x4000226c;
phy_disable_low_rate = 0x40002270;
phy_dig_reg_backup = 0x40002274;
phy_chan_filt_set = 0x40002278;
phy_rx11blr_cfg = 0x4000227c;
set_cca = 0x40002280;
set_rx_sense = 0x40002284;
rx_gain_force = 0x40002288;
rom_phy_en_hw_set_freq = 0x4000228c;
rom_phy_dis_hw_set_freq = 0x40002290;
wr_rf_freq_mem = 0x40002294;
freq_i2c_write_set = 0x40002298;
write_pll_cap_mem = 0x4000229c;
pll_dac_mem_update = 0x400022a0;
pll_cap_mem_update = 0x400022a4;
get_rf_freq_cap = 0x400022a8;
get_rf_freq_init = 0x400022ac;
freq_get_i2c_data = 0x400022b0;
freq_i2c_data_write = 0x400022b4;
set_chan_freq_hw_init = 0x400022b8;
set_chan_freq_sw_start = 0x400022bc;
rom_get_i2c_read_mask = 0x400022c0;
rom_get_i2c_mst0_mask = 0x400022c4;
rom_get_i2c_hostid = 0x400022c8;
rom_chip_i2c_readReg_org = 0x400022cc;
rom_chip_i2c_readReg = 0x400022d0;
rom_i2c_paral_set_mst0 = 0x400022d4;
rom_i2c_paral_set_read = 0x400022d8;
rom_i2c_paral_read = 0x400022dc;
rom_i2c_paral_write = 0x400022e0;
rom_i2c_paral_write_num = 0x400022e4;
rom_i2c_paral_write_mask = 0x400022e8;
rom_i2c_readReg = 0x400022ec;
rom_chip_i2c_writeReg = 0x400022f0;
rom_i2c_writeReg = 0x400022f4;
rom_i2c_readReg_Mask = 0x400022f8;
rom_i2c_writeReg_Mask = 0x400022fc;
rom_set_txcap_reg = 0x40002300;
i2c_sar2_init_code = 0x40002304;
phy_i2c_init1 = 0x40002308;
phy_i2c_init2 = 0x4000230c;
phy_get_i2c_data = 0x40002310;
bias_reg_set = 0x40002314;
i2c_rc_cal_set = 0x40002318;
i2c_bbpll_set = 0x4000231c;
rom_phy_xpd_rf = 0x40002320;
phy_wakeup_init_rom = 0x40002324;
register_chipv7_phy_init_param = 0x40002328;
phy_reg_init = 0x4000232c;
phy_close_rf_rom = 0x40002330;
rom_pbus_force_mode = 0x40002334;
rom_pbus_rd_addr = 0x40002338;
rom_pbus_rd_shift = 0x4000233c;
rom_pbus_force_test = 0x40002340;
rom_pbus_rd = 0x40002344;
rom_pbus_debugmode = 0x40002348;
rom_pbus_workmode = 0x4000234c;
rom_pbus_set_rxgain = 0x40002350;
rom_pbus_xpd_rx_off = 0x40002354;
rom_pbus_xpd_rx_on = 0x40002358;
rom_pbus_xpd_tx_off = 0x4000235c;
rom_pbus_xpd_tx_on = 0x40002360;
rom_pbus_set_dco = 0x40002364;
rom_set_loopback_gain = 0x40002368;
rom_txcal_debuge_mode = 0x4000236c;
rom_txcal_work_mode = 0x40002370;
set_pbus_mem = 0x40002374;
rom_pwdet_sar2_init = 0x40002378;
rom_en_pwdet = 0x4000237c;
rom_get_sar_sig_ref = 0x40002380;
rom_pwdet_tone_start = 0x40002384;
rom_get_tone_sar_dout = 0x40002388;
rom_get_fm_sar_dout = 0x4000238c;
rom_txtone_linear_pwr = 0x40002390;
rom_get_power_db = 0x40002394;
rom_meas_tone_pwr_db = 0x40002398;
rom_pkdet_vol_start = 0x4000239c;
rom_read_sar_dout = 0x400023a0;
rom_read_sar2_code = 0x400023a4;
rom_get_sar2_vol = 0x400023a8;
rom_get_pll_vol = 0x400023ac;
rom_tx_pwctrl_bg_init = 0x400023b0;
rom_phy_pwdet_always_en = 0x400023b4;
rom_phy_pwdet_onetime_en = 0x400023b8;
linear_to_db = 0x400023bc;
rom_disable_agc = 0x400023c0;
rom_enable_agc = 0x400023c4;
rom_disable_wifi_agc = 0x400023c8;
rom_enable_wifi_agc = 0x400023cc;
rom_write_gain_mem = 0x400023d0;
rom_bb_bss_cbw40_dig = 0x400023d4;
rom_cbw2040_cfg = 0x400023d8;
rom_mac_tx_chan_offset = 0x400023dc;
rom_tx_paon_set = 0x400023e0;
rom_i2cmst_reg_init = 0x400023e4;
rom_bt_gain_offset = 0x400023e8;
rom_fe_reg_init = 0x400023ec;
rom_mac_enable_bb = 0x400023f0;
rom_bb_wdg_cfg = 0x400023f4;
rom_fe_txrx_reset = 0x400023f8;
rom_set_rx_comp = 0x400023fc;
rom_write_chan_freq = 0x40002400;
rom_agc_reg_init = 0x40002404;
rom_bb_reg_init = 0x40002408;
rom_write_txrate_power_offset = 0x4000240c;
rom_open_i2c_xpd = 0x40002410;
rom_txiq_set_reg = 0x40002414;
rom_rxiq_set_reg = 0x40002418;
rom_phy_bbpll_cal = 0x4000241c;
phy_disable_cca = 0x40002420;
phy_enable_cca = 0x40002424;
force_txon = 0x40002428;
set_txclk_en = 0x4000242c;
set_rxclk_en = 0x40002430;
start_tx_tone_step = 0x40002434;
stop_tx_tone = 0x40002438;
bb_wdg_test_en = 0x4000243c;
noise_floor_auto_set = 0x40002440;
read_hw_noisefloor = 0x40002444;
iq_corr_enable = 0x40002448;
bt_tx_dig_gain = 0x4000244c;
wifi_tx_dig_reg = 0x40002450;
wifi_agc_sat_gain = 0x40002454;
phy_ant_init = 0x40002458;
phy_set_bbfreq_init = 0x4000245c;
wifi_fbw_sel = 0x40002460;
phy_rx_sense_set = 0x40002464;
tx_state_set = 0x40002468;
phy_close_pa = 0x4000246c;
bt_filter_reg = 0x40002470;
phy_freq_correct = 0x40002474;
set_pbus_reg = 0x40002478;
wifi_rifs_mode_en = 0x4000247c;
rfagc_disable = 0x40002480;
rom_restart_cal = 0x40002484;
rom_write_rfpll_sdm = 0x40002488;
rom_wait_rfpll_cal_end = 0x4000248c;
rom_rfpll_set_freq = 0x40002490;
rom_rfpll_cap_init_cal = 0x40002494;
rom_set_rfpll_freq = 0x40002498;
rom_write_pll_cap = 0x4000249c;
rom_read_pll_cap = 0x400024a0;
mhz2ieee = 0x400024a4;
chan_to_freq = 0x400024a8;
set_rf_freq_offset = 0x400024ac;
set_channel_rfpll_freq = 0x400024b0;
rfpll_cap_correct = 0x400024b4;
phy_set_freq = 0x400024b8;
correct_rfpll_offset = 0x400024bc;
pll_vol_cal = 0x400024c0;
chip_v7_set_chan_misc = 0x400024c4;
chip_v7_set_chan = 0x400024c8;
chip_v7_set_chan_offset = 0x400024cc;
chip_v7_set_chan_ana = 0x400024d0;
set_chanfreq = 0x400024d4;
rom_rxiq_cover_mg_mp = 0x400024d8;
rom_rfcal_rxiq = 0x400024dc;
rom_get_rfcal_rxiq_data = 0x400024e0;
rom_pbus_rx_dco_cal = 0x400024e4;
rom_rxdc_est_min = 0x400024e8;
rom_pbus_rx_dco_cal_1step = 0x400024ec;
rom_set_lb_txiq = 0x400024f0;
rom_set_rx_gain_cal_iq = 0x400024f4;
rom_set_rx_gain_cal_dc = 0x400024f8;
iq_est_enable = 0x400024fc;
iq_est_disable = 0x40002500;
dc_iq_est = 0x40002504;
set_cal_rxdc = 0x40002508;
rxiq_get_mis = 0x4000250c;
spur_reg_write_one_tone = 0x40002510;
spur_cal = 0x40002514;
spur_coef_cfg = 0x40002518;
gen_rx_gain_table = 0x4000251c;
wr_rx_gain_mem = 0x40002520;
set_rx_gain_param = 0x40002524;
set_rx_gain_table = 0x40002528;
rom_tester_wifi_cali = 0x4000252c;
esp_recover_efuse_data = 0x40002530;
/* bt_track_pll_cap = 0x40002534;*/
rfpll_cap_track = 0x40002538;
phy_param_track = 0x4000253c;
txpwr_correct = 0x40002540;
txpwr_cal_track = 0x40002544;
/* tx_pwctrl_background = 0x40002548;*/
bt_track_tx_power = 0x4000254c;
wifi_track_tx_power = 0x40002550;
rom_code_to_temp = 0x40002554;
rom_tsens_index_to_dac = 0x40002558;
rom_tsens_index_to_offset = 0x4000255c;
rom_tsens_dac_cal = 0x40002560;
rom_tsens_code_read = 0x40002564;
rom_tsens_temp_read = 0x40002568;
rom_temp_to_power = 0x4000256c;
tsens_read_init = 0x40002570;
get_temp_init = 0x40002574;
rom_txiq_cover = 0x40002578;
rom_rfcal_txiq = 0x4000257c;
rom_get_power_atten = 0x40002580;
rom_tx_pwctrl_init_cal = 0x40002584;
bt_txdc_cal = 0x40002588;
bt_txiq_cal = 0x4000258c;
txiq_cal_init = 0x40002590;
txdc_cal_init = 0x40002594;
txdc_cal_v70 = 0x40002598;
txiq_get_mis_pwr = 0x4000259c;
pwdet_ref_code = 0x400025a0;
pwdet_code_cal = 0x400025a4;
rfcal_txcap = 0x400025a8;
tx_cap_init = 0x400025ac;
rfcal_pwrctrl = 0x400025b0;
tx_pwctrl_init = 0x400025b4;
bt_tx_pwctrl_init = 0x400025b8;
bt_txpwr_freq = 0x400025bc;
rom_txbbgain_to_index = 0x400025c0;
rom_index_to_txbbgain = 0x400025c4;
rom_bt_index_to_bb = 0x400025c8;
rom_bt_bb_to_index = 0x400025cc;
rom_bt_get_tx_gain = 0x400025d0;
rom_get_tx_gain_value = 0x400025d4;
rom_wifi_get_tx_gain = 0x400025d8;
rom_set_tx_gain_mem = 0x400025dc;
rom_get_rate_fcc_index = 0x400025e0;
rom_get_chan_target_power = 0x400025e4;
rom_wifi_tx_dig_gain = 0x400025e8;
rom_wifi_set_tx_gain = 0x400025ec;
rom_bt_set_tx_gain = 0x400025f0;
wifi_11g_rate_chg = 0x400025f4;
bt_chan_pwr_interp = 0x400025f8;
bt_tx_gain_init = 0x400025fc;
/* Data (.data, .bss, .rodata) */
phy_param_rom = 0x3fcdfaa4;


/***************************************
 Group rom_btbb
 ***************************************/

/* Functions */
bt_agc_gain_offset = 0x40002600;
bt_agc_gain_max = 0x40002604;
bt_set_rx_comp = 0x40002608;
bt_agc_gain_set = 0x4000260c;
bt_agc_rssi_thresh = 0x40002610;
bt_agc_target_set = 0x40002614;
bt_agc_restart_set = 0x40002618;
bt_agc_recorrect_set = 0x4000261c;
bt_agc_detect_set = 0x40002620;
bt_bb_rx_correlator_set = 0x40002624;
bt_bb_rx_dpo_set = 0x40002628;
bt_bb_rx_filter_sel = 0x4000262c;
bt_bb_rx_set1 = 0x40002630;
bt_bb_v2_rx_set = 0x40002634;
bt_bb_v2_tx_set = 0x40002638;
bt_bb_tx_cca_set = 0x4000263c;
bt_bb_tx_cca_period = 0x40002640;
bt_bb_tx_cca_fifo_reset = 0x40002644;
bt_bb_tx_cca_fifo_empty = 0x40002648;
bt_bb_tx_cca_fifo_full = 0x4000264c;
bt_bb_tx_cca_fifo_count = 0x40002650;
bt_bb_tx_cca_fifo_read = 0x40002654;
coex_pti_v2 = 0x40002658;
bt_bb_set_le_tx_on_delay = 0x4000265c;
bt_bb_set_corr_thresh_le = 0x40002660;

/***************************************
 Group rom_mbedtls md5
 ***************************************/
mbedtls_md5_starts_ret = 0x40002be4;
mbedtls_md5_update_ret = 0x40002be8;
mbedtls_md5_finish_ret = 0x40002bec;
