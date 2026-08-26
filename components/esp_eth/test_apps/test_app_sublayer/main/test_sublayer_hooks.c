/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Target tests for Ethernet sublayer TX / RX / post-TX hooks.
 *
 * PHY loopback mode and L2TAP read/write are used so the tests are
 * self-contained.  Both untagged and tagged (802.1Q) netifs are
 * exercised in the same cases where applicable.
 */

#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_eth_spec.h"
#include "esp_log.h"
#include "esp_vfs_l2tap.h"
#include "unity.h"
#include "ethernet_init.h"
#include "lwip/prot/ethernet.h"
#include "arpa/inet.h"
#include "test_sublayer_common.h"

static const char *TAG = "sublayer_hooks_test";

#define L2TAP_SEND_DELAY_MS    1000
#define L2TAP_IO_TIMEOUT_MS    5000

#define IFKEY_UNTAGGED "ETH_DEF"
#define IFKEY_VLAN     "ETH_VLAN"
#define TEST_VLAN_ID   100

#define ETH_TYPE_HOOK_UNTAGGED 0x88B5
#define ETH_TYPE_HOOK_VLAN     0x88B6
#define ETH_TYPE_HOOK_HEAD     0x88B7  /* Ethertype of the TX hook inserted leading segment */

#define TX_PREFIX_HEAD_SEG_LEN (ETH_HEADER_LEN + TX_PREFIX_LEN)

#define TX_PREFIX_MAGIC_0 0xDE
#define TX_PREFIX_MAGIC_1 0xAD
#define TX_PREFIX_MAGIC_2 0xBE
#define TX_PREFIX_MAGIC_3 0xEF
#define TX_PREFIX_LEN     4

#define FRAME_MARKER_TX 0xA5
#define FRAME_MARKER_RX 0x5A

/*
 * Use large frames so a missed free in a sublayer/hook path exceeds the
 * non-zero leak threshold (see TEST_MEMORY_LEAK_THRESHOLD in test_main.c;
 * some lwIP resources are lazy-allocated and not under our control).
 */
#define HOOK_TEST_FRAME_LEN  1000
#define HOOK_TEST_RX_BUF_LEN (HOOK_TEST_FRAME_LEN + TX_PREFIX_LEN)

typedef enum {
    HOOK_TX_OP_NONE = 0,
    HOOK_TX_OP_PREFIX,
    HOOK_TX_OP_PREFIX_HEAD,
    HOOK_TX_OP_MERGE,
    HOOK_TX_OP_SKIP,
    HOOK_TX_OP_ABORT,
    HOOK_TX_OP_POST_TX_FREE,
} hook_tx_op_t;

typedef enum {
    HOOK_RX_OP_NONE = 0,
    HOOK_RX_OP_MODIFY,
    HOOK_RX_OP_DROP,
    HOOK_RX_OP_ABORT,
} hook_rx_op_t;

typedef enum {
    HOOK_DIR_NONE = 0,
    HOOK_DIR_TX,
    HOOK_DIR_RX,
} hook_dir_t;

typedef struct {
    hook_tx_op_t op;
    uint32_t hook_calls;
    uint32_t post_hook_calls;
    uint32_t rx_strip_calls; /*!< PREFIX_HEAD companion strips on loopback */
    void *freed_ptr;
} hooks_tx_ctx_t;

typedef struct {
    hook_rx_op_t op;
    uint32_t hook_calls;
} hooks_rx_ctx_t;

typedef struct {
    hook_dir_t dir;
    union {
        hooks_tx_ctx_t tx;
        hooks_rx_ctx_t rx;
    };
} hooks_ctx_t;

typedef struct {
    struct eth_hdr header;
    uint8_t marker;
    uint8_t pad[HOOK_TEST_FRAME_LEN - ETH_HEADER_LEN - 1];
} __attribute__((packed)) hook_test_frame_t;

_Static_assert(sizeof(hook_test_frame_t) == HOOK_TEST_FRAME_LEN,
               "hook_test_frame_t must be HOOK_TEST_FRAME_LEN bytes");

typedef struct {
    sublayer_test_ctx_t base;
    esp_eth_sublayer_vlan_handle_t vlan_untagged;
    esp_eth_sublayer_vlan_handle_t vlan_tagged;
    hooks_ctx_t *hook_ctx;
} hooks_test_ctx_t;

static hooks_ctx_t s_hooks_ctx;

/* ------------------------------------------------------------------ */
/*  Hook implementations                                              */
/* ------------------------------------------------------------------ */

/**
 * TX hook used by the sublayer hook tests.  The active scenario is selected
 * via hooks_ctx_t::tx.op when dir == HOOK_DIR_TX.
 *
 * Scenarios exercised (see esp_eth_sublayer.h TX hook contract):
 *  - HOOK_TX_OP_PREFIX:    Modify buf pointers/len and increase buf_count —
 *                          insert a 4-byte prefix before payload; on untagged
 *                          frames split one descriptor into header + prefixed
 *                          payload (bufs[0]/bufs[1]); on VLAN-tagged frames
 *                          the sublayer already provides two descriptors.
 *  - HOOK_TX_OP_PREFIX_HEAD: Insert a new custom leading bufs[0] (broadcast DA,
 *                          device SA, ETH_TYPE_HOOK_HEAD, magic payload), shift
 *                          original descriptors down and increase buf_count.
 *                          The RX hook strips that segment on loopback by
 *                          recognizing ETH_TYPE_HOOK_HEAD.
 *  - HOOK_TX_OP_MERGE:     Merge scattered segments into one contiguous bufs[0]
 *                          (synthetic split for untagged, native VLAN split
 *                          for tagged); buf_count reduced to 1.
 *  - HOOK_TX_OP_SKIP:      Set buf_count to 0 — driver transmit and post_tx_hook
 *                          are skipped; tx_bufs remains caller-owned.
 *  - HOOK_TX_OP_ABORT:     Return non-ESP_OK — transmit aborted, post_tx_hook
 *                          not called.
 *  - HOOK_TX_OP_POST_TX_FREE: Replace bufs[0] with a malloc'd copy so that
 *                          post_tx_hook can free hook-owned memory after TX.
 */
static void test_fill_prefix_magics(uint8_t *dst)
{
    dst[0] = TX_PREFIX_MAGIC_0;
    dst[1] = TX_PREFIX_MAGIC_1;
    dst[2] = TX_PREFIX_MAGIC_2;
    dst[3] = TX_PREFIX_MAGIC_3;
}

static bool test_prefix_magics_match(const uint8_t *src)
{
    uint8_t expected[TX_PREFIX_LEN];

    test_fill_prefix_magics(expected);
    return memcmp(src, expected, TX_PREFIX_LEN) == 0;
}

static esp_err_t test_tx_hook(esp_eth_handle_t eth, esp_eth_sublayer_tx_bufs_t *tx_bufs, void *ctx)
{
    hooks_ctx_t *hctx = (hooks_ctx_t *)ctx;

    if (hctx->dir != HOOK_DIR_TX) {
        return ESP_OK;
    }
    hctx->tx.hook_calls++;

    esp_eth_buf_desc_t *bufs = tx_bufs->bufs;
    size_t *buf_count = tx_bufs->buf_count;
    size_t buf_capacity = tx_bufs->buf_capacity;

    switch (hctx->tx.op) {
    case HOOK_TX_OP_PREFIX: {
        /* Change buf pointers/len and bump buf_count (untagged) or edit bufs[1] (tagged). */
        if (*buf_count < 1 || buf_capacity < 2) {
            return ESP_ERR_INVALID_SIZE;
        }
        if (*buf_count >= 2 && bufs[0].len == ETH_HEADER_LEN + ETH_VLAN_TAG_LEN) {
            /* Tagged VLAN path: header already in bufs[0], prefix payload in bufs[1]. */
            size_t payload_len = bufs[1].len;
            uint8_t *prefixed = malloc(TX_PREFIX_LEN + payload_len);
            if (prefixed == NULL) {
                return ESP_ERR_NO_MEM;
            }
            test_fill_prefix_magics(prefixed);
            memcpy(prefixed + TX_PREFIX_LEN, bufs[1].buf, payload_len);
            bufs[1] = (esp_eth_buf_desc_t){ .buf = prefixed, .len = TX_PREFIX_LEN + payload_len };
            return ESP_OK;
        }
        /* Untagged path: single descriptor — shrink bufs[0] to header, add prefixed bufs[1]. */
        size_t payload_len = bufs[0].len - ETH_HEADER_LEN;
        uint8_t *prefixed = malloc(TX_PREFIX_LEN + payload_len);
        if (prefixed == NULL) {
            return ESP_ERR_NO_MEM;
        }
        test_fill_prefix_magics(prefixed);
        memcpy(prefixed + TX_PREFIX_LEN, bufs[0].buf + ETH_HEADER_LEN, payload_len);
        bufs[0].len = ETH_HEADER_LEN;
        bufs[1] = (esp_eth_buf_desc_t){ .buf = prefixed, .len = TX_PREFIX_LEN + payload_len };
        *buf_count = 2;
        return ESP_OK;
    }
    case HOOK_TX_OP_PREFIX_HEAD: {
        /*
         * Insert a standalone leading Ethernet frame at bufs[0] (any ethertype
         * is fine on TX).  Shift the original frame descriptors down and bump
         * buf_count.  RX hook strips this segment before VLAN demux / L2TAP.
         */
        if (*buf_count < 1 || buf_capacity < *buf_count + 1) {
            return ESP_ERR_INVALID_SIZE;
        }
        uint8_t *head = malloc(TX_PREFIX_HEAD_SEG_LEN);
        if (head == NULL) {
            return ESP_ERR_NO_MEM;
        }
        memset(head, 0xFF, ETH_ADDR_LEN);
        esp_eth_ioctl(eth, ETH_CMD_G_MAC_ADDR, head + ETH_ADDR_LEN);
        head[ETH_HEADER_LEN - sizeof(uint16_t)] = (uint8_t)(ETH_TYPE_HOOK_HEAD >> 8);
        head[ETH_HEADER_LEN - sizeof(uint16_t) + 1] = (uint8_t)ETH_TYPE_HOOK_HEAD;
        test_fill_prefix_magics(head + ETH_HEADER_LEN);
        for (size_t i = *buf_count; i > 0; i--) {
            bufs[i] = bufs[i - 1];
        }
        bufs[0] = (esp_eth_buf_desc_t){ .buf = head, .len = TX_PREFIX_HEAD_SEG_LEN };
        *buf_count += 1;
        return ESP_OK;
    }
    case HOOK_TX_OP_MERGE: {
        /* Merge scattered segments into one malloc'd bufs[0]; reduce buf_count to 1. */
        if (*buf_count == 1 && bufs[0].len > ETH_HEADER_LEN) {
            bufs[1] = (esp_eth_buf_desc_t){
                .buf = bufs[0].buf + ETH_HEADER_LEN,
                .len = bufs[0].len - ETH_HEADER_LEN,
            };
            bufs[0].len = ETH_HEADER_LEN;
            *buf_count = 2;
        }
        if (*buf_count < 2) {
            return ESP_ERR_INVALID_SIZE;
        }
        size_t total = 0;
        for (size_t i = 0; i < *buf_count; i++) {
            total += bufs[i].len;
        }
        uint8_t *merged = malloc(total);
        if (merged == NULL) {
            return ESP_ERR_NO_MEM;
        }
        size_t off = 0;
        for (size_t i = 0; i < *buf_count; i++) {
            memcpy(merged + off, bufs[i].buf, bufs[i].len);
            off += bufs[i].len;
        }
        bufs[0] = (esp_eth_buf_desc_t){ .buf = merged, .len = total };
        *buf_count = 1;
        return ESP_OK;
    }
    case HOOK_TX_OP_SKIP:
        /* buf_count = 0: skip driver TX and post_tx_hook; tx_bufs stays caller-owned. */
        *buf_count = 0;
        return ESP_OK;
    case HOOK_TX_OP_ABORT:
        /* Non-ESP_OK: abort transmit for this packet; post_tx_hook not called. */
        return ESP_ERR_INVALID_STATE;
    case HOOK_TX_OP_POST_TX_FREE:
        /* Allocate replacement buffer; post_tx_hook must free bufs[0] after TX. */
        if (*buf_count >= 1) {
            uint8_t *copy = malloc(bufs[0].len);
            if (copy == NULL) {
                return ESP_ERR_NO_MEM;
            }
            memcpy(copy, bufs[0].buf, bufs[0].len);
            copy[ETH_HEADER_LEN + 1] ^= 0x01;
            bufs[0].buf = copy;
        }
        return ESP_OK;
    default:
        return ESP_OK;
    }
    (void)eth;
}

static void test_post_tx_hook(esp_eth_handle_t eth, const esp_eth_sublayer_tx_bufs_t *tx_bufs, void *ctx)
{
    hooks_ctx_t *hctx = (hooks_ctx_t *)ctx;

    if (hctx->dir != HOOK_DIR_TX) {
        return;
    }
    hctx->tx.post_hook_calls++;

    esp_eth_buf_desc_t *bufs = tx_bufs->bufs;
    size_t buf_count = *tx_bufs->buf_count;

    switch (hctx->tx.op) {
    case HOOK_TX_OP_PREFIX:
        if (buf_count >= 2) {
            hctx->tx.freed_ptr = bufs[1].buf;
            free(bufs[1].buf);
        }
        break;
    case HOOK_TX_OP_PREFIX_HEAD:
        if (buf_count >= 1) {
            hctx->tx.freed_ptr = bufs[0].buf;
            free(bufs[0].buf);
        }
        break;
    case HOOK_TX_OP_MERGE:
    case HOOK_TX_OP_POST_TX_FREE:
        if (buf_count >= 1) {
            hctx->tx.freed_ptr = bufs[0].buf;
            free(bufs[0].buf);
        }
        break;
    default:
        break;
    }
    (void)eth;
}

static esp_err_t test_rx_hook(esp_eth_handle_t eth, uint8_t **buf, uint32_t *len,
                              esp_eth_sublayer_rx_info_t *info, void *ctx)
{
    hooks_ctx_t *hctx = (hooks_ctx_t *)ctx;

    if (hctx->dir == HOOK_DIR_TX) {
        /*
         * TX PREFIX_HEAD companion: strip the custom leading segment on loopback
         * before VLAN demux / L2TAP.  Counted on the TX context because the op
         * union holds tx while dir == HOOK_DIR_TX.
         */
        if (*len >= TX_PREFIX_HEAD_SEG_LEN) {
            uint16_t etype = (uint16_t)(((*buf)[12] << 8) | (*buf)[13]);
            if (etype == ETH_TYPE_HOOK_HEAD) {
                if (!test_prefix_magics_match(*buf + ETH_HEADER_LEN)) {
                    return ESP_ERR_INVALID_ARG;
                }
                hctx->tx.rx_strip_calls++;
                *buf += TX_PREFIX_HEAD_SEG_LEN;
                *len -= TX_PREFIX_HEAD_SEG_LEN;
                return ESP_OK;
            }
        }
        return ESP_OK;
    }

    if (hctx->dir != HOOK_DIR_RX) {
        return ESP_OK;
    }
    hctx->rx.hook_calls++;

    switch (hctx->rx.op) {
    case HOOK_RX_OP_MODIFY:
        if (*len > ETH_HEADER_LEN + 1) {
            size_t marker_off = ETH_HEADER_LEN;
            if (*len > ETH_HEADER_LEN + ETH_VLAN_TAG_LEN + 1 &&
                ((uint16_t)(((*buf)[12] << 8) | (*buf)[13]) == ETH_T_8021Q)) {
                marker_off = ETH_HEADER_LEN + ETH_VLAN_TAG_LEN;
            }
            if (marker_off + 1 < *len && (*buf)[marker_off] == FRAME_MARKER_TX) {
                (*buf)[marker_off] = FRAME_MARKER_RX;
            }
        }
        return ESP_OK;
    case HOOK_RX_OP_DROP:
        *len = 0;
        // rx hook took the buffer, so we need to free it
        free(info->l2_buffer);
        info->l2_buffer = NULL;
        return ESP_OK;
    case HOOK_RX_OP_ABORT:
        return ESP_ERR_INVALID_STATE;
    default:
        return ESP_OK;
    }
    (void)eth;
    (void)info;
}

/* ------------------------------------------------------------------ */
/*  Test helpers                                                      */
/* ------------------------------------------------------------------ */

static int l2tap_open_filtered(const char *if_key, uint16_t ethertype_filter)
{
    int fd = open(L2TAP_VFS_DEFAULT_PATH, O_NONBLOCK);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, fd, "failed to open L2TAP device");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, ioctl(fd, L2TAP_S_INTF_DEVICE, if_key),
                                  "failed to bind L2TAP fd to interface");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, ioctl(fd, L2TAP_S_RCV_FILTER, &ethertype_filter),
                                  "failed to set L2TAP ethertype filter");
    return fd;
}

static int l2tap_open_tx(const char *if_key)
{
    int fd = open(L2TAP_VFS_DEFAULT_PATH, O_NONBLOCK);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, fd, "failed to open L2TAP device");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, ioctl(fd, L2TAP_S_INTF_DEVICE, if_key),
                                  "failed to bind L2TAP fd to interface");
    return fd;
}

static void test_build_frame(hook_test_frame_t *frame, esp_eth_handle_t eth,
                             uint16_t ethertype, uint8_t marker)
{
    memset(frame, 0, sizeof(*frame));
    esp_eth_ioctl(eth, ETH_CMD_G_MAC_ADDR, frame->header.src.addr);
    memcpy(frame->header.dest.addr, frame->header.src.addr, ETH_ADDR_LEN);
    frame->header.type = htons(ethertype);
    frame->marker = marker;
}

static void test_setup(hooks_test_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    memset(&s_hooks_ctx, 0, sizeof(s_hooks_ctx));
    ctx->hook_ctx = &s_hooks_ctx;

    sublayer_test_create_event_loop(&ctx->base);
    sublayer_test_init_ethernet(&ctx->base);
    sublayer_test_register_l2tap(&ctx->base);
    sublayer_test_register_ctx(&ctx->base);

    esp_eth_sublayer_config_t sub_cfg = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();
    sub_cfg.eth_handle = ctx->base.eth_handles[0];
    sub_cfg.tx_hook = test_tx_hook;
    sub_cfg.post_tx_hook = test_post_tx_hook;
    sub_cfg.rx_hook = test_rx_hook;
    sub_cfg.hook_ctx = ctx->hook_ctx;
    TEST_ESP_OK(esp_eth_sublayer_new(&sub_cfg, &ctx->base.sub));

    TEST_ESP_OK(esp_eth_sublayer_vlan_add(ctx->base.sub, ESP_ETH_SUBLAYER_UNTAGGED_VID, &ctx->vlan_untagged));
    TEST_ESP_OK(esp_eth_sublayer_vlan_add(ctx->base.sub, TEST_VLAN_ID, &ctx->vlan_tagged));

    esp_netif_t *netif_untagged = sublayer_test_create_eth_netif(&ctx->base, IFKEY_UNTAGGED, "eth");
    esp_netif_t *netif_vlan = sublayer_test_create_eth_netif(&ctx->base, IFKEY_VLAN, "eth_vlan");

    TEST_ESP_OK(esp_netif_attach(netif_untagged, ctx->vlan_untagged));
    TEST_ESP_OK(esp_netif_attach(netif_vlan, ctx->vlan_tagged));

    sublayer_test_start_and_wait_connect(&ctx->base);
}

static void test_teardown(hooks_test_ctx_t *ctx)
{
    sublayer_test_teardown(&ctx->base);
    sublayer_test_unregister_ctx();
}

static bool verify_loopback_frame(const uint8_t *rx_buf, size_t rx_len,
                                  const hook_test_frame_t *tx_frame)
{
    return rx_len >= sizeof(*tx_frame) &&
           memcmp(rx_buf, tx_frame, sizeof(*tx_frame)) == 0;
}

typedef struct {
    const char *if_key;
    const hook_test_frame_t *frame;
} l2tap_send_arg_t;

static void l2tap_send_task(void *arg)
{
    l2tap_send_arg_t *send_arg = (l2tap_send_arg_t *)arg;
    vTaskDelay(pdMS_TO_TICKS(L2TAP_SEND_DELAY_MS));
    int tx_fd = l2tap_open_tx(send_arg->if_key);
    ssize_t n = write(tx_fd, send_arg->frame, sizeof(*send_arg->frame));
    TEST_ASSERT_NOT_EQUAL(-1, n);
    close(tx_fd);
    vTaskDelete(NULL);
}

static bool l2tap_wait_for_frame_timeout(int fd, uint8_t *rx_buf, size_t rx_buf_size,
                                         size_t *rx_len, int timeout_ms)
{
    struct timeval tv = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000,
    };
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    int ret = select(fd + 1, &rfds, NULL, NULL, &tv);
    if (ret <= 0 || !FD_ISSET(fd, &rfds)) {
        return false;
    }

    ssize_t n = read(fd, rx_buf, rx_buf_size);
    if (n <= 0) {
        return false;
    }
    *rx_len = (size_t)n;
    return true;
}

static bool l2tap_wait_for_frame(int fd, uint8_t *rx_buf, size_t rx_buf_size, size_t *rx_len)
{
    return l2tap_wait_for_frame_timeout(fd, rx_buf, rx_buf_size, rx_len, L2TAP_IO_TIMEOUT_MS);
}

static bool verify_frame_prefix(const uint8_t *rx_buf, size_t rx_len,
                                const hook_test_frame_t *tx_frame)
{
    size_t expected_len = sizeof(*tx_frame) + TX_PREFIX_LEN;
    if (rx_len < expected_len) {
        return false;
    }
    if (memcmp(rx_buf, tx_frame, ETH_HEADER_LEN) != 0) {
        return false;
    }
    if (!test_prefix_magics_match(rx_buf + ETH_HEADER_LEN)) {
        return false;
    }
    return memcmp(rx_buf + ETH_HEADER_LEN + TX_PREFIX_LEN,
                  ((const uint8_t *)tx_frame) + ETH_HEADER_LEN,
                  sizeof(*tx_frame) - ETH_HEADER_LEN) == 0;
}

/* Send a frame and verify the TX PREFIX_HEAD rewrite (new leading buf) is
 * delivered intact to L2TAP via EMAC loopback. */
static void test_tx_prefix_head_on_interface(hooks_test_ctx_t *ctx, const char *if_key,
                                             uint16_t ethertype)
{
    hook_test_frame_t tx_frame;
    test_build_frame(&tx_frame, ctx->base.eth_handles[0], ethertype, FRAME_MARKER_TX);

    int rx_fd = l2tap_open_filtered(if_key, ethertype);
    l2tap_send_arg_t send_arg = { .if_key = if_key, .frame = &tx_frame };
    xTaskCreate(l2tap_send_task, "l2tap_send", 2048, &send_arg, tskIDLE_PRIORITY + 2, NULL);

    uint8_t rx_buf[HOOK_TEST_RX_BUF_LEN];
    size_t rx_len = 0;
    bool got_frame = l2tap_wait_for_frame(rx_fd, rx_buf, sizeof(rx_buf), &rx_len);
    bool frame_ok = got_frame && verify_loopback_frame(rx_buf, rx_len, &tx_frame);
    close(rx_fd);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_TRUE(got_frame);
    TEST_ASSERT_TRUE(frame_ok);
}

/* Send a frame and verify the TX PREFIX rewrite (extra segment + ethertype
 * tweak) is delivered to L2TAP via EMAC loopback. */
static void test_tx_prefix_on_interface(hooks_test_ctx_t *ctx, const char *if_key,
                                        uint16_t ethertype)
{
    hook_test_frame_t tx_frame;
    test_build_frame(&tx_frame, ctx->base.eth_handles[0], ethertype, FRAME_MARKER_TX);

    int rx_fd = l2tap_open_filtered(if_key, ethertype);
    l2tap_send_arg_t send_arg = { .if_key = if_key, .frame = &tx_frame };
    xTaskCreate(l2tap_send_task, "l2tap_send", 2048, &send_arg, tskIDLE_PRIORITY + 2, NULL);

    uint8_t rx_buf[HOOK_TEST_RX_BUF_LEN];
    size_t rx_len = 0;
    bool got_frame = l2tap_wait_for_frame(rx_fd, rx_buf, sizeof(rx_buf), &rx_len);
    bool frame_ok = got_frame && verify_frame_prefix(rx_buf, rx_len, &tx_frame);
    close(rx_fd);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_TRUE(got_frame);
    TEST_ASSERT_TRUE(frame_ok);
}

/* Send a VLAN frame and verify the TX MERGE rewrite (contiguous buffer) is
 * delivered to L2TAP via EMAC loopback. */
static void test_tx_merge_on_vlan(hooks_test_ctx_t *ctx)
{
    hook_test_frame_t tx_frame;
    test_build_frame(&tx_frame, ctx->base.eth_handles[0], ETH_TYPE_HOOK_VLAN, FRAME_MARKER_TX);

    int rx_fd = l2tap_open_filtered(IFKEY_VLAN, ETH_TYPE_HOOK_VLAN);
    l2tap_send_arg_t send_arg = { .if_key = IFKEY_VLAN, .frame = &tx_frame };
    xTaskCreate(l2tap_send_task, "l2tap_send", 2048, &send_arg, tskIDLE_PRIORITY + 2, NULL);

    uint8_t rx_buf[HOOK_TEST_RX_BUF_LEN];
    size_t rx_len = 0;
    bool got_frame = l2tap_wait_for_frame(rx_fd, rx_buf, sizeof(rx_buf), &rx_len);
    bool frame_ok = got_frame && verify_loopback_frame(rx_buf, rx_len, &tx_frame);
    close(rx_fd);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_TRUE(got_frame);
    TEST_ASSERT_TRUE(frame_ok);
}

/* Send a frame on if_key and assert L2TAP never receives it.
 * Used when a TX hook skips/aborts transmit, or an RX hook drops/aborts the
 * frame before delivery. expect_write_error selects whether write() itself
 * must fail (TX abort) or succeed (skip / RX drop-or-abort). */
static void test_expect_no_frame_on_interface(hooks_test_ctx_t *ctx, const char *if_key,
                                              uint16_t ethertype, bool expect_write_error)
{
    hook_test_frame_t tx_frame;
    test_build_frame(&tx_frame, ctx->base.eth_handles[0], ethertype, FRAME_MARKER_TX);

    int rx_fd = l2tap_open_filtered(if_key, ethertype);
    int tx_fd = l2tap_open_tx(if_key);
    ssize_t wr = write(tx_fd, &tx_frame, sizeof(tx_frame));
    close(tx_fd);
    if (expect_write_error) {
        TEST_ASSERT_EQUAL(-1, wr);
    } else {
        TEST_ASSERT_NOT_EQUAL(-1, wr);
    }

    uint8_t rx_buf[HOOK_TEST_RX_BUF_LEN];
    size_t rx_len = 0;
    bool got_frame = l2tap_wait_for_frame_timeout(rx_fd, rx_buf, sizeof(rx_buf), &rx_len, 1500);
    close(rx_fd);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_FALSE(got_frame);
    (void)ctx;
}

/* Send a frame and verify the RX hook rewrote the payload marker
 * (FRAME_MARKER_TX -> FRAME_MARKER_RX) before L2TAP delivery. */
static void test_rx_modify_on_interface(hooks_test_ctx_t *ctx, const char *if_key,
                                        uint16_t ethertype)
{
    hook_test_frame_t tx_frame;
    test_build_frame(&tx_frame, ctx->base.eth_handles[0], ethertype, FRAME_MARKER_TX);

    int rx_fd = l2tap_open_filtered(if_key, ethertype);
    l2tap_send_arg_t send_arg = { .if_key = if_key, .frame = &tx_frame };
    xTaskCreate(l2tap_send_task, "l2tap_send", 2048, &send_arg, tskIDLE_PRIORITY + 2, NULL);

    uint8_t rx_buf[HOOK_TEST_RX_BUF_LEN];
    size_t rx_len = 0;
    bool got_frame = l2tap_wait_for_frame(rx_fd, rx_buf, sizeof(rx_buf), &rx_len);
    bool marker_ok = got_frame && rx_len >= ETH_HEADER_LEN + 1 &&
                     rx_buf[ETH_HEADER_LEN] == FRAME_MARKER_RX;
    close(rx_fd);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_TRUE(got_frame);
    TEST_ASSERT_TRUE(marker_ok);
}

static void test_set_tx_op(hooks_test_ctx_t *ctx, hook_tx_op_t tx_op)
{
    memset(&s_hooks_ctx, 0, sizeof(s_hooks_ctx));
    s_hooks_ctx.dir = HOOK_DIR_TX;
    s_hooks_ctx.tx.op = tx_op;
    ctx->hook_ctx = &s_hooks_ctx;
}

static void test_set_rx_op(hooks_test_ctx_t *ctx, hook_rx_op_t rx_op)
{
    memset(&s_hooks_ctx, 0, sizeof(s_hooks_ctx));
    s_hooks_ctx.dir = HOOK_DIR_RX;
    s_hooks_ctx.rx.op = rx_op;
    ctx->hook_ctx = &s_hooks_ctx;
}

/* ------------------------------------------------------------------ */
/*  Test cases                                                        */
/* ------------------------------------------------------------------ */

TEST_CASE("sublayer tx and post tx hooks", "[sublayer_hooks]")
{
    hooks_test_ctx_t ctx;
    test_setup(&ctx);
    test_set_tx_op(&ctx, HOOK_TX_OP_PREFIX);

    ESP_LOGI(TAG, "TX hook: insert prefix, move frame to bufs[1]");
    test_tx_prefix_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED);
    test_tx_prefix_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.hook_calls);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.post_hook_calls);

    ESP_LOGI(TAG, "TX hook: insert custom leading frame + reorder; RX hook strips on loopback");
    test_set_tx_op(&ctx, HOOK_TX_OP_PREFIX_HEAD);
    test_tx_prefix_head_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED);
    test_tx_prefix_head_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.hook_calls);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.rx_strip_calls);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.post_hook_calls);

    ESP_LOGI(TAG, "TX hook: merge tagged header and payload into one buffer");
    test_set_tx_op(&ctx, HOOK_TX_OP_MERGE);
    test_tx_merge_on_vlan(&ctx);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.hook_calls);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.post_hook_calls);

    ESP_LOGI(TAG, "TX hook: buf_count=0 takes ownership, no loopback frame");
    test_set_tx_op(&ctx, HOOK_TX_OP_SKIP);
    test_expect_no_frame_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED, false);
    test_expect_no_frame_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN, false);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.hook_calls);
    TEST_ASSERT_EQUAL(0, ctx.hook_ctx->tx.post_hook_calls);

    ESP_LOGI(TAG, "TX hook: non-ESP_OK aborts transmit");
    test_set_tx_op(&ctx, HOOK_TX_OP_ABORT);
    test_expect_no_frame_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED, true);
    test_expect_no_frame_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN, true);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.hook_calls);
    TEST_ASSERT_EQUAL(0, ctx.hook_ctx->tx.post_hook_calls);

    ESP_LOGI(TAG, "post_tx_hook frees buffer allocated in tx hook");
    test_set_tx_op(&ctx, HOOK_TX_OP_POST_TX_FREE);
    hook_test_frame_t tx_frame;
    test_build_frame(&tx_frame, ctx.base.eth_handles[0], ETH_TYPE_HOOK_UNTAGGED, FRAME_MARKER_TX);
    int rx_fd = l2tap_open_filtered(IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED);
    l2tap_send_arg_t send_arg = { .if_key = IFKEY_UNTAGGED, .frame = &tx_frame };
    xTaskCreate(l2tap_send_task, "l2tap_send", 2048, &send_arg, tskIDLE_PRIORITY + 2, NULL);
    uint8_t rx_buf[HOOK_TEST_RX_BUF_LEN];
    size_t rx_len = 0;
    bool got_frame = l2tap_wait_for_frame(rx_fd, rx_buf, sizeof(rx_buf), &rx_len);
    close(rx_fd);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_TRUE(got_frame);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->tx.post_hook_calls);
    TEST_ASSERT_NOT_NULL(ctx.hook_ctx->tx.freed_ptr);

    test_teardown(&ctx);
}

TEST_CASE("sublayer rx hooks", "[sublayer_hooks]")
{
    hooks_test_ctx_t ctx;
    test_setup(&ctx);
    test_set_rx_op(&ctx, HOOK_RX_OP_MODIFY);

    ESP_LOGI(TAG, "RX hook: modify payload before VLAN demux / L2TAP delivery");
    test_rx_modify_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED);
    test_rx_modify_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->rx.hook_calls);

    ESP_LOGI(TAG, "RX hook: len=0 drops frame on untagged and tagged");
    test_set_rx_op(&ctx, HOOK_RX_OP_DROP);
    test_expect_no_frame_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED, false);
    test_expect_no_frame_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN, false);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->rx.hook_calls);

    ESP_LOGI(TAG, "RX hook: non-ESP_OK aborts processing");
    test_set_rx_op(&ctx, HOOK_RX_OP_ABORT);
    test_expect_no_frame_on_interface(&ctx, IFKEY_UNTAGGED, ETH_TYPE_HOOK_UNTAGGED, false);
    test_expect_no_frame_on_interface(&ctx, IFKEY_VLAN, ETH_TYPE_HOOK_VLAN, false);
    TEST_ASSERT_GREATER_THAN(0, ctx.hook_ctx->rx.hook_calls);

    test_teardown(&ctx);
}
