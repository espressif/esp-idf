/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "unity.h"
#include "esp_riscv_trace.h"
#include "test_riscv_trace_common.h"

static const char *TAG = "riscv_trace_filter";

/* Iterations per call. High enough that the filtered window alone still emits at least one sync
 * packet, but low enough that the full baseline workload (the three calls in capture_and_count:
 * noise, filtered, noise) fits the buffer without wrapping. */
#define TRACE_FILTER_WORKLOAD_ITERATIONS 512

static volatile uint32_t s_filter_sink;
static uintptr_t s_filtered_end;
static uintptr_t s_noise_end;

static void __attribute__((noinline)) IRAM_ATTR filtered_work(void)
{
    uint32_t acc = s_filter_sink;
    for (volatile int i = 0; i < TRACE_FILTER_WORKLOAD_ITERATIONS; i++) {
        if ((i & 3) == 0) {
            acc += (uint32_t)i;
        } else {
            acc ^= ((uint32_t)i << 1);
        }
    }
    s_filter_sink = acc;
filtered_end:
    s_filtered_end = (uintptr_t) && filtered_end;
}

static void __attribute__((noinline)) noise_work(void)
{
    uint32_t acc = s_filter_sink;
    for (volatile int i = 0; i < TRACE_FILTER_WORKLOAD_ITERATIONS; i++) {
        if ((i & 3) == 0) {
            acc += (uint32_t)i + 0x55u;
        } else {
            acc ^= ((uint32_t)i << 1);
        }
    }
    s_filter_sink = acc;
noise_end:
    s_noise_end = (uintptr_t) && noise_end;
}

typedef struct {
    size_t packets;       /* all framed packets */
    size_t total;         /* sync packets */
    size_t in_filtered;   /* sync PCs inside the filtered_work window */
    size_t in_noise;      /* sync PCs inside the noise_work window */
} filter_sync_counts_t;

static bool pc_in_window(uint32_t pc, uintptr_t start, uintptr_t end)
{
    return pc >= start && pc <= end;
}

/* Walk a non-wrapped snapshot [0, head) and classify each sync packet's PC by window. */
static filter_sync_counts_t count_syncs_in_windows(const uint8_t *buf, size_t head)
{
    filter_sync_counts_t c = { 0 };
    size_t off = 0;
    while (off < head) {
        if (buf[off] == 0) {   /* skip anchor zero-runs between packets */
            while (off < head && buf[off] == 0) {
                off++;
            }
            continue;
        }
        uint8_t plen = buf[off] & 0x1F;
        if (plen < TRACE_PACKET_MIN_LEN || plen > TRACE_PACKET_MAX_LEN || off + plen > head) {
            break;
        }
        c.packets++;
        uint32_t pc = 0;
        if (decode_sync_pc(&buf[off + TRACE_PACKET_PREFIX_SIZE], plen - TRACE_PACKET_PREFIX_SIZE, &pc)) {
            c.total++;
            if (pc_in_window(pc, (uintptr_t)&filtered_work, s_filtered_end)) {
                c.in_filtered++;
            } else if (pc_in_window(pc, (uintptr_t)&noise_work, s_noise_end)) {
                c.in_noise++;
            }
        }
        off += plen;
    }
    return c;
}

static filter_sync_counts_t capture_and_count(esp_riscv_trace_core_t core)
{
    TEST_ESP_OK(esp_riscv_trace_start(core));
    noise_work();
    filtered_work();
    noise_work();
    TEST_ESP_OK(esp_riscv_trace_stop(core, TRACE_TEST_STOP_TIMEOUT_US));
    validate_trace_status(core);

    uint8_t *buf = NULL;
    size_t capacity = 0;
    size_t head = 0;
    TEST_ESP_OK(esp_riscv_trace_get_buffer(core, &buf, &capacity, &head));
    TEST_ASSERT_FALSE_MESSAGE(buffer_has_wrapped(buf, capacity, head),
                              "filter capture wrapped. Reduce TRACE_FILTER_WORKLOAD_ITERATIONS.");
    return count_syncs_in_windows(buf, head);
}

/* Verifies the address-window filter restricts tracing to a chosen PC range. Captures the same
 * workload twice. Filter off (baseline), then a window over filtered_work only. Checks the
 * filtered run produced fewer packets, with every sync inside the filtered window and none in the
 * noise window. */
TEST_CASE("RISC-V trace address-window filter", "[riscv_trace]")
{
    const esp_riscv_trace_core_t core = ESP_RISCV_TRACE_CORE_0;
    TEST_ASSERT_EQUAL_MESSAGE(ESP_RISCV_TRACE_CORE_0, xPortGetCoreID(), "test must run on core 0");

    /* Prime: run each function once (not traced) to record its end-marker PC. */
    filtered_work();
    noise_work();
    TEST_ASSERT_GREATER_THAN_UINT(0, s_filtered_end - (uintptr_t)&filtered_work);
    TEST_ASSERT_GREATER_THAN_UINT(0, s_noise_end - (uintptr_t)&noise_work);

    /* Baseline (filter off): all three calls traced. Gives the reference packet count that the
     * filtered run is compared against below. */
    esp_riscv_trace_filter_config_t filter_off = { .enable = false };
    TEST_ESP_OK(esp_riscv_trace_set_filter(core, &filter_off));
    filter_sync_counts_t base = capture_and_count(core);
    ESP_LOGI(TAG, "baseline: %u packets, %u sync (filtered %u, noise %u)",
             (unsigned)base.packets, (unsigned)base.total,
             (unsigned)base.in_filtered, (unsigned)base.in_noise);
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, base.packets, "baseline produced no trace");

    /* Filtered: AND-window over filtered_work only (P: iaddr>=start, S: iaddr<=end). */
    esp_riscv_trace_filter_config_t filter_window = {
        .enable = true,
        .match_comparators = true,
        .primary = {
            .input = ESP_RISCV_TRACE_FILTER_INPUT_IADDR,
            .function = ESP_RISCV_TRACE_FILTER_COMPARATOR_GE,
            .match_value = (uint32_t)(uintptr_t) &filtered_work,
        },
        .secondary = {
            .input = ESP_RISCV_TRACE_FILTER_INPUT_IADDR,
            .function = ESP_RISCV_TRACE_FILTER_COMPARATOR_LE,
            .match_value = (uint32_t)s_filtered_end,
        },
        .mode = ESP_RISCV_TRACE_FILTER_MODE_AND,
    };
    TEST_ESP_OK(esp_riscv_trace_set_filter(core, &filter_window));
    filter_sync_counts_t filtered = capture_and_count(core);
    ESP_LOGI(TAG, "filtered: %u packets, %u sync (filtered %u, noise %u)",
             (unsigned)filtered.packets, (unsigned)filtered.total,
             (unsigned)filtered.in_filtered, (unsigned)filtered.in_noise);
    /* The filter dropped the noise calls, so the filtered capture has fewer packets than the baseline. */
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, filtered.packets, "filtered window produced no trace");
    TEST_ASSERT_LESS_THAN_UINT_MESSAGE(base.packets, filtered.packets,
                                       "filter did not reduce trace volume");
    /*
       Every sync is in the filtered window and none in the noise window.
       The workload guarantees >=1 sync in the window.
    */
    TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(0, filtered.in_filtered, "filtered: no sync located in filtered window");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(filtered.in_filtered, filtered.total,
                                   "filtered: a sync was located outside the filtered window");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(0, filtered.in_noise, "filtered: a noise-window sync was observed");

    /* Clear the filter so it does not affect other test cases or re-runs. */
    TEST_ESP_OK(esp_riscv_trace_set_filter(core, &filter_off));
}
