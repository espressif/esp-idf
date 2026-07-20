/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "soc/soc.h"
#include "unity.h"
#include "esp_riscv_trace.h"
#include "test_riscv_trace_common.h"

static const char *TAG = "riscv_trace_basic";

/* Large enough to wrap each core's buffer several times, so each physical side carries anchor tags. */
#define TRACE_TEST_WORKLOAD_ITERATIONS  8192

/* Per-core buffer sizes, applied via the strong esp_riscv_trace_get_user_config() below to
 * demonstrate (and test) that each core's encoder is configured independently. */
#define TRACE_CORE0_BUFFER_SIZE  2048
#define TRACE_CORE1_BUFFER_SIZE  4096

static volatile uint32_t s_trace_sink;

/* Strong override of the weak startup hook: configure each core's encoder independently. Only the
 * buffer size differs here; everything else stays at the Kconfig defaults. */
esp_riscv_trace_config_t esp_riscv_trace_get_user_config(int core_id)
{
    esp_riscv_trace_config_t config = ESP_RISCV_TRACE_DEFAULT_CONFIG();
    config.buffer_size = (core_id == ESP_RISCV_TRACE_CORE_1) ? TRACE_CORE1_BUFFER_SIZE
                         : TRACE_CORE0_BUFFER_SIZE;
    return config;
}

typedef struct {
    size_t packets;
    size_t syncs;
} trace_walk_result_t;

/* A branchy workload so the encoder emits many Format 1 (branch) packets. */
static void IRAM_ATTR branchy_work(void)
{
    uint32_t acc = s_trace_sink;
    for (volatile int i = 0; i < TRACE_TEST_WORKLOAD_ITERATIONS; i++) {
        if ((i & 3) == 0) {
            acc += (uint32_t)i;
        } else {
            acc ^= ((uint32_t)i << 1);
        }
    }
    s_trace_sink = acc;
}

static bool addr_in_code(uint32_t addr)
{
    /* A periodic sync can be taken anywhere the core executes */
    return (addr >= SOC_IROM_LOW && addr < SOC_IROM_HIGH) ||
           (addr >= SOC_IRAM_LOW && addr < SOC_IRAM_HIGH) ||
           (addr >= SOC_IROM_MASK_LOW && addr < SOC_IROM_MASK_HIGH);
}

/* If payload is a sync packet, validate its full address is in a code region and return true. */
static bool sync_addr_ok(esp_riscv_trace_core_t core, const uint8_t *payload, size_t payload_len)
{
    uint32_t addr = 0;
    if (!decode_sync_pc(payload, payload_len, &addr)) {
        return false;
    }
    bool in_code = addr_in_code(addr);
    if (!in_code) {
        ESP_LOGE(TAG, "core %d: sync address 0x%08x not in a code region", (int)core, (unsigned)addr);
    }
    TEST_ASSERT_TRUE_MESSAGE(in_code, "sync address not in a code region");
    return true;
}

static uint8_t *linearize_ring(const uint8_t *buf, size_t capacity, size_t head)
{
    /* Linearize the ring into chronological order: [head..capacity) then [0..head). */
    uint8_t *lin = malloc(capacity);
    TEST_ASSERT_NOT_NULL(lin);
    memcpy(lin, buf + head, capacity - head);
    memcpy(lin + (capacity - head), buf, head);
    return lin;
}

static bool find_anchor_boundary(const uint8_t *lin, size_t start, size_t end, size_t *offset)
{
    /* Find the first anchor tag (>=14 zeros); the next byte is a clean packet boundary. */
    size_t zero_run = 0;
    bool at_boundary = false;
    *offset = start;
    while (*offset < end) {
        if (lin[*offset] == 0) {
            zero_run++;
            (*offset)++;
            if (zero_run >= TRACE_ANCHOR_ZERO_RUN_BYTES) {
                at_boundary = true;
            }
        } else if (at_boundary) {
            return true;
        } else {
            zero_run = 0;
            (*offset)++;
        }
    }
    return false;
}

static trace_walk_result_t walk_packets(esp_riscv_trace_core_t core, const uint8_t *lin,
                                        size_t offset, size_t end)
{
    trace_walk_result_t result = { 0 };
    bool have_index = false;
    uint16_t prev_index = 0;

    while (offset < end) {
        if (lin[offset] == 0) {
            while (offset < end && lin[offset] == 0) {
                offset++;
            }
            continue;
        }
        uint8_t plen = lin[offset] & 0x1F;
        if (plen < TRACE_PACKET_MIN_LEN || plen > TRACE_PACKET_MAX_LEN || offset + plen > end) {
            break;   /* reached this side's newest end; stop cleanly */
        }
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, lin[offset] >> 5, "reserved header bits set");
        uint16_t index = (uint16_t)(lin[offset + 1] | (lin[offset + 2] << 8));
        const uint8_t *payload = &lin[offset + TRACE_PACKET_PREFIX_SIZE];
        uint8_t format = payload[0] & 0x3;
        TEST_ASSERT_TRUE_MESSAGE(format == 1 || format == 2 || format == 3, "invalid packet format");
        if (have_index) {
            TEST_ASSERT_EQUAL_UINT16_MESSAGE((uint16_t)(prev_index + 1), index, "index not monotonic");
        }
        prev_index = index;
        have_index = true;
        if (sync_addr_ok(core, payload, plen - TRACE_PACKET_PREFIX_SIZE)) {
            result.syncs++;
        }
        offset += plen;
        result.packets++;
    }

    return result;
}

static trace_walk_result_t walk_side_from_anchor(esp_riscv_trace_core_t core, const uint8_t *lin,
                                                 size_t start, size_t end)
{
    trace_walk_result_t result = { 0 };
    size_t off = 0;
    if (find_anchor_boundary(lin, start, end, &off)) {
        result = walk_packets(core, lin, off, end);
    }
    return result;
}

static trace_walk_result_t add_walk_results(trace_walk_result_t a, trace_walk_result_t b)
{
    trace_walk_result_t result = {
        .packets = a.packets + b.packets,
        .syncs = a.syncs + b.syncs,
    };
    return result;
}

/* Loop mode has no "wrapped" HW flag, so infer the wrap from data */
static void validate_wrapped_stream(esp_riscv_trace_core_t core)
{
    TEST_ASSERT_EQUAL_MESSAGE(ESP_RISCV_TRACE_MEM_LOOP, CONFIG_ESP_RISCV_TRACE_MEM_MODE,
                              "test expects loop memory mode");
    TEST_ASSERT_EQUAL_MESSAGE(ESP_RISCV_TRACE_RESYNC_PACKET, CONFIG_ESP_RISCV_TRACE_RESYNC_MODE,
                              "test expects packet resync mode");
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, CONFIG_ESP_RISCV_TRACE_RESYNC_THRESHOLD,
                                          "resync threshold must be nonzero");

    uint8_t *buf = NULL;
    size_t capacity = 0;
    size_t head = 0;
    TEST_ESP_OK(esp_riscv_trace_get_buffer(core, &buf, &capacity, &head));
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, capacity, "trace buffer capacity is zero");
    TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(capacity, head, "trace buffer head is out of range");
    TEST_ASSERT_TRUE_MESSAGE(buffer_has_wrapped(buf, capacity, head),
                             "buffer did not wrap (shrink buffer or grow workload)");

    /* The encoder keeps no packet/index continuity across the physical wrap (at capacity - head),
     * so decode each side from its own anchor tag instead of walking across it. */
    uint8_t *lin = linearize_ring(buf, capacity, head);
    const size_t mem_wrap = capacity - head;
    trace_walk_result_t before_wrap = walk_side_from_anchor(core, lin, 0, mem_wrap);
    trace_walk_result_t after_wrap = walk_side_from_anchor(core, lin, mem_wrap, capacity);
    trace_walk_result_t walk = add_walk_results(before_wrap, after_wrap);
    free(lin);

    /* ~1 sync per `threshold` packets; each side may miss its first/last interval, so allow margin. */
    const size_t threshold = CONFIG_ESP_RISCV_TRACE_RESYNC_THRESHOLD;
    const size_t expected = walk.packets / threshold;
    ESP_LOGI(TAG, "core %d wrap: capacity %u, head %u, %u packets, %u sync (approx %u, tolerance -2/+1)",
             (int)core, (unsigned)capacity, (unsigned)head,
             (unsigned)walk.packets, (unsigned)walk.syncs, (unsigned)expected);
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, walk.packets, "no packets walked from wrapped buffer");
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, walk.syncs, "no sync survived in the wrapped buffer");
    TEST_ASSERT_TRUE_MESSAGE(walk.syncs + 2 >= expected && walk.syncs <= expected + 1,
                             "packet-resync sync count does not match packets / threshold");
}

static void __attribute__((unused)) core1_work_task(void *arg)
{
    TaskHandle_t notify_to = (TaskHandle_t)arg;
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   /* wait until tracing is started */
    branchy_work();
    xTaskNotifyGive(notify_to);                /* signal work done */
    vTaskSuspend(NULL);                        /* park until the test deletes us */
}

/* End-to-end loop-buffer capture: start tracing, run a branchy workload that wraps the ring buffer
 * several times, then stop and check the captured stream (buffer wrapped, packets well-formed with
 * monotonic indices, sync addresses land in code, sync count ~ packets/threshold). Runs on core 0,
 * and on core 1 in parallel if it is in the configured core mask. */
TEST_CASE("RISC-V trace loop capture", "[riscv_trace]")
{
    TEST_ASSERT_EQUAL_MESSAGE(ESP_RISCV_TRACE_CORE_0, xPortGetCoreID(), "test must run on core 0");
    TEST_ASSERT_TRUE_MESSAGE(CONFIG_ESP_RISCV_TRACE_CORE_MASK & (1 << ESP_RISCV_TRACE_CORE_0),
                             "core 0 must be in the configured trace core mask");
    const bool trace_core1 = (CONFIG_ESP_RISCV_TRACE_CORE_MASK & (1 << ESP_RISCV_TRACE_CORE_1)) != 0;

    TaskHandle_t worker = NULL;
    if (trace_core1) {
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(core1_work_task, "trc_c1", 4096,
                                                          xTaskGetCurrentTaskHandle(), 5, &worker, 1));
    }

    TEST_ESP_OK(esp_riscv_trace_start(ESP_RISCV_TRACE_CORE_0));
    if (trace_core1) {
        TEST_ESP_OK(esp_riscv_trace_start(ESP_RISCV_TRACE_CORE_1));
        xTaskNotifyGive(worker);   /* let core 1 run its workload */
    }

    branchy_work();                /* core 0 workload */

    if (trace_core1) {
        TEST_ASSERT_EQUAL(1, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));
        TEST_ESP_OK(esp_riscv_trace_stop(ESP_RISCV_TRACE_CORE_1, TRACE_TEST_STOP_TIMEOUT_US));
    }
    TEST_ESP_OK(esp_riscv_trace_stop(ESP_RISCV_TRACE_CORE_0, TRACE_TEST_STOP_TIMEOUT_US));

    validate_trace_status(ESP_RISCV_TRACE_CORE_0);
    validate_wrapped_stream(ESP_RISCV_TRACE_CORE_0);
    if (trace_core1) {
        validate_trace_status(ESP_RISCV_TRACE_CORE_1);
        validate_wrapped_stream(ESP_RISCV_TRACE_CORE_1);
        vTaskDelete(worker);
    }
}

/* Each core was set up by esp_riscv_trace_get_user_config() with a different buffer size, proving
 * the two encoders are configured independently rather than sharing one config. */
TEST_CASE("RISC-V trace per-core independent config", "[riscv_trace]")
{
    TEST_ASSERT_TRUE_MESSAGE(CONFIG_ESP_RISCV_TRACE_CORE_MASK & (1 << ESP_RISCV_TRACE_CORE_0),
                             "core 0 must be in the configured trace core mask");
    if ((CONFIG_ESP_RISCV_TRACE_CORE_MASK & (1 << ESP_RISCV_TRACE_CORE_1)) == 0) {
        TEST_IGNORE_MESSAGE("per-core comparison needs core 1 in the trace core mask");
    }

    uint8_t *buf0 = NULL, *buf1 = NULL;
    size_t cap0 = 0, cap1 = 0, head0 = 0, head1 = 0;
    TEST_ESP_OK(esp_riscv_trace_get_buffer(ESP_RISCV_TRACE_CORE_0, &buf0, &cap0, &head0));
    TEST_ESP_OK(esp_riscv_trace_get_buffer(ESP_RISCV_TRACE_CORE_1, &buf1, &cap1, &head1));

    ESP_LOGI(TAG, "core 0 buffer %p cap %u, core 1 buffer %p cap %u",
             buf0, (unsigned)cap0, buf1, (unsigned)cap1);

    TEST_ASSERT_EQUAL_UINT_MESSAGE(TRACE_CORE0_BUFFER_SIZE, cap0, "core 0 buffer size mismatch");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(TRACE_CORE1_BUFFER_SIZE, cap1, "core 1 buffer size mismatch");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(cap0, cap1, "cores ended up with identical config");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(buf0, buf1, "cores share the same trace buffer");
}
