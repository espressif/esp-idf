/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Unit tests for inbound ACL reassembly C2H credit paths
 * (hci/packet_fragmenter.c).
 *
 * START credits stay with the partial until reassembly completes or
 * flush_reassembly; continuation / short / drop paths return credit immediately.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "unity.h"
#include "unity_test_runner.h"

#include "common/bt_target.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)

#include "hci/hci_internals.h"
#include "hci/hci_layer.h"
#include "hci/packet_fragmenter.h"
#include "osi/allocator.h"
#include "stack/bt_types.h"

#define TEST_HANDLE           0x0042
#define START_FLAG            0x2000
#define CONT_FLAG             0x1000

static const packet_fragmenter_t *s_frag;
static uint32_t s_credit_total;
static uint16_t s_credit_last_handle;
static uint16_t s_credit_last_num;
static uint32_t s_reassembled_count;
static BT_HDR *s_reassembled_pkt;

static void on_fragmented(BT_HDR *packet, bool send_transmit_finished)
{
    (void)send_transmit_finished;
    osi_free(packet);
}

static void on_reassembled(BT_HDR *packet)
{
    s_reassembled_count++;
    s_reassembled_pkt = packet;
}

static void on_transmit_finished(BT_HDR *packet, bool all_fragments_sent)
{
    (void)all_fragments_sent;
    osi_free(packet);
}

static void on_c2h_credit(uint16_t handle, uint16_t num)
{
    s_credit_total += num;
    s_credit_last_handle = handle;
    s_credit_last_num = num;
}

static const packet_fragmenter_callbacks_t s_cbs = {
    .fragmented = on_fragmented,
    .reassembled = on_reassembled,
    .transmit_finished = on_transmit_finished,
    .c2h_credit = on_c2h_credit,
};

static BT_HDR *make_acl(uint16_t handle_with_flags, const uint8_t *payload, uint16_t payload_len)
{
    BT_HDR *pkt = (BT_HDR *)osi_calloc(sizeof(BT_HDR) + HCI_ACL_PREAMBLE_SIZE + payload_len);
    uint8_t *p;

    TEST_ASSERT_NOT_NULL(pkt);
    pkt->event = MSG_HC_TO_STACK_HCI_ACL;
    pkt->offset = 0;
    pkt->len = HCI_ACL_PREAMBLE_SIZE + payload_len;
    p = pkt->data;
    UINT16_TO_STREAM(p, handle_with_flags);
    UINT16_TO_STREAM(p, payload_len);
    if (payload_len != 0 && payload != NULL) {
        memcpy(p, payload, payload_len);
    }
    return pkt;
}

static void frag_begin(void)
{
    s_credit_total = 0;
    s_credit_last_handle = 0;
    s_credit_last_num = 0;
    s_reassembled_count = 0;
    s_reassembled_pkt = NULL;

    s_frag = packet_fragmenter_get_test_interface();
    TEST_ASSERT_NOT_NULL(s_frag);
    s_frag->init(&s_cbs);
}

static void frag_end(void)
{
    if (s_reassembled_pkt != NULL) {
        osi_free(s_reassembled_pkt);
        s_reassembled_pkt = NULL;
    }
    if (s_frag != NULL) {
        s_frag->cleanup();
        s_frag = NULL;
    }
}

TEST_CASE("fragmenter continuation returns credit immediately", "[packet_fragmenter]")
{
    /* L2CAP info length 20 → full ACL = 4 + 4 + 20 = 28; first START carries 8. */
    uint8_t start_payload[8] = {20, 0, 0x40, 0x00, 1, 2, 3, 4};
    uint8_t cont_payload[16];
    BT_HDR *start;
    BT_HDR *cont;

    memset(cont_payload, 0xAB, sizeof(cont_payload));
    frag_begin();

    start = make_acl(TEST_HANDLE | START_FLAG, start_payload, sizeof(start_payload));
    s_frag->reassemble_and_dispatch(start);
    TEST_ASSERT_EQUAL_UINT32(0, s_credit_total);
    TEST_ASSERT_EQUAL_UINT32(0, s_reassembled_count);

    cont = make_acl(TEST_HANDLE | CONT_FLAG, cont_payload, sizeof(cont_payload));
    s_frag->reassemble_and_dispatch(cont);
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);
    TEST_ASSERT_EQUAL_UINT16(TEST_HANDLE, s_credit_last_handle);
    TEST_ASSERT_EQUAL_UINT32(1, s_reassembled_count);
    /* START credit is owned by the reassembled buffer (L2CAP returns it). */
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);

    frag_end();
}

TEST_CASE("fragmenter flush_reassembly returns held START credit", "[packet_fragmenter]")
{
    uint8_t start_payload[8] = {20, 0, 0x40, 0x00, 1, 2, 3, 4};
    BT_HDR *start;

    frag_begin();

    start = make_acl(TEST_HANDLE | START_FLAG, start_payload, sizeof(start_payload));
    s_frag->reassemble_and_dispatch(start);
    TEST_ASSERT_EQUAL_UINT32(0, s_credit_total);

    s_frag->flush_reassembly(TEST_HANDLE);
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);
    TEST_ASSERT_EQUAL_UINT16(TEST_HANDLE, s_credit_last_handle);
    TEST_ASSERT_EQUAL_UINT32(0, s_reassembled_count);

    /* Second flush is a no-op. */
    s_frag->flush_reassembly(TEST_HANDLE);
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);

    frag_end();
}

TEST_CASE("fragmenter short ACL with handle returns credit", "[packet_fragmenter]")
{
    BT_HDR *pkt = (BT_HDR *)osi_calloc(sizeof(BT_HDR) + 2);
    uint8_t *p;

    TEST_ASSERT_NOT_NULL(pkt);
    frag_begin();

    pkt->event = MSG_HC_TO_STACK_HCI_ACL;
    pkt->offset = 0;
    pkt->len = 2;
    p = pkt->data;
    UINT16_TO_STREAM(p, TEST_HANDLE | START_FLAG);

    s_frag->reassemble_and_dispatch(pkt);
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);
    TEST_ASSERT_EQUAL_UINT16(TEST_HANDLE, s_credit_last_handle);

    frag_end();
}

TEST_CASE("fragmenter short START without L2CAP header returns credit", "[packet_fragmenter]")
{
    /* HCI preamble only — no L2CAP length field. */
    BT_HDR *pkt = make_acl(TEST_HANDLE | START_FLAG, NULL, 0);

    frag_begin();
    s_frag->reassemble_and_dispatch(pkt);
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);
    TEST_ASSERT_EQUAL_UINT16(TEST_HANDLE, s_credit_last_handle);
    TEST_ASSERT_EQUAL_UINT32(0, s_reassembled_count);

    frag_end();
}

TEST_CASE("fragmenter unknown continuation returns credit", "[packet_fragmenter]")
{
    uint8_t cont_payload[4] = {1, 2, 3, 4};
    BT_HDR *cont = make_acl(TEST_HANDLE | CONT_FLAG, cont_payload, sizeof(cont_payload));

    frag_begin();
    s_frag->reassemble_and_dispatch(cont);
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);
    TEST_ASSERT_EQUAL_UINT16(TEST_HANDLE, s_credit_last_handle);

    frag_end();
}

TEST_CASE("fragmenter new START drops old partial and returns its credit", "[packet_fragmenter]")
{
    uint8_t start_payload[8] = {20, 0, 0x40, 0x00, 1, 2, 3, 4};
    BT_HDR *first;
    BT_HDR *second;

    frag_begin();

    first = make_acl(TEST_HANDLE | START_FLAG, start_payload, sizeof(start_payload));
    s_frag->reassemble_and_dispatch(first);
    TEST_ASSERT_EQUAL_UINT32(0, s_credit_total);

    second = make_acl(TEST_HANDLE | START_FLAG, start_payload, sizeof(start_payload));
    s_frag->reassemble_and_dispatch(second);
    /* Old partial START credit returned; new START still held. */
    TEST_ASSERT_EQUAL_UINT32(1, s_credit_total);

    s_frag->flush_reassembly(TEST_HANDLE);
    TEST_ASSERT_EQUAL_UINT32(2, s_credit_total);

    frag_end();
}

#endif /* C2H_FLOW_CONTROL_INCLUDED == TRUE */
