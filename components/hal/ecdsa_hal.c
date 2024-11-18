/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/assert.h"
#include "hal/ecdsa_ll.h"
#include "hal/ecdsa_hal.h"
#include "hal/efuse_hal.h"

#if CONFIG_HAL_ECDSA_GEN_SIG_CM
#include "esp_fault.h"
#include "esp_random.h"
#endif

#ifdef SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
#include "hal/key_mgr_ll.h"
#endif

#define ECDSA_HAL_P192_COMPONENT_LEN        24
#define ECDSA_HAL_P256_COMPONENT_LEN        32

static void configure_ecdsa_periph(ecdsa_hal_config_t *conf)
{

    if (conf->use_km_key == 0) {
        efuse_hal_set_ecdsa_key(conf->efuse_key_blk);

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
        // Force Key Manager to use eFuse key for XTS-AES operation
        key_mgr_ll_set_key_usage(ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
#endif
    }
#if SOC_KEY_MANAGER_SUPPORTED
    else {
        key_mgr_ll_set_key_usage(ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_USE_OWN_KEY);
    }
#endif

    ecdsa_ll_set_mode(conf->mode);
    ecdsa_ll_set_curve(conf->curve);

    if (conf->mode != ECDSA_MODE_EXPORT_PUBKEY) {
        ecdsa_ll_set_z_mode(conf->sha_mode);
    }

#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    ecdsa_ll_set_k_type(conf->sign_type);

    if (conf->sign_type == ECDSA_K_TYPE_DETERMINISITIC) {
        ecdsa_ll_set_deterministic_loop(conf->loop_number);
    }
#endif
}

bool ecdsa_hal_get_operation_result(void)
{
    return ecdsa_ll_get_operation_result();
}

static void ecdsa_hal_gen_signature_inner(const uint8_t *hash, uint8_t *r_out,
                              uint8_t *s_out, uint16_t len)
{
    ecdsa_ll_set_stage(ECDSA_STAGE_START_CALC);

    while(ecdsa_ll_get_state() != ECDSA_STATE_LOAD) {
        ;
    }

    ecdsa_ll_write_param(ECDSA_PARAM_Z, hash, len);

    ecdsa_ll_set_stage(ECDSA_STAGE_LOAD_DONE);

    while (ecdsa_ll_get_state() != ECDSA_STATE_GET) {
        ;
    }

    ecdsa_ll_read_param(ECDSA_PARAM_R, r_out, len);
    ecdsa_ll_read_param(ECDSA_PARAM_S, s_out, len);

    ecdsa_ll_set_stage(ECDSA_STAGE_GET_DONE);

    while (ecdsa_ll_get_state() != ECDSA_STATE_IDLE) {
        ;
    }
}

#if CONFIG_HAL_ECDSA_GEN_SIG_CM
__attribute__((optimize("O0"))) static void ecdsa_hal_gen_signature_with_countermeasure(const uint8_t *hash, uint8_t *r_out,
                       uint8_t *s_out, uint16_t len)
{
    uint8_t tmp_r_out[32] = {};
    uint8_t tmp_s_out[32] = {};
    uint8_t tmp_hash[64] = {};

    uint8_t dummy_op_count_prior = esp_random() % ECDSA_SIGN_MAX_DUMMY_OP_COUNT;
    uint8_t dummy_op_count_later = ECDSA_SIGN_MAX_DUMMY_OP_COUNT - dummy_op_count_prior;
    ESP_FAULT_ASSERT((dummy_op_count_prior != 0) || (dummy_op_count_later != 0));
    ESP_FAULT_ASSERT(dummy_op_count_prior + dummy_op_count_later == ECDSA_SIGN_MAX_DUMMY_OP_COUNT);

    esp_fill_random(tmp_hash, 64);
    /* Dummy ecdsa signature operations prior to the actual one */
    for (int i = 0; i < dummy_op_count_prior; i++) {
        ecdsa_hal_gen_signature_inner(tmp_hash + ((6 * i) % 32), (uint8_t *) tmp_r_out, (uint8_t *) tmp_s_out, len);
    }

    /* Actual ecdsa signature operation */
    ecdsa_hal_gen_signature_inner(hash, r_out, s_out, len);

    /* Dummy ecdsa signature operations after the actual one */
    for (int i = 0; i < dummy_op_count_later; i++) {
        ecdsa_hal_gen_signature_inner(tmp_hash + ((6 * i) % 32), (uint8_t *)tmp_r_out, (uint8_t *)tmp_s_out, len);
    }

}
#endif /* CONFIG_HAL_ECDSA_GEN_SIG_CM */



void ecdsa_hal_gen_signature(ecdsa_hal_config_t *conf, const uint8_t *hash,
                        uint8_t *r_out, uint8_t *s_out, uint16_t len)
{
    if (len != ECDSA_HAL_P192_COMPONENT_LEN && len != ECDSA_HAL_P256_COMPONENT_LEN) {
        HAL_ASSERT(false && "Incorrect length");
    }

    if (conf->sha_mode == ECDSA_Z_USER_PROVIDED && hash == NULL) {
        HAL_ASSERT(false && "Mismatch in SHA configuration");
    }

    if (ecdsa_ll_get_state() != ECDSA_STATE_IDLE) {
        HAL_ASSERT(false && "Incorrect ECDSA state");
    }

    configure_ecdsa_periph(conf);

#if CONFIG_HAL_ECDSA_GEN_SIG_CM
    ecdsa_hal_gen_signature_with_countermeasure(hash, r_out, s_out, len);
#else /* CONFIG_HAL_ECDSA_GEN_SIG_CM */
    ecdsa_hal_gen_signature_inner(hash, r_out, s_out, len);
#endif /* !CONFIG_HAL_ECDSA_GEN_SIG_CM */

}

int ecdsa_hal_verify_signature(ecdsa_hal_config_t *conf, const uint8_t *hash, const uint8_t *r, const uint8_t *s,
                               const uint8_t *pub_x, const uint8_t *pub_y, uint16_t len)
{
    if (len != ECDSA_HAL_P192_COMPONENT_LEN && len != ECDSA_HAL_P256_COMPONENT_LEN) {
        HAL_ASSERT(false && "Incorrect length");
    }

    if (ecdsa_ll_get_state() != ECDSA_STATE_IDLE) {
        HAL_ASSERT(false && "Incorrect ECDSA state");
    }

    configure_ecdsa_periph(conf);

    ecdsa_ll_set_stage(ECDSA_STAGE_START_CALC);

    while(ecdsa_ll_get_state() != ECDSA_STATE_LOAD) {
        ;
    }

    ecdsa_ll_write_param(ECDSA_PARAM_Z, hash, len);
    ecdsa_ll_write_param(ECDSA_PARAM_R, r, len);
    ecdsa_ll_write_param(ECDSA_PARAM_S, s, len);
    ecdsa_ll_write_param(ECDSA_PARAM_QAX, pub_x, len);
    ecdsa_ll_write_param(ECDSA_PARAM_QAY, pub_y, len);

    ecdsa_ll_set_stage(ECDSA_STAGE_LOAD_DONE);

    while (ecdsa_ll_get_state() != ECDSA_STATE_IDLE) {
        ;
    }

    bool res = ecdsa_hal_get_operation_result();

    return (res ? 0 : -1);
}

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
void ecdsa_hal_export_pubkey(ecdsa_hal_config_t *conf, uint8_t *pub_x, uint8_t *pub_y, uint16_t len)
{
    if (len != ECDSA_HAL_P192_COMPONENT_LEN && len != ECDSA_HAL_P256_COMPONENT_LEN) {
        HAL_ASSERT(false && "Incorrect length");
    }

    if (ecdsa_ll_get_state() != ECDSA_STATE_IDLE) {
        HAL_ASSERT(false && "Incorrect ECDSA state");
    }

    configure_ecdsa_periph(conf);

    ecdsa_ll_set_stage(ECDSA_STAGE_START_CALC);

    while(ecdsa_ll_get_state() != ECDSA_STATE_LOAD) {
        ;
    }

    ecdsa_ll_set_stage(ECDSA_STAGE_LOAD_DONE);

    while (ecdsa_ll_get_state() != ECDSA_STATE_GET) {
        ;
    }

    ecdsa_ll_read_param(ECDSA_PARAM_QAX, pub_x, len);
    ecdsa_ll_read_param(ECDSA_PARAM_QAY, pub_y, len);

    ecdsa_ll_set_stage(ECDSA_STAGE_GET_DONE);

    while (ecdsa_ll_get_state() != ECDSA_STATE_IDLE) {
        ;
    }
}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE

bool ecdsa_hal_det_signature_k_check(void)
{
    return (ecdsa_ll_check_k_value() == 0);
}

#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
