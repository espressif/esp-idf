/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "sdkconfig.h"
#include "IQmathLib.h"
#include "clarke_park.h"

#include "board_config.h"
#include "board_inverter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* User-configurable FOC parameters. */
#define FOC_CORE_CONTROL_FREQ_HZ \
    BOARD_INVERTER_MODULATION_FREQ_HZ                   /* One FOC control cycle per inverter modulation cycle */
#define FOC_CORE_OPENLOOP_ALIGN_DURATION_MS \
    CONFIG_EXAMPLE_FOC_OPENLOOP_ALIGN_DURATION_MS       /* Rotor alignment duration, in milliseconds */
#define FOC_CORE_OPENLOOP_TARGET_ELEC_FREQ_HZ \
    CONFIG_EXAMPLE_FOC_OPENLOOP_TARGET_ELEC_FREQ_HZ     /* Target electrical frequency for open-loop drag, in Hz */
#define FOC_CORE_OPENLOOP_RAMP_UP_DIVISIONS \
    CONFIG_EXAMPLE_FOC_OPENLOOP_RAMP_UP_DIVISIONS       /* Nominal number of open-loop ramp divisions */
#define FOC_CORE_OPENLOOP_UD_REF_PERCENT \
    CONFIG_EXAMPLE_FOC_OPENLOOP_UD_REF_PERCENT          /* D-axis voltage reference as a percentage of Udc / sqrt(3) */

/* Derived parameters. Do not modify them. */
#define FOC_CORE_OPENLOOP_UD_REF_Q15 \
    _IQ15((float)FOC_CORE_OPENLOOP_UD_REF_PERCENT / 100.0f)      /* D-axis voltage open-loop reference, normalized to Udc / sqrt(3) */

#define FOC_CORE_OPENLOOP_THETA_STEP_DEG_TARGET_Q15 \
    _IQ15(((float)FOC_CORE_OPENLOOP_TARGET_ELEC_FREQ_HZ * 360.0f) / (float)FOC_CORE_CONTROL_FREQ_HZ)

#define FOC_CORE_OPENLOOP_THETA_STEP_DEG_INCREMENT_Q15 \
    (FOC_CORE_OPENLOOP_THETA_STEP_DEG_TARGET_Q15 / (int32_t)FOC_CORE_OPENLOOP_RAMP_UP_DIVISIONS)

#define FOC_CORE_OPENLOOP_ALIGN_DURATION_CYCLES \
    ((uint32_t)(((uint64_t)FOC_CORE_OPENLOOP_ALIGN_DURATION_MS * FOC_CORE_CONTROL_FREQ_HZ + 500U) / 1000U))

typedef enum {
    FOC_CORE_STATE_STOPPED = 0,
    FOC_CORE_STATE_OPENLOOP_ALIGN,
    FOC_CORE_STATE_OPENLOOP_RAMP_UP,
    FOC_CORE_STATE_OPENLOOP_CONSTANT_SPEED,
} foc_core_state_t;

typedef struct {
    uint32_t cnt_align_cycles;      /* Elapsed alignment control cycles */

    _iq15 theta_step_deg;             /* Electrical angle increment per control cycle, gradually linearly increase */
    _iq15 theta_step_deg_target;      /* Target electrical angle increment per cycle */
    _iq15 theta_step_deg_increment;   /* Incremental slope of electrical angle increment */

    clarke_park_dq_iq15_t udq_ref;               /* Open-loop stator voltage reference in dq frame */
} foc_openloop_t;

typedef struct {
    inverter_handle_t inverter;
    uint32_t pwm_period_ticks;      /* PWM period in MCPWM timer ticks */

    foc_core_state_t state;

    foc_openloop_t openloop;

    _iq15 theta_deg;                  /* Electrical rotor angle */

    clarke_park_dq_iq15_t udq_ref;               /* Stator voltage reference in dq frame normalized to Udc / sqrt(3) */
    clarke_park_ab_iq15_t uab_ref;               /* Stator voltage reference in alpha-beta frame normalized to Udc / sqrt(3) */
} foc_core_t;

/**
 * @brief Initialize the FOC core.
 */
void foc_core_init(foc_core_t *core, inverter_handle_t inverter);

/**
 * @brief Execute one FOC control cycle.
 *
 * This function must be called at FOC_CORE_CONTROL_FREQ_HZ frequency.
 */
esp_err_t foc_core_execute(foc_core_t *core);

#ifdef __cplusplus
}
#endif
