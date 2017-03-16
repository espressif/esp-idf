/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
#ifndef DYN_MEM_H
#define DYN_MEM_H

#include "sdkconfig.h"
#if CONFIG_CLASSIC_BT_ENABLED

#define SDP_DYNAMIC_MEMORY         FALSE
#define RFC_DYNAMIC_MEMORY         FALSE
#define TCS_DYNAMIC_MEMORY         FALSE
#define BNEP_DYNAMIC_MEMORY        FALSE
#define AVDT_DYNAMIC_MEMORY        FALSE
#define AVCT_DYNAMIC_MEMORY        FALSE
#define MCA_DYNAMIC_MEMORY         FALSE
#define A2D_DYNAMIC_MEMORY         FALSE
#define VDP_DYNAMIC_MEMORY         FALSE
#define AVRC_DYNAMIC_MEMORY        FALSE
#define BIP_DYNAMIC_MEMORY         FALSE
#define BPP_DYNAMIC_MEMORY         FALSE
#define CTP_DYNAMIC_MEMORY         FALSE
#define FTP_DYNAMIC_MEMORY         FALSE
#define HCRP_DYNAMIC_MEMORY        FALSE
#define HFP_DYNAMIC_MEMORY         FALSE
#define HID_DYNAMIC_MEMORY         FALSE
#define HSP2_DYNAMIC_MEMORY        FALSE
#define ICP_DYNAMIC_MEMORY         FALSE
#define OPP_DYNAMIC_MEMORY         FALSE
#define PAN_DYNAMIC_MEMORY         FALSE
#define SPP_DYNAMIC_MEMORY         FALSE
#define SLIP_DYNAMIC_MEMORY        FALSE
#define LLCP_DYNAMIC_MEMORY        FALSE

#else  /* #if CONFIG_CLASSIC_BT_ENABLED */

#define SDP_DYNAMIC_MEMORY         TRUE
#define RFC_DYNAMIC_MEMORY         TRUE
#define TCS_DYNAMIC_MEMORY         TRUE
#define BNEP_DYNAMIC_MEMORY        TRUE
#define AVDT_DYNAMIC_MEMORY        TRUE
#define AVCT_DYNAMIC_MEMORY        TRUE
#define MCA_DYNAMIC_MEMORY         TRUE
#define A2D_DYNAMIC_MEMORY         TRUE
#define VDP_DYNAMIC_MEMORY         TRUE
#define AVRC_DYNAMIC_MEMORY        TRUE
#define BIP_DYNAMIC_MEMORY         TRUE
#define BPP_DYNAMIC_MEMORY         TRUE
#define CTP_DYNAMIC_MEMORY         TRUE
#define FTP_DYNAMIC_MEMORY         TRUE
#define HCRP_DYNAMIC_MEMORY        TRUE
#define HFP_DYNAMIC_MEMORY         TRUE
#define HID_DYNAMIC_MEMORY         TRUE
#define HSP2_DYNAMIC_MEMORY        TRUE
#define ICP_DYNAMIC_MEMORY         TRUE
#define OPP_DYNAMIC_MEMORY         TRUE
#define PAN_DYNAMIC_MEMORY         TRUE
#define SPP_DYNAMIC_MEMORY         TRUE
#define SLIP_DYNAMIC_MEMORY        TRUE
#define LLCP_DYNAMIC_MEMORY        TRUE

#endif /* #if CONFIG_CLASSIC_BT_ENABLED */
/****************************************************************************
** Define memory usage for each CORE component (if not defined in bdroid_buildcfg.h)
**  The default for each component is to use static memory allocations.
*/
#ifndef BTU_DYNAMIC_MEMORY
#define BTU_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BTM_DYNAMIC_MEMORY
#define BTM_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SDP_DYNAMIC_MEMORY
#define SDP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef L2C_DYNAMIC_MEMORY
#define L2C_DYNAMIC_MEMORY  FALSE
#endif

#ifndef RFC_DYNAMIC_MEMORY
#define RFC_DYNAMIC_MEMORY  FALSE
#endif

#ifndef TCS_DYNAMIC_MEMORY
#define TCS_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BNEP_DYNAMIC_MEMORY
#define BNEP_DYNAMIC_MEMORY FALSE
#endif

#ifndef AVDT_DYNAMIC_MEMORY
#define AVDT_DYNAMIC_MEMORY FALSE
#endif

#ifndef AVCT_DYNAMIC_MEMORY
#define AVCT_DYNAMIC_MEMORY FALSE
#endif

#ifndef MCA_DYNAMIC_MEMORY
#define MCA_DYNAMIC_MEMORY FALSE
#endif

#ifndef GATT_DYNAMIC_MEMORY
#define GATT_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SMP_DYNAMIC_MEMORY
#define SMP_DYNAMIC_MEMORY  FALSE
#endif

/****************************************************************************
** Define memory usage for each PROFILE component (if not defined in bdroid_buildcfg.h)
**  The default for each component is to use static memory allocations.
*/
#ifndef A2D_DYNAMIC_MEMORY
#define A2D_DYNAMIC_MEMORY  FALSE
#endif

#ifndef VDP_DYNAMIC_MEMORY
#define VDP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef AVRC_DYNAMIC_MEMORY
#define AVRC_DYNAMIC_MEMORY FALSE
#endif

#ifndef BIP_DYNAMIC_MEMORY
#define BIP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BPP_DYNAMIC_MEMORY
#define BPP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef CTP_DYNAMIC_MEMORY
#define CTP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef FTP_DYNAMIC_MEMORY
#define FTP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef HCRP_DYNAMIC_MEMORY
#define HCRP_DYNAMIC_MEMORY FALSE
#endif

#ifndef HFP_DYNAMIC_MEMORY
#define HFP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef HID_DYNAMIC_MEMORY
#define HID_DYNAMIC_MEMORY  FALSE
#endif

#ifndef HSP2_DYNAMIC_MEMORY
#define HSP2_DYNAMIC_MEMORY FALSE
#endif

#ifndef ICP_DYNAMIC_MEMORY
#define ICP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef OPP_DYNAMIC_MEMORY
#define OPP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef PAN_DYNAMIC_MEMORY
#define PAN_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SPP_DYNAMIC_MEMORY
#define SPP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SLIP_DYNAMIC_MEMORY
#define SLIP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef LLCP_DYNAMIC_MEMORY
#define LLCP_DYNAMIC_MEMORY  FALSE
#endif

/****************************************************************************
** Define memory usage for BTA (if not defined in bdroid_buildcfg.h)
**  The default for each component is to use static memory allocations.
*/
#ifndef BTA_DYNAMIC_MEMORY
#define BTA_DYNAMIC_MEMORY FALSE
#endif

#endif  /* #ifdef DYN_MEM_H */

