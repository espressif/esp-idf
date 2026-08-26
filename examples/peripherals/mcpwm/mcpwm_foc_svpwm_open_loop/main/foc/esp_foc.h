/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <math.h>

// Use IQ18 type, range [-8,192 8,191.999 996 185]
// This definition should be added before including "IQmathLib.h"
#define GLOBAL_IQ               18
#include "IQmathLib.h"

// 3-phase uvw coord data type
typedef struct foc_uvw_coord {
    _iq u;      // U phase data in IQ type
    _iq v;      // V phase data in IQ type
    _iq w;      // W phase data in IQ type
} foc_uvw_coord_t;

// alpha-beta axis static coord data type
typedef struct foc_ab_coord {
    _iq alpha;  // alpha axis data in IQ type
    _iq beta;   // beta axis data in IQ type
} foc_ab_coord_t;

//d-q (direct-quadrature) axis rotate coord data type
typedef struct foc_dq_coord {
    _iq d;      // direct axis data in IQ type
    _iq q;      // quadrature axis data in IQ type
} foc_dq_coord_t;

/**
 * @brief clark transform, to transform value in 3phase uvw system to static alpha_beta system
 *
 * @param[in] v_uvw     data in 3-phase coord to be transformed
 * @param[out] v_ab     output data in alpha-beta coord
 */
void foc_clarke_transform(const foc_uvw_coord_t *v_uvw, foc_ab_coord_t *v_ab);

/**
 * @brief inverse clark transform, to transform value in alpha_beta system to 3phase uvw system
 *
 * @param[in] v_ab      data in alpha-beta coord to be transformed
 * @param[out] v_uvw    output data in 3-phase coord
 */
void foc_inverse_clarke_transform(const foc_ab_coord_t *v_ab, foc_uvw_coord_t *v_uvw);

/**
 * @brief park transform, to transform value in static alpha_beta system to rotate d-q system
 *
 * @param[in] theta_rad     theta of dq_coord refer to alpha-beta coord, in rad
 * @param[in] v_ab          data in alpha-beta coord to be transformed
 * @param[out] v_dq         output data in dq coord
 */
void foc_park_transform(_iq theta_rad, const foc_ab_coord_t *v_ab, foc_dq_coord_t *v_dq);

/**
 * @brief inverse park transform, to transform value in rotate d-q system to alpha_beta system
 *
 * @param[in] theta_rad     theta of dq_coord refer to alpha-beta coord, in rad
 * @param[in] v_dq          data in dq coord to be transformed
 * @param[out] v_ab         output data in alpha-beta coord
 */
void foc_inverse_park_transform(_iq theta_rad, const foc_dq_coord_t *v_dq, foc_ab_coord_t *v_ab);

/**
 * @brief 7-segment svpwm modulation
 *
 * @param v_ab[in]      input value in alpha-beta coord
 * @param out_uvw[out]  output modulated pwm duty in IQ type
 */
void foc_svpwm_duty_calculate(const foc_ab_coord_t *v_ab, foc_uvw_coord_t *out_uvw);
