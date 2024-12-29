/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void register_cmd_wifi(void);

void register_srv_attestation(void);

void register_srv_tee_ota(void);

void register_srv_user_ota(void);

void register_cmd_msg_sha256(void);

void register_srv_sec_stg_gen_key(void);

void register_srv_sec_stg_sign(void);

void register_srv_sec_stg_encrypt(void);

void register_srv_sec_stg_decrypt(void);

#ifdef __cplusplus
}
#endif
