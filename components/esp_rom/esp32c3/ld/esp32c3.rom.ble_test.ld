/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c3.rom.ld for esp32c3
 *
 *
 * Generated from ./interface-esp32c3.yml md5sum 93b28a9e1fe42d212018eb4336849208
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */


/* overwrite */
lld_acl_rx_ind_handler = 0;
lld_con_estab_ind_handler = 0;
lld_adv_rep_ind_handler = 0;
llm_rpa_renew_to_handler = 0;
lld_scan_end_ind_handler = 0;
llm_scan_period_to_handler = 0;

/* nvds */
r_nvds_init = 0;
f_nvds_get = 0;
f_nvds_del = 0;
f_nvds_put = 0;

/* controller flash */
r_flash_init = 0;
r_flash_env_init = 0;
r_flash_env_deinit = 0;

/* QA test */
f_hci_vendor_ble_qa_test_cmd_handler = 0;
