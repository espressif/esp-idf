/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_foc.h"

/**
 * alpha = u - (v + w)sin(30) * (2/3),  (2/3): Equal amplitude transformation const
 * beta  = (v - w)cos(30) * (2/3)
 */
void foc_clarke_transform(const foc_uvw_coord_t *v_uvw, foc_ab_coord_t *v_ab)
{
    const _iq foc_clark_k1_iq = _IQ(2.0 / 3.0);
    const _iq foc_clark_k2_iq = _IQ(1.0 / 3.0);
    const _iq foc_clark_k3_iq = _IQ(M_SQRT3 / 3.0);

    v_ab->alpha = _IQmpy(v_uvw->u, foc_clark_k1_iq) - _IQmpy(v_uvw->v + v_uvw->w, foc_clark_k2_iq);
    v_ab->beta  = _IQmpy(v_uvw->v - v_uvw->w, foc_clark_k3_iq);
}

void foc_inverse_clarke_transform(const foc_ab_coord_t *v_ab, foc_uvw_coord_t *v_uvw)
{
    v_uvw->u = v_ab->alpha;
    v_uvw->v = _IQdiv2(_IQmpy(v_ab->beta, _IQ(M_SQRT3)) - v_ab->alpha);
    v_uvw->w = -v_uvw->u - v_uvw->v;
}

void foc_park_transform(_iq theta_rad, const foc_ab_coord_t *v_ab, foc_dq_coord_t *v_dq)
{
    _iq sin = _IQsin(theta_rad);
    _iq cos = _IQcos(theta_rad);

    v_dq->d = _IQmpy(v_ab->alpha, cos) + _IQmpy(v_ab->beta, sin);
    v_dq->q = _IQmpy(v_ab->beta, cos) - _IQmpy(v_ab->alpha, sin);
}

void foc_inverse_park_transform(_iq theta_rad, const foc_dq_coord_t *v_dq, foc_ab_coord_t *v_ab)
{
    _iq sin = _IQsin(theta_rad);
    _iq cos = _IQcos(theta_rad);

    v_ab->alpha = _IQmpy(v_dq->d, cos) - _IQmpy(v_dq->q, sin);
    v_ab->beta  = _IQmpy(v_dq->q, cos) + _IQmpy(v_dq->d, sin);
}

void foc_svpwm_duty_calculate(const foc_ab_coord_t *v_ab, foc_uvw_coord_t *out_uvw)
{
    int sextant;
    if (v_ab->beta > 0.0f) {
        if (v_ab->alpha > 0.0f) {
            //quadrant I
            if (v_ab->beta > _IQmpy(v_ab->alpha, _IQ(M_SQRT3))) {
                sextant = 2;    //sextant v2-v3
            } else {
                sextant = 1;    //sextant v1-v2
            }
        } else {
            //quadrant II
            if (-v_ab->beta > _IQmpy(v_ab->alpha, _IQ(M_SQRT3))) {
                sextant = 3;    //sextant v3-v4
            } else {
                sextant = 2;    //sextant v2-v3
            }
        }
    } else {
        if (v_ab->alpha > 0.0f) {
            //quadrant IV
            if (-v_ab->beta > _IQmpy(v_ab->alpha, _IQ(M_SQRT3))) {
                sextant = 5;    //sextant v5-v6
            } else {
                sextant = 6;    //sextant v6-v1
            }
        } else {
            //quadrant III
            if (v_ab->beta > _IQmpy(v_ab->alpha, _IQ(M_SQRT3))) {
                sextant = 4;    //sextant v4-v5
            } else {
                sextant = 5;    //sextant v5-v6
            }
        }
    }

    switch (sextant) {
    // sextant v1-v2
    case 1: {
        _iq t1 = -_IQmpy(v_ab->alpha, _IQ(M_SQRT3)) + v_ab->beta;
        _iq t2 = -_IQmpy2(v_ab->beta);

        // PWM timings
        out_uvw->u = _IQdiv2(_IQ(1.F) - t1 - t2);
        out_uvw->v = out_uvw->u + t1;
        out_uvw->w = out_uvw->v + t2;
    } break;

    // sextant v2-v3
    case 2: {
        _iq t2 = -_IQmpy(v_ab->alpha, _IQ(M_SQRT3)) - v_ab->beta;
        _iq t3 =  _IQmpy(v_ab->alpha, _IQ(M_SQRT3)) - v_ab->beta;

        // PWM timings
        out_uvw->v = _IQdiv2(_IQ(1.F) - t2 - t3);
        out_uvw->u = out_uvw->v + t3;
        out_uvw->w = out_uvw->u + t2;
    } break;

    // sextant v3-v4
    case 3: {
        _iq t3 = -_IQmpy2(v_ab->beta);
        _iq t4 = _IQmpy(v_ab->alpha, _IQ(M_SQRT3)) + v_ab->beta;

        // PWM timings
        out_uvw->v = _IQdiv2(_IQ(1.F) - t3 - t4);
        out_uvw->w = out_uvw->v + t3;
        out_uvw->u = out_uvw->w + t4;
    } break;

    // sextant v4-v5
    case 4: {
        _iq t4 = _IQmpy(v_ab->alpha, _IQ(M_SQRT3)) - v_ab->beta;
        _iq t5 = _IQmpy2(v_ab->beta);

        // PWM timings
        out_uvw->w = _IQdiv2(_IQ(1.F) - t4 - t5);
        out_uvw->v = out_uvw->w + t5;
        out_uvw->u = out_uvw->v + t4;
    } break;

    // sextant v5-v6
    case 5: {
        _iq t5 =  _IQmpy(v_ab->alpha, _IQ(M_SQRT3)) + v_ab->beta;
        _iq t6 = -_IQmpy(v_ab->alpha, _IQ(M_SQRT3)) + v_ab->beta;

        // PWM timings
        out_uvw->w = _IQdiv2(_IQ(1.F) - t5 - t6);
        out_uvw->u = out_uvw->w + t5;
        out_uvw->v = out_uvw->u + t6;
    } break;

    // sextant v6-v1
    case 6: {
        _iq t6 = _IQmpy2(v_ab->beta);
        _iq t1 = -_IQmpy(v_ab->alpha, _IQ(M_SQRT3)) - v_ab->beta;

        // PWM timings
        out_uvw->u = _IQdiv2(_IQ(1.F) - t6 - t1);
        out_uvw->w = out_uvw->u + t1;
        out_uvw->v = out_uvw->w + t6;
    } break;
    }
}
