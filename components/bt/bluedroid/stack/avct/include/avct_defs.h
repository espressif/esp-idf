/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This contains constants definitions and other information from the AVCTP
 *  specification.  This file is intended for use internal to AVCT only.
 *
 ******************************************************************************/
#ifndef AVCT_DEFS_H
#define AVCT_DEFS_H

/*****************************************************************************
** constants
*****************************************************************************/

/* packet type */
#define AVCT_PKT_TYPE_SINGLE        0       /* single packet */
#define AVCT_PKT_TYPE_START         1       /* start packet */
#define AVCT_PKT_TYPE_CONT          2       /* continue packet */
#define AVCT_PKT_TYPE_END           3       /* end packet */

/* header lengths for different packet types */
#define AVCT_HDR_LEN_SINGLE         3
#define AVCT_HDR_LEN_START          4
#define AVCT_HDR_LEN_CONT           1
#define AVCT_HDR_LEN_END            1

/* invalid cr+ipid value */
#define AVCT_CR_IPID_INVALID        1

/*****************************************************************************
** message parsing and building macros
*****************************************************************************/

#define AVCT_BLD_HDR(p, label, type, cr_ipid) \
    *(p)++ = ((label) << 4) | ((type) << 2) | (cr_ipid);

#define AVCT_PRS_HDR(p, label, type, cr_ipid) \
    label = *(p) >> 4; \
    type = (*(p) >> 2) & 3; \
    cr_ipid = *(p)++ & 3;

#define AVCT_PRS_PKT_TYPE(p, type) \
    type = (*(p) >> 2) & 3;

#endif /* AVCT_DEFS_H */
