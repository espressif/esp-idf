/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6.rom.ld for esp32c6
 *
 *
 * Generated from ./target/esp32c6/interface-esp32c6.yml md5sum 06c13e133e0743d09b87aba30d3e213b
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
rtc_get_wakeup_cause = 0x40000020;
rtc_unhold_all_pads = 0x40000024;
ets_printf = 0x40000028;
ets_install_putc1 = 0x4000002c;
ets_install_putc2 = 0x40000030;
ets_install_uart_printf = 0x40000034;
ets_install_usb_printf = 0x40000038;
ets_get_printf_channel = 0x4000003c;
ets_delay_us = 0x40000040;
ets_get_cpu_frequency = 0x40000044;
ets_update_cpu_frequency = 0x40000048;
ets_install_lock = 0x4000004c;
UartRxString = 0x40000050;
UartGetCmdLn = 0x40000054;
uart_tx_one_char = 0x40000058;
uart_tx_one_char2 = 0x4000005c;
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
roundup2 = 0x40000088;
multofup = 0x4000008c;
software_reset = 0x40000090;
software_reset_cpu = 0x40000094;
ets_clk_assist_debug_clock_enable = 0x40000098;
clear_super_wdt_reset_flag = 0x4000009c;
disable_default_watchdog = 0x400000a0;
esp_rom_set_rtc_wake_addr = 0x400000a4;
esp_rom_get_rtc_wake_addr = 0x400000a8;
send_packet = 0x400000ac;
recv_packet = 0x400000b0;
GetUartDevice = 0x400000b4;
UartDwnLdProc = 0x400000b8;
GetSecurityInfoProc = 0x400000bc;
Uart_Init = 0x400000c0;
ets_set_user_start = 0x400000c4;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x4004fffc;
ets_ops_table_ptr = 0x4087fff8;
g_saved_pc = 0x4087fffc;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x400000c8;
mz_free = 0x400000cc;
tdefl_compress = 0x400000d0;
tdefl_compress_buffer = 0x400000d4;
tdefl_compress_mem_to_heap = 0x400000d8;
tdefl_compress_mem_to_mem = 0x400000dc;
tdefl_compress_mem_to_output = 0x400000e0;
tdefl_get_adler32 = 0x400000e4;
tdefl_get_prev_return_status = 0x400000e8;
tdefl_init = 0x400000ec;
tdefl_write_image_to_png_file_in_memory = 0x400000f0;
tdefl_write_image_to_png_file_in_memory_ex = 0x400000f4;
tinfl_decompress = 0x400000f8;
tinfl_decompress_mem_to_callback = 0x400000fc;
tinfl_decompress_mem_to_heap = 0x40000100;
tinfl_decompress_mem_to_mem = 0x40000104;


/***************************************
 Group tjpgd
 ***************************************/

/* Functions */
jd_prepare = 0x40000108;
jd_decomp = 0x4000010c;


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
esp_rom_spiflash_wait_idle = 0x40000110;
esp_rom_spiflash_write_encrypted = 0x40000114;
esp_rom_spiflash_write_encrypted_dest = 0x40000118;
esp_rom_spiflash_write_encrypted_enable = 0x4000011c;
esp_rom_spiflash_write_encrypted_disable = 0x40000120;
esp_rom_spiflash_erase_chip = 0x40000124;
_esp_rom_spiflash_erase_sector = 0x40000128;
_esp_rom_spiflash_erase_block = 0x4000012c;
_esp_rom_spiflash_write = 0x40000130;
_esp_rom_spiflash_read = 0x40000134;
_esp_rom_spiflash_unlock = 0x40000138;
_SPIEraseArea = 0x4000013c;
_SPI_write_enable = 0x40000140;
esp_rom_spiflash_erase_sector = 0x40000144;
esp_rom_spiflash_erase_block = 0x40000148;
esp_rom_spiflash_write = 0x4000014c;
esp_rom_spiflash_read = 0x40000150;
esp_rom_spiflash_unlock = 0x40000154;
SPIEraseArea = 0x40000158;
SPI_write_enable = 0x4000015c;
esp_rom_spiflash_config_param = 0x40000160;
esp_rom_spiflash_read_user_cmd = 0x40000164;
esp_rom_spiflash_select_qio_pins = 0x40000168;
esp_rom_spi_flash_auto_sus_res = 0x4000016c;
esp_rom_spi_flash_send_resume = 0x40000170;
esp_rom_spi_flash_update_id = 0x40000174;
esp_rom_spiflash_config_clk = 0x40000178;
esp_rom_spiflash_config_readmode = 0x4000017c;
esp_rom_spiflash_read_status = 0x40000180;
esp_rom_spiflash_read_statushigh = 0x40000184;
esp_rom_spiflash_write_status = 0x40000188;
spi_cache_mode_switch = 0x4000018c;
spi_common_set_dummy_output = 0x40000190;
spi_common_set_flash_cs_timing = 0x40000194;
esp_rom_spi_set_address_bit_len = 0x40000198;
SPILock = 0x4000019c;
SPIMasterReadModeCnfig = 0x400001a0;
SPI_Common_Command = 0x400001a4;
SPI_WakeUp = 0x400001a8;
SPI_block_erase = 0x400001ac;
SPI_chip_erase = 0x400001b0;
SPI_init = 0x400001b4;
SPI_page_program = 0x400001b8;
SPI_read_data = 0x400001bc;
SPI_sector_erase = 0x400001c0;
SelectSpiFunction = 0x400001c4;
SetSpiDrvs = 0x400001c8;
Wait_SPI_Idle = 0x400001cc;
spi_dummy_len_fix = 0x400001d0;
Disable_QMode = 0x400001d4;
Enable_QMode = 0x400001d8;
spi_flash_attach = 0x400001dc;
spi_flash_get_chip_size = 0x400001e0;
spi_flash_guard_set = 0x400001e4;
spi_flash_guard_get = 0x400001e8;
spi_flash_read_encrypted = 0x400001ec;
/* Data (.data, .bss, .rodata) */
rom_spiflash_legacy_funcs = 0x4087fff0;
rom_spiflash_legacy_data = 0x4087ffec;
g_flash_guard_ops = 0x4087fff4;

/* Note: esp_rom_spiflash_write_disable was moved from esp32c6.rom.spiflash.ld */
esp_rom_spiflash_write_disable = 0x40000278;

/***************************************
 Group hal_systimer
 ***************************************/

/* Functions */
/* The following ROM functions are commented out because they're patched in the esp_rom_systimer.c */
/* systimer_hal_init = 0x400003c0; */
/* systimer_hal_deinit = 0x400003c4; */

systimer_hal_set_tick_rate_ops = 0x400003c8;
systimer_hal_get_counter_value = 0x400003cc;
systimer_hal_get_time = 0x400003d0;
systimer_hal_set_alarm_target = 0x400003d4;
systimer_hal_set_alarm_period = 0x400003d8;
systimer_hal_get_alarm_value = 0x400003dc;
systimer_hal_enable_alarm_int = 0x400003e0;
systimer_hal_on_apb_freq_update = 0x400003e4;
systimer_hal_counter_value_advance = 0x400003e8;
systimer_hal_enable_counter = 0x400003ec;
systimer_hal_select_alarm_mode = 0x400003f0;
systimer_hal_connect_alarm_counter = 0x400003f4;
systimer_hal_counter_can_stall_by_cpu = 0x400003f8;


/***************************************
 Group cache
 ***************************************/

/* Functions */
Cache_Get_ICache_Line_Size = 0x40000628;
Cache_Get_Mode = 0x4000062c;
Cache_Address_Through_Cache = 0x40000630;
ROM_Boot_Cache_Init = 0x40000634;
MMU_Set_Page_Mode = 0x40000638;
MMU_Get_Page_Mode = 0x4000063c;
Cache_Invalidate_ICache_Items = 0x40000640;
Cache_Op_Addr = 0x40000644;
Cache_Invalidate_Addr = 0x40000648;
Cache_Invalidate_ICache_All = 0x4000064c;
Cache_Mask_All = 0x40000650;
Cache_UnMask_Dram0 = 0x40000654;
Cache_Suspend_ICache_Autoload = 0x40000658;
Cache_Resume_ICache_Autoload = 0x4000065c;
Cache_Start_ICache_Preload = 0x40000660;
Cache_ICache_Preload_Done = 0x40000664;
Cache_End_ICache_Preload = 0x40000668;
Cache_Config_ICache_Autoload = 0x4000066c;
Cache_Enable_ICache_Autoload = 0x40000670;
Cache_Disable_ICache_Autoload = 0x40000674;
Cache_Enable_ICache_PreLock = 0x40000678;
Cache_Disable_ICache_PreLock = 0x4000067c;
Cache_Lock_ICache_Items = 0x40000680;
Cache_Unlock_ICache_Items = 0x40000684;
Cache_Lock_Addr = 0x40000688;
Cache_Unlock_Addr = 0x4000068c;
Cache_Disable_ICache = 0x40000690;
Cache_Enable_ICache = 0x40000694;
Cache_Suspend_ICache = 0x40000698;
Cache_Resume_ICache = 0x4000069c;
Cache_Freeze_ICache_Enable = 0x400006a0;
Cache_Freeze_ICache_Disable = 0x400006a4;
Cache_Set_IDROM_MMU_Size = 0x400006a8;
Cache_Get_IROM_MMU_End = 0x400006ac;
Cache_Get_DROM_MMU_End = 0x400006b0;
Cache_MMU_Init = 0x400006b4;
Cache_MSPI_MMU_Set = 0x400006b8;
Cache_Travel_Tag_Memory = 0x400006bc;
Cache_Get_Virtual_Addr = 0x400006c0;
/* Data (.data, .bss, .rodata) */
rom_cache_op_cb = 0x4087ffcc;
rom_cache_internal_table_ptr = 0x4087ffc8;


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_clk_get_xtal_freq = 0x400006c4;
ets_clk_get_cpu_freq = 0x400006c8;
ets_clk_apb_wait_ready = 0x400006cc;
ets_clk_mspi_apb_wait_ready = 0x400006d0;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_input_get = 0x400006d4;
gpio_matrix_in = 0x400006d8;
gpio_matrix_out = 0x400006dc;
gpio_output_disable = 0x400006e0;
gpio_output_enable = 0x400006e4;
gpio_output_set = 0x400006e8;
gpio_pad_hold = 0x400006ec;
gpio_pad_input_disable = 0x400006f0;
gpio_pad_input_enable = 0x400006f4;
gpio_pad_pulldown = 0x400006f8;
gpio_pad_pullup = 0x400006fc;
gpio_pad_select_gpio = 0x40000700;
gpio_pad_set_drv = 0x40000704;
gpio_pad_unhold = 0x40000708;
gpio_pin_wakeup_disable = 0x4000070c;
gpio_pin_wakeup_enable = 0x40000710;
gpio_bypass_matrix_in = 0x40000714;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x40000718;
esprv_intc_int_set_threshold = 0x4000071c;
esprv_intc_int_enable = 0x40000720;
esprv_intc_int_disable = 0x40000724;
esprv_intc_int_set_type = 0x40000728;
PROVIDE( intr_handler_set = 0x4000072c );
intr_matrix_set = 0x40000730;
ets_intr_lock = 0x40000734;
ets_intr_unlock = 0x40000738;
ets_isr_attach = 0x4000073c;
ets_isr_mask = 0x40000740;
ets_isr_unmask = 0x40000744;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
md5_vector = 0x40000748;
MD5Init = 0x4000074c;
MD5Update = 0x40000750;
MD5Final = 0x40000754;
crc32_le = 0x40000758;
crc16_le = 0x4000075c;
crc8_le = 0x40000760;
crc32_be = 0x40000764;
crc16_be = 0x40000768;
crc8_be = 0x4000076c;
esp_crc8 = 0x40000770;
ets_sha_enable = 0x40000774;
ets_sha_disable = 0x40000778;
ets_sha_get_state = 0x4000077c;
ets_sha_init = 0x40000780;
ets_sha_process = 0x40000784;
ets_sha_starts = 0x40000788;
ets_sha_update = 0x4000078c;
ets_sha_finish = 0x40000790;
ets_sha_clone = 0x40000794;
ets_hmac_enable = 0x40000798;
ets_hmac_disable = 0x4000079c;
ets_hmac_calculate_message = 0x400007a0;
ets_hmac_calculate_downstream = 0x400007a4;
ets_hmac_invalidate_downstream = 0x400007a8;
ets_jtag_enable_temporarily = 0x400007ac;
ets_aes_enable = 0x400007b0;
ets_aes_disable = 0x400007b4;
ets_aes_setkey = 0x400007b8;
ets_aes_block = 0x400007bc;
ets_aes_setkey_dec = 0x400007c0;
ets_aes_setkey_enc = 0x400007c4;
ets_bigint_enable = 0x400007c8;
ets_bigint_disable = 0x400007cc;
ets_bigint_multiply = 0x400007d0;
ets_bigint_modmult = 0x400007d4;
ets_bigint_modexp = 0x400007d8;
ets_bigint_wait_finish = 0x400007dc;
ets_bigint_getz = 0x400007e0;
ets_ds_enable = 0x400007e4;
ets_ds_disable = 0x400007e8;
ets_ds_start_sign = 0x400007ec;
ets_ds_is_busy = 0x400007f0;
ets_ds_finish_sign = 0x400007f4;
ets_ds_encrypt_params = 0x400007f8;
ets_mgf1_sha256 = 0x400007fc;
/* Data (.data, .bss, .rodata) */
crc32_le_table_ptr = 0x4004fff8;
crc16_le_table_ptr = 0x4004fff4;
crc8_le_table_ptr = 0x4004fff0;
crc32_be_table_ptr = 0x4004ffec;
crc16_be_table_ptr = 0x4004ffe8;
crc8_be_table_ptr = 0x4004ffe4;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x40000800;
ets_efuse_program = 0x40000804;
ets_efuse_clear_program_registers = 0x40000808;
ets_efuse_write_key = 0x4000080c;
ets_efuse_get_read_register_address = 0x40000810;
ets_efuse_get_key_purpose = 0x40000814;
ets_efuse_key_block_unused = 0x40000818;
ets_efuse_find_unused_key_block = 0x4000081c;
ets_efuse_rs_calculate = 0x40000820;
ets_efuse_count_unused_key_blocks = 0x40000824;
ets_efuse_secure_boot_enabled = 0x40000828;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x4000082c;
ets_efuse_cache_encryption_enabled = 0x40000830;
ets_efuse_download_modes_disabled = 0x40000834;
ets_efuse_find_purpose = 0x40000838;
ets_efuse_force_send_resume = 0x4000083c;
ets_efuse_get_flash_delay_us = 0x40000840;
ets_efuse_get_mac = 0x40000844;
ets_efuse_get_uart_print_control = 0x40000848;
ets_efuse_direct_boot_mode_disabled = 0x4000084c;
ets_efuse_security_download_modes_enabled = 0x40000850;
ets_efuse_set_timing = 0x40000854;
ets_efuse_jtag_disabled = 0x40000858;
ets_efuse_usb_print_is_disabled = 0x4000085c;
ets_efuse_usb_download_mode_disabled = 0x40000860;
ets_efuse_usb_device_disabled = 0x40000864;
ets_efuse_secure_boot_fast_wake_enabled = 0x40000868;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x4000086c;
ets_rsa_pss_verify = 0x40000870;
ets_secure_boot_verify_bootloader_with_keys = 0x40000874;
ets_secure_boot_verify_signature = 0x40000878;
ets_secure_boot_read_key_digests = 0x4000087c;
ets_secure_boot_revoke_public_key_digest = 0x40000880;


/***************************************
 Group usb_device_uart
 ***************************************/

/* Functions */
usb_serial_device_rx_one_char = 0x40000a80;
usb_serial_device_rx_one_char_block = 0x40000a84;
usb_serial_device_tx_flush = 0x40000a88;
usb_serial_device_tx_one_char = 0x40000a8c;


/***************************************
 Group lldesc
 ***************************************/

/* Functions */
lldesc_build_chain = 0x40000a90;


/***************************************
 Group sip
 ***************************************/

/* Functions */
sip_after_tx_complete = 0x40000a94;
sip_alloc_to_host_evt = 0x40000a98;
sip_download_begin = 0x40000a9c;
sip_get_ptr = 0x40000aa0;
sip_get_state = 0x40000aa4;
sip_init_attach = 0x40000aa8;
sip_install_rx_ctrl_cb = 0x40000aac;
sip_install_rx_data_cb = 0x40000ab0;
sip_is_active = 0x40000ab4;
sip_post_init = 0x40000ab8;
sip_reclaim_from_host_cmd = 0x40000abc;
sip_reclaim_tx_data_pkt = 0x40000ac0;
sip_send = 0x40000ac4;
sip_to_host_chain_append = 0x40000ac8;
sip_to_host_evt_send_done = 0x40000acc;


/***************************************
 Group slc
 ***************************************/

/* Functions */
slc_add_credits = 0x40000ad0;
slc_enable = 0x40000ad4;
slc_from_host_chain_fetch = 0x40000ad8;
slc_from_host_chain_recycle = 0x40000adc;
slc_has_pkt_to_host = 0x40000ae0;
slc_init_attach = 0x40000ae4;
slc_init_credit = 0x40000ae8;
slc_reattach = 0x40000aec;
slc_send_to_host_chain = 0x40000af0;
slc_set_host_io_max_window = 0x40000af4;
slc_to_host_chain_recycle = 0x40000af8;
