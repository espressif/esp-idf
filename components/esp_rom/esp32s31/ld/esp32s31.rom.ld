/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group common
 ***************************************/

/* Functions */
rtc_get_reset_reason = 0x2f800018;
rtc_get_wakeup_cause = 0x2f80001c;
pmu_enable_unhold_pads = 0x2f800020;
ets_printf = 0x2f800024;
ets_install_putc1 = 0x2f800028;
ets_install_putc2 = 0x2f80002c;
ets_install_uart_printf = 0x2f800030;
ets_install_usb_printf = 0x2f800034;
ets_get_printf_channel = 0x2f800038;
ets_delay_us = 0x2f80003c;
ets_get_cpu_frequency = 0x2f800040;
ets_update_cpu_frequency = 0x2f800044;
ets_install_lock = 0x2f800048;
UartRxString = 0x2f80004c;
UartGetCmdLn = 0x2f800050;
uart_tx_one_char = 0x2f800054;
uart_tx_one_char2 = 0x2f800058;
uart_tx_one_char3 = 0x2f80005c;
uart_rx_one_char = 0x2f800060;
uart_rx_one_char_block = 0x2f800064;
uart_rx_intr_handler = 0x2f800068;
uart_rx_readbuff = 0x2f80006c;
uartAttach = 0x2f800070;
uart_tx_flush = 0x2f800074;
uart_tx_wait_idle = 0x2f800078;
uart_div_modify = 0x2f80007c;
ets_write_char_uart = 0x2f800080;
uart_tx_switch = 0x2f800084;
uart_buff_switch = 0x2f800088;
roundup2 = 0x2f80008c;
multofup = 0x2f800090;
software_reset = 0x2f800094;
software_reset_cpu = 0x2f800098;
ets_clk_assist_debug_clock_enable = 0x2f80009c;
clear_super_wdt_reset_flag = 0x2f8000a0;
disable_default_watchdog = 0x2f8000a4;
ets_set_appcpu_boot_addr = 0x2f8000a8;
esp_rom_set_rtc_wake_addr = 0x2f8000ac;
esp_rom_get_rtc_wake_addr = 0x2f8000b0;
send_packet = 0x2f8000b4;
recv_packet = 0x2f8000b8;
GetUartDevice = 0x2f8000bc;
UartDwnLdProc = 0x2f8000c0;
GetSecurityInfoProc = 0x2f8000c4;
Uart_Init = 0x2f8000c8;
ets_set_user_start = 0x2f8000cc;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x2f84fffc;
ets_ops_table_ptr = 0x2f07fff4;
g_saved_pc = 0x2f07fff8;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x2f8000d0;
mz_free = 0x2f8000d4;
tdefl_compress = 0x2f8000d8;
tdefl_compress_buffer = 0x2f8000dc;
tdefl_compress_mem_to_heap = 0x2f8000e0;
tdefl_compress_mem_to_mem = 0x2f8000e4;
tdefl_compress_mem_to_output = 0x2f8000e8;
tdefl_get_adler32 = 0x2f8000ec;
tdefl_get_prev_return_status = 0x2f8000f0;
tdefl_init = 0x2f8000f4;
tdefl_write_image_to_png_file_in_memory = 0x2f8000f8;
tdefl_write_image_to_png_file_in_memory_ex = 0x2f8000fc;
tinfl_decompress = 0x2f800100;
tinfl_decompress_mem_to_callback = 0x2f800104;
tinfl_decompress_mem_to_heap = 0x2f800108;
tinfl_decompress_mem_to_mem = 0x2f80010c;


/***************************************
 Group tjpgd
 ***************************************/

/* Functions */
jd_prepare = 0x2f800110;
jd_decomp = 0x2f800114;


/***************************************
 Group esp_dsp
 ***************************************/

/* Data (.data, .bss, .rodata) */
dsps_fft2r_w_table_fc32_1024 = 0x2f07fff0;


/***************************************
 Group spi_common_usr_cmd
 ***************************************/

/* Functions */
esp_rom_opiflash_exec_cmd = 0x2f800118;
/* Data (.data, .bss, .rodata) */
rom_spi_usr_cmd_legacy_funcs = 0x2f07ffec;


/***************************************
 Group spi_extmem_common
 ***************************************/

/* Functions */
esp_rom_spi_cmd_config = 0x2f80011c;
esp_rom_spi_cmd_start = 0x2f800120;
esp_rom_spi_set_op_mode = 0x2f800124;
esp_rom_spi_set_dtr_swap_mode = 0x2f800128;


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
esp_rom_spiflash_wait_idle = 0x2f80012c;
esp_rom_spiflash_write_encrypted = 0x2f800130;
esp_rom_spiflash_write_encrypted_dest = 0x2f800134;
esp_rom_spiflash_write_encrypted_enable = 0x2f800138;
esp_rom_spiflash_write_encrypted_disable = 0x2f80013c;
esp_rom_spiflash_erase_chip = 0x2f800140;
_esp_rom_spiflash_erase_sector = 0x2f800144;
_esp_rom_spiflash_erase_block = 0x2f800148;
_esp_rom_spiflash_write = 0x2f80014c;
_esp_rom_spiflash_read = 0x2f800150;
_esp_rom_spiflash_unlock = 0x2f800154;
_SPIEraseArea = 0x2f800158;
_SPI_write_enable = 0x2f80015c;
esp_rom_spiflash_erase_sector = 0x2f800160;
esp_rom_spiflash_erase_block = 0x2f800164;
esp_rom_spiflash_write = 0x2f800168;
esp_rom_spiflash_read = 0x2f80016c;
esp_rom_spiflash_unlock = 0x2f800170;
SPIEraseArea = 0x2f800174;
SPI_write_enable = 0x2f800178;
esp_rom_spiflash_config_param = 0x2f80017c;
esp_rom_spiflash_read_user_cmd = 0x2f800180;
esp_rom_spiflash_select_qio_pins = 0x2f800184;
esp_rom_spi_flash_auto_sus_res = 0x2f800188;
esp_rom_spiflash_cache_mode_config = 0x2f80018c;
esp_rom_spi_flash_send_resume = 0x2f800190;
esp_rom_spi_flash_update_id = 0x2f800194;
esp_rom_spiflash_config_clk = 0x2f800198;
esp_rom_spiflash_config_readmode = 0x2f80019c;
esp_rom_spiflash_read_status = 0x2f8001a0;
esp_rom_spiflash_read_statushigh = 0x2f8001a4;
esp_rom_spiflash_write_status = 0x2f8001a8;
esp_rom_spiflash_write_disable = 0x2f8001ac;
spi_cache_mode_switch = 0x2f8001b0;
spi_common_set_dummy_output = 0x2f8001b4;
spi_common_set_flash_cs_timing = 0x2f8001b8;
esp_rom_spi_set_address_bit_len = 0x2f8001bc;
SPILock = 0x2f8001c0;
SPIMasterReadModeCnfig = 0x2f8001c4;
SPI_Common_Command = 0x2f8001c8;
SPI_WakeUp = 0x2f8001cc;
SPI_block_erase = 0x2f8001d0;
SPI_chip_erase = 0x2f8001d4;
SPI_init = 0x2f8001d8;
SPI_page_program = 0x2f8001dc;
SPI_read_data = 0x2f8001e0;
SPI_sector_erase = 0x2f8001e4;
SelectSpiFunction = 0x2f8001e8;
SetSpiDrvs = 0x2f8001ec;
Wait_SPI_Idle = 0x2f8001f0;
spi_dummy_len_fix = 0x2f8001f4;
Disable_QMode = 0x2f8001f8;
Enable_QMode = 0x2f8001fc;
spi_flash_attach = 0x2f800200;
spi_flash_get_chip_size = 0x2f800204;
spi_flash_guard_set = 0x2f800208;
spi_flash_guard_get = 0x2f80020c;
spi_flash_read_encrypted = 0x2f800210;
/* Data (.data, .bss, .rodata) */
rom_spiflash_legacy_funcs = 0x2f07ffe4;
rom_spiflash_legacy_data = 0x2f07ffe0;
g_flash_guard_ops = 0x2f07ffe8;


/***************************************
 Group cache
 ***************************************/

/* Functions */
Cache_Get_L1_ICache_Line_Size = 0x2f8005d0;
Cache_Get_L1_DCache_Line_Size = 0x2f8005d4;
Cache_Get_Mode = 0x2f8005d8;
Cache_Address_Through_Cache = 0x2f8005dc;
ROM_Boot_Cache_Init = 0x2f8005e0;
Cache_Sync_Addr = 0x2f8005e4;
Cache_Invalidate_Addr = 0x2f8005e8;
Cache_Clean_Addr = 0x2f8005ec;
Cache_WriteBack_Addr = 0x2f8005f0;
Cache_WriteBack_Invalidate_Addr = 0x2f8005f4;
Cache_Invalidate_All = 0x2f8005f8;
Cache_Clean_All = 0x2f8005fc;
Cache_WriteBack_All = 0x2f800600;
Cache_WriteBack_Invalidate_All = 0x2f800604;
Cache_Mask_All = 0x2f800608;
Cache_Suspend_L1_CORE0_ICache_Autoload = 0x2f80060c;
Cache_Resume_L1_CORE0_ICache_Autoload = 0x2f800610;
Cache_Suspend_L1_CORE1_ICache_Autoload = 0x2f800614;
Cache_Resume_L1_CORE1_ICache_Autoload = 0x2f800618;
Cache_Suspend_L1_DCache_Autoload = 0x2f80061c;
Cache_Resume_L1_DCache_Autoload = 0x2f800620;
Cache_Start_L1_CORE0_ICache_Preload = 0x2f800624;
Cache_L1_CORE0_ICache_Preload_Done = 0x2f800628;
Cache_End_L1_CORE0_ICache_Preload = 0x2f80062c;
Cache_Start_L1_CORE1_ICache_Preload = 0x2f800630;
Cache_L1_CORE1_ICache_Preload_Done = 0x2f800634;
Cache_End_L1_CORE1_ICache_Preload = 0x2f800638;
Cache_Start_L1_DCache_Preload = 0x2f80063c;
Cache_L1_DCache_Preload_Done = 0x2f800640;
Cache_End_L1_DCache_Preload = 0x2f800644;
Cache_Config_L1_CORE0_ICache_Autoload = 0x2f800648;
Cache_Enable_L1_CORE0_ICache_Autoload = 0x2f80064c;
Cache_Disable_L1_CORE0_ICache_Autoload = 0x2f800650;
Cache_Config_L1_CORE1_ICache_Autoload = 0x2f800654;
Cache_Enable_L1_CORE1_ICache_Autoload = 0x2f800658;
Cache_Disable_L1_CORE1_ICache_Autoload = 0x2f80065c;
Cache_Config_L1_DCache_Autoload = 0x2f800660;
Cache_Enable_L1_DCache_Autoload = 0x2f800664;
Cache_Disable_L1_DCache_Autoload = 0x2f800668;
Cache_Enable_L1_CORE0_ICache_PreLock = 0x2f80066c;
Cache_Disable_L1_CORE0_ICache_PreLock = 0x2f800670;
Cache_Enable_L1_CORE1_ICache_PreLock = 0x2f800674;
Cache_Disable_L1_CORE1_ICache_PreLock = 0x2f800678;
Cache_Enable_L1_DCache_PreLock = 0x2f80067c;
Cache_Disable_L1_DCache_PreLock = 0x2f800680;
Cache_Lock_Addr = 0x2f800684;
Cache_Unlock_Addr = 0x2f800688;
Cache_Disable_L1_CORE0_ICache = 0x2f80068c;
Cache_Enable_L1_CORE0_ICache = 0x2f800690;
Cache_Suspend_L1_CORE0_ICache = 0x2f800694;
Cache_Resume_L1_CORE0_ICache = 0x2f800698;
Cache_Disable_L1_CORE1_ICache = 0x2f80069c;
Cache_Enable_L1_CORE1_ICache = 0x2f8006a0;
Cache_Suspend_L1_CORE1_ICache = 0x2f8006a4;
Cache_Resume_L1_CORE1_ICache = 0x2f8006a8;
Cache_Disable_L1_DCache = 0x2f8006ac;
Cache_Enable_L1_DCache = 0x2f8006b0;
Cache_Suspend_L1_DCache = 0x2f8006b4;
Cache_Resume_L1_DCache = 0x2f8006b8;
Cache_FLASH_MMU_Init = 0x2f8006bc;
Cache_PSRAM_MMU_Init = 0x2f8006c0;
Cache_FLASH_MMU_Set = 0x2f8006c4;
Cache_FLASH_MMU_Set_Secure = 0x2f8006c8;
Cache_PSRAM_MMU_Set = 0x2f8006cc;
Cache_PSRAM_MMU_Set_Secure = 0x2f8006d0;
Cache_Count_Flash_Pages = 0x2f8006d4;
Cache_Flash_To_SPIRAM_Copy = 0x2f8006d8;
Cache_Travel_Tag_Memory = 0x2f8006dc;
Cache_Travel_Tag_Memory2 = 0x2f8006e0;
Cache_Get_Virtual_Addr = 0x2f8006e4;
Cache_Set_IDROM_MMU_Size = 0x2f8006e8;
flash2spiram_instruction_offset = 0x2f8006ec;
flash2spiram_rodata_offset = 0x2f8006f0;
flash_instr_rodata_start_page = 0x2f8006f4;
flash_instr_rodata_end_page = 0x2f8006f8;
Cache_Set_IDROM_MMU_Info = 0x2f8006fc;
Cache_Get_IROM_MMU_End = 0x2f800700;
Cache_Get_DROM_MMU_End = 0x2f800704;
/* Data (.data, .bss, .rodata) */
rom_cache_op_cb = 0x2f07ffc0;
rom_cache_internal_table_ptr = 0x2f07ffbc;


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_clk_get_xtal_freq = 0x2f800708;
ets_clk_get_cpu_freq = 0x2f80070c;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
rom_gpio_set_output_level = 0x2f800710;
rom_gpio_get_input_level = 0x2f800714;
rom_gpio_matrix_in = 0x2f800718;
rom_gpio_matrix_out = 0x2f80071c;
rom_gpio_bypass_matrix_in = 0x2f800720;
rom_gpio_output_disable = 0x2f800724;
rom_gpio_output_enable = 0x2f800728;
rom_gpio_pad_input_disable = 0x2f80072c;
rom_gpio_pad_input_enable = 0x2f800730;
rom_gpio_pad_pulldown = 0x2f800734;
rom_gpio_pad_pullup = 0x2f800738;
rom_gpio_pad_select_gpio = 0x2f80073c;
rom_gpio_pad_set_drv = 0x2f800740;
rom_gpio_pad_unhold = 0x2f800744;
rom_gpio_pad_hold = 0x2f800748;
rom_gpio_lppad_select_mux = 0x2f80074c;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x2f800750;
esprv_intc_int_set_threshold = 0x2f800754;
esprv_intc_int_enable = 0x2f800758;
esprv_intc_int_disable = 0x2f80075c;
esprv_intc_int_set_type = 0x2f800760;
PROVIDE( intr_handler_set = 0x2f800764 );
intr_matrix_set = 0x2f800768;
ets_intr_register_ctx = 0x2f80076c;
ets_intr_lock = 0x2f800770;
ets_intr_unlock = 0x2f800774;
ets_isr_attach = 0x2f800778;
ets_isr_mask = 0x2f80077c;
ets_isr_unmask = 0x2f800780;


/***************************************
 Group crc
 ***************************************/

/* Functions */
crc32_le = 0x2f800784;
crc16_le = 0x2f800788;
crc8_le = 0x2f80078c;
crc32_be = 0x2f800790;
crc16_be = 0x2f800794;
crc8_be = 0x2f800798;
esp_crc8 = 0x2f80079c;
/* Data (.data, .bss, .rodata) */
crc32_le_table_ptr = 0x2f84fff8;
crc16_le_table_ptr = 0x2f84fff4;
crc8_le_table_ptr = 0x2f84fff0;
crc32_be_table_ptr = 0x2f84ffec;
crc16_be_table_ptr = 0x2f84ffe8;
crc8_be_table_ptr = 0x2f84ffe4;


/***************************************
 Group md5
 ***************************************/

/* Functions */
md5_vector = 0x2f8007a0;
MD5Init = 0x2f8007a4;
MD5Update = 0x2f8007a8;
MD5Final = 0x2f8007ac;


/***************************************
 Group hwcrypto
 ***************************************/

/* Functions */
ets_sha_enable = 0x2f8007b0;
ets_sha_disable = 0x2f8007b4;
ets_sha_get_state = 0x2f8007b8;
ets_sha_init = 0x2f8007bc;
ets_sha_process = 0x2f8007c0;
ets_sha_starts = 0x2f8007c4;
ets_sha_update = 0x2f8007c8;
ets_sha_finish = 0x2f8007cc;
ets_sha_clone = 0x2f8007d0;
ets_hmac_enable = 0x2f8007d4;
ets_hmac_disable = 0x2f8007d8;
ets_hmac_calculate_message = 0x2f8007dc;
ets_hmac_calculate_downstream = 0x2f8007e0;
ets_hmac_invalidate_downstream = 0x2f8007e4;
ets_aes_enable = 0x2f8007e8;
ets_aes_disable = 0x2f8007ec;
ets_aes_setkey = 0x2f8007f0;
ets_aes_block = 0x2f8007f4;
ets_aes_setkey_dec = 0x2f8007f8;
ets_aes_setkey_enc = 0x2f8007fc;
ets_bigint_enable = 0x2f800800;
ets_bigint_disable = 0x2f800804;
ets_bigint_multiply = 0x2f800808;
ets_bigint_modmult = 0x2f80080c;
ets_bigint_modexp = 0x2f800810;
ets_bigint_wait_finish = 0x2f800814;
ets_bigint_getz = 0x2f800818;
ets_ds_enable = 0x2f80081c;
ets_ds_disable = 0x2f800820;
ets_ds_start_sign = 0x2f800824;
ets_ds_is_busy = 0x2f800828;
ets_ds_finish_sign = 0x2f80082c;
ets_ds_encrypt_params = 0x2f800830;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x2f800834;
ets_efuse_program = 0x2f800838;
ets_efuse_clear_program_registers = 0x2f80083c;
ets_efuse_write_key = 0x2f800840;
ets_efuse_get_read_register_address = 0x2f800844;
ets_efuse_get_key_purpose = 0x2f800848;
ets_efuse_key_block_unused = 0x2f80084c;
ets_efuse_find_unused_key_block = 0x2f800850;
ets_efuse_rs_calculate = 0x2f800854;
ets_efuse_count_unused_key_blocks = 0x2f800858;
ets_efuse_secure_boot_enabled = 0x2f80085c;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x2f800860;
ets_efuse_cache_encryption_enabled = 0x2f800864;
ets_efuse_download_modes_disabled = 0x2f800868;
ets_efuse_find_purpose = 0x2f80086c;
ets_efuse_force_send_resume = 0x2f800870;
ets_efuse_get_flash_delay_us = 0x2f800874;
ets_efuse_get_uart_print_control = 0x2f800878;
ets_efuse_direct_boot_mode_disabled = 0x2f80087c;
ets_efuse_security_download_modes_enabled = 0x2f800880;
ets_efuse_jtag_disabled = 0x2f800884;
ets_efuse_usb_print_is_disabled = 0x2f800888;
ets_efuse_usb_download_mode_disabled = 0x2f80088c;
ets_efuse_usb_device_disabled = 0x2f800890;
ets_efuse_get_km_huk_gen_state = 0x2f800894;
ets_efuse_get_km_deploy_only_once = 0x2f800898;
ets_efuse_get_force_use_km_key = 0x2f80089c;
ets_efuse_xts_key_length_256 = 0x2f8008a0;
ets_efuse_get_km_key_lock = 0x2f8008a4;
ets_jtag_enable_temporarily = 0x2f8008a8;


/***************************************
 Group key_mgr
 ***************************************/

/* Functions */
esp_rom_check_recover_key = 0x2f8008ac;
esp_rom_km_huk_conf = 0x2f8008b0;
esp_rom_km_huk_risk = 0x2f8008b4;
esp_rom_recover_key = 0x2f8008b8;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x2f8008bc;
ets_rsa_pss_verify = 0x2f8008c0;
ets_ecdsa_verify = 0x2f8008c4;
ets_secure_boot_verify_bootloader_with_keys = 0x2f8008c8;
ets_secure_boot_verify_signature = 0x2f8008cc;
ets_secure_boot_read_key_digests = 0x2f8008d0;
ets_mgf1_sha256 = 0x2f8008d4;
ets_secure_boot_revoke_public_key_digest = 0x2f8008d8;


/***************************************
 Group usb_device_uart
 ***************************************/

/* Functions */
usb_serial_device_rx_one_char = 0x2f800a4c;
usb_serial_device_rx_one_char_block = 0x2f800a50;
usb_serial_device_tx_flush = 0x2f800a54;
usb_serial_device_tx_one_char = 0x2f800a58;


/***************************************
 Group usb_dwcotg_uart
 ***************************************/

/* Functions */
Uart_Init_USB = 0x2f800a5c;
usb_serial_otg_rx_one_char = 0x2f800a60;
usb_serial_otg_rx_one_char_block = 0x2f800a64;
usb_serial_otg_tx_flush = 0x2f800a68;
usb_serial_otg_tx_one_char = 0x2f800a6c;
/* Data (.data, .bss, .rodata) */
uart_acm_dev = 0x2f07ffb8;


/***************************************
 Group usb_dwcotg_module
 ***************************************/

/* Functions */
cdc_acm_class_handle_req = 0x2f800a70;
cdc_acm_init = 0x2f800a74;
cdc_acm_fifo_fill = 0x2f800a78;
cdc_acm_rx_fifo_cnt = 0x2f800a7c;
cdc_acm_fifo_read = 0x2f800a80;
cdc_acm_irq_tx_enable = 0x2f800a84;
cdc_acm_irq_tx_disable = 0x2f800a88;
cdc_acm_irq_state_enable = 0x2f800a8c;
cdc_acm_irq_state_disable = 0x2f800a90;
cdc_acm_irq_tx_ready = 0x2f800a94;
cdc_acm_irq_rx_enable = 0x2f800a98;
cdc_acm_irq_rx_disable = 0x2f800a9c;
cdc_acm_irq_rx_ready = 0x2f800aa0;
cdc_acm_irq_is_pending = 0x2f800aa4;
cdc_acm_irq_callback_set = 0x2f800aa8;
cdc_acm_line_ctrl_set = 0x2f800aac;
cdc_acm_line_ctrl_get = 0x2f800ab0;
cdc_acm_poll_out = 0x2f800ab4;
chip_usb_dw_did_persist = 0x2f800ab8;
chip_usb_dw_init = 0x2f800abc;
chip_usb_detach = 0x2f800ac0;
chip_usb_dw_prepare_persist = 0x2f800ac4;
chip_usb_get_persist_flags = 0x2f800ac8;
chip_usb_set_persist_flags = 0x2f800acc;
cpio_start = 0x2f800ad0;
cpio_feed = 0x2f800ad4;
cpio_done = 0x2f800ad8;
cpio_destroy = 0x2f800adc;
dfu_flash_init = 0x2f800ae0;
dfu_flash_erase = 0x2f800ae4;
dfu_flash_program = 0x2f800ae8;
dfu_flash_read = 0x2f800aec;
dfu_flash_attach = 0x2f800af0;
dfu_cpio_callback = 0x2f800af4;
dfu_updater_get_err = 0x2f800af8;
dfu_updater_clear_err = 0x2f800afc;
dfu_updater_enable = 0x2f800b00;
dfu_updater_begin = 0x2f800b04;
dfu_updater_feed = 0x2f800b08;
dfu_updater_end = 0x2f800b0c;
dfu_updater_set_raw_addr = 0x2f800b10;
dfu_updater_flash_read = 0x2f800b14;
usb_dc_prepare_persist = 0x2f800b18;
usb_dw_isr_handler = 0x2f800b1c;
usb_dc_attach = 0x2f800b20;
usb_dc_detach = 0x2f800b24;
usb_dc_reset = 0x2f800b28;
usb_dc_set_address = 0x2f800b2c;
usb_dc_ep_check_cap = 0x2f800b30;
usb_dc_ep_configure = 0x2f800b34;
usb_dc_ep_set_stall = 0x2f800b38;
usb_dc_ep_clear_stall = 0x2f800b3c;
usb_dc_ep_halt = 0x2f800b40;
usb_dc_ep_is_stalled = 0x2f800b44;
usb_dc_ep_enable = 0x2f800b48;
usb_dc_ep_disable = 0x2f800b4c;
usb_dc_ep_flush = 0x2f800b50;
usb_dc_ep_write_would_block = 0x2f800b54;
usb_dc_ep_write = 0x2f800b58;
usb_dc_ep_read_wait = 0x2f800b5c;
usb_dc_ep_read_continue = 0x2f800b60;
usb_dc_ep_read = 0x2f800b64;
usb_dc_ep_set_callback = 0x2f800b68;
usb_dc_set_status_callback = 0x2f800b6c;
usb_dc_ep_mps = 0x2f800b70;
usb_dc_check_poll_for_interrupts = 0x2f800b74;
mac_addr_to_serial_str_desc = 0x2f800b78;
usb_set_current_descriptor = 0x2f800b7c;
usb_get_descriptor = 0x2f800b80;
usb_dev_resume = 0x2f800b84;
usb_dev_get_configuration = 0x2f800b88;
usb_set_config = 0x2f800b8c;
usb_deconfig = 0x2f800b90;
usb_enable = 0x2f800b94;
usb_disable = 0x2f800b98;
usb_write_would_block = 0x2f800b9c;
usb_write = 0x2f800ba0;
usb_read = 0x2f800ba4;
usb_ep_set_stall = 0x2f800ba8;
usb_ep_clear_stall = 0x2f800bac;
usb_ep_read_wait = 0x2f800bb0;
usb_ep_read_continue = 0x2f800bb4;
usb_transfer_ep_callback = 0x2f800bb8;
usb_transfer = 0x2f800bbc;
usb_cancel_transfer = 0x2f800bc0;
usb_transfer_sync = 0x2f800bc4;
usb_dfu_set_detach_cb = 0x2f800bc8;
dfu_class_handle_req = 0x2f800bcc;
dfu_status_cb = 0x2f800bd0;
dfu_custom_handle_req = 0x2f800bd4;
usb_dfu_init = 0x2f800bd8;
usb_dfu_force_detach = 0x2f800bdc;
usb_dev_deinit = 0x2f800be0;
usb_dw_ctrl_deinit = 0x2f800be4;
/* Data (.data, .bss, .rodata) */
s_usb_osglue = 0x2f07ffac;


/***************************************
 Group recovery_bootloader
 ***************************************/

/* Functions */
ets_get_bootloader_offset = 0x2f800be8;
ets_set_bootloader_offset = 0x2f800bec;
