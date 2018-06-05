
// xos_regaccess.h - Access routines for various processor special registers.

// Copyright (c) 2003-2015 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef __REGACCESS_H__
#define __REGACCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "xos_types.h"

#include <xtensa/config/core.h>

#if defined (__XCC__)
#if XCHAL_HAVE_CCOUNT
#include <xtensa/tie/xt_timer.h>
#endif
#endif


//-----------------------------------------------------------------------------
// Read CCOUNT register.
//-----------------------------------------------------------------------------
static __inline__ uint32_t xos_get_ccount(void)
{
#if XCHAL_HAVE_CCOUNT

#if defined (__XCC__)
  return XT_RSR_CCOUNT();
#else
  uint32_t ccount;

  __asm__ __volatile__ ( "rsr     %0, ccount" : "=a" (ccount) );
  return ccount;
#endif

#else

  return 0;

#endif
}


//-----------------------------------------------------------------------------
// Read CCOMPARE0
//-----------------------------------------------------------------------------
static __inline__ uint32_t xos_get_ccompare0(void)
{
#if XCHAL_HAVE_CCOUNT

#if defined (__XCC__)
  return XT_RSR_CCOMPARE0();
#else
  uint32_t ccompare0;

  __asm__ __volatile__ ( "rsr     %0, ccompare0" : "=a" (ccompare0));
  return ccompare0;
#endif

#else

  return 0;

#endif
}


//-----------------------------------------------------------------------------
// Read CCOMPARE1
//-----------------------------------------------------------------------------
#if (XCHAL_NUM_TIMERS > 1)
static __inline__ uint32_t xos_get_ccompare1(void)
{
#if defined (__XCC__)
  return XT_RSR_CCOMPARE1();
#else
  uint32_t ccompare1;

  __asm__ __volatile__ ( "rsr     %0, ccompare1" : "=a" (ccompare1));
  return ccompare1;
#endif
}
#endif


//-----------------------------------------------------------------------------
// Read CCOMPARE2
//-----------------------------------------------------------------------------
#if (XCHAL_NUM_TIMERS > 2)
static __inline__ uint32_t xos_get_ccompare2(void)
{
#if defined (__XCC__)
  return XT_RSR_CCOMPARE2();
#else
  uint32_t ccompare2;

  __asm__ __volatile__ ( "rsr     %0, ccompare2" : "=a" (ccompare2));
  return ccompare2;
#endif
}
#endif


//-----------------------------------------------------------------------------
// Write CCOMPARE0
//-----------------------------------------------------------------------------
static __inline__ void xos_set_ccompare0(uint32_t val)
{
#if XCHAL_HAVE_CCOUNT

#if defined (__XCC__)
  XT_WSR_CCOMPARE0(val);
  XT_ISYNC();
#else
  __asm__ __volatile__ (
    "wsr     %0, ccompare0\n"
    "isync"
    :
    : "a" (val)
  );
#endif

#else

    // Empty

#endif
}


//-----------------------------------------------------------------------------
// Write CCOMPARE1
//-----------------------------------------------------------------------------
#if (XCHAL_NUM_TIMERS > 1)
static __inline__ void xos_set_ccompare1(uint32_t val)
{
#if defined (__XCC__)
  XT_WSR_CCOMPARE1(val);
  XT_ISYNC();
#else
  __asm__ __volatile__ (
    "wsr     %0, ccompare1\n"
    "isync"
    :
    : "a" (val)
  );
#endif
}
#endif


//-----------------------------------------------------------------------------
// Write CCOMPARE2
//-----------------------------------------------------------------------------
#if (XCHAL_NUM_TIMERS > 2)
static __inline__ void xos_set_ccompare2(uint32_t val)
{
#if defined (__XCC__)
  XT_WSR_CCOMPARE2(val);
  XT_ISYNC();
#else
  __asm__ __volatile__ (
    "wsr     %0, ccompare2\n"
    "isync"
    :
    : "a" (val)
  );
#endif
}
#endif


#ifdef __cplusplus
}
#endif

#endif	// __REGACCESS_H__

