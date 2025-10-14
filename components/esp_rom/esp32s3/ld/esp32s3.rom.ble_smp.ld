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

/* SMP */
f_ll_pause_enc_req_handler = 0x40000000;
f_ll_pause_enc_rsp_handler = 0x40000000;
f_ll_enc_req_handler = 0x40000000;
f_ll_enc_rsp_handler = 0x40000000;
f_ll_start_enc_req_handler = 0x40000000;
f_ll_start_enc_rsp_handler = 0x40000000;
f_hci_le_start_enc_cmd_handler = 0x40000000;
f_hci_le_ltk_req_reply_cmd_handler = 0x40000000;
f_hci_le_ltk_req_neg_reply_cmd_handler = 0x40000000;
f_llc_encrypt_ind_handler = 0x40000000;
f_llc_op_encrypt_ind_handler = 0x40000000;
f_hci_le_rd_local_p256_public_key_cmd_handler = 0x40000000;
f_hci_le_generate_dhkey_cmd_handler = 0x40000000;
f_hci_le_enc_cmd_handler = 0x40000000;
r_rwip_crypt_evt_handler = 0x40000000;

/* LE ping */
f_ll_ping_req_handler = 0x40000000;
f_ll_ping_rsp_handler = 0x40000000;
f_llc_op_le_ping_ind_handler = 0x40000000;
f_llc_auth_payl_nearly_op_handler = 0x40000000;
f_llc_auth_payl_real_to_handler = 0x40000000;
f_llc_auth_payl_nearly_to_handler = 0x40000000;

/* ecc */
r_ecc_init = 0x40000000;
