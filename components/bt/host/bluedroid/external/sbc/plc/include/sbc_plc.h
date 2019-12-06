// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SBC_PLC_H
#define _SBC_PLC_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

/* Paramter for PLC (16 kHZ)*/
#define SBC_FS          120                     /* SBC Frame Size */
#define SBC_N           256                     /* 16ms - Window Length for pattern matching */
#define SBC_M           64                      /* 4ms - Template for matching */
#define SBC_LHIST       (SBC_N + SBC_FS - 1)    /* Length of history buffer required */
#define SBC_RT          36                      /* SBC Reconvergence Time (samples) */
#define SBC_OLAL        16                      /* OverLap-Add Length (samples) */

/* PLC State Information */
typedef struct sbc_plc_state {
    int16_t hist[SBC_LHIST + SBC_FS + SBC_RT + SBC_OLAL];
    int16_t bestlag;
    int     nbf;
} sbc_plc_state_t;

/* Prototypes */
/**
 * Perform PLC initialization of memory vectors.
 *
 * @param plc_state pointer to PLC state memory
 */
void sbc_plc_init(sbc_plc_state_t *plc_state);

/**
 * Perform PLC deinitialization of memory vectors.
 *
 * @param plc_state pointer to PLC state memory
 */
void sbc_plc_deinit(sbc_plc_state_t *plc_state);

/**
 * Perform bad frame processing.
 *
 * @param plc_state pointer to PLC state memory
 * @param ZIRbuf    pointer to the ZIR response of the SBC decoder
 * @param out       pointer to the output samples
 */
void sbc_plc_bad_frame(sbc_plc_state_t *plc_state, int16_t *ZIRbuf, int16_t *out);

/**
 * Perform good frame processing. Most of the time, this function
 * just updates history buffers and passes the input to the output,
 * but in the first good frame after frame loss, it must conceal the
 * received signal as it reconverges with the true output.
 *
 * @param plc_state pointer to PLC state memory
 * @param in        pointer to the input vector
 * @param out       pointer to the output samples
 */
void sbc_plc_good_frame(sbc_plc_state_t *plc_state, int16_t *in, int16_t *out);

/**
 * Get a zero signal eSCO frame
 * @return  pointer to data buffer
 */
uint8_t * sbc_plc_zero_signal_frame(void);

/**
 * Get a zero signal eSCO pcm frame
 * @return  pointer to data buffer
 */
int16_t * sbc_plc_zero_signal_frame_pcm(void);

#if defined __cplusplus
}
#endif

#endif /// _SBC_PLC_H
