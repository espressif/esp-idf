/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

/**
 @file

 Dequantizer for SBC decoder; reconstructs quantized representation of subband samples.

 @ingroup codec_internal
 */

/**
@addtogroup codec_internal
@{
*/

/**
 This function is a fixed-point approximation of a modification of the following
 dequantization operation defined in the spec, as inferred from section 12.6.4:

 @code
 dequant = 2^(scale_factor+1) * ((raw * 2.0 + 1.0) / ((2^bits) - 1) - 1)

 2 <= bits <= 16
 0 <= raw < (2^bits)-1   (the -1 is because quantized values with all 1's are forbidden)

 -65535 < dequant < 65535
 @endcode

 The code below computes the dequantized value divided by a scaling constant
 equal to about 1.38. This constant is chosen to ensure that the entry in the
 dequant_long_scaled table for 16 bits is as accurate as possible, since it has
 the least relative precision available to it due to its small magnitude.

 This routine outputs in Q16.15 format.

 The helper array dequant_long is defined as follows:

 @code
 dequant_long_long[bits] = round(2^31 * 1/((2^bits - 1) / 1.38...)  for 2 <= bits <= 16
 @endcode


 Additionally, the table entries have the following property:

 @code
 dequant_long_scaled[bits] <= 2^31 / ((2^bits - 1))  for 2 <= bits <= 16
 @endcode

 Therefore

 @code
 d = 2 * raw + 1              1 <= d <= 2^bits - 2

 d' = d * dequant_long[bits]

                  d * dequant_long_scaled[bits] <= (2^bits - 2) * (2^31 / (2^bits - 1))
                  d * dequant_long_scaled[bits] <= 2^31 * (2^bits - 2)/(2^bits - 1) < 2^31
 @endcode

 Therefore, d' doesn't overflow a signed 32-bit value.

 @code

 d' =~ 2^31 * (raw * 2.0 + 1.0) / (2^bits - 1) / 1.38...

 result = d' - 2^31/1.38... =~ 2^31 * ((raw * 2.0 + 1.0) / (2^bits - 1) - 1) / 1.38...

 result is therefore a scaled approximation to dequant. It remains only to
 turn 2^31 into 2^(scale_factor+1). Since we're aiming for Q16.15 format,
 this is achieved by shifting right by (15-scale_factor):

  (2^31 * x) >> (15-scale_factor) =~ 2^(31-15+scale_factor) * x = 2^15 * 2^(1+scale_factor) * x
 @endcode

 */

#include "common/bt_target.h"
#include <oi_codec_sbc_private.h>

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)

#ifndef SBC_DEQUANT_LONG_SCALED_OFFSET
#define SBC_DEQUANT_LONG_SCALED_OFFSET 1555931970
#endif

#ifndef SBC_DEQUANT_LONG_UNSCALED_OFFSET
#define SBC_DEQUANT_LONG_UNSCALED_OFFSET 2147483648
#endif

#ifndef SBC_DEQUANT_SCALING_FACTOR
#define SBC_DEQUANT_SCALING_FACTOR 1.38019122262781f
#endif

const OI_UINT32 dequant_long_scaled[17];
const OI_UINT32 dequant_long_unscaled[17];

/** Scales x by y bits to the right, adding a rounding factor.
 */
#ifndef SCALE
#define SCALE(x, y) (((x) + (1 <<((y)-1))) >> (y))
#endif

#ifdef DEBUG_DEQUANTIZATION

#include <math.h>

static INLINE float dequant_float(OI_UINT32 raw, OI_UINT scale_factor, OI_UINT bits)
{
    float result = (1 << (scale_factor + 1)) * ((raw * 2.0f + 1.0f) / ((1 << bits) - 1.0f) - 1.0f);

    result /= SBC_DEQUANT_SCALING_FACTOR;

    /* Unless the encoder screwed up, all correct dequantized values should
     * satisfy this inequality. Non-compliant encoders which generate quantized
     * values with all 1-bits set can, theoretically, trigger this assert. This
     * is unlikely, however, and only an issue in debug mode.
     */
    OI_ASSERT(fabs(result) < 32768 * 1.6);

    return result;
}

#endif


INLINE OI_INT32 OI_SBC_Dequant(OI_UINT32 raw, OI_UINT scale_factor, OI_UINT bits)
{
    OI_UINT32 d;
    OI_INT32 result;

    OI_ASSERT(scale_factor <= 15);
    OI_ASSERT(bits <= 16);

    if (bits <= 1) {
        return 0;
    }

    d = (raw * 2) + 1;
    d *= dequant_long_scaled[bits];
    result = d - SBC_DEQUANT_LONG_SCALED_OFFSET;

#ifdef DEBUG_DEQUANTIZATION
    {
        OI_INT32 integerized_float_result;
        float float_result;

        float_result = dequant_float(raw, scale_factor, bits);
        integerized_float_result = (OI_INT32)floor(0.5f + float_result * (1 << 15));

        /* This detects overflow */
        OI_ASSERT(((result >= 0) && (integerized_float_result >= 0)) ||
                  ((result <= 0) && (integerized_float_result <= 0)));
    }
#endif
    return result >> (15 - scale_factor);
}

/* This version of Dequant does not incorporate the scaling factor of 1.38. It
 * is intended for use with implementations of the filterbank which are
 * hard-coded into a DSP. Output is Q16.4 format, so that after joint stereo
 * processing (which leaves the most significant bit equal to the sign bit if
 * the encoder is conformant) the result will fit a 24 bit fixed point signed
 * value.*/

INLINE OI_INT32 OI_SBC_Dequant_Unscaled(OI_UINT32 raw, OI_UINT scale_factor, OI_UINT bits)
{
    OI_UINT32 d;
    OI_INT32 result;

    OI_ASSERT(scale_factor <= 15);
    OI_ASSERT(bits <= 16);


    if (bits <= 1) {
        return 0;
    }
    if (bits == 16) {
        result = (raw << 16) + raw - 0x7fff7fff;
        return SCALE(result, 24 - scale_factor);
    }


    d = (raw * 2) + 1;
    d *= dequant_long_unscaled[bits];
    result = d - 0x80000000;

    return SCALE(result, 24 - scale_factor);
}

/**
@}
*/

#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
