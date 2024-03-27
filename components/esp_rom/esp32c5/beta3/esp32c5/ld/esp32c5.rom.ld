/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 2476337377df636dda217b0b3c1a63db
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
ets_rom_layout_p = 0x4004fffc;
ets_ops_table_ptr = 0x4087fff8;
g_saved_pc = 0x4087fffc;


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
 Group spiflash_legacy
 ***************************************/

/* Functions */
esp_rom_spiflash_wait_idle = 0x4000010c;
esp_rom_spiflash_write_encrypted = 0x40000110;
esp_rom_spiflash_write_encrypted_dest = 0x40000114;
esp_rom_spiflash_write_encrypted_enable = 0x40000118;
esp_rom_spiflash_write_encrypted_disable = 0x4000011c;
esp_rom_spiflash_erase_chip = 0x40000120;
_esp_rom_spiflash_erase_sector = 0x40000124;
_esp_rom_spiflash_erase_block = 0x40000128;
_esp_rom_spiflash_write = 0x4000012c;
_esp_rom_spiflash_read = 0x40000130;
_esp_rom_spiflash_unlock = 0x40000134;
_SPIEraseArea = 0x40000138;
_SPI_write_enable = 0x4000013c;
esp_rom_spiflash_erase_sector = 0x40000140;
esp_rom_spiflash_erase_block = 0x40000144;
esp_rom_spiflash_write = 0x40000148;
esp_rom_spiflash_read = 0x4000014c;
esp_rom_spiflash_unlock = 0x40000150;
SPIEraseArea = 0x40000154;
SPI_write_enable = 0x40000158;
esp_rom_spiflash_config_param = 0x4000015c;
esp_rom_spiflash_read_user_cmd = 0x40000160;
esp_rom_spiflash_select_qio_pins = 0x40000164;
esp_rom_spi_flash_auto_sus_res = 0x40000168;
esp_rom_spi_flash_send_resume = 0x4000016c;
esp_rom_spi_flash_update_id = 0x40000170;
esp_rom_spiflash_config_clk = 0x40000174;
esp_rom_spiflash_config_readmode = 0x40000178;
esp_rom_spiflash_read_status = 0x4000017c;
esp_rom_spiflash_read_statushigh = 0x40000180;
esp_rom_spiflash_write_status = 0x40000184;
esp_rom_spiflash_write_disable = 0x40000188;
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


/***************************************
 Group cache
 ***************************************/

/* Functions */
Cache_Get_ICache_Line_Size = 0x40000624;
Cache_Get_Mode = 0x40000628;
Cache_Address_Through_Cache = 0x4000062c;
ROM_Boot_Cache_Init = 0x40000630;
MMU_Set_Page_Mode = 0x40000634;
MMU_Get_Page_Mode = 0x40000638;
Cache_Invalidate_ICache_Items = 0x4000063c;
Cache_Op_Addr = 0x40000640;
Cache_Invalidate_Addr = 0x40000644;
Cache_Invalidate_ICache_All = 0x40000648;
Cache_Mask_All = 0x4000064c;
Cache_UnMask_Dram0 = 0x40000650;
Cache_Suspend_ICache_Autoload = 0x40000654;
Cache_Resume_ICache_Autoload = 0x40000658;
Cache_Start_ICache_Preload = 0x4000065c;
Cache_ICache_Preload_Done = 0x40000660;
Cache_End_ICache_Preload = 0x40000664;
Cache_Config_ICache_Autoload = 0x40000668;
Cache_Enable_ICache_Autoload = 0x4000066c;
Cache_Disable_ICache_Autoload = 0x40000670;
Cache_Enable_ICache_PreLock = 0x40000674;
Cache_Disable_ICache_PreLock = 0x40000678;
Cache_Lock_ICache_Items = 0x4000067c;
Cache_Unlock_ICache_Items = 0x40000680;
Cache_Lock_Addr = 0x40000684;
Cache_Unlock_Addr = 0x40000688;
Cache_Disable_ICache = 0x4000068c;
Cache_Enable_ICache = 0x40000690;
Cache_Suspend_ICache = 0x40000694;
Cache_Resume_ICache = 0x40000698;
Cache_Freeze_ICache_Enable = 0x4000069c;
Cache_Freeze_ICache_Disable = 0x400006a0;
Cache_Set_IDROM_MMU_Size = 0x400006a4;
Cache_Get_IROM_MMU_End = 0x400006a8;
Cache_Get_DROM_MMU_End = 0x400006ac;
Cache_MMU_Init = 0x400006b0;
Cache_MSPI_MMU_Set = 0x400006b4;
Cache_MSPI_MMU_Set_Secure = 0x400006b8;
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


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_set_output_level = 0x400006cc;
gpio_get_input_level = 0x400006d0;
gpio_matrix_in = 0x400006d4;
gpio_matrix_out = 0x400006d8;
gpio_bypass_matrix_in = 0x400006dc;
gpio_output_disable = 0x400006e0;
gpio_output_enable = 0x400006e4;
gpio_pad_input_disable = 0x400006e8;
gpio_pad_input_enable = 0x400006ec;
gpio_pad_pulldown = 0x400006f0;
gpio_pad_pullup = 0x400006f4;
gpio_pad_select_gpio = 0x400006f8;
gpio_pad_set_drv = 0x400006fc;
gpio_pad_unhold = 0x40000700;
gpio_pad_hold = 0x40000704;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
esprv_intc_int_set_priority = 0x40000708;
esprv_intc_int_set_threshold = 0x4000070c;
esprv_intc_int_enable = 0x40000710;
esprv_intc_int_disable = 0x40000714;
esprv_intc_int_set_type = 0x40000718;
PROVIDE( intr_handler_set = 0x4000071c );
intr_matrix_set = 0x40000720;
ets_intr_lock = 0x40000724;
ets_intr_unlock = 0x40000728;
ets_isr_attach = 0x4000072c;
ets_isr_mask = 0x40000730;
ets_isr_unmask = 0x40000734;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
md5_vector = 0x40000738;
MD5Init = 0x4000073c;
MD5Update = 0x40000740;
MD5Final = 0x40000744;
crc32_le = 0x40000748;
crc16_le = 0x4000074c;
crc8_le = 0x40000750;
crc32_be = 0x40000754;
crc16_be = 0x40000758;
crc8_be = 0x4000075c;
esp_crc8 = 0x40000760;
ets_sha_enable = 0x40000764;
ets_sha_disable = 0x40000768;
ets_sha_get_state = 0x4000076c;
ets_sha_init = 0x40000770;
ets_sha_process = 0x40000774;
ets_sha_starts = 0x40000778;
ets_sha_update = 0x4000077c;
ets_sha_finish = 0x40000780;
ets_sha_clone = 0x40000784;
ets_hmac_enable = 0x40000788;
ets_hmac_disable = 0x4000078c;
ets_hmac_calculate_message = 0x40000790;
ets_hmac_calculate_downstream = 0x40000794;
ets_hmac_invalidate_downstream = 0x40000798;
ets_jtag_enable_temporarily = 0x4000079c;
ets_aes_enable = 0x400007a0;
ets_aes_disable = 0x400007a4;
ets_aes_setkey = 0x400007a8;
ets_aes_block = 0x400007ac;
ets_aes_setkey_dec = 0x400007b0;
ets_aes_setkey_enc = 0x400007b4;
ets_bigint_enable = 0x400007b8;
ets_bigint_disable = 0x400007bc;
ets_bigint_multiply = 0x400007c0;
ets_bigint_modmult = 0x400007c4;
ets_bigint_modexp = 0x400007c8;
ets_bigint_wait_finish = 0x400007cc;
ets_bigint_getz = 0x400007d0;
ets_ds_enable = 0x400007d4;
ets_ds_disable = 0x400007d8;
ets_ds_start_sign = 0x400007dc;
ets_ds_is_busy = 0x400007e0;
ets_ds_finish_sign = 0x400007e4;
ets_ds_encrypt_params = 0x400007e8;
ets_mgf1_sha256 = 0x400007ec;
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
ets_efuse_read = 0x400007f0;
ets_efuse_program = 0x400007f4;
ets_efuse_clear_program_registers = 0x400007f8;
ets_efuse_write_key = 0x400007fc;
ets_efuse_get_read_register_address = 0x40000800;
ets_efuse_get_key_purpose = 0x40000804;
ets_efuse_key_block_unused = 0x40000808;
ets_efuse_find_unused_key_block = 0x4000080c;
ets_efuse_rs_calculate = 0x40000810;
ets_efuse_count_unused_key_blocks = 0x40000814;
ets_efuse_secure_boot_enabled = 0x40000818;
ets_efuse_secure_boot_aggressive_revoke_enabled = 0x4000081c;
ets_efuse_cache_encryption_enabled = 0x40000820;
ets_efuse_download_modes_disabled = 0x40000824;
ets_efuse_find_purpose = 0x40000828;
ets_efuse_force_send_resume = 0x4000082c;
ets_efuse_get_flash_delay_us = 0x40000830;
ets_efuse_get_uart_print_control = 0x40000834;
ets_efuse_direct_boot_mode_disabled = 0x40000838;
ets_efuse_security_download_modes_enabled = 0x4000083c;
ets_efuse_jtag_disabled = 0x40000840;
ets_efuse_usb_print_is_disabled = 0x40000844;
ets_efuse_usb_download_mode_disabled = 0x40000848;
ets_efuse_usb_device_disabled = 0x4000084c;
ets_efuse_secure_boot_fast_wake_enabled = 0x40000850;


/***************************************
 Group secureboot
 ***************************************/

/* Functions */
ets_emsa_pss_verify = 0x40000854;
ets_rsa_pss_verify = 0x40000858;
ets_ecdsa_verify = 0x4000085c;
ets_secure_boot_verify_bootloader_with_keys = 0x40000860;
ets_secure_boot_verify_signature = 0x40000864;
ets_secure_boot_read_key_digests = 0x40000868;
ets_secure_boot_revoke_public_key_digest = 0x4000086c;


/***************************************
 Group usb_device_uart
 ***************************************/

/* Functions */
usb_serial_device_rx_one_char = 0x40000a6c;
usb_serial_device_rx_one_char_block = 0x40000a70;
usb_serial_device_tx_flush = 0x40000a74;
usb_serial_device_tx_one_char = 0x40000a78;


/***************************************
 Group usb_dwcotg_uart
 ***************************************/

/* Functions */
Uart_Init_USB = 0x40000a7c;
usb_serial_otg_rx_one_char = 0x40000a80;
usb_serial_otg_rx_one_char_block = 0x40000a84;
usb_serial_otg_tx_flush = 0x40000a88;
usb_serial_otg_tx_one_char = 0x40000a8c;
/* Data (.data, .bss, .rodata) */
uart_acm_dev = 0x4087ffc4;


/***************************************
 Group usb_dwcotg_module
 ***************************************/

/* Functions */
cdc_acm_class_handle_req = 0x40000a90;
cdc_acm_init = 0x40000a94;
cdc_acm_fifo_fill = 0x40000a98;
cdc_acm_rx_fifo_cnt = 0x40000a9c;
cdc_acm_fifo_read = 0x40000aa0;
cdc_acm_irq_tx_enable = 0x40000aa4;
cdc_acm_irq_tx_disable = 0x40000aa8;
cdc_acm_irq_state_enable = 0x40000aac;
cdc_acm_irq_state_disable = 0x40000ab0;
cdc_acm_irq_tx_ready = 0x40000ab4;
cdc_acm_irq_rx_enable = 0x40000ab8;
cdc_acm_irq_rx_disable = 0x40000abc;
cdc_acm_irq_rx_ready = 0x40000ac0;
cdc_acm_irq_is_pending = 0x40000ac4;
cdc_acm_irq_callback_set = 0x40000ac8;
cdc_acm_line_ctrl_set = 0x40000acc;
cdc_acm_line_ctrl_get = 0x40000ad0;
cdc_acm_poll_out = 0x40000ad4;
chip_usb_dw_did_persist = 0x40000ad8;
chip_usb_dw_init = 0x40000adc;
chip_usb_detach = 0x40000ae0;
chip_usb_dw_prepare_persist = 0x40000ae4;
chip_usb_get_persist_flags = 0x40000ae8;
chip_usb_set_persist_flags = 0x40000aec;
cpio_start = 0x40000af0;
cpio_feed = 0x40000af4;
cpio_done = 0x40000af8;
cpio_destroy = 0x40000afc;
dfu_flash_init = 0x40000b00;
dfu_flash_erase = 0x40000b04;
dfu_flash_program = 0x40000b08;
dfu_flash_read = 0x40000b0c;
dfu_flash_attach = 0x40000b10;
dfu_cpio_callback = 0x40000b14;
dfu_updater_get_err = 0x40000b18;
dfu_updater_clear_err = 0x40000b1c;
dfu_updater_enable = 0x40000b20;
dfu_updater_begin = 0x40000b24;
dfu_updater_feed = 0x40000b28;
dfu_updater_end = 0x40000b2c;
dfu_updater_set_raw_addr = 0x40000b30;
dfu_updater_flash_read = 0x40000b34;
usb_dc_prepare_persist = 0x40000b38;
usb_dw_isr_handler = 0x40000b3c;
usb_dc_attach = 0x40000b40;
usb_dc_detach = 0x40000b44;
usb_dc_reset = 0x40000b48;
usb_dc_set_address = 0x40000b4c;
usb_dc_ep_check_cap = 0x40000b50;
usb_dc_ep_configure = 0x40000b54;
usb_dc_ep_set_stall = 0x40000b58;
usb_dc_ep_clear_stall = 0x40000b5c;
usb_dc_ep_halt = 0x40000b60;
usb_dc_ep_is_stalled = 0x40000b64;
usb_dc_ep_enable = 0x40000b68;
usb_dc_ep_disable = 0x40000b6c;
usb_dc_ep_flush = 0x40000b70;
usb_dc_ep_write_would_block = 0x40000b74;
usb_dc_ep_write = 0x40000b78;
usb_dc_ep_read_wait = 0x40000b7c;
usb_dc_ep_read_continue = 0x40000b80;
usb_dc_ep_read = 0x40000b84;
usb_dc_ep_set_callback = 0x40000b88;
usb_dc_set_status_callback = 0x40000b8c;
usb_dc_ep_mps = 0x40000b90;
usb_dc_check_poll_for_interrupts = 0x40000b94;
mac_addr_to_serial_str_desc = 0x40000b98;
usb_set_current_descriptor = 0x40000b9c;
usb_get_descriptor = 0x40000ba0;
usb_dev_resume = 0x40000ba4;
usb_dev_get_configuration = 0x40000ba8;
usb_set_config = 0x40000bac;
usb_deconfig = 0x40000bb0;
usb_enable = 0x40000bb4;
usb_disable = 0x40000bb8;
usb_write_would_block = 0x40000bbc;
usb_write = 0x40000bc0;
usb_read = 0x40000bc4;
usb_ep_set_stall = 0x40000bc8;
usb_ep_clear_stall = 0x40000bcc;
usb_ep_read_wait = 0x40000bd0;
usb_ep_read_continue = 0x40000bd4;
usb_transfer_ep_callback = 0x40000bd8;
usb_transfer = 0x40000bdc;
usb_cancel_transfer = 0x40000be0;
usb_transfer_sync = 0x40000be4;
usb_dfu_set_detach_cb = 0x40000be8;
dfu_class_handle_req = 0x40000bec;
dfu_status_cb = 0x40000bf0;
dfu_custom_handle_req = 0x40000bf4;
usb_dfu_init = 0x40000bf8;
usb_dfu_force_detach = 0x40000bfc;
usb_dev_deinit = 0x40000c00;
usb_dw_ctrl_deinit = 0x40000c04;
/* Data (.data, .bss, .rodata) */
s_usb_osglue = 0x4087ffb8;


/***************************************
 Group lldesc
 ***************************************/

/* Functions */
lldesc_build_chain = 0x40000c08;


/***************************************
 Group sip
 ***************************************/

/* Functions */
sip_after_tx_complete = 0x40000c0c;
sip_alloc_to_host_evt = 0x40000c10;
sip_download_begin = 0x40000c14;
sip_get_ptr = 0x40000c18;
sip_get_state = 0x40000c1c;
sip_init_attach = 0x40000c20;
sip_install_rx_ctrl_cb = 0x40000c24;
sip_install_rx_data_cb = 0x40000c28;
sip_is_active = 0x40000c2c;
sip_post_init = 0x40000c30;
sip_reclaim_from_host_cmd = 0x40000c34;
sip_reclaim_tx_data_pkt = 0x40000c38;
sip_send = 0x40000c3c;
sip_to_host_chain_append = 0x40000c40;
sip_to_host_evt_send_done = 0x40000c44;


/***************************************
 Group slc
 ***************************************/

/* Functions */
slc_add_credits = 0x40000c48;
slc_enable = 0x40000c4c;
slc_from_host_chain_fetch = 0x40000c50;
slc_from_host_chain_recycle = 0x40000c54;
slc_has_pkt_to_host = 0x40000c58;
slc_init_attach = 0x40000c5c;
slc_init_credit = 0x40000c60;
slc_reattach = 0x40000c64;
slc_send_to_host_chain = 0x40000c68;
slc_set_host_io_max_window = 0x40000c6c;
slc_to_host_chain_recycle = 0x40000c70;
