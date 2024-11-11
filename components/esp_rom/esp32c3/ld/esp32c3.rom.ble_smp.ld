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

/* SMP */
f_ll_pause_enc_req_handler = 0;
f_ll_pause_enc_rsp_handler = 0;
f_ll_enc_req_handler = 0;
f_ll_enc_rsp_handler = 0;
f_ll_start_enc_req_handler = 0;
f_ll_start_enc_rsp_handler = 0;
f_hci_le_start_enc_cmd_handler = 0;
f_hci_le_ltk_req_reply_cmd_handler = 0;
f_hci_le_ltk_req_neg_reply_cmd_handler = 0;
f_llc_encrypt_ind_handler = 0;
f_llc_op_encrypt_ind_handler = 0;
f_hci_le_rd_local_p256_public_key_cmd_handler = 0;
f_hci_le_generate_dhkey_cmd_handler = 0;
f_hci_le_enc_cmd_handler = 0;
r_rwip_crypt_evt_handler = 0;

/* LE ping */
f_ll_ping_req_handler = 0;
f_ll_ping_rsp_handler = 0;
r_llc_le_ping_set = 0;
r_llc_le_ping_restart = 0;
f_llc_op_le_ping_ind_handler = 0;
f_llc_auth_payl_nearly_op_handler = 0;
f_llc_auth_payl_real_to_handler = 0;
f_llc_auth_payl_nearly_to_handler = 0;

/* ecc */
r_ecc_init = 0;
