/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "IQmathLib.h"

#include "foc_svpwm.h"

void modulation_svpwm_7seg(const clarke_park_ab_iq15_t *uab, uint32_t pwm_period_ticks, svpwm_uvw_switch_ticks_t *ret_switch_ticks)
{
    svpwm_uvw_switch_ticks_t switch_ticks = {0};
    int32_t period_ticks = (int32_t)pwm_period_ticks;
    const _iq15 sqrt3_half = _IQ15(0.8660254038f);
    const _iq15 one_half = _IQ15(0.5f);

    /* Determine the sector index. */
    _iq15 vref1 = uab->beta;
    _iq15 vref2 = _IQ15mpy(uab->alpha, sqrt3_half) - _IQ15mpy(uab->beta, one_half);
    _iq15 vref3 = -_IQ15mpy(uab->alpha, sqrt3_half) - _IQ15mpy(uab->beta, one_half);

    uint8_t N = (vref1 > 0) + ((vref2 > 0) << 1) + ((vref3 > 0) << 2);

    /* Compute active-vector dwell times in timer ticks. */
    int32_t X_ticks = _IQ15mpyI32int(uab->beta, period_ticks);
    int32_t Y_ticks = _IQ15mpyI32int(_IQ15mpy(uab->alpha, sqrt3_half) + _IQ15mpy(uab->beta, one_half), period_ticks);
    int32_t Z_ticks = _IQ15mpyI32int(-_IQ15mpy(uab->alpha, sqrt3_half) + _IQ15mpy(uab->beta, one_half), period_ticks);

    int32_t T1stvec_ticks = 0;
    int32_t T2ndvec_ticks = 0;

    switch (N) {
    case 1: /* Sector 2 */
        T1stvec_ticks = Z_ticks;
        T2ndvec_ticks = Y_ticks;
        break;
    case 2: /* Sector 6 */
        T1stvec_ticks = Y_ticks;
        T2ndvec_ticks = -X_ticks;
        break;
    case 3: /* Sector 1 */
        T1stvec_ticks = -Z_ticks;
        T2ndvec_ticks = X_ticks;
        break;
    case 4: /* Sector 4 */
        T1stvec_ticks = -X_ticks;
        T2ndvec_ticks = Z_ticks;
        break;
    case 5: /* Sector 3 */
        T1stvec_ticks = X_ticks;
        T2ndvec_ticks = -Y_ticks;
        break;
    case 6: /* Sector 5 */
        T1stvec_ticks = -Y_ticks;
        T2ndvec_ticks = -Z_ticks;
        break;
    default:
        T1stvec_ticks = 0;
        T2ndvec_ticks = 0;
        break;
    }
    if (T1stvec_ticks < 0) {
        T1stvec_ticks = 0;
    }
    if (T2ndvec_ticks < 0) {
        T2ndvec_ticks = 0;
    }

    /* Handle overmodulation. */
    int32_t Tvalidvec_ticks = T1stvec_ticks + T2ndvec_ticks;
    if (Tvalidvec_ticks > period_ticks) {
        T1stvec_ticks = (int32_t)(((int64_t)T1stvec_ticks * period_ticks) / Tvalidvec_ticks);
        T2ndvec_ticks = period_ticks - T1stvec_ticks;
    }
    int32_t Tzerovec_ticks = period_ticks - T1stvec_ticks - T2ndvec_ticks;

    /* Calculate switch points in the first half PWM period. */
    int32_t T1stswion_ticks = Tzerovec_ticks / 4;
    int32_t T2ndswion_ticks = T1stswion_ticks + T1stvec_ticks / 2;
    int32_t T3rdswion_ticks = T2ndswion_ticks + T2ndvec_ticks / 2;

    switch (N) {
    case 1: /* Sector 2 */
        switch_ticks.u = (uint32_t)T2ndswion_ticks;
        switch_ticks.v = (uint32_t)T1stswion_ticks;
        switch_ticks.w = (uint32_t)T3rdswion_ticks;
        break;
    case 2: /* Sector 6 */
        switch_ticks.u = (uint32_t)T1stswion_ticks;
        switch_ticks.v = (uint32_t)T3rdswion_ticks;
        switch_ticks.w = (uint32_t)T2ndswion_ticks;
        break;
    case 3: /* Sector 1 */
        switch_ticks.u = (uint32_t)T1stswion_ticks;
        switch_ticks.v = (uint32_t)T2ndswion_ticks;
        switch_ticks.w = (uint32_t)T3rdswion_ticks;
        break;
    case 4: /* Sector 4 */
        switch_ticks.u = (uint32_t)T3rdswion_ticks;
        switch_ticks.v = (uint32_t)T2ndswion_ticks;
        switch_ticks.w = (uint32_t)T1stswion_ticks;
        break;
    case 5: /* Sector 3 */
        switch_ticks.u = (uint32_t)T3rdswion_ticks;
        switch_ticks.v = (uint32_t)T1stswion_ticks;
        switch_ticks.w = (uint32_t)T2ndswion_ticks;
        break;
    case 6: /* Sector 5 */
        switch_ticks.u = (uint32_t)T2ndswion_ticks;
        switch_ticks.v = (uint32_t)T3rdswion_ticks;
        switch_ticks.w = (uint32_t)T1stswion_ticks;
        break;
    default: /* Produce zero line-to-line voltage. */
        switch_ticks.u = (uint32_t)(period_ticks / 2);
        switch_ticks.v = (uint32_t)(period_ticks / 2);
        switch_ticks.w = (uint32_t)(period_ticks / 2);
        break;
    }

    *ret_switch_ticks = switch_ticks;
}
