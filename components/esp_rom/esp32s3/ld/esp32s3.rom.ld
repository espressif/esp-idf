/* ROM function interface esp32s3.rom.ld for esp32s3
 *
 *
 * Generated from ./interface-esp32s3.yml md5sum 39c4ce259b11323b9404c192b01b712b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group common
 ***************************************/

/* Functions */
rtc_get_reset_reason = 0x4000057c;
analog_super_wdt_reset_happened = 0x40000588;
jtag_cpu_reset_happened = 0x40000594;
rtc_get_wakeup_cause = 0x400005a0;
rtc_select_apb_bridge = 0x400005ac;
rtc_unhold_all_pads = 0x400005b8;
ets_is_print_boot = 0x400005c4;
ets_printf = 0x400005d0;
ets_install_putc1 = 0x400005dc;
ets_install_uart_printf = 0x400005e8;
ets_install_putc2 = 0x400005f4;
PROVIDE( ets_delay_us = 0x40000600 );
ets_get_stack_info = 0x4000060c;
ets_install_lock = 0x40000618;
ets_backup_dma_copy = 0x40000624;
ets_apb_backup_init_lock_func = 0x40000630;
UartRxString = 0x4000063c;
uart_tx_one_char = 0x40000648;
uart_tx_one_char2 = 0x40000654;
uart_rx_one_char = 0x40000660;
uart_rx_one_char_block = 0x4000066c;
uart_rx_readbuff = 0x40000678;
uartAttach = 0x40000684;
uart_tx_flush = 0x40000690;
uart_tx_wait_idle = 0x4000069c;
uart_div_modify = 0x400006a8;
ets_write_char_uart = 0x400006b4;
uart_tx_switch = 0x400006c0;
multofup = 0x400006cc;
software_reset = 0x400006d8;
software_reset_cpu = 0x400006e4;
assist_debug_clock_enable = 0x400006f0;
assist_debug_record_enable = 0x400006fc;
clear_super_wdt_reset_flag = 0x40000708;
disable_default_watchdog = 0x40000714;
ets_set_appcpu_boot_addr = 0x40000720;
esp_rom_set_rtc_wake_addr = 0x4000072c;
esp_rom_get_rtc_wake_addr = 0x40000738;
send_packet = 0x40000744;
recv_packet = 0x40000750;
GetUartDevice = 0x4000075c;
UartDwnLdProc = 0x40000768;
Uart_Init = 0x40000774;
ets_set_user_start = 0x40000780;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x3ff1fffc;
ets_ops_table_ptr = 0x3fcefffc;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x4000078c;
mz_crc32 = 0x40000798;
mz_free = 0x400007a4;
tdefl_compress = 0x400007b0;
tdefl_compress_buffer = 0x400007bc;
tdefl_compress_mem_to_heap = 0x400007c8;
tdefl_compress_mem_to_mem = 0x400007d4;
tdefl_compress_mem_to_output = 0x400007e0;
tdefl_get_adler32 = 0x400007ec;
tdefl_get_prev_return_status = 0x400007f8;
tdefl_init = 0x40000804;
tdefl_write_image_to_png_file_in_memory = 0x40000810;
tdefl_write_image_to_png_file_in_memory_ex = 0x4000081c;
tinfl_decompress = 0x40000828;
tinfl_decompress_mem_to_callback = 0x40000834;
tinfl_decompress_mem_to_heap = 0x40000840;
tinfl_decompress_mem_to_mem = 0x4000084c;


/***************************************
 Group tjpgd
 ***************************************/

/* Functions */
PROVIDE( jd_prepare = 0x40000858 );
PROVIDE (jd_decomp = 0x40000864 );


/***************************************
 Group esp-dsp
 ***************************************/

/* Data (.data, .bss, .rodata) */
dsps_fft2r_w_table_fc32_1024 = 0x3fcefff8;


/***************************************
 Group opi_flash
 ***************************************/

/* Functions */
PROVIDE( opi_flash_set_lock_func = 0x40000870 );
PROVIDE( esp_rom_spi_cmd_config = 0x4000087c );
PROVIDE( esp_rom_spi_cmd_start = 0x40000888 );
PROVIDE( esp_rom_opiflash_pin_config = 0x40000894 );
PROVIDE( esp_rom_spi_set_op_mode = 0x400008a0 );
PROVIDE( esp_rom_opiflash_mode_reset = 0x400008ac );
PROVIDE( esp_rom_opiflash_exec_cmd = 0x400008b8 );
PROVIDE( esp_rom_opiflash_soft_reset = 0x400008c4 );
PROVIDE( esp_rom_opiflash_read_id = 0x400008d0 );
PROVIDE( esp_rom_opiflash_rdsr = 0x400008dc );
PROVIDE( esp_rom_opiflash_wait_idle = 0x400008e8 );
PROVIDE( esp_rom_opiflash_wren = 0x400008f4 );
PROVIDE( esp_rom_opiflash_erase_sector = 0x40000900 );
PROVIDE( esp_rom_opiflash_erase_block_64k = 0x4000090c );
PROVIDE( esp_rom_opiflash_erase_area = 0x40000918 );
PROVIDE( esp_rom_opiflash_read = 0x40000924 );
PROVIDE( esp_rom_opiflash_write = 0x40000930 );
PROVIDE( esp_rom_spi_set_dtr_swap_mode = 0x4000093c );
PROVIDE( esp_rom_opiflash_exit_continuous_read_mode = 0x40000948 );
PROVIDE( esp_rom_opiflash_legacy_driver_init = 0x40000954 );
PROVIDE( esp_rom_opiflash_read_raw = 0x4004d9d4);
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_opiflash_cmd_def = 0x3fcefff4 );
PROVIDE( rom_spi_usr_cmd_legacy_funcs = 0x3fcefff0 );


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
PROVIDE( esp_rom_spiflash_wait_idle = 0x40000960 );
PROVIDE( esp_rom_spiflash_write_encrypted = 0x4000096c );
PROVIDE( esp_rom_spiflash_write_encrypted_dest = 0x40000978 );
PROVIDE( esp_rom_spiflash_write_encrypted_enable = 0x40000984 );
PROVIDE( esp_rom_spiflash_write_encrypted_disable = 0x40000990 );
PROVIDE( esp_rom_spiflash_erase_chip = 0x4000099c );
PROVIDE( _esp_rom_spiflash_erase_sector = 0x400009a8 );
PROVIDE( _esp_rom_spiflash_erase_block = 0x400009b4 );
PROVIDE( _esp_rom_spiflash_write = 0x400009c0 );
PROVIDE( _esp_rom_spiflash_read = 0x400009cc );
PROVIDE( _esp_rom_spiflash_unlock = 0x400009d8 );
PROVIDE( _SPIEraseArea = 0x400009e4 );
PROVIDE( _SPI_write_enable = 0x400009f0 );
PROVIDE( esp_rom_spiflash_erase_sector = 0x400009fc );
PROVIDE( esp_rom_spiflash_erase_block = 0x40000a08 );
PROVIDE( esp_rom_spiflash_write = 0x40000a14 );
PROVIDE( esp_rom_spiflash_read = 0x40000a20 );
PROVIDE( esp_rom_spiflash_unlock = 0x40000a2c );
PROVIDE( SPIEraseArea = 0x40000a38 );
PROVIDE( SPI_write_enable = 0x40000a44 );
PROVIDE( esp_rom_spiflash_config_param = 0x40000a50 );
PROVIDE( esp_rom_spiflash_read_user_cmd = 0x40000a5c );
PROVIDE( esp_rom_spiflash_select_qio_pins = 0x40000a68 );
PROVIDE( esp_rom_spi_flash_auto_sus_res = 0x40000a74 );
PROVIDE( esp_rom_spi_flash_send_resume = 0x40000a80 );
PROVIDE( esp_rom_spi_flash_update_id = 0x40000a8c );
PROVIDE( esp_rom_spiflash_config_clk = 0x40000a98 );
PROVIDE( esp_rom_spiflash_config_readmode = 0x40000aa4 );
PROVIDE( esp_rom_spiflash_read_status = 0x40000ab0 );
PROVIDE( esp_rom_spiflash_read_statushigh = 0x40000abc );
PROVIDE( esp_rom_spiflash_write_status = 0x40000ac8 );
PROVIDE( esp_rom_opiflash_cache_mode_config = 0x40000ad4 );
PROVIDE( esp_rom_spiflash_auto_wait_idle = 0x40000ae0 );
PROVIDE( spi_flash_attach = 0x40000aec );
PROVIDE( spi_flash_get_chip_size = 0x40000af8 );
PROVIDE( spi_flash_guard_set = 0x40000b04 );
PROVIDE( spi_flash_guard_get = 0x40000b10 );
PROVIDE( spi_flash_write_config_set = 0x40000b1c );
PROVIDE( spi_flash_write_config_get = 0x40000b28 );
PROVIDE( spi_flash_safe_write_address_func_set = 0x40000b34 );
PROVIDE( spi_flash_unlock = 0x40000b40 );
PROVIDE( spi_flash_erase_range = 0x40000b4c );
PROVIDE( spi_flash_erase_sector = 0x40000b58 );
PROVIDE( spi_flash_write = 0x40000b64 );
PROVIDE( spi_flash_read = 0x40000b70 );
PROVIDE( spi_flash_write_encrypted = 0x40000b7c );
PROVIDE( spi_flash_read_encrypted = 0x40000b88 );
PROVIDE( spi_flash_mmap_os_func_set = 0x40000b94 );
PROVIDE( spi_flash_mmap_page_num_init = 0x40000ba0 );
PROVIDE( spi_flash_mmap = 0x40000bac );
PROVIDE( spi_flash_mmap_pages = 0x40000bb8 );
PROVIDE( spi_flash_munmap = 0x40000bc4 );
PROVIDE( spi_flash_mmap_dump = 0x40000bd0 );
PROVIDE( spi_flash_check_and_flush_cache = 0x40000bdc );
PROVIDE( spi_flash_mmap_get_free_pages = 0x40000be8 );
PROVIDE( spi_flash_cache2phys = 0x40000bf4 );
PROVIDE( spi_flash_phys2cache = 0x40000c00 );
PROVIDE( spi_flash_disable_cache = 0x40000c0c );
PROVIDE( spi_flash_restore_cache = 0x40000c18 );
PROVIDE( spi_flash_cache_enabled = 0x40000c24 );
PROVIDE( spi_flash_enable_cache = 0x40000c30 );
PROVIDE( spi_cache_mode_switch = 0x40000c3c );
PROVIDE( spi_common_set_dummy_output = 0x40000c48 );
PROVIDE( spi_common_set_flash_cs_timing = 0x40000c54 );
PROVIDE( esp_rom_spi_set_address_bit_len = 0x40000c60 );
PROVIDE( esp_enable_cache_flash_wrap = 0x40000c6c );
PROVIDE( SPILock = 0x40000c78 );
PROVIDE( SPIMasterReadModeCnfig = 0x40000c84 );
PROVIDE( SPI_Common_Command = 0x40000c90 );
PROVIDE( SPI_WakeUp = 0x40000c9c );
PROVIDE( SPI_block_erase = 0x40000ca8 );
PROVIDE( SPI_chip_erase = 0x40000cb4 );
PROVIDE( SPI_init = 0x40000cc0 );
PROVIDE( SPI_page_program = 0x40000ccc );
PROVIDE( SPI_read_data = 0x40000cd8 );
PROVIDE( SPI_sector_erase = 0x40000ce4 );
PROVIDE( SelectSpiFunction = 0x40000cf0 );
PROVIDE( SetSpiDrvs = 0x40000cfc );
PROVIDE( Wait_SPI_Idle = 0x40000d08 );
PROVIDE( spi_dummy_len_fix = 0x40000d14 );
PROVIDE( Disable_QMode = 0x40000d20 );
PROVIDE( Enable_QMode = 0x40000d2c );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_spiflash_legacy_funcs = 0x3fceffe8 );
PROVIDE( rom_spiflash_legacy_data = 0x3fceffe4 );
PROVIDE( g_flash_guard_ops = 0x3fceffec );


/***************************************
 Group hal_soc
 ***************************************/

/* Functions */
PROVIDE( spi_flash_hal_poll_cmd_done = 0x40000d38 );
PROVIDE( spi_flash_hal_device_config = 0x40000d44 );
PROVIDE( spi_flash_hal_configure_host_io_mode = 0x40000d50 );
PROVIDE( spi_flash_hal_common_command = 0x40000d5c );
PROVIDE( spi_flash_hal_read = 0x40000d68 );
PROVIDE( spi_flash_hal_erase_chip = 0x40000d74 );
PROVIDE( spi_flash_hal_erase_sector = 0x40000d80 );
PROVIDE( spi_flash_hal_erase_block = 0x40000d8c );
PROVIDE( spi_flash_hal_program_page = 0x40000d98 );
PROVIDE( spi_flash_hal_set_write_protect = 0x40000da4 );
PROVIDE( spi_flash_hal_host_idle = 0x40000db0 );


/***************************************
 Group spi_flash_chips
 ***************************************/

/* Functions */
PROVIDE( spi_flash_chip_generic_probe = 0x40000ed0 );
PROVIDE( spi_flash_chip_generic_detect_size = 0x40000edc );
PROVIDE( spi_flash_chip_generic_write = 0x40000ee8 );
PROVIDE( spi_flash_chip_generic_write_encrypted = 0x40000ef4 );
PROVIDE( spi_flash_chip_generic_set_write_protect = 0x40000f00 );
PROVIDE( spi_flash_common_write_status_16b_wrsr = 0x40000f0c );
PROVIDE( spi_flash_chip_generic_reset = 0x40000f18 );
PROVIDE( spi_flash_chip_generic_erase_chip = 0x40000f24 );
PROVIDE( spi_flash_chip_generic_erase_sector = 0x40000f30 );
PROVIDE( spi_flash_chip_generic_erase_block = 0x40000f3c );
PROVIDE( spi_flash_chip_generic_page_program = 0x40000f48 );
PROVIDE( spi_flash_chip_generic_get_write_protect = 0x40000f54 );
PROVIDE( spi_flash_common_read_status_16b_rdsr_rdsr2 = 0x40000f60 );
PROVIDE( spi_flash_chip_generic_read_reg = 0x40000f6c );
PROVIDE( spi_flash_chip_generic_yield = 0x40000f78 );
PROVIDE( spi_flash_generic_wait_host_idle = 0x40000f84 );
PROVIDE( spi_flash_chip_generic_wait_idle = 0x40000f90 );
PROVIDE( spi_flash_chip_generic_config_host_io_mode = 0x40000f9c );
PROVIDE( spi_flash_chip_generic_read = 0x40000fa8 );
PROVIDE( spi_flash_common_read_status_8b_rdsr2 = 0x40000fb4 );
PROVIDE( spi_flash_chip_generic_get_io_mode = 0x40000fc0 );
PROVIDE( spi_flash_common_read_status_8b_rdsr = 0x40000fcc );
PROVIDE( spi_flash_common_write_status_8b_wrsr = 0x40000fd8 );
PROVIDE( spi_flash_common_write_status_8b_wrsr2 = 0x40000fe4 );
PROVIDE( spi_flash_common_set_io_mode = 0x40000ff0 );
PROVIDE( spi_flash_chip_generic_set_io_mode = 0x40000ffc );
PROVIDE( spi_flash_chip_gd_get_io_mode = 0x40001008 );
PROVIDE( spi_flash_chip_gd_probe = 0x40001014 );
PROVIDE( spi_flash_chip_gd_set_io_mode = 0x40001020 );
/* Data (.data, .bss, .rodata) */
PROVIDE( spi_flash_chip_generic_config_data = 0x3fceffe0 );


/***************************************
 Group memspi_host
 ***************************************/

/* Functions */
PROVIDE( memspi_host_read_id_hs = 0x4000102c );
PROVIDE( memspi_host_read_status_hs = 0x40001038 );
PROVIDE( memspi_host_flush_cache = 0x40001044 );
PROVIDE( memspi_host_erase_chip = 0x40001050 );
PROVIDE( memspi_host_erase_sector = 0x4000105c );
PROVIDE( memspi_host_erase_block = 0x40001068 );
PROVIDE( memspi_host_program_page = 0x40001074 );
PROVIDE( memspi_host_read = 0x40001080 );
PROVIDE( memspi_host_set_write_protect = 0x4000108c );
PROVIDE( memspi_host_set_max_read_len = 0x40001098 );
PROVIDE( memspi_host_read_data_slicer = 0x400010a4 );
PROVIDE( memspi_host_write_data_slicer = 0x400010b0 );


/***************************************
 Group esp_flash
 ***************************************/

/* Functions */
PROVIDE( esp_flash_chip_driver_initialized = 0x400010bc );
PROVIDE( esp_flash_read_id = 0x400010c8 );
PROVIDE( esp_flash_get_size = 0x400010d4 );
PROVIDE( esp_flash_erase_chip = 0x400010e0 );
PROVIDE( rom_esp_flash_erase_region = 0x400010ec );
PROVIDE( esp_flash_get_chip_write_protect = 0x400010f8 );
PROVIDE( esp_flash_set_chip_write_protect = 0x40001104 );
PROVIDE( esp_flash_get_protectable_regions = 0x40001110 );
PROVIDE( esp_flash_get_protected_region = 0x4000111c );
PROVIDE( esp_flash_set_protected_region = 0x40001128 );
PROVIDE( esp_flash_read = 0x40001134 );
PROVIDE( esp_flash_write = 0x40001140 );
PROVIDE( esp_flash_write_encrypted = 0x4000114c );
PROVIDE( esp_flash_read_encrypted = 0x40001158 );
PROVIDE( esp_flash_get_io_mode = 0x40001164 );
PROVIDE( esp_flash_set_io_mode = 0x40001170 );
PROVIDE( spi_flash_boot_attach = 0x4000117c );
PROVIDE( spi_flash_dump_counters = 0x40001188 );
PROVIDE( spi_flash_get_counters = 0x40001194 );
PROVIDE( spi_flash_op_counters_config = 0x400011a0 );
PROVIDE( spi_flash_reset_counters = 0x400011ac );
PROVIDE( esp_flash_read_chip_id = 0x400011b8 );
PROVIDE( detect_spi_flash_chip = 0x400011c4 );
PROVIDE( esp_rom_spiflash_write_disable = 0x400011d0 );
/* Data (.data, .bss, .rodata) */
PROVIDE( esp_flash_default_chip = 0x3fceffdc );
PROVIDE( esp_flash_api_funcs = 0x3fceffd8 );


/***************************************
 Group cache
 ***************************************/

/* Functions */
PROVIDE( Cache_Get_ICache_Line_Size = 0x400015fc );
PROVIDE( Cache_Get_DCache_Line_Size = 0x40001608 );
PROVIDE( Cache_Get_Mode = 0x40001614 );
PROVIDE( Cache_Set_ICache_Mode = 0x40001620 );
PROVIDE( Cache_Set_DCache_Mode = 0x4000162c );
PROVIDE( Cache_Address_Through_ICache = 0x40001638 );
PROVIDE( Cache_Address_Through_DCache = 0x40001644 );
PROVIDE( Cache_Set_Default_Mode = 0x40001650 );
PROVIDE( Cache_Enable_Defalut_ICache_Mode = 0x4000165c );
PROVIDE( ROM_Boot_Cache_Init = 0x40001668 );
PROVIDE( Cache_Invalidate_ICache_Items = 0x40001674 );
PROVIDE( Cache_Invalidate_DCache_Items = 0x40001680 );
PROVIDE( Cache_Clean_Items = 0x4000168c );
PROVIDE( Cache_WriteBack_Items = 0x40001698 );
PROVIDE( Cache_Op_Addr = 0x400016a4 );
PROVIDE( Cache_Invalidate_Addr = 0x400016b0 );
PROVIDE( Cache_Clean_Addr = 0x400016bc );
PROVIDE( rom_Cache_WriteBack_Addr = 0x400016c8 );
PROVIDE( Cache_Invalidate_ICache_All = 0x400016d4 );
PROVIDE( Cache_Invalidate_DCache_All = 0x400016e0 );
PROVIDE( Cache_Clean_All = 0x400016ec );
PROVIDE( Cache_WriteBack_All = 0x400016f8 );
PROVIDE( Cache_Mask_All = 0x40001704 );
PROVIDE( Cache_UnMask_Dram0 = 0x40001710 );
PROVIDE( Cache_Suspend_ICache_Autoload = 0x4000171c );
PROVIDE( Cache_Resume_ICache_Autoload = 0x40001728 );
PROVIDE( Cache_Suspend_DCache_Autoload = 0x40001734 );
PROVIDE( Cache_Resume_DCache_Autoload = 0x40001740 );
PROVIDE( Cache_Start_ICache_Preload = 0x4000174c );
PROVIDE( Cache_ICache_Preload_Done = 0x40001758 );
PROVIDE( Cache_End_ICache_Preload = 0x40001764 );
PROVIDE( Cache_Start_DCache_Preload = 0x40001770 );
PROVIDE( Cache_DCache_Preload_Done = 0x4000177c );
PROVIDE( Cache_End_DCache_Preload = 0x40001788 );
PROVIDE( Cache_Config_ICache_Autoload = 0x40001794 );
PROVIDE( Cache_Config_ICache_Region_Autoload = 0x400017a0 );
PROVIDE( Cache_Enable_ICache_Autoload = 0x400017ac );
PROVIDE( Cache_Disable_ICache_Autoload = 0x400017b8 );
PROVIDE( Cache_Config_DCache_Autoload = 0x400017c4 );
PROVIDE( Cache_Config_DCache_Region_Autoload = 0x400017d0 );
PROVIDE( Cache_Enable_DCache_Autoload = 0x400017dc );
PROVIDE( Cache_Disable_DCache_Autoload = 0x400017e8 );
PROVIDE( Cache_Enable_ICache_PreLock = 0x400017f4 );
PROVIDE( Cache_Disable_ICache_PreLock = 0x40001800 );
PROVIDE( Cache_Lock_ICache_Items = 0x4000180c );
PROVIDE( Cache_Unlock_ICache_Items = 0x40001818 );
PROVIDE( Cache_Enable_DCache_PreLock = 0x40001824 );
PROVIDE( Cache_Disable_DCache_PreLock = 0x40001830 );
PROVIDE( Cache_Lock_DCache_Items = 0x4000183c );
PROVIDE( Cache_Unlock_DCache_Items = 0x40001848 );
PROVIDE( Cache_Lock_Addr = 0x40001854 );
PROVIDE( Cache_Unlock_Addr = 0x40001860 );
PROVIDE( Cache_Disable_ICache = 0x4000186c );
PROVIDE( Cache_Enable_ICache = 0x40001878 );
PROVIDE( Cache_Disable_DCache = 0x40001884 );
PROVIDE( Cache_Enable_DCache = 0x40001890 );
PROVIDE( rom_Cache_Suspend_ICache = 0x4000189c );
PROVIDE( Cache_Resume_ICache = 0x400018a8 );
PROVIDE( rom_Cache_Suspend_DCache = 0x400018b4 );
PROVIDE( Cache_Resume_DCache = 0x400018c0 );
PROVIDE( Cache_Occupy_Items = 0x400018cc );
PROVIDE( Cache_Occupy_Addr = 0x400018d8 );
PROVIDE( rom_Cache_Freeze_ICache_Enable = 0x400018e4 );
PROVIDE( Cache_Freeze_ICache_Disable = 0x400018f0 );
PROVIDE( rom_Cache_Freeze_DCache_Enable = 0x400018fc );
PROVIDE( Cache_Freeze_DCache_Disable = 0x40001908 );
PROVIDE( Cache_Set_IDROM_MMU_Size = 0x40001914 );
PROVIDE( flash2spiram_instruction_offset = 0x40001920 );
PROVIDE( flash2spiram_rodata_offset = 0x4000192c );
PROVIDE( flash_instr_rodata_start_page = 0x40001938 );
PROVIDE( flash_instr_rodata_end_page = 0x40001944 );
PROVIDE( Cache_Set_IDROM_MMU_Info = 0x40001950 );
PROVIDE( Cache_Get_IROM_MMU_End = 0x4000195c );
PROVIDE( Cache_Get_DROM_MMU_End = 0x40001968 );
PROVIDE( Cache_Owner_Init = 0x40001974 );
PROVIDE( Cache_Occupy_ICache_MEMORY = 0x40001980 );
PROVIDE( Cache_Occupy_DCache_MEMORY = 0x4000198c );
PROVIDE( Cache_MMU_Init = 0x40001998 );
PROVIDE( Cache_Ibus_MMU_Set = 0x400019a4 );
PROVIDE( Cache_Dbus_MMU_Set = 0x400019b0 );
PROVIDE( rom_Cache_Count_Flash_Pages = 0x400019bc );
PROVIDE( Cache_Flash_To_SPIRAM_Copy = 0x400019c8 );
PROVIDE( Cache_Travel_Tag_Memory = 0x400019d4 );
PROVIDE( Cache_Travel_Tag_Memory2 = 0x400019e0 );
PROVIDE( Cache_Get_Virtual_Addr = 0x400019ec );
PROVIDE( Cache_Get_Memory_BaseAddr = 0x400019f8 );
PROVIDE( Cache_Get_Memory_Addr = 0x40001a04 );
PROVIDE( Cache_Get_Memory_value = 0x40001a10 );
PROVIDE( rom_config_instruction_cache_mode = 0x40001a1c );
PROVIDE( rom_config_data_cache_mode = 0x40001a28 );
/* Data (.data, .bss, .rodata) */
PROVIDE( rom_cache_op_cb = 0x3fceffc8 );
PROVIDE( rom_cache_internal_table_ptr = 0x3fceffc4 );


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_get_apb_freq = 0x40001a34;
ets_get_cpu_frequency = 0x40001a40;
ets_update_cpu_frequency = 0x40001a4c;
ets_get_printf_channel = 0x40001a58;
ets_get_xtal_div = 0x40001a64;
ets_set_xtal_div = 0x40001a70;
ets_get_xtal_freq = 0x40001a7c;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_input_get = 0x40001a88;
gpio_matrix_in = 0x40001a94;
gpio_matrix_out = 0x40001aa0;
/* gpio_output_disable = 0x40001aac; */
/* gpio_output_enable = 0x40001ab8; */
gpio_output_set = 0x40001ac4;
gpio_pad_hold = 0x40001ad0;
gpio_pad_input_disable = 0x40001adc;
gpio_pad_input_enable = 0x40001ae8;
gpio_pad_pulldown = 0x40001af4;
gpio_pad_pullup = 0x40001b00;
gpio_pad_select_gpio = 0x40001b0c;
gpio_pad_set_drv = 0x40001b18;
gpio_pad_unhold = 0x40001b24;
gpio_pin_wakeup_disable = 0x40001b30;
gpio_pin_wakeup_enable = 0x40001b3c;
gpio_bypass_matrix_in = 0x40001b48;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
intr_matrix_set = 0x40001b54;
ets_intr_lock = 0x40001b60;
ets_intr_unlock = 0x40001b6c;
ets_isr_attach = 0x40001b78;
ets_isr_mask = 0x40001b84;
ets_isr_unmask = 0x40001b90;


/***************************************
 Group xtos
 ***************************************/

/* Functions */
xthal_bcopy = 0x40001b9c;
xthal_memcpy = 0x40001ba8;
xthal_get_ccompare = 0x40001bb4;
xthal_set_ccompare = 0x40001bc0;
xthal_get_ccount = 0x40001bcc;
xthal_get_interrupt = 0x40001bd8;
xthal_set_intclear = 0x40001be4;
_xtos_ints_off = 0x40001bf0;
_xtos_ints_on = 0x40001bfc;
_xtos_restore_intlevel = 0x40001c08;
_xtos_set_exception_handler = 0x40001c14;
_xtos_set_interrupt_handler = 0x40001c20;
_xtos_set_interrupt_handler_arg = 0x40001c2c;
_xtos_set_intlevel = 0x40001c38;
_xtos_set_vpri = 0x40001c44;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
md5_vector = 0x40001c50;
MD5Init = 0x40001c5c;
MD5Update = 0x40001c68;
MD5Final = 0x40001c74;
hmac_md5_vector = 0x40001c80;
hmac_md5 = 0x40001c8c;
crc32_le = 0x40001c98;
crc32_be = 0x40001ca4;
crc16_le = 0x40001cb0;
crc16_be = 0x40001cbc;
crc8_le = 0x40001cc8;
crc8_be = 0x40001cd4;
esp_crc8 = 0x40001ce0;
ets_sha_enable = 0x40001cec;
ets_sha_disable = 0x40001cf8;
ets_sha_get_state = 0x40001d04;
ets_sha_init = 0x40001d10;
ets_sha_process = 0x40001d1c;
ets_sha_starts = 0x40001d28;
ets_sha_update = 0x40001d34;
ets_sha_finish = 0x40001d40;
ets_sha_clone = 0x40001d4c;
ets_hmac_enable = 0x40001d58;
ets_hmac_disable = 0x40001d64;
ets_hmac_calculate_message = 0x40001d70;
ets_hmac_calculate_downstream = 0x40001d7c;
ets_hmac_invalidate_downstream = 0x40001d88;
ets_jtag_enable_temporarily = 0x40001d94;
ets_aes_enable = 0x40001da0;
ets_aes_disable = 0x40001dac;
ets_aes_setkey = 0x40001db8;
ets_aes_block = 0x40001dc4;
ets_bigint_enable = 0x40001dd0;
ets_bigint_disable = 0x40001ddc;
ets_bigint_multiply = 0x40001de8;
ets_bigint_modmult = 0x40001df4;
ets_bigint_modexp = 0x40001e00;
ets_bigint_wait_finish = 0x40001e0c;
ets_bigint_getz = 0x40001e18;
ets_ds_enable = 0x40001e24;
ets_ds_disable = 0x40001e30;
ets_ds_start_sign = 0x40001e3c;
ets_ds_is_busy = 0x40001e48;
ets_ds_finish_sign = 0x40001e54;
ets_ds_encrypt_params = 0x40001e60;
ets_aes_setkey_dec = 0x40001e6c;
ets_aes_setkey_enc = 0x40001e78;
ets_mgf1_sha256 = 0x40001e84;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x40001e90;
ets_efuse_program = 0x40001e9c;
ets_efuse_clear_program_registers = 0x40001ea8;
ets_efuse_write_key = 0x40001eb4;
ets_efuse_get_read_register_address = 0x40001ec0;
ets_efuse_get_key_purpose = 0x40001ecc;
ets_efuse_key_block_unused = 0x40001ed8;
ets_efuse_find_unused_key_block = 0x40001ee4;
ets_efuse_rs_calculate = 0x40001ef0;
ets_efuse_count_unused_key_blocks = 0x40001efc;
ets_efuse_secure_boot_enabled = 0x40001f08;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x40001f14;
ets_efuse_cache_encryption_enabled = 0x40001f20;
ets_efuse_download_modes_disabled = 0x40001f2c;
ets_efuse_find_purpose = 0x40001f38;
ets_efuse_flash_opi_5pads_power_sel_vddspi = 0x40001f44;
ets_efuse_force_send_resume = 0x40001f50;
ets_efuse_get_flash_delay_us = 0x40001f5c;
ets_efuse_get_mac = 0x40001f68;
ets_efuse_get_spiconfig = 0x40001f74;
ets_efuse_usb_print_is_disabled = 0x40001f80;
/*ets_efuse_get_uart_print_channel = 0x40001f8c;*/
ets_efuse_usb_serial_jtag_print_is_disabled = 0x40001f8c;
ets_efuse_get_uart_print_control = 0x40001f98;
ets_efuse_get_wp_pad = 0x40001fa4;
ets_efuse_legacy_spi_boot_mode_disabled = 0x40001fb0;
ets_efuse_security_download_modes_enabled = 0x40001fbc;
ets_efuse_set_timing = 0x40001fc8;
ets_efuse_jtag_disabled = 0x40001fd4;
ets_efuse_usb_download_mode_disabled = 0x40001fe0;
ets_efuse_usb_module_disabled = 0x40001fec;
ets_efuse_usb_device_disabled = 0x40001ff8;
ets_efuse_flash_octal_mode = 0x40002004;
ets_efuse_ecc_en = 0x40002010;
ets_efuse_ecc_flash_page_size = 0x4000201c;
ets_efuse_ecc_16to17_mode = 0x40002028;


/***************************************
 Group ecc
 ***************************************/

/* Functions */
ets_ecc_flash_enable = 0x40002034;
ets_ecc_flash_enable_all = 0x40002040;
ets_ecc_flash_disable = 0x4000204c;
ets_ecc_flash_disable_all = 0x40002058;
ets_ecc_get_flash_page_size = 0x40002064;
ets_ecc_set_flash_page_size = 0x40002070;
ets_ecc_set_flash_byte_mode = 0x4000207c;
ets_ecc_get_flash_byte_mode = 0x40002088;
ets_ecc_set_flash_range = 0x40002094;
ets_ecc_get_flash_range = 0x400020a0;
ets_ecc_sram_enable = 0x400020ac;
ets_ecc_sram_disable = 0x400020b8;
ets_ecc_sram_enable_all = 0x400020c4;
ets_ecc_sram_disable_all = 0x400020d0;
ets_ecc_get_sram_page_size = 0x400020dc;
ets_ecc_set_sram_page_size = 0x400020e8;
ets_ecc_get_sram_byte_mode = 0x400020f4;
ets_ecc_set_sram_byte_mode = 0x40002100;
ets_ecc_set_sram_range = 0x4000210c;
ets_ecc_get_sram_range = 0x40002118;
/* Data (.data, .bss, .rodata) */
ets_ecc_table_ptr = 0x3fceffc0;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x40002124;
ets_rsa_pss_verify = 0x40002130;
ets_secure_boot_verify_bootloader_with_keys = 0x4000213c;
ets_secure_boot_verify_signature = 0x40002148;
ets_secure_boot_read_key_digests = 0x40002154;
ets_secure_boot_revoke_public_key_digest = 0x40002160;


/***************************************
 Group usb_uart
 ***************************************/

/* Functions */
PROVIDE( usb_uart_otg_rx_one_char = 0x400025a4 );
PROVIDE( usb_uart_otg_rx_one_char_block = 0x400025b0 );
PROVIDE( usb_uart_otg_tx_flush = 0x400025bc );
PROVIDE( usb_uart_otg_tx_one_char = 0x400025c8 );
PROVIDE( usb_uart_device_rx_one_char = 0x400025d4 );
PROVIDE( usb_uart_device_rx_one_char_block = 0x400025e0 );
PROVIDE( usb_uart_device_tx_flush = 0x400025ec );
PROVIDE( usb_uart_device_tx_one_char = 0x400025f8 );
PROVIDE( Uart_Init_USB = 0x40002604 );
/* Data (.data, .bss, .rodata) */
PROVIDE( uart_acm_dev = 0x3fceffbc );
PROVIDE( g_uart_print = 0x3fceffb9 );
PROVIDE( g_usb_print = 0x3fceffb8 );


/***************************************
 Group usb_module
 ***************************************/

/* Functions */
cdc_acm_class_handle_req = 0x40002610;
cdc_acm_init = 0x4000261c;
cdc_acm_fifo_fill = 0x40002628;
cdc_acm_rx_fifo_cnt = 0x40002634;
cdc_acm_fifo_read = 0x40002640;
cdc_acm_irq_tx_enable = 0x4000264c;
cdc_acm_irq_tx_disable = 0x40002658;
cdc_acm_irq_state_enable = 0x40002664;
cdc_acm_irq_state_disable = 0x40002670;
cdc_acm_irq_tx_ready = 0x4000267c;
cdc_acm_irq_rx_enable = 0x40002688;
cdc_acm_irq_rx_disable = 0x40002694;
cdc_acm_irq_rx_ready = 0x400026a0;
cdc_acm_irq_is_pending = 0x400026ac;
cdc_acm_irq_callback_set = 0x400026b8;
cdc_acm_line_ctrl_set = 0x400026c4;
cdc_acm_line_ctrl_get = 0x400026d0;
cdc_acm_poll_out = 0x400026dc;
chip_usb_dw_did_persist = 0x400026e8;
chip_usb_dw_init = 0x400026f4;
chip_usb_detach = 0x40002700;
chip_usb_dw_prepare_persist = 0x4000270c;
chip_usb_get_persist_flags = 0x40002718;
chip_usb_set_persist_flags = 0x40002724;
cpio_start = 0x40002730;
cpio_feed = 0x4000273c;
cpio_done = 0x40002748;
cpio_destroy = 0x40002754;
dfu_flash_init = 0x40002760;
dfu_flash_erase = 0x4000276c;
dfu_flash_program = 0x40002778;
dfu_flash_read = 0x40002784;
dfu_flash_attach = 0x40002790;
dfu_cpio_callback = 0x4000279c;
dfu_updater_get_err = 0x400027a8;
dfu_updater_clear_err = 0x400027b4;
dfu_updater_enable = 0x400027c0;
dfu_updater_begin = 0x400027cc;
dfu_updater_feed = 0x400027d8;
dfu_updater_end = 0x400027e4;
dfu_updater_set_raw_addr = 0x400027f0;
dfu_updater_flash_read = 0x400027fc;
usb_dc_prepare_persist = 0x40002808;
usb_dw_isr_handler = 0x40002814;
usb_dc_attach = 0x40002820;
usb_dc_detach = 0x4000282c;
usb_dc_reset = 0x40002838;
usb_dc_set_address = 0x40002844;
usb_dc_ep_check_cap = 0x40002850;
usb_dc_ep_configure = 0x4000285c;
usb_dc_ep_set_stall = 0x40002868;
usb_dc_ep_clear_stall = 0x40002874;
usb_dc_ep_halt = 0x40002880;
usb_dc_ep_is_stalled = 0x4000288c;
usb_dc_ep_enable = 0x40002898;
usb_dc_ep_disable = 0x400028a4;
usb_dc_ep_flush = 0x400028b0;
usb_dc_ep_write_would_block = 0x400028bc;
usb_dc_ep_write = 0x400028c8;
usb_dc_ep_read_wait = 0x400028d4;
usb_dc_ep_read_continue = 0x400028e0;
usb_dc_ep_read = 0x400028ec;
usb_dc_ep_set_callback = 0x400028f8;
usb_dc_set_status_callback = 0x40002904;
usb_dc_ep_mps = 0x40002910;
usb_dc_check_poll_for_interrupts = 0x4000291c;
mac_addr_to_serial_str_desc = 0x40002928;
usb_set_current_descriptor = 0x40002934;
usb_get_descriptor = 0x40002940;
usb_dev_resume = 0x4000294c;
usb_dev_get_configuration = 0x40002958;
usb_set_config = 0x40002964;
usb_deconfig = 0x40002970;
usb_enable = 0x4000297c;
usb_disable = 0x40002988;
usb_write_would_block = 0x40002994;
usb_write = 0x400029a0;
usb_read = 0x400029ac;
usb_ep_set_stall = 0x400029b8;
usb_ep_clear_stall = 0x400029c4;
usb_ep_read_wait = 0x400029d0;
usb_ep_read_continue = 0x400029dc;
usb_transfer_ep_callback = 0x400029e8;
usb_transfer = 0x400029f4;
usb_cancel_transfer = 0x40002a00;
usb_transfer_sync = 0x40002a0c;
usb_dfu_set_detach_cb = 0x40002a18;
dfu_class_handle_req = 0x40002a24;
dfu_status_cb = 0x40002a30;
dfu_custom_handle_req = 0x40002a3c;
usb_dfu_init = 0x40002a48;
usb_dfu_force_detach = 0x40002a54;
usb_dev_deinit = 0x40002a60;
usb_dw_ctrl_deinit = 0x40002a6c;
/* Data (.data, .bss, .rodata) */
rom_usb_osglue = 0x3fceffac;


/***************************************
 Group bluetooth
 ***************************************/

/* Data (.data, .bss, .rodata) */
bt_rf_coex_cfg_p = 0x3fceffa8;
bt_rf_coex_hooks_p = 0x3fceffa4;
btdm_env_p = 0x3fceffa0;
g_rw_controller_task_handle = 0x3fceff9c;
g_rw_init_sem = 0x3fceff98;
g_rw_schd_queue = 0x3fceff94;
lld_init_env = 0x3fceff90;
lld_rpa_renew_env = 0x3fceff8c;
lld_scan_env = 0x3fceff88;
lld_scan_sync_env = 0x3fceff84;
lld_test_env = 0x3fceff80;
p_ble_util_buf_env = 0x3fceff7c;
p_lld_env = 0x3fceff78;
p_llm_env = 0x3fceff74;
r_h4tl_eif_p = 0x3fceff70;
r_hli_funcs_p = 0x3fceff6c;
r_ip_funcs_p = 0x3fceff68;
r_modules_funcs_p = 0x3fceff64;
r_osi_funcs_p = 0x3fceff60;
r_plf_funcs_p = 0x3fceff5c;
vhci_env_p = 0x3fceff58;
aa_gen = 0x3fceff54;
aes_env = 0x3fceff48;
bt_rf_coex_cfg_cb = 0x3fcefef8;
btdm_pwr_state = 0x3fcefef4;
btdm_slp_err = 0x3fcefef0;
ecc_env = 0x3fcefee8;
esp_handler = 0x3fcefee0;
esp_vendor_cmd = 0x3fcefed8;
g_adv_delay_dis = 0x3fcefed4;
g_conflict_elt = 0x3fcefed0;
g_eif_api = 0x3fcefec0;
g_event_empty = 0x3fcefeb4;
g_llc_state = 0x3fcefeaa;
g_llm_state = 0x3fcefea9;
g_max_evt_env = 0x3fcefea7;
g_misc_state = 0x3fcefea6;
g_rma_rule_db = 0x3fcefe8a;
g_rtp_rule_db = 0x3fcefe6e;
g_scan_forever = 0x3fcefe6d;
g_time_msb = 0x3fcefe6c;
h4tl_env = 0x3fcefe44;
hci_env = 0x3fcefe21;
hci_ext_host = 0x3fcefe20;
hci_fc_env = 0x3fcefe18;
hci_tl_env = 0x3fcefdec;
ke_env = 0x3fcefdbc;
ke_event_env = 0x3fcefd7c;
ke_task_env = 0x3fcefd00;
llc_env = 0x3fcefcd8;
lld_adv_env = 0x3fcefcb0;
lld_con_env = 0x3fcefc88;
lld_exp_sync_pos_tab = 0x3fcefc80;
lld_per_adv_env = 0x3fcefc58;
lld_sync_env = 0x3fcefc30;
llm_le_adv_flow_env = 0x3fcefc24;
rw_sleep_enable = 0x3fcefc20;
rwble_env = 0x3fcefc18;
rwip_env = 0x3fcefbfc;
rwip_param = 0x3fcefbf0;
rwip_prog_delay = 0x3fcefbec;
rwip_rf = 0x3fcefbb4;
sch_alarm_env = 0x3fcefbac;
sch_arb_env = 0x3fcefb98;
sch_plan_env = 0x3fcefb90;
sch_prog_env = 0x3fcefa8c;
sch_slice_env = 0x3fcefa2c;
sch_slice_params = 0x3fcefa24;
timer_env = 0x3fcefa1c;
unloaded_area = 0x3fcefa18;
vshci_state = 0x3fcefa14;
TASK_DESC_LLC = 0x3fcefa08;
TASK_DESC_LLM = 0x3fcef9fc;
TASK_DESC_VSHCI = 0x3fcef9f0;
co_default_bdaddr = 0x3fcef9e8;
dbg_assert_block = 0x3fcef9e4;
g_bt_plf_log_level = 0x3fcef9e0;
hci_cmd_desc_tab_vs_esp = 0x3fcef9bc;
hci_command_handler_tab_esp = 0x3fcef9a4;
privacy_en = 0x3fcef9a0;
sdk_cfg_priv_opts = 0x3fcef958;
BasePoint_x_256 = 0x3ff1ffdc;
BasePoint_y_256 = 0x3ff1ffbc;
DebugE256PublicKey_x = 0x3ff1ff9c;
DebugE256PublicKey_y = 0x3ff1ff7c;
DebugE256SecretKey = 0x3ff1ff5c;
ECC_4Win_Look_up_table = 0x3ff1f7a0;
LLM_AA_CT1 = 0x3ff1f79a;
LLM_AA_CT2 = 0x3ff1f798;
RF_TX_PW_CONV_TBL = 0x3ff1f790;
TASK_DESC_MISC = 0x3ff1f784;
adv_evt_prop2type = 0x3ff1f766;
adv_evt_type2prop = 0x3ff1f761;
aes_cmac_zero = 0x3ff1f751;
aes_k2_salt = 0x3ff1f741;
aes_k3_id64 = 0x3ff1f73c;
aes_k3_salt = 0x3ff1f72c;
aes_k4_id6 = 0x3ff1f728;
aes_k4_salt = 0x3ff1f718;
bigHexP256 = 0x3ff1f6ec;
byte_tx_time = 0x3ff1f6e2;
co_null_bdaddr = 0x3ff1f6dc;
co_phy_mask_to_rate = 0x3ff1f6d7;
co_phy_mask_to_value = 0x3ff1f6d2;
co_phy_to_rate = 0x3ff1f6ce;
co_phy_value_to_mask = 0x3ff1f6ca;
co_rate_to_byte_dur_us = 0x3ff1f6c5;
co_rate_to_phy = 0x3ff1f6c0;
co_rate_to_phy_mask = 0x3ff1f6bc;
co_sca2ppm = 0x3ff1f6ac;
coef_B = 0x3ff1f680;
connect_req_dur_tab = 0x3ff1f678;
ecc_Jacobian_InfinityPoint256 = 0x3ff1f5f4;
em_base_reg_lut = 0x3ff1f526;
fixed_tx_time = 0x3ff1f51e;
h4tl_msgtype2hdrlen = 0x3ff1f518;
hci_cmd_desc_root_tab = 0x3ff1f4e8;
hci_cmd_desc_tab_ctrl_bb = 0x3ff1f47c;
hci_cmd_desc_tab_info_par = 0x3ff1f44c;
hci_cmd_desc_tab_le = 0x3ff1f0b0;
hci_cmd_desc_tab_lk_ctrl = 0x3ff1f098;
hci_cmd_desc_tab_stat_par = 0x3ff1f08c;
hci_cmd_desc_tab_vs = 0x3ff1f050;
hci_evt_desc_tab = 0x3ff1f008;
hci_evt_le_desc_tab = 0x3ff1ef68;
hci_evt_le_desc_tab_esp = 0x3ff1ef60;
hci_rsvd_evt_msk = 0x3ff1ef57;
lld_aux_phy_to_rate = 0x3ff1ef54;
lld_init_max_aux_dur_tab = 0x3ff1ef4c;
lld_scan_map_legacy_pdu_to_evt_type = 0x3ff1ef44;
lld_scan_max_aux_dur_tab = 0x3ff1ef3c;
lld_sync_max_aux_dur_tab = 0x3ff1ef34;
llm_local_le_feats = 0x3ff1ef2c;
llm_local_le_states = 0x3ff1ef24;
llm_local_supp_cmds = 0x3ff1eefc;
maxSecretKey_256 = 0x3ff1eedc;
max_data_tx_time = 0x3ff1eed4;
one_bits = 0x3ff1eec3;
rwip_coex_cfg = 0x3ff1eebe;
rwip_priority = 0x3ff1eea8;
veryBigHexP256 = 0x3ff1ee5c;

/***************************************
 Group rom_pp
 ***************************************/

/* Functions */
esp_pp_rom_version_get = 0x40005250;
RC_GetBlockAckTime = 0x4000525c;
ebuf_list_remove = 0x40005268;
/*esf_buf_alloc = 0x40005274;*/
/*esf_buf_alloc_dynamic = 0x40005280;*/
/*esf_buf_recycle = 0x4000528c;*/
GetAccess = 0x40005298;
hal_mac_is_low_rate_enabled = 0x400052a4;
hal_mac_tx_get_blockack = 0x400052b0;
/* hal_mac_tx_set_ppdu = 0x400052bc;*/
ic_get_trc = 0x400052c8;
/* ic_mac_deinit = 0x400052d4; */
ic_mac_init = 0x400052e0;
ic_interface_enabled = 0x400052ec;
is_lmac_idle = 0x400052f8;
/*lmacAdjustTimestamp = 0x40005304;*/
lmacDiscardAgedMSDU = 0x40005310;
/*lmacDiscardMSDU = 0x4000531c;*/
/*lmacEndFrameExchangeSequence = 0x40005328;*/
lmacIsIdle = 0x40005334;
lmacIsLongFrame = 0x40005340;
/*lmacMSDUAged = 0x4000534c;*/
lmacPostTxComplete = 0x40005358;
lmacProcessAllTxTimeout = 0x40005364;
lmacProcessCollisions = 0x40005370;
lmacProcessRxSucData = 0x4000537c;
lmacReachLongLimit = 0x40005388;
lmacReachShortLimit = 0x40005394;
lmacRecycleMPDU = 0x400053a0;
lmacRxDone = 0x400053ac;
/*lmacSetTxFrame = 0x400053b8;*/
lmacTxDone = 0x400053c4;
/*lmacTxFrame = 0x400053d0;*/
mac_tx_set_duration = 0x400053dc;
/* mac_tx_set_htsig = 0x400053e8;*/
mac_tx_set_plcp0 = 0x400053f4;
/* mac_tx_set_plcp1 = 0x40005400;*/
mac_tx_set_plcp2 = 0x4000540c;
/* pm_check_state = 0x40005418; */
pm_disable_dream_timer = 0x40005424;
pm_disable_sleep_delay_timer = 0x40005430;
/*pm_dream = 0x4000543c;*/
pm_mac_wakeup = 0x40005448;
pm_mac_sleep = 0x40005454;
pm_enable_active_timer = 0x40005460;
pm_enable_sleep_delay_timer = 0x4000546c;
pm_local_tsf_process = 0x40005478;
pm_set_beacon_filter = 0x40005484;
pm_is_in_wifi_slice_threshold = 0x40005490;
pm_is_waked = 0x4000549c;
pm_keep_alive = 0x400054a8;
/* pm_on_beacon_rx = 0x400054b4; */
pm_on_data_rx = 0x400054c0;
pm_on_tbtt = 0x400054cc;
/* pm_parse_beacon = 0x400054d8;*/
/* pm_process_tim = 0x400054e4; */
/*pm_rx_beacon_process = 0x400054f0;*/
/* pm_rx_data_process = 0x400054fc; */
/*pm_sleep = 0x40005508;*/
pm_sleep_for = 0x40005514;
/* pm_tbtt_process = 0x40005520; */
ppAMPDU2Normal = 0x4000552c;
/*ppAssembleAMPDU = 0x40005538;*/
ppCalFrameTimes = 0x40005544;
ppCalSubFrameLength = 0x40005550;
/*ppCalTxAMPDULength = 0x4000555c;*/
ppCheckTxAMPDUlength = 0x40005568;
ppDequeueRxq_Locked = 0x40005574;
ppDequeueTxQ = 0x40005580;
ppEmptyDelimiterLength = 0x4000558c;
ppEnqueueRxq = 0x40005598;
ppEnqueueTxDone = 0x400055a4;
ppGetTxQFirstAvail_Locked = 0x400055b0;
ppGetTxframe = 0x400055bc;
/*ppMapTxQueue = 0x400055c8;*/
ppProcessRxPktHdr = 0x400055e0;
/*ppProcessTxQ = 0x400055ec;*/
ppRecordBarRRC = 0x400055f8;
lmacRequestTxopQueue = 0x40005604;
lmacReleaseTxopQueue = 0x40005610;
ppRecycleAmpdu = 0x4000561c;
ppRecycleRxPkt = 0x40005628;
ppResortTxAMPDU = 0x40005634;
ppResumeTxAMPDU = 0x40005640;
/* ppRxFragmentProc = 0x4000564c; */
/* ppRxPkt = 0x40005658; */
ppRxProtoProc = 0x40005664;
ppSearchTxQueue = 0x40005670;
ppSearchTxframe = 0x4000567c;
ppSelectNextQueue = 0x40005688;
ppSubFromAMPDU = 0x40005694;
/* ppTask = 0x400056a0; */
ppTxPkt = 0x400056ac;
ppTxProtoProc = 0x400056b8;
ppTxqUpdateBitmap = 0x400056c4;
/*pp_coex_tx_request = 0x400056d0;*/
pp_hdrsize = 0x400056dc;
pp_post = 0x400056e8;
pp_process_hmac_waiting_txq = 0x400056f4;
rcGetAmpduSched = 0x40005700;
rcUpdateRxDone = 0x4000570c;
rc_get_trc = 0x40005718;
rc_get_trc_by_index = 0x40005724;
rcAmpduLowerRate = 0x40005730;
rcampduuprate = 0x4000573c;
rcClearCurAMPDUSched = 0x40005748;
rcClearCurSched = 0x40005754;
rcClearCurStat = 0x40005760;
/*rcGetSched = 0x4000576c;*/
rcLowerSched = 0x40005778;
rcSetTxAmpduLimit = 0x40005784;
/* rcTxUpdatePer = 0x40005790;*/
rcUpdateAckSnr = 0x4000579c;
/*rcUpdateRate = 0x400057a8;*/
/* rcUpdateTxDone = 0x400057b4; */
/* rcUpdateTxDoneAmpdu2 = 0x400057c0;*/
rcUpSched = 0x400057cc;
rssi_margin = 0x400057d8;
rx11NRate2AMPDULimit = 0x400057e4;
TRC_AMPDU_PER_DOWN_THRESHOLD = 0x400057f0;
TRC_AMPDU_PER_UP_THRESHOLD = 0x400057fc;
trc_calc_duration = 0x40005808;
trc_isTxAmpduOperational = 0x40005814;
trc_onAmpduOp = 0x40005820;
TRC_PER_IS_GOOD = 0x4000582c;
trc_SetTxAmpduState = 0x40005838;
trc_tid_isTxAmpduOperational = 0x40005844;
trcAmpduSetState = 0x40005850;
wDevCheckBlockError = 0x4000585c;
/*wDev_AppendRxBlocks = 0x40005868;*/
wDev_DiscardFrame = 0x40005874;
wDev_GetNoiseFloor = 0x40005880;
wDev_IndicateAmpdu = 0x4000588c;
/*wDev_IndicateFrame = 0x40005898;*/
wdev_bank_store = 0x400058a4;
wdev_bank_load = 0x400058b0;
wdev_mac_reg_load = 0x400058bc;
wdev_mac_reg_store = 0x400058c8;
wdev_mac_special_reg_load = 0x400058d4;
wdev_mac_special_reg_store = 0x400058e0;
wdev_mac_wakeup = 0x400058ec;
wdev_mac_sleep = 0x400058f8;
hal_mac_is_dma_enable = 0x40005904;
/* wDev_ProcessFiq = 0x40005910; */
/*wDev_ProcessRxSucData = 0x4000591c;*/
wdevProcessRxSucDataAll = 0x40005928;
wdev_csi_len_align = 0x40005934;
ppDequeueTxDone_Locked = 0x40005940;
/*ppProcTxDone = 0x4000594c;*/
/*pm_tx_data_done_process = 0x40005958;*/
config_is_cache_tx_buf_enabled = 0x40005964;
//ppMapWaitTxq = 0x40005970;
ppProcessWaitingQueue = 0x4000597c;
ppDisableQueue = 0x40005988;
pm_allow_tx = 0x40005994;
wdev_is_data_in_rxlist = 0x400059a0;
ppProcTxCallback = 0x400059ac;
/* Data (.data, .bss, .rodata) */
our_instances_ptr = 0x3ff1ee58;
pTxRx = 0x3fcef954;
lmacConfMib_ptr = 0x3fcef950;
our_wait_eb = 0x3fcef94c;
our_tx_eb = 0x3fcef948;
pp_wdev_funcs = 0x3fcef944;
g_osi_funcs_p = 0x3fcef940;
wDevCtrl_ptr = 0x3fcef93c;
g_wdev_last_desc_reset_ptr = 0x3ff1ee54;
wDevMacSleep_ptr = 0x3fcef938;
g_lmac_cnt_ptr = 0x3fcef934;
our_controls_ptr = 0x3ff1ee50;
pp_sig_cnt_ptr = 0x3fcef930;
g_eb_list_desc_ptr = 0x3fcef92c;
s_fragment_ptr = 0x3fcef928;
if_ctrl_ptr = 0x3fcef924;
g_intr_lock_mux = 0x3fcef920;
g_wifi_global_lock = 0x3fcef91c;
s_wifi_queue = 0x3fcef918;
pp_task_hdl = 0x3fcef914;
s_pp_task_create_sem = 0x3fcef910;
s_pp_task_del_sem = 0x3fcef90c;
g_wifi_menuconfig_ptr = 0x3fcef908;
xphyQueue = 0x3fcef904;
ap_no_lr_ptr = 0x3fcef900;
rc11BSchedTbl_ptr = 0x3fcef8fc;
rc11NSchedTbl_ptr = 0x3fcef8f8;
rcLoRaSchedTbl_ptr = 0x3fcef8f4;
BasicOFDMSched_ptr = 0x3fcef8f0;
trc_ctl_ptr = 0x3fcef8ec;
g_pm_cnt_ptr = 0x3fcef8e8;
g_pm_ptr = 0x3fcef8e4;
g_pm_cfg_ptr = 0x3fcef8e0;
g_esp_mesh_quick_funcs_ptr = 0x3fcef8dc;
g_txop_queue_status_ptr = 0x3fcef8d8;
g_mac_sleep_en_ptr = 0x3fcef8d4;
g_mesh_is_root_ptr = 0x3fcef8d0;
g_mesh_topology_ptr = 0x3fcef8cc;
g_mesh_init_ps_type_ptr = 0x3fcef8c8;
g_mesh_is_started_ptr = 0x3fcef8c4;
g_config_func = 0x3fcef8c0;
g_net80211_tx_func = 0x3fcef8bc;
g_timer_func = 0x3fcef8b8;
s_michael_mic_failure_cb = 0x3fcef8b4;
wifi_sta_rx_probe_req = 0x3fcef8b0;
g_tx_done_cb_func = 0x3fcef8ac;
g_per_conn_trc = 0x3fcef860;
s_encap_amsdu_func = 0x3fcef85c;


/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x400059b8;
ampdu_dispatch = 0x400059c4;
ampdu_dispatch_all = 0x400059d0;
ampdu_dispatch_as_many_as_possible = 0x400059dc;
ampdu_dispatch_movement = 0x400059e8;
ampdu_dispatch_upto = 0x400059f4;
chm_is_at_home_channel = 0x40005a00;
cnx_node_is_existing = 0x40005a0c;
cnx_node_search = 0x40005a18;
ic_ebuf_recycle_rx = 0x40005a24;
ic_ebuf_recycle_tx = 0x40005a30;
ic_reset_rx_ba = 0x40005a3c;
ieee80211_align_eb = 0x40005a48;
/* ieee80211_ampdu_reorder = 0x40005a54; */
ieee80211_ampdu_start_age_timer = 0x40005a60;
/* ieee80211_encap_esfbuf = 0x40005a6c; */
ieee80211_is_tx_allowed = 0x40005a78;
ieee80211_output_pending_eb = 0x40005a84;
/*ieee80211_output_process = 0x40005a90;*/
ieee80211_set_tx_desc = 0x40005a9c;
/*sta_input = 0x40005aa8;*/
wifi_get_macaddr = 0x40005ab4;
wifi_rf_phy_disable = 0x40005ac0;
wifi_rf_phy_enable = 0x40005acc;
ic_ebuf_alloc = 0x40005ad8;
/*ieee80211_classify = 0x40005ae4;*/
ieee80211_copy_eb_header = 0x40005af0;
ieee80211_recycle_cache_eb = 0x40005afc;
ieee80211_search_node = 0x40005b08;
roundup2 = 0x40005b14;
ieee80211_crypto_encap = 0x40005b20;
/* ieee80211_crypto_decap = 0x40005b2c; */
/* ieee80211_decap = 0x40005b38; */
ieee80211_set_tx_pti = 0x40005b44;
wifi_is_started = 0x40005b50;
ieee80211_gettid = 0x40005b5c;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x3fcef858;
g_scan = 0x3fcef854;
g_chm = 0x3fcef850;
g_ic_ptr = 0x3fcef84c;
g_hmac_cnt_ptr = 0x3fcef848;
g_tx_cacheq_ptr = 0x3fcef844;
s_netstack_free = 0x3fcef840;
mesh_rxcb = 0x3fcef83c;
sta_rxcb = 0x3fcef838;


/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
esp_coex_rom_version_get = 0x40005b68;
coex_bt_release = 0x40005b74;
coex_bt_request = 0x40005b80;
coex_core_ble_conn_dyn_prio_get = 0x40005b8c;
/*coex_core_event_duration_get = 0x40005b98;*/
coex_core_pti_get = 0x40005ba4;
coex_core_release = 0x40005bb0;
coex_core_request = 0x40005bbc;
coex_core_status_get = 0x40005bc8;
/*coex_core_timer_idx_get = 0x40005bd4;*/
coex_event_duration_get = 0x40005be0;
coex_hw_timer_disable = 0x40005bec;
coex_hw_timer_enable = 0x40005bf8;
coex_hw_timer_set = 0x40005c04;
coex_schm_interval_set = 0x40005c10;
coex_schm_lock = 0x40005c1c;
coex_schm_unlock = 0x40005c28;
/*coex_status_get = 0x40005c34;*/
coex_wifi_release = 0x40005c40;
esp_coex_ble_conn_dynamic_prio_get = 0x40005c4c;
/* Data (.data, .bss, .rodata) */
coex_env_ptr = 0x3fcef834;
coex_pti_tab_ptr = 0x3fcef830;
coex_schm_env_ptr = 0x3fcef82c;
coexist_funcs = 0x3fcef828;
g_coa_funcs_p = 0x3fcef824;
g_coex_param_ptr = 0x3fcef820;


/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
phy_get_romfuncs = 0x40005c58;
rom_abs_temp = 0x40005c64;
rom_bb_bss_cbw40_dig = 0x40005c70;
rom_bb_wdg_test_en = 0x40005c7c;
rom_bb_wdt_get_status = 0x40005c88;
rom_bb_wdt_int_enable = 0x40005c94;
rom_bb_wdt_rst_enable = 0x40005ca0;
rom_bb_wdt_timeout_clear = 0x40005cac;
rom_cbw2040_cfg = 0x40005cb8;
rom_check_noise_floor = 0x40005cc4;
rom_chip_i2c_readReg = 0x40005cd0;
rom_chip_i2c_writeReg = 0x40005cdc;
rom_dc_iq_est = 0x40005ce8;
rom_disable_agc = 0x40005cf4;
rom_en_pwdet = 0x40005d00;
rom_enable_agc = 0x40005d0c;
rom_get_bbgain_db = 0x40005d18;
rom_get_data_sat = 0x40005d24;
rom_get_i2c_read_mask = 0x40005d30;
rom_get_pwctrl_correct = 0x40005d3c;
rom_i2c_readReg = 0x40005d48;
rom_i2c_readReg_Mask = 0x40005d54;
rom_i2c_writeReg = 0x40005d60;
rom_i2c_writeReg_Mask = 0x40005d6c;
rom_index_to_txbbgain = 0x40005d78;
rom_iq_est_disable = 0x40005d84;
rom_iq_est_enable = 0x40005d90;
rom_linear_to_db = 0x40005d9c;
rom_loopback_mode_en = 0x40005da8;
rom_mhz2ieee = 0x40005db4;
rom_noise_floor_auto_set = 0x40005dc0;
rom_pbus_debugmode = 0x40005dcc;
rom_pbus_force_mode = 0x40005dd8;
rom_pbus_force_test = 0x40005de4;
rom_pbus_rd = 0x40005df0;
rom_pbus_rd_addr = 0x40005dfc;
rom_pbus_rd_shift = 0x40005e08;
rom_pbus_set_dco = 0x40005e14;
rom_pbus_set_rxgain = 0x40005e20;
rom_pbus_workmode = 0x40005e2c;
rom_pbus_xpd_rx_off = 0x40005e38;
rom_pbus_xpd_rx_on = 0x40005e44;
rom_pbus_xpd_tx_off = 0x40005e50;
rom_pbus_xpd_tx_on = 0x40005e5c;
rom_phy_byte_to_word = 0x40005e68;
rom_phy_disable_cca = 0x40005e74;
rom_phy_enable_cca = 0x40005e80;
rom_phy_get_noisefloor = 0x40005e8c;
rom_phy_get_rx_freq = 0x40005e98;
rom_phy_set_bbfreq_init = 0x40005ea4;
rom_pow_usr = 0x40005eb0;
rom_pwdet_sar2_init = 0x40005ebc;
rom_read_hw_noisefloor = 0x40005ec8;
rom_read_sar_dout = 0x40005ed4;
rom_set_cal_rxdc = 0x40005ee0;
rom_set_chan_cal_interp = 0x40005eec;
rom_set_loopback_gain = 0x40005ef8;
rom_set_noise_floor = 0x40005f04;
rom_set_rxclk_en = 0x40005f10;
rom_set_tx_dig_gain = 0x40005f1c;
rom_set_txcap_reg = 0x40005f28;
rom_set_txclk_en = 0x40005f34;
rom_spur_cal = 0x40005f40;
rom_spur_reg_write_one_tone = 0x40005f4c;
rom_target_power_add_backoff = 0x40005f58;
rom_tx_pwctrl_bg_init = 0x40005f64;
rom_txbbgain_to_index = 0x40005f70;
rom_wifi_11g_rate_chg = 0x40005f7c;
rom_write_gain_mem = 0x40005f88;
chip728_phyrom_version = 0x40005f94;
rom_disable_wifi_agc = 0x40005fa0;
rom_enable_wifi_agc = 0x40005fac;
rom_bt_index_to_bb = 0x40005fb8;
rom_bt_bb_to_index = 0x40005fc4;
rom_spur_coef_cfg = 0x40005fd0;
rom_bb_bss_cbw40 = 0x40005fdc;
rom_set_cca = 0x40005fe8;
rom_tx_paon_set = 0x40005ff4;
rom_i2cmst_reg_init = 0x40006000;
rom_iq_corr_enable = 0x4000600c;
rom_fe_reg_init = 0x40006018;
rom_agc_reg_init = 0x40006024;
rom_bb_reg_init = 0x40006030;
rom_mac_enable_bb = 0x4000603c;
rom_bb_wdg_cfg = 0x40006048;
rom_force_txon = 0x40006054;
rom_fe_txrx_reset = 0x40006060;
rom_set_rx_comp = 0x4000606c;
rom_set_pbus_reg = 0x40006078;
rom_write_chan_freq = 0x40006084;
rom_phy_xpd_rf = 0x40006090;
rom_set_xpd_sar = 0x4000609c;
rom_get_target_power_offset = 0x400060a8;
rom_write_txrate_power_offset = 0x400060b4;
rom_get_rate_fcc_index = 0x400060c0;
rom_get_rate_target_power = 0x400060cc;
rom_pkdet_vol_start = 0x400060d8;
rom_read_sar2_code = 0x400060e4;
rom_get_sar2_vol = 0x400060f0;
rom_get_pll_vol = 0x400060fc;
rom_get_phy_target_power = 0x40006108;
rom_temp_to_power = 0x40006114;
rom_phy_track_pll_cap = 0x40006120;
rom_phy_pwdet_always_en = 0x4000612c;
rom_phy_pwdet_onetime_en = 0x40006138;
rom_get_i2c_mst0_mask = 0x40006144;
rom_get_i2c_hostid = 0x40006150;
rom_enter_critical_phy = 0x4000615c;
rom_exit_critical_phy = 0x40006168;
rom_chip_i2c_readReg_org = 0x40006174;
rom_i2c_paral_set_mst0 = 0x40006180;
rom_i2c_paral_set_read = 0x4000618c;
rom_i2c_paral_read = 0x40006198;
rom_i2c_paral_write = 0x400061a4;
rom_i2c_paral_write_num = 0x400061b0;
rom_i2c_paral_write_mask = 0x400061bc;
rom_bb_bss_cbw40_ana = 0x400061c8;
rom_chan_to_freq = 0x400061d4;
rom_open_i2c_xpd = 0x400061e0;
rom_dac_rate_set = 0x400061ec;
rom_tsens_read_init = 0x400061f8;
rom_tsens_code_read = 0x40006204;
rom_tsens_index_to_dac = 0x40006210;
rom_tsens_index_to_offset = 0x4000621c;
rom_tsens_dac_cal = 0x40006228;
rom_code_to_temp = 0x40006234;
rom_write_pll_cap_mem = 0x40006240;
rom_pll_correct_dcap = 0x4000624c;
rom_phy_en_hw_set_freq = 0x40006258;
rom_phy_dis_hw_set_freq = 0x40006264;
rom_pll_vol_cal = 0x40006270;
rom_wrtie_pll_cap = 0x4000627c;
rom_set_tx_gain_mem = 0x40006288;
rom_bt_tx_dig_gain = 0x40006294;
rom_bt_get_tx_gain = 0x400062a0;
rom_get_chan_target_power = 0x400062ac;
rom_get_tx_gain_value = 0x400062b8;
rom_wifi_tx_dig_gain = 0x400062c4;
rom_wifi_get_tx_gain = 0x400062d0;
rom_fe_i2c_reg_renew = 0x400062dc;
rom_wifi_agc_sat_gain = 0x400062e8;
rom_i2c_master_reset = 0x400062f4;
rom_bt_filter_reg = 0x40006300;
rom_phy_bbpll_cal = 0x4000630c;
rom_i2c_sar2_init_code = 0x40006318;
rom_phy_param_addr = 0x40006324;
rom_phy_reg_init = 0x40006330;
rom_set_chan_reg = 0x4000633c;
rom_phy_wakeup_init = 0x40006348;
rom_phy_i2c_init1 = 0x40006354;
rom_tsens_temp_read = 0x40006360;
rom_bt_track_pll_cap = 0x4000636c;
rom_wifi_track_pll_cap = 0x40006378;
rom_wifi_set_tx_gain = 0x40006384;
rom_txpwr_cal_track = 0x40006390;
rom_tx_pwctrl_background = 0x4000639c;
rom_bt_set_tx_gain = 0x400063a8;
rom_noise_check_loop = 0x400063b4;
rom_phy_close_rf = 0x400063c0;
rom_phy_xpd_tsens = 0x400063cc;
rom_phy_freq_mem_backup = 0x400063d8;
rom_phy_ant_init = 0x400063e4;
rom_bt_track_tx_power = 0x400063f0;
rom_wifi_track_tx_power = 0x400063fc;
rom_phy_dig_reg_backup = 0x40006408;
chip728_phyrom_version_num = 0x40006414;
rom_mac_tx_chan_offset = 0x40006420;
rom_rx_gain_force = 0x4000642c;
/* Data (.data, .bss, .rodata) */
phy_param_rom = 0x3fcef81c;
