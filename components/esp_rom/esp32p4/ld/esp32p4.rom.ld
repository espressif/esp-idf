/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4.rom.ld for esp32p4
 *
 *
 * Generated from ./target/esp32p4/interface-esp32p4.yml md5sum f6516bd9708d890f63db87f8aed53ca7
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group common
 ***************************************/

/* Functions */
rtc_get_reset_reason = 0x4fc00018;
rtc_get_wakeup_cause = 0x4fc0001c;
pmu_enable_unhold_pads = 0x4fc00020;
ets_printf = 0x4fc00024;
ets_install_putc1 = 0x4fc00028;
ets_install_putc2 = 0x4fc0002c;
ets_install_uart_printf = 0x4fc00030;
ets_install_usb_printf = 0x4fc00034;
ets_get_printf_channel = 0x4fc00038;
ets_delay_us = 0x4fc0003c;
ets_get_cpu_frequency = 0x4fc00040;
ets_update_cpu_frequency = 0x4fc00044;
ets_install_lock = 0x4fc00048;
UartRxString = 0x4fc0004c;
UartGetCmdLn = 0x4fc00050;
uart_tx_one_char = 0x4fc00054;
uart_tx_one_char2 = 0x4fc00058;
uart_tx_one_char3 = 0x4fc0005c;
uart_rx_one_char = 0x4fc00060;
uart_rx_one_char_block = 0x4fc00064;
uart_rx_intr_handler = 0x4fc00068;
uart_rx_readbuff = 0x4fc0006c;
uartAttach = 0x4fc00070;
uart_tx_flush = 0x4fc00074;
uart_tx_wait_idle = 0x4fc00078;
uart_div_modify = 0x4fc0007c;
ets_write_char_uart = 0x4fc00080;
uart_tx_switch = 0x4fc00084;
uart_buff_switch = 0x4fc00088;
roundup2 = 0x4fc0008c;
multofup = 0x4fc00090;
software_reset = 0x4fc00094;
software_reset_cpu = 0x4fc00098;
ets_clk_assist_debug_clock_enable = 0x4fc0009c;
clear_super_wdt_reset_flag = 0x4fc000a0;
disable_default_watchdog = 0x4fc000a4;
ets_set_appcpu_boot_addr = 0x4fc000a8;
send_packet = 0x4fc000ac;
recv_packet = 0x4fc000b0;
GetUartDevice = 0x4fc000b4;
UartDwnLdProc = 0x4fc000b8;
GetSecurityInfoProc = 0x4fc000bc;
Uart_Init = 0x4fc000c0;
ets_set_user_start = 0x4fc000c4;
/* Data (.data, .bss, .rodata) */
ets_rom_layout_p = 0x4fc1fffc;
ets_ops_table_ptr = 0x4ff3fff4;
g_saved_pc = 0x4ff3fff8;


/***************************************
 Group miniz
 ***************************************/

/* Functions */
mz_adler32 = 0x4fc000c8;
mz_free = 0x4fc000cc;
tdefl_compress = 0x4fc000d0;
tdefl_compress_buffer = 0x4fc000d4;
tdefl_compress_mem_to_heap = 0x4fc000d8;
tdefl_compress_mem_to_mem = 0x4fc000dc;
tdefl_compress_mem_to_output = 0x4fc000e0;
tdefl_get_adler32 = 0x4fc000e4;
tdefl_get_prev_return_status = 0x4fc000e8;
tdefl_init = 0x4fc000ec;
tdefl_write_image_to_png_file_in_memory = 0x4fc000f0;
tdefl_write_image_to_png_file_in_memory_ex = 0x4fc000f4;
tinfl_decompress = 0x4fc000f8;
tinfl_decompress_mem_to_callback = 0x4fc000fc;
tinfl_decompress_mem_to_heap = 0x4fc00100;
tinfl_decompress_mem_to_mem = 0x4fc00104;


/***************************************
 Group spi_extmem_common
 ***************************************/

/* Functions */
esp_rom_spi_cmd_config = 0x4fc00108;
esp_rom_spi_cmd_start = 0x4fc0010c;
esp_rom_spi_set_op_mode = 0x4fc00110;
esp_rom_spi_set_dtr_swap_mode = 0x4fc00114;


/***************************************
 Group spiflash_legacy
 ***************************************/

/* Functions */
esp_rom_spiflash_wait_idle = 0x4fc00118;
esp_rom_spiflash_write_encrypted = 0x4fc0011c;
esp_rom_spiflash_write_encrypted_dest = 0x4fc00120;
esp_rom_spiflash_write_encrypted_enable = 0x4fc00124;
esp_rom_spiflash_write_encrypted_disable = 0x4fc00128;
esp_rom_spiflash_erase_chip = 0x4fc0012c;
_esp_rom_spiflash_erase_sector = 0x4fc00130;
_esp_rom_spiflash_erase_block = 0x4fc00134;
_esp_rom_spiflash_write = 0x4fc00138;
_esp_rom_spiflash_read = 0x4fc0013c;
_esp_rom_spiflash_unlock = 0x4fc00140;
_SPIEraseArea = 0x4fc00144;
_SPI_write_enable = 0x4fc00148;
esp_rom_spiflash_erase_sector = 0x4fc0014c;
esp_rom_spiflash_erase_block = 0x4fc00150;
esp_rom_spiflash_write = 0x4fc00154;
esp_rom_spiflash_read = 0x4fc00158;
esp_rom_spiflash_unlock = 0x4fc0015c;
SPIEraseArea = 0x4fc00160;
SPI_write_enable = 0x4fc00164;
esp_rom_spiflash_config_param = 0x4fc00168;
esp_rom_spiflash_read_user_cmd = 0x4fc0016c;
esp_rom_spiflash_select_qio_pins = 0x4fc00170;
esp_rom_spi_flash_auto_sus_res = 0x4fc00174;
esp_rom_spi_flash_send_resume = 0x4fc00178;
esp_rom_spi_flash_update_id = 0x4fc0017c;
esp_rom_spiflash_config_clk = 0x4fc00180;
esp_rom_spiflash_config_readmode = 0x4fc00184;
esp_rom_spiflash_read_status = 0x4fc00188;
esp_rom_spiflash_read_statushigh = 0x4fc0018c;
esp_rom_spiflash_write_status = 0x4fc00190;
esp_rom_spiflash_write_disable = 0x4fc00194;
spi_cache_mode_switch = 0x4fc00198;
spi_common_set_dummy_output = 0x4fc0019c;
spi_common_set_flash_cs_timing = 0x4fc001a0;
esp_rom_spi_set_address_bit_len = 0x4fc001a4;
SPILock = 0x4fc001a8;
SPIMasterReadModeCnfig = 0x4fc001ac;
SPI_Common_Command = 0x4fc001b0;
SPI_WakeUp = 0x4fc001b4;
SPI_block_erase = 0x4fc001b8;
SPI_chip_erase = 0x4fc001bc;
SPI_init = 0x4fc001c0;
SPI_page_program = 0x4fc001c4;
SPI_read_data = 0x4fc001c8;
SPI_sector_erase = 0x4fc001cc;
SelectSpiFunction = 0x4fc001d0;
SetSpiDrvs = 0x4fc001d4;
Wait_SPI_Idle = 0x4fc001d8;
spi_dummy_len_fix = 0x4fc001dc;
Disable_QMode = 0x4fc001e0;
Enable_QMode = 0x4fc001e4;
spi_flash_attach = 0x4fc001e8;
spi_flash_get_chip_size = 0x4fc001ec;
spi_flash_guard_set = 0x4fc001f0;
spi_flash_guard_get = 0x4fc001f4;
spi_flash_read_encrypted = 0x4fc001f8;
/* Data (.data, .bss, .rodata) */
rom_spiflash_legacy_funcs = 0x4ff3ffec;
rom_spiflash_legacy_data = 0x4ff3ffe8;
g_flash_guard_ops = 0x4ff3fff0;


/***************************************
 Group cache
 ***************************************/

/* Functions */
Cache_Get_L1_ICache_Line_Size = 0x4fc003c4;
Cache_Get_L1_DCache_Line_Size = 0x4fc003c8;
Cache_Get_L2_Cache_Line_Size = 0x4fc003cc;
Cache_Get_Mode = 0x4fc003d0;
Cache_Set_L2_Cache_Mode = 0x4fc003d4;
Cache_Address_Through_Cache = 0x4fc003d8;
ROM_Boot_Cache_Init = 0x4fc003dc;
Cache_Sync_Addr = 0x4fc003e0;
Cache_Invalidate_Addr = 0x4fc003e4;
Cache_Invalidate_Addr_Gid = 0x4fc003e8;
Cache_Clean_Addr = 0x4fc003ec;
Cache_Clean_Addr_Gid = 0x4fc003f0;
Cache_WriteBack_Addr = 0x4fc003f4;
Cache_WriteBack_Addr_Gid = 0x4fc003f8;
Cache_WriteBack_Invalidate_Addr = 0x4fc003fc;
Cache_WriteBack_Invalidate_Addr_Gid = 0x4fc00400;
Cache_Invalidate_All = 0x4fc00404;
Cache_Invalidate_All_Gid = 0x4fc00408;
Cache_Clean_All = 0x4fc0040c;
Cache_Clean_All_Gid = 0x4fc00410;
Cache_WriteBack_All = 0x4fc00414;
Cache_WriteBack_All_Gid = 0x4fc00418;
Cache_WriteBack_Invalidate_All = 0x4fc0041c;
Cache_WriteBack_Invalidate_All_Gid = 0x4fc00420;
Cache_Mask_All = 0x4fc00424;
Cache_Suspend_L1_CORE0_ICache_Autoload = 0x4fc00428;
Cache_Resume_L1_CORE0_ICache_Autoload = 0x4fc0042c;
Cache_Suspend_L1_CORE1_ICache_Autoload = 0x4fc00430;
Cache_Resume_L1_CORE1_ICache_Autoload = 0x4fc00434;
Cache_Suspend_L1_DCache_Autoload = 0x4fc00438;
Cache_Resume_L1_DCache_Autoload = 0x4fc0043c;
Cache_Suspend_L2_Cache_Autoload = 0x4fc00440;
Cache_Resume_L2_Cache_Autoload = 0x4fc00444;
Cache_Start_L1_CORE0_ICache_Preload = 0x4fc00448;
Cache_L1_CORE0_ICache_Preload_Done = 0x4fc0044c;
Cache_End_L1_CORE0_ICache_Preload = 0x4fc00450;
Cache_Start_L1_CORE1_ICache_Preload = 0x4fc00454;
Cache_L1_CORE1_ICache_Preload_Done = 0x4fc00458;
Cache_End_L1_CORE1_ICache_Preload = 0x4fc0045c;
Cache_Start_L1_DCache_Preload = 0x4fc00460;
Cache_L1_DCache_Preload_Done = 0x4fc00464;
Cache_End_L1_DCache_Preload = 0x4fc00468;
Cache_Start_L2_Cache_Preload = 0x4fc0046c;
Cache_L2_Cache_Preload_Done = 0x4fc00470;
Cache_End_L2_Cache_Preload = 0x4fc00474;
Cache_Config_L1_CORE0_ICache_Autoload = 0x4fc00478;
Cache_Enable_L1_CORE0_ICache_Autoload = 0x4fc0047c;
Cache_Disable_L1_CORE0_ICache_Autoload = 0x4fc00480;
Cache_Config_L1_CORE1_ICache_Autoload = 0x4fc00484;
Cache_Enable_L1_CORE1_ICache_Autoload = 0x4fc00488;
Cache_Disable_L1_CORE1_ICache_Autoload = 0x4fc0048c;
Cache_Config_L1_DCache_Autoload = 0x4fc00490;
Cache_Enable_L1_DCache_Autoload = 0x4fc00494;
Cache_Disable_L1_DCache_Autoload = 0x4fc00498;
Cache_Config_L2_Cache_Autoload = 0x4fc0049c;
Cache_Enable_L2_Cache_Autoload = 0x4fc004a0;
Cache_Disable_L2_Cache_Autoload = 0x4fc004a4;
Cache_Enable_L1_CORE0_ICache_PreLock = 0x4fc004a8;
Cache_Disable_L1_CORE0_ICache_PreLock = 0x4fc004ac;
Cache_Enable_L1_CORE1_ICache_PreLock = 0x4fc004b0;
Cache_Disable_L1_CORE1_ICache_PreLock = 0x4fc004b4;
Cache_Enable_L1_DCache_PreLock = 0x4fc004b8;
Cache_Disable_L1_DCache_PreLock = 0x4fc004bc;
Cache_Enable_L2_Cache_PreLock = 0x4fc004c0;
Cache_Disable_L2_Cache_PreLock = 0x4fc004c4;
Cache_Lock_Addr = 0x4fc004c8;
Cache_Unlock_Addr = 0x4fc004cc;
Cache_Disable_L1_CORE0_ICache = 0x4fc004d0;
Cache_Enable_L1_CORE0_ICache = 0x4fc004d4;
Cache_Suspend_L1_CORE0_ICache = 0x4fc004d8;
Cache_Resume_L1_CORE0_ICache = 0x4fc004dc;
Cache_Disable_L1_CORE1_ICache = 0x4fc004e0;
Cache_Enable_L1_CORE1_ICache = 0x4fc004e4;
Cache_Suspend_L1_CORE1_ICache = 0x4fc004e8;
Cache_Resume_L1_CORE1_ICache = 0x4fc004ec;
Cache_Disable_L1_DCache = 0x4fc004f0;
Cache_Enable_L1_DCache = 0x4fc004f4;
Cache_Suspend_L1_DCache = 0x4fc004f8;
Cache_Resume_L1_DCache = 0x4fc004fc;
Cache_Disable_L2_Cache = 0x4fc00500;
Cache_Enable_L2_Cache = 0x4fc00504;
Cache_Suspend_L2_Cache = 0x4fc00508;
Cache_Resume_L2_Cache = 0x4fc0050c;
Cache_FLASH_MMU_Init = 0x4fc00510;
Cache_PSRAM_MMU_Init = 0x4fc00514;
Cache_FLASH_MMU_Set = 0x4fc00518;
Cache_FLASH_MMU_Set_Secure = 0x4fc0051c;
Cache_PSRAM_MMU_Set = 0x4fc00520;
Cache_PSRAM_MMU_Set_Secure = 0x4fc00524;
Cache_Count_Flash_Pages = 0x4fc00528;
Cache_Flash_To_SPIRAM_Copy = 0x4fc0052c;
Cache_Travel_Tag_Memory = 0x4fc00530;
Cache_Travel_Tag_Memory2 = 0x4fc00534;
Cache_Get_Virtual_Addr = 0x4fc00538;
Cache_Set_IDROM_MMU_Size = 0x4fc0053c;
flash2spiram_instruction_offset = 0x4fc00540;
flash2spiram_rodata_offset = 0x4fc00544;
flash_instr_rodata_start_page = 0x4fc00548;
flash_instr_rodata_end_page = 0x4fc0054c;
Cache_Set_IDROM_MMU_Info = 0x4fc00550;
Cache_Get_IROM_MMU_End = 0x4fc00554;
Cache_Get_DROM_MMU_End = 0x4fc00558;
/* Data (.data, .bss, .rodata) */
rom_cache_op_cb = 0x4ff3ffdc;
rom_cache_internal_table_ptr = 0x4ff3ffd8;


/***************************************
 Group clock
 ***************************************/

/* Functions */
ets_clk_get_xtal_freq = 0x4fc0055c;
ets_clk_get_cpu_freq = 0x4fc00560;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_set_output_level = 0x4fc00564;
gpio_get_input_level = 0x4fc00568;
gpio_matrix_in = 0x4fc0056c;
gpio_matrix_out = 0x4fc00570;
gpio_bypass_matrix_in = 0x4fc00574;
/* gpio_output_disable = 0x4fc00578; */
/* gpio_output_enable = 0x4fc0057c; */
gpio_pad_input_disable = 0x4fc00580;
gpio_pad_input_enable = 0x4fc00584;
gpio_pad_pulldown = 0x4fc00588;
gpio_pad_pullup = 0x4fc0058c;
gpio_pad_select_gpio = 0x4fc00590;
gpio_pad_set_drv = 0x4fc00594;
gpio_pad_unhold = 0x4fc00598;
gpio_pad_hold = 0x4fc0059c;
gpio_lppad_select_mux = 0x4fc005a0;
gpio_ded_pad_set_drv = 0x4fc005a4;
gpio_ded_pad_pullup = 0x4fc005a8;
gpio_ded_pad_pulldown = 0x4fc005ac;
gpio_ded_pad_hold = 0x4fc005b0;
gpio_ded_pad_unhold = 0x4fc005b4;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x4fc005b8;
esprv_intc_int_set_threshold = 0x4fc005bc;
esprv_intc_int_enable = 0x4fc005c0;
esprv_intc_int_disable = 0x4fc005c4;
PROVIDE( intr_handler_set = 0x4fc005cc );
intr_matrix_set = 0x4fc005d0;
ets_intr_lock = 0x4fc005d4;
ets_intr_unlock = 0x4fc005d8;
ets_isr_attach = 0x4fc005dc;
ets_isr_mask = 0x4fc005e0;
ets_isr_unmask = 0x4fc005e4;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
md5_vector = 0x4fc005e8;
MD5Init = 0x4fc005ec;
MD5Update = 0x4fc005f0;
MD5Final = 0x4fc005f4;
crc32_le = 0x4fc005f8;
crc16_le = 0x4fc005fc;
crc8_le = 0x4fc00600;
crc32_be = 0x4fc00604;
crc16_be = 0x4fc00608;
crc8_be = 0x4fc0060c;
esp_crc8 = 0x4fc00610;
ets_sha_enable = 0x4fc00614;
ets_sha_disable = 0x4fc00618;
ets_sha_get_state = 0x4fc0061c;
ets_sha_init = 0x4fc00620;
ets_sha_process = 0x4fc00624;
ets_sha_starts = 0x4fc00628;
ets_sha_update = 0x4fc0062c;
ets_sha_finish = 0x4fc00630;
ets_sha_clone = 0x4fc00634;
ets_hmac_enable = 0x4fc00638;
ets_hmac_disable = 0x4fc0063c;
ets_hmac_calculate_message = 0x4fc00640;
ets_hmac_calculate_downstream = 0x4fc00644;
ets_hmac_invalidate_downstream = 0x4fc00648;
ets_jtag_enable_temporarily = 0x4fc0064c;
ets_aes_enable = 0x4fc00650;
ets_aes_disable = 0x4fc00654;
ets_aes_setkey = 0x4fc00658;
ets_aes_block = 0x4fc0065c;
ets_aes_setkey_dec = 0x4fc00660;
ets_aes_setkey_enc = 0x4fc00664;
ets_bigint_enable = 0x4fc00668;
ets_bigint_disable = 0x4fc0066c;
ets_bigint_multiply = 0x4fc00670;
ets_bigint_modmult = 0x4fc00674;
ets_bigint_modexp = 0x4fc00678;
ets_bigint_wait_finish = 0x4fc0067c;
ets_bigint_getz = 0x4fc00680;
ets_ds_enable = 0x4fc00684;
ets_ds_disable = 0x4fc00688;
ets_ds_start_sign = 0x4fc0068c;
ets_ds_is_busy = 0x4fc00690;
ets_ds_finish_sign = 0x4fc00694;
ets_ds_encrypt_params = 0x4fc00698;
ets_mgf1_sha256 = 0x4fc0069c;
/* Data (.data, .bss, .rodata) */
crc32_le_table_ptr = 0x4fc1fff8;
crc16_le_table_ptr = 0x4fc1fff4;
crc8_le_table_ptr = 0x4fc1fff0;
crc32_be_table_ptr = 0x4fc1ffec;
crc16_be_table_ptr = 0x4fc1ffe8;
crc8_be_table_ptr = 0x4fc1ffe4;


/***************************************
 Group efuse
 ***************************************/

/* Functions */
ets_efuse_read = 0x4fc006a0;
ets_efuse_program = 0x4fc006a4;
ets_efuse_clear_program_registers = 0x4fc006a8;
ets_efuse_write_key = 0x4fc006ac;
ets_efuse_get_read_register_address = 0x4fc006b0;
ets_efuse_get_key_purpose = 0x4fc006b4;
ets_efuse_key_block_unused = 0x4fc006b8;
ets_efuse_find_unused_key_block = 0x4fc006bc;
ets_efuse_rs_calculate = 0x4fc006c0;
ets_efuse_count_unused_key_blocks = 0x4fc006c4;
ets_efuse_secure_boot_enabled = 0x4fc006c8;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x4fc006cc;
ets_efuse_cache_encryption_enabled = 0x4fc006d0;
ets_efuse_download_modes_disabled = 0x4fc006d4;
ets_efuse_find_purpose = 0x4fc006d8;
ets_efuse_force_send_resume = 0x4fc006dc;
ets_efuse_get_flash_delay_us = 0x4fc006e0;
ets_efuse_get_uart_print_control = 0x4fc006e4;
ets_efuse_direct_boot_mode_disabled = 0x4fc006e8;
ets_efuse_security_download_modes_enabled = 0x4fc006ec;
ets_efuse_jtag_disabled = 0x4fc006f0;
ets_efuse_usb_print_is_disabled = 0x4fc006f4;
ets_efuse_usb_download_mode_disabled = 0x4fc006f8;
ets_efuse_usb_device_disabled = 0x4fc006fc;
ets_efuse_get_km_huk_gen_state = 0x4fc00700;
ets_efuse_get_km_deploy_only_once = 0x4fc00704;
ets_efuse_get_force_use_km_key = 0x4fc00708;
ets_efuse_xts_key_length_256 = 0x4fc0070c;
ets_efuse_get_km_key_lock = 0x4fc00710;


/***************************************
 Group key_mgr
 ***************************************/

/* Functions */
esp_rom_check_recover_key = 0x4fc00714;
esp_rom_km_huk_conf = 0x4fc00718;
esp_rom_km_huk_risk = 0x4fc0071c;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x4fc00720;
ets_rsa_pss_verify = 0x4fc00724;
ets_ecdsa_verify = 0x4fc00728;
ets_secure_boot_verify_bootloader_with_keys = 0x4fc0072c;
ets_secure_boot_verify_signature = 0x4fc00730;
ets_secure_boot_read_key_digests = 0x4fc00734;
ets_secure_boot_revoke_public_key_digest = 0x4fc00738;


/***************************************
 Group usb_device_uart
 ***************************************/

/* Functions */
usb_serial_device_rx_one_char = 0x4fc008b0;
usb_serial_device_rx_one_char_block = 0x4fc008b4;
usb_serial_device_tx_flush = 0x4fc008b8;
usb_serial_device_tx_one_char = 0x4fc008bc;


/***************************************
 Group usb_dwcotg_uart
 ***************************************/

/* Functions */
Uart_Init_USB = 0x4fc008c0;
usb_serial_otg_rx_one_char = 0x4fc008c4;
usb_serial_otg_rx_one_char_block = 0x4fc008c8;
usb_serial_otg_tx_flush = 0x4fc008cc;
usb_serial_otg_tx_one_char = 0x4fc008d0;
/* Data (.data, .bss, .rodata) */
uart_acm_dev = 0x4ff3ffd4;


/***************************************
 Group usb_dwcotg_module
 ***************************************/

/* Functions */
cdc_acm_class_handle_req = 0x4fc008d4;
cdc_acm_init = 0x4fc008d8;
cdc_acm_fifo_fill = 0x4fc008dc;
cdc_acm_rx_fifo_cnt = 0x4fc008e0;
cdc_acm_fifo_read = 0x4fc008e4;
cdc_acm_irq_tx_enable = 0x4fc008e8;
cdc_acm_irq_tx_disable = 0x4fc008ec;
cdc_acm_irq_state_enable = 0x4fc008f0;
cdc_acm_irq_state_disable = 0x4fc008f4;
cdc_acm_irq_tx_ready = 0x4fc008f8;
cdc_acm_irq_rx_enable = 0x4fc008fc;
cdc_acm_irq_rx_disable = 0x4fc00900;
cdc_acm_irq_rx_ready = 0x4fc00904;
cdc_acm_irq_is_pending = 0x4fc00908;
cdc_acm_irq_callback_set = 0x4fc0090c;
cdc_acm_line_ctrl_set = 0x4fc00910;
cdc_acm_line_ctrl_get = 0x4fc00914;
cdc_acm_poll_out = 0x4fc00918;
chip_usb_dw_did_persist = 0x4fc0091c;
chip_usb_dw_init = 0x4fc00920;
chip_usb_detach = 0x4fc00924;
chip_usb_dw_prepare_persist = 0x4fc00928;
chip_usb_get_persist_flags = 0x4fc0092c;
chip_usb_set_persist_flags = 0x4fc00930;
cpio_start = 0x4fc00934;
cpio_feed = 0x4fc00938;
cpio_done = 0x4fc0093c;
cpio_destroy = 0x4fc00940;
dfu_flash_init = 0x4fc00944;
dfu_flash_erase = 0x4fc00948;
dfu_flash_program = 0x4fc0094c;
dfu_flash_read = 0x4fc00950;
dfu_flash_attach = 0x4fc00954;
dfu_cpio_callback = 0x4fc00958;
dfu_updater_get_err = 0x4fc0095c;
dfu_updater_clear_err = 0x4fc00960;
dfu_updater_enable = 0x4fc00964;
dfu_updater_begin = 0x4fc00968;
dfu_updater_feed = 0x4fc0096c;
dfu_updater_end = 0x4fc00970;
dfu_updater_set_raw_addr = 0x4fc00974;
dfu_updater_flash_read = 0x4fc00978;
usb_dc_prepare_persist = 0x4fc0097c;
usb_dw_isr_handler = 0x4fc00980;
usb_dc_attach = 0x4fc00984;
usb_dc_detach = 0x4fc00988;
usb_dc_reset = 0x4fc0098c;
usb_dc_set_address = 0x4fc00990;
usb_dc_ep_check_cap = 0x4fc00994;
usb_dc_ep_configure = 0x4fc00998;
usb_dc_ep_set_stall = 0x4fc0099c;
usb_dc_ep_clear_stall = 0x4fc009a0;
usb_dc_ep_halt = 0x4fc009a4;
usb_dc_ep_is_stalled = 0x4fc009a8;
usb_dc_ep_enable = 0x4fc009ac;
usb_dc_ep_disable = 0x4fc009b0;
usb_dc_ep_flush = 0x4fc009b4;
usb_dc_ep_write_would_block = 0x4fc009b8;
usb_dc_ep_write = 0x4fc009bc;
usb_dc_ep_read_wait = 0x4fc009c0;
usb_dc_ep_read_continue = 0x4fc009c4;
usb_dc_ep_read = 0x4fc009c8;
usb_dc_ep_set_callback = 0x4fc009cc;
usb_dc_set_status_callback = 0x4fc009d0;
usb_dc_ep_mps = 0x4fc009d4;
usb_dc_check_poll_for_interrupts = 0x4fc009d8;
mac_addr_to_serial_str_desc = 0x4fc009dc;
usb_set_current_descriptor = 0x4fc009e0;
usb_get_descriptor = 0x4fc009e4;
usb_dev_resume = 0x4fc009e8;
usb_dev_get_configuration = 0x4fc009ec;
usb_set_config = 0x4fc009f0;
usb_deconfig = 0x4fc009f4;
usb_enable = 0x4fc009f8;
usb_disable = 0x4fc009fc;
usb_write_would_block = 0x4fc00a00;
usb_write = 0x4fc00a04;
usb_read = 0x4fc00a08;
usb_ep_set_stall = 0x4fc00a0c;
usb_ep_clear_stall = 0x4fc00a10;
usb_ep_read_wait = 0x4fc00a14;
usb_ep_read_continue = 0x4fc00a18;
usb_transfer_ep_callback = 0x4fc00a1c;
usb_transfer = 0x4fc00a20;
usb_cancel_transfer = 0x4fc00a24;
usb_transfer_sync = 0x4fc00a28;
usb_dfu_set_detach_cb = 0x4fc00a2c;
dfu_class_handle_req = 0x4fc00a30;
dfu_status_cb = 0x4fc00a34;
dfu_custom_handle_req = 0x4fc00a38;
usb_dfu_init = 0x4fc00a3c;
usb_dfu_force_detach = 0x4fc00a40;
usb_dev_deinit = 0x4fc00a44;
usb_dw_ctrl_deinit = 0x4fc00a48;
/* Data (.data, .bss, .rodata) */
s_usb_osglue = 0x4ff3ffc8;
