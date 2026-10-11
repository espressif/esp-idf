/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>

#include "IQmathLib.h"
#include "clarke_park.h"

#include "board_config.h"
#include "board_inverter.h"
#include "foc_core.h"
#include "foc_svpwm.h"

_Static_assert(
    BOARD_INVERTER_TIMER_RESOLUTION_HZ % FOC_CORE_CONTROL_FREQ_HZ == 0,
    "MCPWM timer resolution must be divisible by FOC control frequency"
);

_Static_assert(
    FOC_CORE_OPENLOOP_THETA_STEP_DEG_INCREMENT_Q15 >= 1,
    "Open-loop ramp increment is below the Q15 resolution; reduce FOC_CORE_OPENLOOP_RAMP_UP_DIVISIONS"
);

void foc_core_init(foc_core_t *core, inverter_handle_t inverter)
{
    *core = (foc_core_t) {
        .inverter = inverter,
        .pwm_period_ticks = BOARD_INVERTER_TIMER_RESOLUTION_HZ / FOC_CORE_CONTROL_FREQ_HZ,
        .state = FOC_CORE_STATE_STOPPED,

        .openloop = {
            .cnt_align_cycles = 0,
            .theta_step_deg = _IQ15(0.0f),
            .theta_step_deg_target = FOC_CORE_OPENLOOP_THETA_STEP_DEG_TARGET_Q15,
            .theta_step_deg_increment = FOC_CORE_OPENLOOP_THETA_STEP_DEG_INCREMENT_Q15,
            .udq_ref = {
                .d = FOC_CORE_OPENLOOP_UD_REF_Q15,
                .q = _IQ15(0.0f),
            },
        },

        .theta_deg = _IQ15(0.0f),
        .udq_ref = {0},
        .uab_ref = {0},
    };
}

esp_err_t foc_core_execute(foc_core_t *core)
{
    /* Update the open-loop startup state and virtual electrical angle. */
    switch (core->state) {
    case FOC_CORE_STATE_STOPPED:
        core->udq_ref = (clarke_park_dq_iq15_t) {
            0
        };
        core->state = FOC_CORE_STATE_OPENLOOP_ALIGN;
        break;

    case FOC_CORE_STATE_OPENLOOP_ALIGN:
        core->theta_deg = _IQ15(0.0f); /* angle fixed */
        core->udq_ref = core->openloop.udq_ref;

        if (++core->openloop.cnt_align_cycles >= FOC_CORE_OPENLOOP_ALIGN_DURATION_CYCLES) {
            core->openloop.cnt_align_cycles = 0;
            core->state = FOC_CORE_STATE_OPENLOOP_RAMP_UP;
        }
        break;

    case FOC_CORE_STATE_OPENLOOP_RAMP_UP:
        core->openloop.theta_step_deg += core->openloop.theta_step_deg_increment;
        if (core->openloop.theta_step_deg >= core->openloop.theta_step_deg_target) {
            core->openloop.theta_step_deg = core->openloop.theta_step_deg_target;
            core->state = FOC_CORE_STATE_OPENLOOP_CONSTANT_SPEED;
        }

        core->theta_deg += core->openloop.theta_step_deg;
        if (core->theta_deg >= _IQ15(360.0f)) {
            core->theta_deg -= _IQ15(360.0f);
        }
        break;

    case FOC_CORE_STATE_OPENLOOP_CONSTANT_SPEED:
        core->theta_deg += core->openloop.theta_step_deg;
        if (core->theta_deg >= _IQ15(360.0f)) {
            core->theta_deg -= _IQ15(360.0f);
        }
        break;

    default:
        core->udq_ref = (clarke_park_dq_iq15_t) {
            0
        };
        break;
    }

    /* Transform the dq voltage reference into the stationary alpha-beta frame. */
    _iq15 theta_rad = _IQ15mpy(core->theta_deg, _IQ15(M_PI / 180.0f));
    clarke_park_ipark_iq15(theta_rad, &core->udq_ref, &core->uab_ref);

    /* Calculate three-phase switching points using seven-segment SVPWM. */
    svpwm_uvw_switch_ticks_t svpwm_switch_ticks = {0};
    modulation_svpwm_7seg(&core->uab_ref, core->pwm_period_ticks, &svpwm_switch_ticks);

    /* Apply the switching points to the inverter. */
    inverter_uvw_switch_ticks_t inverter_switch_ticks = {
        .u = svpwm_switch_ticks.u,
        .v = svpwm_switch_ticks.v,
        .w = svpwm_switch_ticks.w,
    };
    return board_inverter_set_switchpoints(core->inverter, &inverter_switch_ticks);
}
