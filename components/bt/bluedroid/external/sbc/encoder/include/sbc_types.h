/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  Data type declarations.
 *
 ******************************************************************************/

#ifndef SBC_TYPES_H
#define SBC_TYPES_H

#include <stdint.h>

#ifdef BUILDCFG
#include "bt_target.h"
#endif

#include "bt_types.h"

typedef short SINT16;
typedef long SINT32;

#if (SBC_IPAQ_OPT == TRUE)

#if (SBC_FOR_EMBEDDED_LINUX == TRUE)
typedef long long SINT64;
#else
typedef int64_t SINT64;
#endif

#elif (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE) || (SBC_IS_64_MULT_IN_IDCT == TRUE)

#if (SBC_FOR_EMBEDDED_LINUX == TRUE)
typedef long long SINT64;
#else
typedef int64_t SINT64;
#endif

#endif

#define abs32(x) ( (x >= 0) ? x : (-x) )

#endif
