/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c61.rom.ld for esp32c61
 *
 *
 * Generated from ./target/esp32c6lite/interface-esp32c6lite.yml md5sum 27eb0efac0883ee622c22767242c9457
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
rtc_get_wakeup_cause = 0x4000001c;
pmu_enable_unhold_pads = 0x40000020;
ets_printf = 0x40000024;
ets_install_putc1 = 0x40000028;
ets_install_putc2 = 0x4000002c;
ets_install_uart_printf = 0x40000030;
ets_install_usb_printf = 0x40000034;
ets_get_printf_channel = 0x40000038;
ets_delay_us = 0x4000003c;
ets_get_cpu_frequency = 0x40000040;
ets_update_cpu_frequency = 0x40000044;
ets_install_lock = 0x40000048;
UartRxString = 0x4000004c;
UartGetCmdLn = 0x40000050;
uart_tx_one_char = 0x40000054;
uart_tx_one_char2 = 0x40000058;
uart_tx_one_char3 = 0x4000005c;
uart_rx_one_char = 0x40000060;
uart_rx_one_char_block = 0x40000064;
uart_rx_intr_handler = 0x40000068;
uart_rx_readbuff = 0x4000006c;
uartAttach = 0x40000070;
uart_tx_flush = 0x40000074;
uart_tx_wait_idle = 0x40000078;
uart_div_modify = 0x4000007c;
ets_write_char_uart = 0x40000080;
uart_tx_switch = 0x40000084;
uart_buff_switch = 0x40000088;
roundup2 = 0x4000008c;
multofup = 0x40000090;
software_reset = 0x40000094;
software_reset_cpu = 0x40000098;
ets_clk_assist_debug_clock_enable = 0x4000009c;
clear_super_wdt_reset_flag = 0x400000a0;
disable_default_watchdog = 0x400000a4;
esp_rom_set_rtc_wake_addr = 0x400000a8;
esp_rom_get_rtc_wake_addr = 0x400000ac;
send_packet = 0x400000b0;
recv_packet = 0x400000b4;
GetUartDevice = 0x400000b8;
UartDwnLdProc = 0x400000bc;
GetSecurityInfoProc = 0x400000c0;
Uart_Init = 0x400000c4;
ets_set_user_start = 0x400000c8;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x4003fffc;
ets_ops_table_ptr = 0x4084fff8;
g_saved_pc = 0x4084fffc;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x400000cc;
mz_free = 0x400000d0;
tdefl_compress = 0x400000d4;
tdefl_compress_buffer = 0x400000d8;
tdefl_compress_mem_to_heap = 0x400000dc;
tdefl_compress_mem_to_mem = 0x400000e0;
tdefl_compress_mem_to_output = 0x400000e4;
tdefl_get_adler32 = 0x400000e8;
tdefl_get_prev_return_status = 0x400000ec;
tdefl_init = 0x400000f0;
tdefl_write_image_to_png_file_in_memory = 0x400000f4;
tdefl_write_image_to_png_file_in_memory_ex = 0x400000f8;
tinfl_decompress = 0x400000fc;
tinfl_decompress_mem_to_callback = 0x40000100;
tinfl_decompress_mem_to_heap = 0x40000104;
tinfl_decompress_mem_to_mem = 0x40000108;


/***************************************
 Group tjpgd
 ***************************************/

/* Functions */
jd_prepare = 0x4000010c;
jd_decomp = 0x40000110;


/***************************************
 Group spi_extmem_common
 ***************************************/

/* Functions */
esp_rom_spi_cmd_config = 0x40000114;
esp_rom_spi_cmd_start = 0x40000118;
esp_rom_spi_set_op_mode = 0x4000011c;


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
esp_rom_spiflash_wait_idle = 0x40000120;
esp_rom_spiflash_write_encrypted = 0x40000124;
esp_rom_spiflash_write_encrypted_dest = 0x40000128;
esp_rom_spiflash_write_encrypted_enable = 0x4000012c;
esp_rom_spiflash_write_encrypted_disable = 0x40000130;
esp_rom_spiflash_erase_chip = 0x40000134;
_esp_rom_spiflash_erase_sector = 0x40000138;
_esp_rom_spiflash_erase_block = 0x4000013c;
_esp_rom_spiflash_write = 0x40000140;
_esp_rom_spiflash_read = 0x40000144;
_esp_rom_spiflash_unlock = 0x40000148;
_SPIEraseArea = 0x4000014c;
_SPI_write_enable = 0x40000150;
esp_rom_spiflash_erase_sector = 0x40000154;
esp_rom_spiflash_erase_block = 0x40000158;
esp_rom_spiflash_write = 0x4000015c;
esp_rom_spiflash_read = 0x40000160;
esp_rom_spiflash_unlock = 0x40000164;
SPIEraseArea = 0x40000168;
SPI_write_enable = 0x4000016c;
esp_rom_spiflash_config_param = 0x40000170;
esp_rom_spiflash_read_user_cmd = 0x40000174;
esp_rom_spiflash_select_qio_pins = 0x40000178;
esp_rom_spi_flash_auto_sus_res = 0x4000017c;
esp_rom_spi_flash_send_resume = 0x40000180;
esp_rom_spi_flash_update_id = 0x40000184;
esp_rom_spiflash_config_clk = 0x40000188;
esp_rom_spiflash_config_readmode = 0x4000018c;
esp_rom_spiflash_read_status = 0x40000190;
esp_rom_spiflash_read_statushigh = 0x40000194;
esp_rom_spiflash_write_status = 0x40000198;
esp_rom_spiflash_write_disable = 0x4000019c;
spi_cache_mode_switch = 0x400001a0;
spi_common_set_dummy_output = 0x400001a4;
spi_common_set_flash_cs_timing = 0x400001a8;
esp_rom_spi_set_address_bit_len = 0x400001ac;
SPILock = 0x400001b0;
SPIMasterReadModeCnfig = 0x400001b4;
SPI_Common_Command = 0x400001b8;
SPI_WakeUp = 0x400001bc;
SPI_block_erase = 0x400001c0;
SPI_chip_erase = 0x400001c4;
SPI_init = 0x400001c8;
SPI_page_program = 0x400001cc;
SPI_read_data = 0x400001d0;
SPI_sector_erase = 0x400001d4;
SelectSpiFunction = 0x400001d8;
SetSpiDrvs = 0x400001dc;
Wait_SPI_Idle = 0x400001e0;
spi_dummy_len_fix = 0x400001e4;
Disable_QMode = 0x400001e8;
Enable_QMode = 0x400001ec;
spi_flash_attach = 0x400001f0;
spi_flash_get_chip_size = 0x400001f4;
spi_flash_guard_set = 0x400001f8;
spi_flash_guard_get = 0x400001fc;
spi_flash_read_encrypted = 0x40000200;
/* Data (.data, .bss, .rodata) */
rom_spiflash_legacy_funcs = 0x4084fff0;
rom_spiflash_legacy_data = 0x4084ffec;
g_flash_guard_ops = 0x4084fff4;


/***************************************
 Group cache
 ***************************************/

/* Functions */
Cache_Get_Line_Size = 0x40000614;
Cache_Get_Mode = 0x40000618;
Cache_Address_Through_Cache = 0x4000061c;
ROM_Boot_Cache_Init = 0x40000620;
MMU_Set_Page_Mode = 0x40000624;
MMU_Get_Page_Mode = 0x40000628;
Cache_Sync_Items = 0x4000062c;
Cache_Op_Addr = 0x40000630;
/*Cache_Invalidate_Addr = 0x40000634; rom version API has issue that unable to access higher vaddr range, use IDF patch */
Cache_Clean_Addr = 0x40000638;
/*Cache_WriteBack_Addr = 0x4000063c; rom version API has issue that unable to access higher vaddr range, use IDF patch */
Cache_WriteBack_Invalidate_Addr = 0x40000640;
Cache_Invalidate_All = 0x40000644;
Cache_Clean_All = 0x40000648;
Cache_WriteBack_All = 0x4000064c;
Cache_WriteBack_Invalidate_All = 0x40000650;
Cache_Mask_All = 0x40000654;
Cache_UnMask_Dram0 = 0x40000658;
Cache_Suspend_Autoload = 0x4000065c;
Cache_Resume_Autoload = 0x40000660;
Cache_Start_Preload = 0x40000664;
Cache_Preload_Done = 0x40000668;
Cache_End_Preload = 0x4000066c;
Cache_Config_Autoload = 0x40000670;
Cache_Enable_Autoload = 0x40000674;
Cache_Disable_Autoload = 0x40000678;
Cache_Enable_PreLock = 0x4000067c;
Cache_Disable_PreLock = 0x40000680;
Cache_Lock_Items = 0x40000684;
Cache_Lock_Addr = 0x40000688;
Cache_Unlock_Addr = 0x4000068c;
Cache_Disable_Cache = 0x40000690;
Cache_Enable_Cache = 0x40000694;
Cache_Suspend_Cache = 0x40000698;
Cache_Resume_Cache = 0x4000069c;
Cache_Freeze_Enable = 0x400006a0;
Cache_Freeze_Disable = 0x400006a4;
Cache_Set_IDROM_MMU_Size = 0x400006a8;
Cache_Get_IROM_MMU_End = 0x400006ac;
Cache_Get_DROM_MMU_End = 0x400006b0;
Cache_MMU_Init = 0x400006b4;
Cache_MSPI_MMU_Set = 0x400006b8;
Cache_MSPI_MMU_Set_Secure = 0x400006bc;
Cache_Count_Flash_Pages = 0x400006c0;
Cache_Travel_Tag_Memory = 0x400006c4;
Cache_Get_Virtual_Addr = 0x400006c8;
flash2spiram_instruction_offset = 0x400006cc;
flash2spiram_rodata_offset = 0x400006d0;
flash_instr_rodata_start_page = 0x400006d4;
flash_instr_rodata_end_page = 0x400006d8;
Cache_Set_IDROM_MMU_Info = 0x400006dc;
Cache_Flash_To_SPIRAM_Copy = 0x400006e0;
/* Data (.data, .bss, .rodata) */
rom_cache_op_cb = 0x4084ffcc;
rom_cache_internal_table_ptr = 0x4084ffc8;


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_clk_get_xtal_freq = 0x400006e4;
ets_clk_get_cpu_freq = 0x400006e8;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_set_output_level = 0x400006ec;
gpio_get_input_level = 0x400006f0;
gpio_matrix_in = 0x400006f4;
gpio_matrix_out = 0x400006f8;
gpio_bypass_matrix_in = 0x400006fc;
/* gpio_output_disable = 0x40000700; */
/* gpio_output_enable = 0x40000704; */
gpio_pad_input_disable = 0x40000708;
gpio_pad_input_enable = 0x4000070c;
gpio_pad_pulldown = 0x40000710;
gpio_pad_pullup = 0x40000714;
gpio_pad_select_gpio = 0x40000718;
gpio_pad_set_drv = 0x4000071c;
gpio_pad_unhold = 0x40000720;
gpio_pad_hold = 0x40000724;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x40000728;
esprv_intc_int_set_threshold = 0x4000072c;
esprv_intc_int_enable = 0x40000730;
esprv_intc_int_disable = 0x40000734;
esprv_intc_int_set_type = 0x40000738;
PROVIDE( intr_handler_set = 0x4000073c );
intr_matrix_set = 0x40000740;
ets_intr_register_ctx = 0x40000744;
ets_intr_lock = 0x40000748;
ets_intr_unlock = 0x4000074c;
ets_isr_attach = 0x40000750;
ets_isr_mask = 0x40000754;
ets_isr_unmask = 0x40000758;


/***************************************
 Group crc
 ***************************************/

/* Functions */
crc32_le = 0x4000075c;
crc16_le = 0x40000760;
crc8_le = 0x40000764;
crc32_be = 0x40000768;
crc16_be = 0x4000076c;
crc8_be = 0x40000770;
esp_crc8 = 0x40000774;
/* Data (.data, .bss, .rodata) */
crc32_le_table_ptr = 0x4003fff8;
crc16_le_table_ptr = 0x4003fff4;
crc8_le_table_ptr = 0x4003fff0;
crc32_be_table_ptr = 0x4003ffec;
crc16_be_table_ptr = 0x4003ffe8;
crc8_be_table_ptr = 0x4003ffe4;


/***************************************
 Group md5
 ***************************************/

/* Functions */
md5_vector = 0x40000778;
MD5Init = 0x4000077c;
MD5Update = 0x40000780;
MD5Final = 0x40000784;


/***************************************
 Group hwcrypto
 ***************************************/

/* Functions */
ets_sha_enable = 0x40000788;
ets_sha_disable = 0x4000078c;
ets_sha_get_state = 0x40000790;
ets_sha_init = 0x40000794;
ets_sha_process = 0x40000798;
ets_sha_starts = 0x4000079c;
ets_sha_update = 0x400007a0;
ets_sha_finish = 0x400007a4;
ets_sha_clone = 0x400007a8;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x400007ac;
ets_efuse_program = 0x400007b0;
ets_efuse_clear_program_registers = 0x400007b4;
ets_efuse_write_key = 0x400007b8;
ets_efuse_get_read_register_address = 0x400007bc;
ets_efuse_get_key_purpose = 0x400007c0;
ets_efuse_key_block_unused = 0x400007c4;
ets_efuse_find_unused_key_block = 0x400007c8;
ets_efuse_rs_calculate = 0x400007cc;
ets_efuse_count_unused_key_blocks = 0x400007d0;
ets_efuse_secure_boot_enabled = 0x400007d4;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x400007d8;
ets_efuse_cache_encryption_enabled = 0x400007dc;
ets_efuse_download_modes_disabled = 0x400007e0;
ets_efuse_find_purpose = 0x400007e4;
ets_efuse_force_send_resume = 0x400007e8;
ets_efuse_get_flash_delay_us = 0x400007ec;
ets_efuse_get_uart_print_control = 0x400007f0;
ets_efuse_direct_boot_mode_disabled = 0x400007f4;
ets_efuse_security_download_modes_enabled = 0x400007f8;
ets_efuse_jtag_disabled = 0x400007fc;
ets_efuse_usb_print_is_disabled = 0x40000800;
ets_efuse_usb_download_mode_disabled = 0x40000804;
ets_efuse_usb_device_disabled = 0x40000808;
ets_efuse_secure_boot_fast_wake_enabled = 0x4000080c;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_ecdsa_verify = 0x40000810;
ets_secure_boot_verify_bootloader_with_keys = 0x40000814;
ets_secure_boot_verify_signature = 0x40000818;
ets_secure_boot_read_key_digests = 0x4000081c;
ets_secure_boot_revoke_public_key_digest = 0x40000820;


/***************************************
 Group usb_device_uart
 ***************************************/

/* Functions */
usb_serial_device_rx_one_char = 0x40000a20;
usb_serial_device_rx_one_char_block = 0x40000a24;
usb_serial_device_tx_flush = 0x40000a28;
usb_serial_device_tx_one_char = 0x40000a2c;
