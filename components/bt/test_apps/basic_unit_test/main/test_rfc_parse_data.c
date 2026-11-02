/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Length checks in rfc_parse_data(). A frame shorter than 3 bytes must be
 * rejected before any header byte is parsed. Return code alone does not show
 * that: the old code also returned RFC_EVENT_BAD_FRAME after reading past len.
 */

#include <string.h>

#include "unity.h"
#include "common/bt_target.h"

#if (defined(RFCOMM_INCLUDED) && RFCOMM_INCLUDED == TRUE)

#include "osi/allocator.h"
#include "rfc_int.h"
#include "stack/rfcdefs.h"

#define RFC_PARSE_TAIL_LEN 4

static BT_HDR *make_frame(const uint8_t *payload, uint16_t payload_len)
{
    BT_HDR *pkt = (BT_HDR *)osi_calloc(sizeof(BT_HDR) + payload_len + RFC_PARSE_TAIL_LEN);

    TEST_ASSERT_NOT_NULL(pkt);
    pkt->len = payload_len;
    pkt->offset = 0;
    if (payload_len != 0 && payload != NULL) {
        memcpy(pkt->data, payload, payload_len);
    }
    /* Bytes past len stay inside this allocation. The broken parser reads them
     * and writes MX_FRAME; the test then fails without a guard page. */
    memset(pkt->data + payload_len, 0xFF, RFC_PARSE_TAIL_LEN);
    return pkt;
}

static void assert_frame_and_buf_unchanged(const MX_FRAME *before, const MX_FRAME *after,
                                           const BT_HDR *pkt, uint16_t len, uint16_t offset)
{
    TEST_ASSERT_EQUAL_MEMORY(before, after, sizeof(MX_FRAME));
    TEST_ASSERT_EQUAL_UINT16(len, pkt->len);
    TEST_ASSERT_EQUAL_UINT16(offset, pkt->offset);
}

TEST_CASE("rfc_parse_data rejects frames shorter than 3 before parsing", "[rfcomm]")
{
    const uint8_t payload[2] = {0xFF, 0xFF};
    uint16_t len;

    for (len = 0; len < RFCOMM_CTRL_FRAME_LEN; len++) {
        BT_HDR *pkt = make_frame(payload, len);
        tRFC_MCB mcb;
        MX_FRAME frame;
        MX_FRAME untouched;

        memset(&mcb, 0, sizeof(mcb));
        memset(&frame, 0, sizeof(frame));
        untouched = frame;

        TEST_ASSERT_EQUAL_UINT8(RFC_EVENT_BAD_FRAME, rfc_parse_data(&mcb, &frame, pkt));
        assert_frame_and_buf_unchanged(&untouched, &frame, pkt, len, 0);
        osi_free(pkt);
    }
}

TEST_CASE("rfc_parse_data rejects a 3-byte frame before consuming it", "[rfcomm]")
{
    /* EA=1, DLCI=0, UIH, length EA=1 and info length 0. FCS is missing. */
    const uint8_t payload[3] = {0x01, RFCOMM_UIH, 0x01};
    BT_HDR *pkt = make_frame(payload, sizeof(payload));
    tRFC_MCB mcb;
    MX_FRAME frame;

    memset(&mcb, 0, sizeof(mcb));
    memset(&frame, 0, sizeof(frame));

    TEST_ASSERT_EQUAL_UINT8(RFC_EVENT_BAD_FRAME, rfc_parse_data(&mcb, &frame, pkt));
    TEST_ASSERT_EQUAL_UINT16(sizeof(payload), pkt->len);
    TEST_ASSERT_EQUAL_UINT16(0, pkt->offset);
    TEST_ASSERT_EQUAL_UINT8(0, frame.dlci);
    TEST_ASSERT_EQUAL_UINT8(RFCOMM_UIH, frame.type);
    osi_free(pkt);
}

TEST_CASE("rfc_parse_data passes a 4-byte frame through the length checks", "[rfcomm]")
{
    /* EA=1, DLCI=0, type 0, length EA=1 and info length 0, plus FCS.
     * Both length checks pass. Type 0 then falls out of the frame switch. */
    const uint8_t payload[4] = {0x01, 0x00, 0x01, 0x00};
    BT_HDR *pkt = make_frame(payload, sizeof(payload));
    tRFC_MCB mcb;
    MX_FRAME frame;

    memset(&mcb, 0, sizeof(mcb));
    memset(&frame, 0, sizeof(frame));

    TEST_ASSERT_EQUAL_UINT8(RFC_EVENT_BAD_FRAME, rfc_parse_data(&mcb, &frame, pkt));
    TEST_ASSERT_EQUAL_UINT16(0, pkt->len);
    TEST_ASSERT_EQUAL_UINT16(3, pkt->offset);
    osi_free(pkt);
}

#endif /* RFCOMM_INCLUDED == TRUE */
