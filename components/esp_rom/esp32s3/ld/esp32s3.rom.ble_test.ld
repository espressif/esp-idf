/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s3.rom.ld for esp32s3
 *
 *
 * Generated from ./interface-esp32s3.yml md5sum 39c4ce259b11323b9404c192b01b712b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */


/* overwrite */
lld_acl_rx_ind_handler = 0x40000000;
lld_con_estab_ind_handler = 0x40000000;
lld_adv_rep_ind_handler = 0x40000000;
llm_rpa_renew_to_handler = 0x40000000;
lld_scan_end_ind_handler = 0x40000000;
llm_scan_period_to_handler = 0x40000000;

/* nvds */
r_nvds_init = 0x40000000;
f_nvds_get = 0x40000000;
f_nvds_del = 0x40000000;
f_nvds_put = 0x40000000;

/* controller flash */
r_flash_init = 0x40000000;
r_flash_env_init = 0x40000000;
r_flash_env_deinit = 0x40000000;

/* QA test */
f_hci_vendor_ble_qa_test_cmd_handler = 0x40000000;
