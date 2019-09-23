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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/bt_target.h"
#include "sbc_plc.h"

#if (PLC_INCLUDED == TRUE)
/* msbc */
static const uint8_t indices0[] = { 0xad, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, 0x00, 0x77, 0x6d,
0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d,
0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d,
0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d,
0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6c};


/* 8 kHZ */
static const int16_t indices0_pcm[] = {
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0};

/* Raised COSine table for OLA */
/* 16 kHZ */
static float rcos[SBC_OLAL] = {
    0.99148655f,0.96623611f,0.92510857f,0.86950446f,
    0.80131732f,0.72286918f,0.63683150f,0.54613418f,
    0.45386582f,0.36316850f,0.27713082f,0.19868268f,
    0.13049554f,0.07489143f,0.03376389f,0.00851345f};

// /* 8 kHZ */
// static float rcos[SBC_OLAL] = {
//     0.96984631f,0.88302222f,      0.75f,0.58682409f,
//     0.41317591f,      0.25f,0.11697778f,0.09015369f};


static float SqrtByCarmack(const float x){
    union {
        int i;
        float y;
    } float_int;

    float x2;
    const float threehalfs = 1.5f;

    x2 = x * 0.5f;
    float_int.y = x;

    float_int.i = 0x5f375a86 - (float_int.i >> 1);
    float_int.y = float_int.y * (threehalfs - (x2 * float_int.y * float_int.y));
    // float_int.y = float_int.y * (threehalfs - (x2 * float_int.y * float_int.y));
    // float_int.y = float_int.y * (threehalfs - (x2 * float_int.y * float_int.y));

    return (x * float_int.y);
}

static float absolute(float x){
    if (x < 0) {
        x = -x;
    }

    return x;
}

/**
 * Compute the cross correlation according to Eq. (4) of Goodman
 * paper, except that the true correlation is used. His formula
 * seems to be incorrect.
 *
 * @param  x pointer to x input vector
 * @param  y pointer to y input vector
 *
 * @return   value containing the cross-correlation of x and y
 */
static float CrossCorrelation(int16_t *x, int16_t *y){
    int   m;
    float num = 0;
    float den = 0;
    float x2 = 0;
    float y2 = 0;

    for (m = 0; m < SBC_M; m++) {
        num += ((float)x[m]) * y[m];
        x2 += ((float)x[m]) * x[m];
        y2 += ((float)y[m]) * y[m];
    }
    den = (float)SqrtByCarmack(x2 * y2);
    return num / den;
}

/**
 * Perform pattern matching to find the match of template with the
 * history buffer according to Section B of Goodman paper.
 *
 * @param  y pointer to history buffer
 *
 * @return   the lag corresponding to the best match. The lag is
 *           with respect to the beginning of the history buffer.
 *
 */
static int PatternMatch(int16_t *y){
    int   n;
    float maxCn = -999999.0;  // large negative number
    float Cn;
    int   bestmatch = 0;

    for (n = 0; n < SBC_N; n++){
        Cn = CrossCorrelation(&y[SBC_LHIST-SBC_M], &y[n]);
        if (Cn > maxCn){
            bestmatch = n;
            maxCn = Cn;
        }
    }
    return bestmatch;
}

/**
 * Perform amplitude matching using mean-absolute-value according
 * to Goodman paper.
 *
 * @param  y         pointer to history buffer
 * @param  bestmatch value of the lag to the best match
 *
 * @return           scale factor
 */
static float AmplitudeMatch(int16_t *y, int16_t bestmatch) {
    int   i;
    float sumx = 0;
    float sumy = 0.000001f;
    float sf;

    for (i = 0; i < SBC_FS; i++){
        sumx += absolute(y[SBC_LHIST - SBC_FS + i]);
        sumy += absolute(y[bestmatch + i]);
    }
    sf = sumx / sumy;
    // This is not in the paper, but limit the scaling factor to something reasonable to avoid creating artifacts
    if (sf < 0.75f) {
        sf = 0.75f;
    }
    if (sf > 1.2f) {
        sf = 1.2f;
    }
    return sf;
}

static int16_t crop_sample(float val){
    float croped_val = val;
    if (croped_val > 32767.0)  croped_val= 32767.0;
    if (croped_val < -32768.0) croped_val=-32768.0;
    return (int16_t) croped_val;
}

/**
 * Get a zero signal eSCO frame
 * @return  pointer to data buffer
 */
uint8_t * sbc_plc_zero_signal_frame(void){
    return (uint8_t *)&indices0;
}

/**
 * Get a zero signal eSCO pcm frame
 * @return  pointer to data buffer
 */
int16_t * sbc_plc_zero_signal_frame_pcm(void){
    return (int16_t *)&indices0_pcm;
}

/**
 * Perform PLC initialization of memory vectors.
 *
 * @param plc_state pointer to PLC state memory
 */
void sbc_plc_init(sbc_plc_state_t *plc_state){
    plc_state->nbf=0;
    plc_state->bestlag=0;
    memset(plc_state->hist, 0, sizeof(plc_state->hist));
}

/**
 * Perform PLC deinitialization of memory vectors.
 *
 * @param plc_state pointer to PLC state memory
 */
void sbc_plc_deinit(sbc_plc_state_t *plc_state){
    plc_state->nbf=0;
    plc_state->bestlag=0;
    memset(plc_state->hist, 0, sizeof(plc_state->hist));
}

/**
 * Perform bad frame processing.
 *
 * @param plc_state pointer to PLC state memory
 * @param ZIRbuf    pointer to the ZIR response of the SBC decoder
 * @param out       pointer to the output samples
 */
void sbc_plc_bad_frame(sbc_plc_state_t *plc_state, int16_t *ZIRbuf, int16_t *out){
    int   i = 0;
    float val;
    float sf = 1;

    plc_state->nbf++;

    if (plc_state->nbf == 1){
        // Perform pattern matching to find where to replicate
        plc_state->bestlag = PatternMatch(plc_state->hist);
        // the replication begins after the template match
        plc_state->bestlag += SBC_M;

        // Compute Scale Factor to Match Amplitude of Substitution Packet to that of Preceding Packet
        sf = AmplitudeMatch(plc_state->hist, plc_state->bestlag);

        for (i = 0; i < SBC_OLAL; i++){
            val = ZIRbuf[i] * rcos[i]
                + sf * plc_state->hist[plc_state->bestlag + i] * rcos[SBC_OLAL - i - 1];
            plc_state->hist[SBC_LHIST + i] = crop_sample(val);
        }

        for (; i < SBC_FS; i++){
            val = sf*plc_state->hist[plc_state->bestlag + i];
            plc_state->hist[SBC_LHIST + i] = crop_sample(val);
        }

        for (; i < SBC_FS + SBC_OLAL; i++){
            val = sf * plc_state->hist[plc_state->bestlag + i] * rcos[i-SBC_FS]
                + plc_state->hist[plc_state->bestlag + i] * rcos[SBC_OLAL - 1 - i + SBC_FS];
            plc_state->hist[SBC_LHIST + i] = crop_sample(val);
        }

        for (; i < SBC_FS + SBC_RT + SBC_OLAL; i++){
            plc_state->hist[SBC_LHIST + i] = plc_state->hist[plc_state->bestlag + i];
        }
    } else {
        for ( ;i < SBC_FS + SBC_RT + SBC_OLAL; i++){
            plc_state->hist[SBC_LHIST + i] = plc_state->hist[plc_state->bestlag + i];
        }
    }

    for (i = 0; i < SBC_FS; i++){
        out[i] = plc_state->hist[SBC_LHIST + i];
    }

   // shift the history buffer
    for (i = 0; i < SBC_LHIST + SBC_RT + SBC_OLAL; i++){
        plc_state->hist[i] = plc_state->hist[i + SBC_FS];
    }
}

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
void sbc_plc_good_frame(sbc_plc_state_t *plc_state, int16_t *in, int16_t *out){
    int i = 0;

    if (plc_state->nbf > 0){
        for (i = 0; i < SBC_RT; i++){
            out[i] = plc_state->hist[SBC_LHIST + i];
        }

        for (i = SBC_RT; i < SBC_RT + SBC_OLAL; i++){
            out[i] = (int16_t)(plc_state->hist[SBC_LHIST + i] * rcos[i - SBC_RT] + in[i] * rcos[SBC_OLAL - 1 - i + SBC_RT]);
        }
    }

    for (; i < SBC_FS; i++){
        out[i] = in[i];
    }
    // Copy the output to the history buffer
    for (i = 0; i < SBC_FS; i++){
        plc_state->hist[SBC_LHIST + i] = out[i];
    }
    // shift the history buffer
    for (i = 0; i < SBC_LHIST; i++){
        plc_state->hist[i] = plc_state->hist[i + SBC_FS];
    }

    plc_state->nbf = 0;
}

#endif  ///(PLC_INCLUDED == TRUE)