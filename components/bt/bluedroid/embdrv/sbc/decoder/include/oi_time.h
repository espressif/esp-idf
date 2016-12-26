/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
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
#ifndef _OI_TIME_H
#define _OI_TIME_H
/** @file
 *
 * This file provides time type definitions and interfaces to time-related functions.
 *
 * The stack maintains a 64-bit real-time millisecond clock. The choice of
 * milliseconds is for convenience, not accuracy.
 *
 * Timeouts are specified as tenths of seconds in a 32-bit value. Timeout values
 * specified by the Bluetooth specification are usually muliple seconds, so
 * accuracy to a tenth of a second is more than adequate.
 *
 * This file also contains macros to convert between seconds and the Link
 * Manager's 1.28-second units.
 *
 */

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

#include "oi_stddefs.h"


/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif



/**
 * Within the core stack timeouts are specified in intervals of tenths of seconds
 */

typedef OI_UINT16 OI_INTERVAL;
#define OI_INTERVALS_PER_SECOND     10
#define MSECS_PER_OI_INTERVAL       (1000 / OI_INTERVALS_PER_SECOND)

/** maximum interval (54 min 36.7 sec) */
#define OI_MAX_INTERVAL   0x7fff


/**
 * Macro to convert seconds to OI_INTERVAL time units
 */

#define OI_SECONDS(n)    ((OI_INTERVAL) ((n) * OI_INTERVALS_PER_SECOND))

/**
 * Macro to convert milliseconds to OI_INTERVAL time units (Rounded Up)
 */

#define OI_MSECONDS(n)   ((OI_INTERVAL) ((n + MSECS_PER_OI_INTERVAL - 1) / MSECS_PER_OI_INTERVAL))

/**
 * Macro to convert minutes to OI_INTERVAL time units
 */

#define OI_MINUTES(n)    ((OI_INTERVAL) ((n) * OI_SECONDS(60)))

/** Convert an OI_INTERVAL to milliseconds. */
#define OI_INTERVAL_TO_MILLISECONDS(i) ((i) * MSECS_PER_OI_INTERVAL)

/**
 * The stack depends on relative not absolute time. Any mapping between the
 * stack's real-time clock and absolute time and date is implementation-dependent.
 */

typedef struct {
    OI_INT32 seconds;
    OI_INT16 mseconds;
} OI_TIME;

/**
 * Convert an OI_TIME to milliseconds.
 *
 * @param t  the time to convert
 *
 * @return the time in milliseconds
 */
OI_UINT32 OI_Time_ToMS(OI_TIME *t);


/**
 * This function compares two time values.
 *
 * @param T1 first time to compare.
 *
 * @param T2 second time to compare.
 *
 * @return
 @verbatim
     -1 if t1 < t2
      0 if t1 = t2
     +1 if t1 > t2
 @endverbatim
 */

OI_INT16 OI_Time_Compare(OI_TIME *T1,
                         OI_TIME *T2);


/**
 * This function returns the interval between two times to a granularity of 0.1 seconds.
 *
 * @param Sooner a time value more recent that Later
 *
 * @param Later a time value later than Sooner
 *
 * @note The result is an OI_INTERVAL value so this function only works for time intervals
 * that are less than about 71 minutes.
 *
 * @return the time interval between the two times = (Later - Sooner)
 */

OI_INTERVAL OI_Time_Interval(OI_TIME *Sooner,
                             OI_TIME *Later);



/**
 * This function returns the interval between two times to a granularity of milliseconds.
 *
 * @param Sooner a time value more recent that Later
 *
 * @param Later a time value later than Sooner
 *
 * @note The result is an OI_UINT32 value so this function only works for time intervals
 * that are less than about 50 days.
 *
 * @return the time interval between the two times = (Later - Sooner)
 */

OI_UINT32 OI_Time_IntervalMsecs(OI_TIME *Sooner,
                                OI_TIME *Later);



/**
 * This function answers the question, Have we reached or gone past the target time?
 *
 * @param pTargetTime   target time
 *
 * @return  TRUE means time now is at or past target time
 *          FALSE means target time is still some time in the future
 */

OI_BOOL  OI_Time_NowReachedTime(OI_TIME *pTargetTime);

/**
 *  Convert seconds to the Link Manager 1.28-second units
 *  Approximate by using 1.25 conversion factor.
 */

#define OI_SECONDS_TO_LM_TIME_UNITS(lmUnits) ((lmUnits)<4?(lmUnits):(lmUnits)-((lmUnits)>>2))


/**
 *  Convert Link Manager 1.28-second units to seconds.
 *  Approximate by using 1.25 conversion factor.
 */

#define OI_LM_TIME_UNITS_TO_SECONDS(lmUnits) ((lmUnits) + ((lmUnits)>>2))

#ifdef __cplusplus
}
#endif

/**@}*/

/* Include for OI_Time_Now() prototype
 * Must be included at end to obtain OI_TIME typedef
 */
#include "oi_osinterface.h"

/*****************************************************************************/
#endif /* _OI_TIME_H */

