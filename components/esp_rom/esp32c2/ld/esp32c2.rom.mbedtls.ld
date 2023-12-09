/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface esp32c2.rom.mbedtls.ld for esp32c2
 *
 *
 * Generated from ./interface-esp32c2.yml md5sum c679b6ed5e9f0a9c3e7b93e5e0f2a1a3
 *
 * Compatible with ROM where ECO version equal or greater to 1.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_mbedtls
 ***************************************/

/* Functions */
mbedtls_aes_init = 0x40002664;
mbedtls_aes_free = 0x40002688;
mbedtls_aes_setkey_enc = 0x4000268c;
mbedtls_aes_setkey_dec = 0x40002690;
mbedtls_aes_crypt_ecb = 0x40002694;
mbedtls_aes_crypt_cbc = 0x40002698;
mbedtls_internal_aes_encrypt = 0x4000269c;
mbedtls_internal_aes_decrypt = 0x400026a0;
mbedtls_asn1_get_len = 0x400026a4;
mbedtls_asn1_get_tag = 0x400026a8;
mbedtls_asn1_get_bool = 0x400026ac;
mbedtls_asn1_get_int = 0x400026b0;
mbedtls_asn1_get_bitstring = 0x400026b4;
mbedtls_asn1_get_bitstring_null = 0x400026b8;
mbedtls_asn1_get_sequence_of = 0x400026bc;
mbedtls_asn1_get_mpi = 0x400026c0;
mbedtls_asn1_get_alg = 0x400026c4;
mbedtls_asn1_get_alg_null = 0x400026c8;
mbedtls_asn1_write_len = 0x400026cc;
mbedtls_asn1_write_tag = 0x400026d0;
mbedtls_asn1_write_mpi = 0x400026d4;
mbedtls_base64_decode = 0x400026d8;
mbedtls_ccm_star_encrypt_and_tag = 0x40002774;
mbedtls_ccm_star_auth_decrypt = 0x40002778;
mbedtls_cipher_init = 0x4000277c;
mbedtls_cipher_set_padding_mode = 0x40002780;
mbedtls_cipher_reset = 0x40002784;
mbedtls_cipher_finish = 0x40002788;
mbedtls_cipher_crypt = 0x4000278c;
mbedtls_cipher_cmac_starts = 0x40002790;
mbedtls_cipher_cmac_update = 0x40002794;
mbedtls_cipher_cmac_finish = 0x40002798;
mbedtls_ctr_drbg_init = 0x4000279c;
mbedtls_ctr_drbg_seed = 0x400027a0;
mbedtls_ctr_drbg_free = 0x400027a4;
mbedtls_ctr_drbg_reseed = 0x400027a8;
mbedtls_ctr_drbg_random_with_add = 0x400027ac;
mbedtls_ctr_drbg_random = 0x400027b0;
mbedtls_sha1_init = 0x40002a1c;
mbedtls_sha1_free = 0x40002a20;
mbedtls_sha1_clone = 0x40002a24;
mbedtls_sha1_starts = 0x40002a28;
mbedtls_sha1_finish = 0x40002a2c;
mbedtls_sha256_init = 0x40002a30;
mbedtls_sha256_free = 0x40002a34;
mbedtls_sha256_clone = 0x40002a38;
mbedtls_sha256_starts = 0x40002a3c;
mbedtls_sha256_finish = 0x40002a40;
mbedtls_sha256 = 0x40002a44;
mbedtls_sha512_init = 0x40002a48;
mbedtls_sha512_free = 0x40002a4c;
mbedtls_sha512_clone = 0x40002a50;
mbedtls_sha512_starts = 0x40002a54;
mbedtls_sha512_update = 0x40002a58;
mbedtls_sha512_finish = 0x40002a5c;
mbedtls_internal_sha512_process = 0x40002a60;
mbedtls_sha512 = 0x40002a64;
mbedtls_aes_xts_init = 0x40002b68;
mbedtls_aes_xts_free = 0x40002b6c;
mbedtls_aes_xts_setkey_enc = 0x40002b70;
mbedtls_aes_xts_setkey_dec = 0x40002b74;
mbedtls_aes_crypt_xts = 0x40002b78;
mbedtls_aes_crypt_cfb128 = 0x40002b7c;
mbedtls_aes_crypt_ofb = 0x40002b80;
mbedtls_aes_crypt_ctr = 0x40002b84;
mbedtls_ccm_init = 0x40002b98;
mbedtls_ccm_setkey = 0x40002b9c;
mbedtls_ccm_free = 0x40002ba0;
mbedtls_ccm_encrypt_and_tag = 0x40002ba4;
mbedtls_ccm_auth_decrypt = 0x40002ba8;
mbedtls_md5_init = 0x40002bd8;
mbedtls_md5_free = 0x40002bdc;
mbedtls_md5_clone = 0x40002be0;
mbedtls_md5_starts = 0x40002be4;
mbedtls_md5_update = 0x40002be8;
mbedtls_md5_finish = 0x40002bec;
mbedtls_internal_md5_process = 0x40002bf0;
mbedtls_md5 = 0x40002bf4;
mbedtls_sha1 = 0x40002c08;
/* Data (.data, .bss, .rodata) */
mbedtls_rom_osi_funcs_ptr = 0x3fcdfaa0;
AES_FSb_ptr = 0x3fcdfa9c;
AES_RT0_ptr = 0x3fcdfa98;
AES_RT1_ptr = 0x3fcdfa94;
AES_RT2_ptr = 0x3fcdfa90;
AES_RT3_ptr = 0x3fcdfa8c;
AES_FT0_ptr = 0x3fcdfa88;
AES_FT1_ptr = 0x3fcdfa84;
AES_FT2_ptr = 0x3fcdfa80;
AES_FT3_ptr = 0x3fcdfa7c;
bignum_small_prime_ptr = 0x3fcdfa78;
sha512_K_ptr = 0x3fcdfa74;
