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

/******************************************************************************
 *
 *  This contains constants definitions and other information from the AVDTP
 *  specification.  This file is intended for use internal to AVDT only.
 *
 ******************************************************************************/
#ifndef AVDT_DEFS_H
#define AVDT_DEFS_H
#include "common/bt_target.h"

#if (AVDT_INCLUDED == TRUE)

/*****************************************************************************
** constants
*****************************************************************************/

/* signalling packet type */
#define AVDT_PKT_TYPE_SINGLE        0       /* single packet */
#define AVDT_PKT_TYPE_START         1       /* start packet */
#define AVDT_PKT_TYPE_CONT          2       /* continue packet */
#define AVDT_PKT_TYPE_END           3       /* end packet */

/* signalling message type */
#define AVDT_MSG_TYPE_CMD           0       /* command */
#define AVDT_MSG_TYPE_GRJ           1       /* general reject */
#define AVDT_MSG_TYPE_RSP           2       /* response accept */
#define AVDT_MSG_TYPE_REJ           3       /* response reject */

/* signalling messages */
#define AVDT_SIG_DISCOVER           1       /* discover */
#define AVDT_SIG_GETCAP             2       /* get capabilities */
#define AVDT_SIG_SETCONFIG          3       /* set configuration */
#define AVDT_SIG_GETCONFIG          4       /* get configuration */
#define AVDT_SIG_RECONFIG           5       /* reconfigure */
#define AVDT_SIG_OPEN               6       /* open */
#define AVDT_SIG_START              7       /* start */
#define AVDT_SIG_CLOSE              8       /* close */
#define AVDT_SIG_SUSPEND            9       /* suspend */
#define AVDT_SIG_ABORT              10      /* abort */
#define AVDT_SIG_SECURITY           11      /* security control */
#define AVDT_SIG_GET_ALLCAP         12      /* get all capabilities */
#define AVDT_SIG_DELAY_RPT          13      /* delay report */

/* maximum signal value */
#define AVDT_SIG_MAX                AVDT_SIG_DELAY_RPT

/* used for general reject */
#define AVDT_SIG_NONE               0

/* some maximum and minimum sizes of signalling messages */
#define AVDT_DISCOVER_REQ_MIN       1
#define AVDT_DISCOVER_REQ_MAX       124

/* service category information element field values */
#define AVDT_CAT_TRANS              1       /* Media Transport */
#define AVDT_CAT_REPORT             2       /* Reporting */
#define AVDT_CAT_RECOV              3       /* Recovery */
#define AVDT_CAT_PROTECT            4       /* Content Protection */
#define AVDT_CAT_HDRCMP             5       /* Header Compression */
#define AVDT_CAT_MUX                6       /* Multiplexing */
#define AVDT_CAT_CODEC              7       /* Media Codec */
#define AVDT_CAT_DELAY_RPT          8       /* Delay Reporting */
#define AVDT_CAT_MAX_CUR            AVDT_CAT_DELAY_RPT

/* min/max lengths of service category information elements */
#define AVDT_LEN_TRANS_MIN          0
#define AVDT_LEN_REPORT_MIN         0
#define AVDT_LEN_RECOV_MIN          3
#define AVDT_LEN_PROTECT_MIN        2
#define AVDT_LEN_HDRCMP_MIN         1
#define AVDT_LEN_MUX_MIN            3
#define AVDT_LEN_CODEC_MIN          2
#define AVDT_LEN_DELAY_RPT_MIN      0

#define AVDT_LEN_TRANS_MAX          0
#define AVDT_LEN_REPORT_MAX         0
#define AVDT_LEN_RECOV_MAX          3
#define AVDT_LEN_PROTECT_MAX        255
#define AVDT_LEN_HDRCMP_MAX         1
#define AVDT_LEN_MUX_MAX            7
#define AVDT_LEN_CODEC_MAX          255
#define AVDT_LEN_DELAY_RPT_MAX      0

/* minimum possible size of configuration or capabilities data */
#define AVDT_LEN_CFG_MIN            2

/* minimum and maximum lengths for different message types */
#define AVDT_LEN_SINGLE             1
#define AVDT_LEN_SETCONFIG_MIN      2
#define AVDT_LEN_RECONFIG_MIN       1
#define AVDT_LEN_MULTI_MIN          1
#define AVDT_LEN_SECURITY_MIN       1
#define AVDT_LEN_DELAY_RPT          3

/* header lengths for different packet types */
#define AVDT_LEN_TYPE_SINGLE        2       /* single packet */
#define AVDT_LEN_TYPE_START         3       /* start packet */
#define AVDT_LEN_TYPE_CONT          1       /* continue packet */
#define AVDT_LEN_TYPE_END           1       /* end packet */

/* length of general reject message */
#define AVDT_LEN_GEN_REJ            2

/* recovery service capabilities information elements */
#define AVDT_RECOV_MRWS_MIN         0x01    /* min value for maximum recovery window */
#define AVDT_RECOV_MRWS_MAX         0x18    /* max value for maximum recovery window */
#define AVDT_RECOV_MNMP_MIN         0x01    /* min value for maximum number of media packets */
#define AVDT_RECOV_MNMP_MAX         0x18    /* max value for maximum number of media packets */

/* SEID value range */
#define AVDT_SEID_MIN               0x01
#define AVDT_SEID_MAX               0x3E

/* first byte of media packet header */
#define AVDT_MEDIA_OCTET1           0x80

/* for adaptation layer header */
#define AVDT_ALH_LCODE_MASK     0x03    /* coding of length field */
#define AVDT_ALH_LCODE_NONE     0x00    /* No length field present. Take length from l2cap */
#define AVDT_ALH_LCODE_16BIT    0x01    /* 16bit length field */
#define AVDT_ALH_LCODE_9BITM0   0x02    /* 9 bit length field, MSB = 0, 8 LSBs in 1 octet following */
#define AVDT_ALH_LCODE_9BITM1   0x03    /* 9 bit length field, MSB = 1, 8 LSBs in 1 octet following */

#define AVDT_ALH_FRAG_MASK      0x04    /* set this for continuation packet */

/*****************************************************************************
** message parsing and building macros
*****************************************************************************/

#define AVDT_MSG_PRS_HDR(p, lbl, pkt, msg) \
    lbl = *(p) >> 4; \
    pkt = (*(p) >> 2) & 0x03; \
    msg = *(p)++ & 0x03;

#define AVDT_MSG_PRS_DISC(p, seid, in_use, type, tsep) \
    seid = *(p) >> 2; \
    in_use = (*(p)++ >> 1) & 0x01; \
    type = *(p) >> 4; \
    tsep = (*(p)++ >> 3) & 0x01;

#define AVDT_MSG_PRS_SIG(p, sig) \
    sig = *(p)++ & 0x3F;

#define AVDT_MSG_PRS_SEID(p, seid) \
    seid = *(p)++ >> 2;

#define AVDT_MSG_PRS_PKT_TYPE(p, pkt) \
    pkt = (*(p) >> 2) & 0x03;

#define AVDT_MSG_PRS_OCTET1(p, o_v, o_p, o_x, o_cc) \
    o_v = *(p) >> 6; \
    o_p = (*(p) >> 5) & 0x01; \
    o_x = (*(p) >> 4) & 0x01; \
    o_cc = *(p)++ & 0x0F;

#define AVDT_MSG_PRS_RPT_OCTET1(p, o_v, o_p, o_cc) \
    o_v = *(p) >> 6; \
    o_p = (*(p) >> 5) & 0x01; \
    o_cc = *(p)++ & 0x1F;

#define AVDT_MSG_PRS_M_PT(p, m_pt, marker) \
    marker = *(p) >> 7; \
    m_pt = *(p)++ & 0x7F;

#define AVDT_MSG_BLD_HDR(p, lbl, pkt, msg) \
    *(p)++ = (UINT8) ((lbl) << 4) | ((pkt) << 2) | (msg);

#define AVDT_MSG_BLD_DISC(p, seid, in_use, type, tsep) \
    *(p)++ = (UINT8) (((seid) << 2) | ((in_use) << 1)); \
    *(p)++ = (UINT8) (((type) << 4) | ((tsep) << 3));

#define AVDT_MSG_BLD_SIG(p, sig) \
    *(p)++ = (UINT8) (sig);

#define AVDT_MSG_BLD_SEID(p, seid) \
    *(p)++ = (UINT8) ((seid) << 2);

#define AVDT_MSG_BLD_ERR(p, err) \
    *(p)++ = (UINT8) (err);

#define AVDT_MSG_BLD_PARAM(p, param) \
    *(p)++ = (UINT8) (param);

#define AVDT_MSG_BLD_NOSP(p, nosp) \
    *(p)++ = (UINT8) (nosp);

#endif  ///AVRC_INCLUDED == TRUE

#endif /* AVDT_DEFS_H */
