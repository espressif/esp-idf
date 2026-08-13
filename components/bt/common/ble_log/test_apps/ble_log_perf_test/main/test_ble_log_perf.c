/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ble_log.h"
#include "ble_log_lbm.h"
#include "ble_log_prph_test.h"
#include "log_compression/utils.h"
#include "esp_cpu.h"
#include "esp_timer.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "test_ble_log_main.h"
#include "unity.h"

#if !CONFIG_BLE_LOG_PRPH_TEST
#error "BLE Log perf test requires CONFIG_BLE_LOG_PRPH_TEST"
#endif

/* The compressed-log entry points are declared in the build-generated
 * *_log_index.h headers; declare them directly for the perf test. */
extern int ble_log_compressed_hex_print(uint8_t source, uint32_t log_index,
                                        size_t args_cnt, ...);
extern int ble_log_compressed_hex_print_buf(uint8_t source, uint32_t log_index,
                                            uint8_t buf_idx, const uint8_t *buf,
                                            size_t len);
extern int ble_log_compressed_hex_printv(uint8_t source, uint32_t log_index,
                                         size_t args_cnt, va_list args);

/* ---------------- */
/*  Test Parameters */
/* ---------------- */
#define PERF_DURATION_MS            (3000)
#define PERF_READ_TIMEOUT_MS        (20)
#define PERF_SINK_STACK_SIZE        (4096)
#define PERF_WRITER_STACK_SIZE      (2048)
/* Must fit every reference payload profile (largest is s_ref133 = 133);
 * the writer task stack buffer is sized from this. */
#define PERF_MAX_PAYLOAD_LEN        (256)
#define PERF_HIST_BIN_COUNT         (256)
#define PERF_HIST_FINE_SHIFT        (8)
#define PERF_HIST_COARSE_SHIFT      (11)
#define PERF_ISR_PERIOD_US          (1000)
#define PERF_CUSTOM_WRITER_COUNT    (3)
/* custom + ll_task + ll_hci + compressed */
#define PERF_WRITER_MAX             (PERF_CUSTOM_WRITER_COUNT + 3)

/* Throughput links are megabit/s converted to the test peripheral's
 * byte-rate argument: 2 Mbps = 250000 B/s, 20 Mbps = 2500000 B/s.
 * 0 disables pacing entirely: the run is CPU-bound and measures the
 * maximum throughput the LBM can sustain, including any drops that
 * saturation of the buffer pools causes. */
#define PERF_LINK_2MBPS_BPS         (2000000u / 8u)
#define PERF_LINK_20MBPS_BPS        (20000000u / 8u)

static const uint32_t s_throughput_link_Bps[] = {
    PERF_LINK_2MBPS_BPS,
    PERF_LINK_20MBPS_BPS,
    0,
};

static const uint16_t s_fixed32[] = { 32 };
static const uint16_t s_fixed64[] = { 64 };
static const uint16_t s_fixed128[] = { 128 };
static const uint16_t s_mixed_8_16_32_64[] = { 8, 16, 32, 64 };
static const uint16_t s_cycle_sizes[] = { 8, 32, 64, 128 };
/* Reference payload lengths matching each compressed case's record size.
 * A companion write_hex writer in the same run measures the downstream
 * write cost at the same length, from which the encode/write split of the
 * compressed call is derived (no instrumentation in the module itself). */
static const uint16_t s_ref3[] = { 3 };
static const uint16_t s_ref4[] = { 4 };
static const uint16_t s_ref6[] = { 6 };
static const uint16_t s_ref8[] = { 8 };
static const uint16_t s_ref12[] = { 12 };
static const uint16_t s_ref13[] = { 13 };
static const uint16_t s_ref133[] = { 133 };

/* ---------------- */
/*     Statistics   */
/* ---------------- */
typedef struct {
    uint64_t ok_frames;        /* write_hex: returned true; write_hex_ll: all calls */
    uint64_t failed_frames;    /* write_hex only */
    uint64_t total_cycles;
    uint64_t failed_cycles;    /* cycles spent in failed (dropped) writes */
    uint32_t min_cycles;
    uint32_t max_cycles;
    uint32_t hist_fine[PERF_HIST_BIN_COUNT];
    uint32_t hist_coarse[PERF_HIST_BIN_COUNT];
    uint32_t hist_samples;
} perf_cycles_t;

/* Compressed workload shapes, one per public entry point call pattern.
 * Fixed payloads keep the encode path deterministic across runs. */
typedef enum {
    PERF_CP_MODE_U32,      /* hex_print, 0..2 U32 args (UINT32_MAX: rotate) */
    PERF_CP_MODE_U64,      /* hex_print, 1 U64 arg */
    PERF_CP_MODE_STR,      /* hex_print, 1 STR arg */
    PERF_CP_MODE_BUF,      /* hex_print_buf, raw buffer */
    PERF_CP_MODE_VPRINT,   /* hex_printv, va_list built by the caller */
} perf_cp_mode_t;

typedef struct {
    perf_cp_mode_t mode;
    uint32_t args;         /* U32 arg count; UINT32_MAX rotates 0/1/2 */
    uint64_t value;        /* U64 value */
    const char *str;       /* STR data (NUL-terminated) */
    const uint8_t *buf;    /* BUF data */
    size_t len;            /* BUF length */
} perf_compress_cfg_t;

typedef struct {
    const char *name;
    ble_log_src_t src;
    uint32_t ll_flag;          /* UINT32_MAX: use ble_log_write_hex */
    bool compressed;           /* call ble_log_compressed_hex_print instead */
    bool record_cycles;
    bool isolate_write;        /* vTaskSuspendAll around the timed write */
    const uint8_t *ll_append;  /* write_hex_ll append part */
    size_t ll_append_len;
    const perf_compress_cfg_t *compress_cfg;
    const uint16_t *profile;
    size_t profile_count;
    perf_cycles_t *cycles;
    SemaphoreHandle_t ready;
    SemaphoreHandle_t start;
    SemaphoreHandle_t done;
    volatile bool stop;
} perf_writer_t;

typedef struct {
    uint64_t frames_by_src[BLE_LOG_SRC_MAX];
    uint64_t stat_written[BLE_LOG_SRC_MAX];
    uint64_t stat_lost[BLE_LOG_SRC_MAX];
    uint64_t transport_bytes;
    uint64_t transactions;
    uint64_t walk_errors;
    volatile uint32_t bytes_per_second;
    volatile bool drained;       /* test peripheral queue empty at least once */
    volatile bool stat_final_seen; /* final statistics frame received */
    volatile bool stop;
    SemaphoreHandle_t done;
} perf_sink_t;

typedef struct {
    uint8_t payload[PERF_MAX_PAYLOAD_LEN];
    size_t payload_len;
    volatile bool stop;
    uint64_t calls;
    uint64_t total_cycles;
} perf_isr_t;

typedef struct {
    const char *mode_name;
    const char *profile_name;
    uint32_t run_idx;            /* 1-based, for the per-run separator */
    uint32_t run_total;
    const uint16_t *profile;
    size_t profile_count;
    uint32_t bytes_per_second;
    uint32_t custom_writer_count;
    bool ll_task;
    bool ll_hci;
    bool compress;
    bool isr;
    bool measure_cycles;
    bool isolate_write;
    bool expect_no_loss;
    const uint8_t *ll_append;
    size_t ll_append_len;
    const perf_compress_cfg_t *compress_cfg;
} perf_run_cfg_t;

static perf_isr_t s_perf_isr;

static const perf_compress_cfg_t s_cp_mixed_u32 = {
    .mode = PERF_CP_MODE_U32, .args = UINT32_MAX,
};
static const perf_compress_cfg_t s_cp_u32_0 = {
    .mode = PERF_CP_MODE_U32, .args = 0,
};
static const perf_compress_cfg_t s_cp_u32_1 = {
    .mode = PERF_CP_MODE_U32, .args = 1,
};
static const perf_compress_cfg_t s_cp_u32_2 = {
    .mode = PERF_CP_MODE_U32, .args = 2,
};
/* U64 values exercise the leading-zero analysis branches (clz + partial
 * pushes + nibble packing), the CPU-for-memory tradeoff in the encoder. */
static const perf_compress_cfg_t s_cp_u64_full = {
    .mode = PERF_CP_MODE_U64, .value = 0x123456789ABCDEF0ULL, /* 8B, no LZ */
};
static const perf_compress_cfg_t s_cp_u64_lz = {
    .mode = PERF_CP_MODE_U64, .value = 0xFFULL,               /* LZ -> 1B */
};
static const perf_compress_cfg_t s_cp_u64_az = {
    .mode = PERF_CP_MODE_U64, .value = 0ULL,                  /* all-zero */
};
/* hex_printv: va_list built by the caller (prebuilt lib callback pattern).
 * Same records as the hex_print cases so the va_list delta is visible. */
static const perf_compress_cfg_t s_cp_v0 = {
    .mode = PERF_CP_MODE_VPRINT, .args = 0,
};
static const perf_compress_cfg_t s_cp_vu64 = {
    .mode = PERF_CP_MODE_VPRINT, .args = 1, .value = 0x123456789ABCDEF0ULL,
};
/* STR/BUF payloads are filled by their test cases (mutable static globals). */
static perf_compress_cfg_t s_cp_str_cfg;
static perf_compress_cfg_t s_cp_buf_cfg;
static char s_cp_str_buf[129];
static uint8_t s_cp_raw_buf[128];

/* ---------------- */
/*      Helpers     */
/* ---------------- */
static uint32_t cycle_hist_percentile(const uint32_t *hist, uint32_t samples,
                                      uint32_t numerator, uint32_t bin_shift)
{
    uint32_t target = (samples * numerator + 99) / 100;
    uint32_t seen = 0;
    for (uint32_t i = 0; i < PERF_HIST_BIN_COUNT; i++) {
        seen += hist[i];
        if (seen >= target) {
            return (i + 1) << bin_shift;
        }
    }
    return PERF_HIST_BIN_COUNT << bin_shift;
}

static void record_cycles(perf_cycles_t *c, uint32_t cycles)
{
    c->total_cycles += cycles;
    if (cycles < c->min_cycles) {
        c->min_cycles = cycles;
    }
    if (cycles > c->max_cycles) {
        c->max_cycles = cycles;
    }
    uint32_t fine_bin = cycles >> PERF_HIST_FINE_SHIFT;
    uint32_t coarse_bin = cycles >> PERF_HIST_COARSE_SHIFT;
    c->hist_fine[fine_bin < PERF_HIST_BIN_COUNT ? fine_bin
                                                : PERF_HIST_BIN_COUNT - 1]++;
    c->hist_coarse[coarse_bin < PERF_HIST_BIN_COUNT ? coarse_bin
                                                    : PERF_HIST_BIN_COUNT - 1]++;
    c->hist_samples++;
}

static void print_link(uint32_t bytes_per_second)
{
    if (bytes_per_second == 0) {
        printf("link=unlimited");
        return;
    }
    uint32_t mbps = (bytes_per_second * 8u) / 1000000u;
    printf("link=%" PRIu32 "Mbps", mbps);
}

static void print_cycles(const char *name, const perf_cycles_t *c, bool ll)
{
    uint32_t avg = c->ok_frames ? (uint32_t)(c->total_cycles / c->ok_frames) : 0;
    uint32_t avg_failed = c->failed_frames ?
                          (uint32_t)(c->failed_cycles / c->failed_frames) : 0;
    uint32_t max = c->max_cycles == UINT32_MAX ? 0 : c->max_cycles;
    uint32_t p50 = cycle_hist_percentile(c->hist_fine, c->hist_samples, 50,
                                         PERF_HIST_FINE_SHIFT);
    uint32_t p95 = cycle_hist_percentile(c->hist_coarse, c->hist_samples, 95,
                                         PERF_HIST_COARSE_SHIFT);
    uint32_t p99 = cycle_hist_percentile(c->hist_coarse, c->hist_samples, 99,
                                         PERF_HIST_COARSE_SHIFT);
    printf(
        "BLE_LOG_PERF writer=%-12s frames=%" PRIu64 " failed=%" PRIu64
        " avg=%" PRIu32 " avg_failed=%" PRIu32
        " p50=%" PRIu32 " p95=%" PRIu32 " p99=%" PRIu32
        " max=%" PRIu32 " cycles%s\n",
        name, c->ok_frames, c->failed_frames, avg, avg_failed, p50, p95, p99,
        max, ll ? " (call)" : " (ok frame)");
}

static void print_writer_counts(const char *name, const perf_cycles_t *c)
{
    printf("BLE_LOG_PERF writer=%-12s frames=%" PRIu64 " failed=%" PRIu64 "\n",
           name, c->ok_frames, c->failed_frames);
}

static void print_throughput(uint64_t bytes, uint64_t elapsed_us,
                             uint32_t link_Bps)
{
    uint64_t Bps = elapsed_us ? bytes * 1000000ULL / elapsed_us : 0;
    uint64_t bits_per_s = Bps * 8ULL;
    uint32_t mbps = (uint32_t)(bits_per_s / 1000000ULL);
    uint32_t milli = (uint32_t)((bits_per_s % 1000000ULL) / 1000ULL);
    if (link_Bps) {
        printf("BLE_LOG_PERF throughput=%" PRIu64 " B/s (%" PRIu32 ".%03" PRIu32
               " Mbps) link_cap=%" PRIu32 " B/s\n",
               Bps, mbps, milli, link_Bps);
    } else {
        printf("BLE_LOG_PERF throughput=%" PRIu64 " B/s (%" PRIu32 ".%03" PRIu32
               " Mbps) link_cap=unlimited\n",
               Bps, mbps, milli);
    }
}

static void print_run_separator(const char *tag, const perf_run_cfg_t *cfg)
{
    printf("\nBLE_LOG_PERF ============ %s [%u/%u] mode=%s profile=%s ",
           tag, (unsigned)cfg->run_idx, (unsigned)cfg->run_total,
           cfg->mode_name, cfg->profile_name);
    print_link(cfg->bytes_per_second);
    printf(" isolate=%s ============\n",
           cfg->isolate_write ? "sched" : "none");
}

/* ---------------- */
/*   Writer Tasks   */
/* ---------------- */
/* Production hex_printv callers (prebuilt mesh/audio lib callbacks) build
 * the va_list themselves; wrap identically so the measured window includes
 * va_start/va_end, mirroring hex_print's own. */
static int cp_hex_printv_wrap(uint8_t source, uint32_t log_index,
                              size_t args_cnt, ...)
{
    va_list args;
    va_start(args, args_cnt);
    int ret = ble_log_compressed_hex_printv(source, log_index, args_cnt, args);
    va_end(args);
    return ret;
}

static void perf_writer_task(void *arg)
{
    perf_writer_t *w = arg;
    uint8_t payload[PERF_MAX_PAYLOAD_LEN];
    uint32_t frame_index = 0;

    xSemaphoreGive(w->ready);
    xSemaphoreTake(w->start, portMAX_DELAY);

    while (!w->stop) {
        size_t payload_len = w->profile[frame_index % w->profile_count];
        memcpy(payload, &frame_index, sizeof(frame_index));

        if (w->isolate_write) {
            vTaskSuspendAll();
        }
        uint32_t start_cycles = w->record_cycles ? esp_cpu_get_cycle_count() : 0;

        bool ok;
        if (w->compressed) {
            /* Simulate a compressed host log: log_index + typed args.
             * Payloads are fixed per case so the encode path is
             * deterministic across runs. */
            const perf_compress_cfg_t *cp = w->compress_cfg;
            switch (cp->mode) {
            case PERF_CP_MODE_U32: {
                /* UINT32_MAX rotates 0/1/2 per frame (mixed workload);
                 * fixed counts use a constant non-zero value so the encode
                 * path (worst-case full 4-byte push) is deterministic. */
                uint32_t carg = cp->args == UINT32_MAX ?
                                (uint32_t)frame_index : 0xA5A5A5A5;
                switch (cp->args == UINT32_MAX ?
                        frame_index % 3 : cp->args) {
                case 0:
                    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                                 frame_index % 1000, 0);
                    break;
                case 1:
                    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                                 frame_index % 1000, 1,
                                                 ARG_SIZE_TYPE_U32, carg);
                    break;
                default:
                    /* Variadic layout: all arg types first, then all values. */
                    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                                 frame_index % 1000, 2,
                                                 ARG_SIZE_TYPE_U32, ARG_SIZE_TYPE_U32,
                                                 carg, carg);
                    break;
                }
                break;
            }
            case PERF_CP_MODE_U64:
                ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                             frame_index % 1000, 1,
                                             ARG_SIZE_TYPE_U64, cp->value);
                break;
            case PERF_CP_MODE_STR:
                ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                             frame_index % 1000, 1,
                                             ARG_SIZE_TYPE_STR, cp->str);
                break;
            case PERF_CP_MODE_BUF:
                ble_log_compressed_hex_print_buf(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                                 frame_index % 1000, 0,
                                                 cp->buf, cp->len);
                break;
            case PERF_CP_MODE_VPRINT:
                if (cp->args == 0) {
                    cp_hex_printv_wrap(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                       frame_index % 1000, 0);
                } else {
                    cp_hex_printv_wrap(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                       frame_index % 1000, 1,
                                       ARG_SIZE_TYPE_U64, cp->value);
                }
                break;
            }
            ok = true; /* hex_print returns 0 always */
        } else if (w->ll_flag != UINT32_MAX) {
            ble_log_write_hex_ll(payload_len, payload, w->ll_append_len,
                                 w->ll_append, w->ll_flag);
            ok = true; /* write_hex_ll has no return value */
        } else {
            ok = ble_log_write_hex(w->src, payload, payload_len);
        }

        uint32_t write_cycles = 0;
        if (w->record_cycles) {
            write_cycles = esp_cpu_get_cycle_count() - start_cycles;
        }
        if (w->isolate_write) {
            xTaskResumeAll();
        }

        if (ok) {
            w->cycles->ok_frames++;
            if (w->record_cycles) {
                record_cycles(w->cycles, write_cycles);
            }
        } else {
            w->cycles->failed_frames++;
            if (w->record_cycles) {
                w->cycles->failed_cycles += write_cycles;
            }
        }
        frame_index++;
        taskYIELD();
    }

    xSemaphoreGive(w->done);
    vTaskDelete(NULL);
}

/* ---------------- */
/*     ISR Writer   */
/* ---------------- */
#if CONFIG_BLE_LOG_LL_ENABLED
static void IRAM_ATTR perf_isr_write(void *arg)
{
    perf_isr_t *isr = arg;
    if (isr->stop) {
        return;
    }

    uint32_t seq = (uint32_t)isr->calls;
    /* IRAM-safe 4-byte copy: memcpy may live in flash. */
    isr->payload[0] = (uint8_t)seq;
    isr->payload[1] = (uint8_t)(seq >> 8);
    isr->payload[2] = (uint8_t)(seq >> 16);
    isr->payload[3] = (uint8_t)(seq >> 24);
    uint32_t start = esp_cpu_get_cycle_count();
    ble_log_write_hex_ll(isr->payload_len, isr->payload, 0, NULL,
                         BIT(BLE_LOG_LL_FLAG_ISR));
    isr->total_cycles += esp_cpu_get_cycle_count() - start;
    isr->calls++;
}
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

/* ---------------- */
/*     Sink Task    */
/* ---------------- */
static void observe_perf_frame(const test_ble_log_frame_t *frame, void *ctx)
{
    perf_sink_t *sink = ctx;
    if (frame->src < BLE_LOG_SRC_MAX) {
        sink->frames_by_src[frame->src]++;
    }

    /* Capture enhanced statistics records (written/lost per source).
     * Frames carry a 4-byte os_timestamp prefix before the record. */
    if (frame->src == BLE_LOG_SRC_INTERNAL &&
            frame->payload_len == sizeof(uint32_t) + sizeof(ble_log_enh_stat_t) &&
            frame->payload[sizeof(uint32_t)] == BLE_LOG_INT_SRC_ENH_STAT) {
        ble_log_enh_stat_t stat;
        memcpy(&stat, frame->payload + sizeof(uint32_t), sizeof(stat));
        if (stat.src_code < BLE_LOG_SRC_MAX) {
            sink->stat_written[stat.src_code] = stat.written_frame_cnt;
            sink->stat_lost[stat.src_code] = stat.lost_frame_cnt;
        }
    } else if (frame->src == BLE_LOG_SRC_INTERNAL &&
            frame->payload_len == sizeof(uint32_t) + sizeof(ble_log_final_stat_t) &&
            frame->payload[sizeof(uint32_t)] == BLE_LOG_INT_SRC_FINAL_STAT) {
        sink->stat_final_seen = true;
    }
}

static void perf_sink_task(void *arg)
{
    perf_sink_t *sink = arg;
    uint8_t data[BLE_LOG_TRANS_SIZE];

    while (!sink->stop) {
        size_t len = ble_log_prph_test_read(data, sizeof(data),
                                             pdMS_TO_TICKS(PERF_READ_TIMEOUT_MS),
                                             sink->bytes_per_second);
        if (!len) {
            sink->drained = true;
            continue;
        }
        sink->drained = false;
        sink->transactions++;
        sink->transport_bytes += len;
        if (!test_ble_log_walk_frames(data, len, observe_perf_frame, sink)) {
            sink->walk_errors++;
        }
    }

    xSemaphoreGive(sink->done);
    vTaskDelete(NULL);
}

/* ---------------- */
/*   Main Perf Run  */
/* ---------------- */
static void fill_writer(perf_writer_t *w, const char *name, ble_log_src_t src,
                        uint32_t ll_flag, bool compressed,
                        const perf_run_cfg_t *cfg, perf_cycles_t *cycles,
                        SemaphoreHandle_t start)
{
    w->name = name;
    w->src = src;
    w->ll_flag = ll_flag;
    w->compressed = compressed;
    w->record_cycles = cfg->measure_cycles;
    w->isolate_write = cfg->isolate_write;
    w->ll_append = cfg->ll_append;
    w->ll_append_len = cfg->ll_append_len;
    w->compress_cfg = cfg->compress_cfg;
    w->profile = cfg->profile;
    w->profile_count = cfg->profile_count;
    for (size_t i = 0; i < w->profile_count; i++) {
        TEST_ASSERT_TRUE(w->profile[i] <= PERF_MAX_PAYLOAD_LEN);
    }
    w->cycles = cycles;
    w->ready = xSemaphoreCreateBinary();
    w->start = start;
    w->done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(w->ready);
    TEST_ASSERT_NOT_NULL(w->done);
}

static void run_perf_case(const perf_run_cfg_t *cfg)
{
    /* Histogram-heavy state stays static: Unity's task stack is small. */
    static perf_cycles_t custom_cycles[PERF_CUSTOM_WRITER_COUNT];
    static perf_cycles_t ll_task_cycles;
    static perf_cycles_t ll_hci_cycles;
#if CONFIG_BLE_COMPRESSED_LOG_ENABLE
    static perf_cycles_t compressed_cycles;
#endif
    static perf_writer_t writers[PERF_WRITER_MAX];
    static TaskHandle_t writer_tasks[PERF_WRITER_MAX];
    perf_sink_t sink = {0};
    TaskHandle_t sink_task = NULL;
    esp_timer_handle_t isr_timer = NULL;
    uint32_t writer_count = 0;
    uint32_t i;

    print_run_separator("RUN", cfg);

    memset(&custom_cycles, 0, sizeof(custom_cycles));
    memset(&ll_task_cycles, 0, sizeof(ll_task_cycles));
    memset(&ll_hci_cycles, 0, sizeof(ll_hci_cycles));
#if CONFIG_BLE_COMPRESSED_LOG_ENABLE
    memset(&compressed_cycles, 0, sizeof(compressed_cycles));
    compressed_cycles.min_cycles = UINT32_MAX;
#endif
    for (i = 0; i < PERF_WRITER_MAX; i++) {
        memset(&writers[i], 0, sizeof(writers[i]));
    }
    for (i = 0; i < PERF_CUSTOM_WRITER_COUNT; i++) {
        custom_cycles[i].min_cycles = UINT32_MAX;
    }
    ll_task_cycles.min_cycles = UINT32_MAX;
    ll_hci_cycles.min_cycles = UINT32_MAX;

    bool ll_task = cfg->ll_task;
    bool ll_hci = cfg->ll_hci;
    bool compress = cfg->compress;
    bool isr = cfg->isr;
#if !CONFIG_BLE_LOG_LL_ENABLED
    ll_task = false;
    ll_hci = false;
    isr = false;
#endif
#if !CONFIG_BLE_COMPRESSED_LOG_ENABLE
    compress = false;
#endif

    sink.bytes_per_second = cfg->bytes_per_second;
    sink.done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(sink.done);
    /* All tasks are pinned to core 0: esp_cpu_get_cycle_count() reads the
     * per-core cycle counter, and on dual-core chips (ESP32/S3/S31) a task
     * preempted and resumed on the other core would mix two counters that
     * are not phase-aligned, corrupting the per-frame cycle measurement. */
    TEST_ASSERT_EQUAL(pdPASS,
                      xTaskCreatePinnedToCore(perf_sink_task, "perf_sink",
                                              PERF_SINK_STACK_SIZE, &sink,
                                              uxTaskPriorityGet(NULL) + 1,
                                              &sink_task, 0));

    SemaphoreHandle_t start = xSemaphoreCreateCounting(32, 0);
    TEST_ASSERT_NOT_NULL(start);

    TEST_ASSERT_TRUE(cfg->custom_writer_count <= PERF_CUSTOM_WRITER_COUNT);
    for (i = 0; i < cfg->custom_writer_count; i++) {
        fill_writer(&writers[writer_count], "write_hex", BLE_LOG_SRC_CUSTOM,
                    UINT32_MAX, false, cfg, &custom_cycles[i], start);
        char name[32];
        snprintf(name, sizeof(name), "perf_w%" PRIu32, i);
        TEST_ASSERT_EQUAL(pdPASS,
                          xTaskCreatePinnedToCore(perf_writer_task, name,
                                                  PERF_WRITER_STACK_SIZE,
                                                  &writers[writer_count],
                                                  uxTaskPriorityGet(NULL),
                                                  &writer_tasks[writer_count], 0));
        writer_count++;
    }

#if CONFIG_BLE_LOG_LL_ENABLED
    if (ll_task) {
        fill_writer(&writers[writer_count], "write_hex_ll", BLE_LOG_SRC_LL_TASK,
                    0, false, cfg, &ll_task_cycles, start);
        TEST_ASSERT_EQUAL(pdPASS,
                          xTaskCreatePinnedToCore(perf_writer_task, "perf_ll",
                                                  PERF_WRITER_STACK_SIZE,
                                                  &writers[writer_count],
                                                  uxTaskPriorityGet(NULL),
                                                  &writer_tasks[writer_count], 0));
        writer_count++;
    }

    if (ll_hci) {
        fill_writer(&writers[writer_count], "write_hex_ll_hci", BLE_LOG_SRC_LL_HCI,
                    BIT(BLE_LOG_LL_FLAG_HCI), false, cfg, &ll_hci_cycles, start);
        TEST_ASSERT_EQUAL(pdPASS,
                          xTaskCreatePinnedToCore(perf_writer_task, "perf_hci",
                                                  PERF_WRITER_STACK_SIZE,
                                                  &writers[writer_count],
                                                  uxTaskPriorityGet(NULL),
                                                  &writer_tasks[writer_count], 0));
        writer_count++;
    }
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

#if CONFIG_BLE_COMPRESSED_LOG_ENABLE
    if (compress) {
        fill_writer(&writers[writer_count], "compress", BLE_LOG_SRC_ENCODE,
                    UINT32_MAX, true, cfg, &compressed_cycles, start);
        TEST_ASSERT_EQUAL(pdPASS,
                          xTaskCreatePinnedToCore(perf_writer_task, "perf_cp",
                                                  PERF_WRITER_STACK_SIZE,
                                                  &writers[writer_count],
                                                  uxTaskPriorityGet(NULL),
                                                  &writer_tasks[writer_count], 0));
        writer_count++;
    }
#endif /* CONFIG_BLE_COMPRESSED_LOG_ENABLE */

#if CONFIG_BLE_LOG_LL_ENABLED
    if (isr) {
        memset(&s_perf_isr, 0, sizeof(s_perf_isr));
        s_perf_isr.payload_len = cfg->profile[0];
        const esp_timer_create_args_t isr_args = {
            .callback = perf_isr_write,
            .arg = &s_perf_isr,
            .dispatch_method = ESP_TIMER_ISR,
            .name = "perf_isr",
        };
        TEST_ESP_OK(esp_timer_create(&isr_args, &isr_timer));
    }
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

    for (i = 0; i < writer_count; i++) {
        TEST_ASSERT_EQUAL(pdTRUE,
                          xSemaphoreTake(writers[i].ready, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)xTaskGetCoreID(writer_tasks[i]));
    }

    TEST_ASSERT_TRUE(ble_log_enable(true));
    int64_t run_start_us = esp_timer_get_time();
#if CONFIG_BLE_LOG_LL_ENABLED
    if (isr) {
        TEST_ESP_OK(esp_timer_start_periodic(isr_timer, PERF_ISR_PERIOD_US));
    }
#endif
    for (i = 0; i < writer_count; i++) {
        xSemaphoreGive(start);
    }

    vTaskDelay(pdMS_TO_TICKS(PERF_DURATION_MS));

    for (i = 0; i < writer_count; i++) {
        writers[i].stop = true;
    }
#if CONFIG_BLE_LOG_LL_ENABLED
    if (isr) {
        s_perf_isr.stop = true;
        TEST_ESP_OK(esp_timer_stop(isr_timer));
    }
#endif
    for (i = 0; i < writer_count; i++) {
        TEST_ASSERT_EQUAL(pdTRUE,
                          xSemaphoreTake(writers[i].done, pdMS_TO_TICKS(1000)));
    }
    int64_t run_end_us = esp_timer_get_time();

    /* Let the sink drain everything already queued (no link cap) so the
     * buffer pool is fully recycled before flushing. */
    sink.bytes_per_second = 0;
    TickType_t drain_deadline = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
    while (!sink.drained && (int32_t)(drain_deadline - xTaskGetTickCount()) > 0) {
        vTaskDelay(1);
    }

    uint32_t flush_start = esp_cpu_get_cycle_count();
    ble_log_flush();
    uint32_t flush_cycles = esp_cpu_get_cycle_count() - flush_start;

    TickType_t stat_deadline = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
    while (!sink.stat_final_seen &&
           (int32_t)(stat_deadline - xTaskGetTickCount()) > 0) {
        vTaskDelay(1);
    }
    sink.stop = true;
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sink.done, pdMS_TO_TICKS(1000)));

    uint8_t discard[BLE_LOG_TRANS_SIZE];
    while (ble_log_prph_test_read(discard, sizeof(discard), 0, 0)) {
    }

    uint64_t elapsed_us = run_end_us - run_start_us;
    printf("BLE_LOG_PERF mode=%s profile=%s payload=%uB ",
           cfg->mode_name, cfg->profile_name, cfg->profile[0]);
    print_link(cfg->bytes_per_second);
    printf(" isolate=%s duration=%" PRIu64 " ms\n",
           cfg->isolate_write ? "sched" : "none", elapsed_us / 1000);
    printf("BLE_LOG_PERF flush=%" PRIu32 " cycles\n", flush_cycles);

    uint64_t ok_total = 0;
    uint64_t failed_total = 0;
    for (i = 0; i < cfg->custom_writer_count; i++) {
        char name[32];
        snprintf(name, sizeof(name), "write_hex%" PRIu32, i);
        if (cfg->measure_cycles) {
            print_cycles(name, &custom_cycles[i], false);
        } else {
            print_writer_counts(name, &custom_cycles[i]);
        }
        ok_total += custom_cycles[i].ok_frames;
        failed_total += custom_cycles[i].failed_frames;
    }
#if CONFIG_BLE_LOG_LL_ENABLED
    if (ll_task) {
        if (cfg->measure_cycles) {
            print_cycles("write_hex_ll", &ll_task_cycles, true);
        } else {
            print_writer_counts("write_hex_ll", &ll_task_cycles);
        }
        ok_total += ll_task_cycles.ok_frames;
        failed_total += ll_task_cycles.failed_frames;
    }
    if (ll_hci) {
        if (cfg->measure_cycles) {
            print_cycles("write_hex_ll_hci", &ll_hci_cycles, true);
        } else {
            print_writer_counts("write_hex_ll_hci", &ll_hci_cycles);
        }
        ok_total += ll_hci_cycles.ok_frames;
        failed_total += ll_hci_cycles.failed_frames;
    }
    if (isr) {
        printf("BLE_LOG_PERF writer=%-12s frames=%" PRIu64 " avg=%" PRIu32
               " cycles (call, ISR)\n",
               "write_hex_ll_isr", s_perf_isr.calls,
               s_perf_isr.calls ?
               (uint32_t)(s_perf_isr.total_cycles / s_perf_isr.calls) : 0);
        ok_total += s_perf_isr.calls;
    }
#endif
#if CONFIG_BLE_COMPRESSED_LOG_ENABLE
    if (compress) {
        if (cfg->measure_cycles) {
            print_cycles("compress", &compressed_cycles, true);
        } else {
            print_writer_counts("compress", &compressed_cycles);
        }
        ok_total += compressed_cycles.ok_frames;
        failed_total += compressed_cycles.failed_frames;
        if (cfg->measure_cycles && cfg->custom_writer_count == 1) {
            /* Derived split: the companion write_hex writer in this run
             * measured a plain write at the same record length, in the
             * same conditions. encode = full call - write portion. */
            uint64_t comp_avg = compressed_cycles.ok_frames ?
                                compressed_cycles.total_cycles /
                                compressed_cycles.ok_frames : 0;
            uint64_t ref_avg = custom_cycles[0].ok_frames ?
                               custom_cycles[0].total_cycles /
                               custom_cycles[0].ok_frames : 0;
            printf("BLE_LOG_PERF writer=%-12s frames=%" PRIu64 " avg=%" PRIu64
                   " cycles (write_hex, derived)\n", "compress_wh",
                   custom_cycles[0].ok_frames, ref_avg);
            printf("BLE_LOG_PERF writer=%-12s frames=%" PRIu64 " avg=%" PRIu64
                   " cycles (encode, derived)\n", "compress_enc",
                   compressed_cycles.ok_frames,
                   comp_avg > ref_avg ? comp_avg - ref_avg : 0);
        }
    }
#endif
    uint64_t sink_frames = sink.frames_by_src[BLE_LOG_SRC_CUSTOM] +
                           sink.frames_by_src[BLE_LOG_SRC_LL_TASK] +
                           sink.frames_by_src[BLE_LOG_SRC_LL_HCI] +
                           sink.frames_by_src[BLE_LOG_SRC_LL_ISR] +
                           sink.frames_by_src[BLE_LOG_SRC_ENCODE];
    printf("BLE_LOG_PERF total ok=%" PRIu64 " failed=%" PRIu64
           " sink_frames=%" PRIu64 " sink_bytes=%" PRIu64
           " walk_errors=%" PRIu64 " internal_frames=%" PRIu64 "\n",
           ok_total, failed_total, sink_frames, sink.transport_bytes,
           sink.walk_errors, sink.frames_by_src[BLE_LOG_SRC_INTERNAL]);
    if (!cfg->measure_cycles) {
        print_throughput(sink.transport_bytes, elapsed_us, cfg->bytes_per_second);
    }
    printf("BLE_LOG_PERF stat written=[custom=%" PRIu64 " ll_task=%" PRIu64
           " ll_hci=%" PRIu64 " ll_isr=%" PRIu64 " encode=%" PRIu64
           "] lost=[custom=%" PRIu64 " ll_task=%" PRIu64 " ll_hci=%" PRIu64
           " ll_isr=%" PRIu64 " encode=%" PRIu64 "]\n",
           sink.stat_written[BLE_LOG_SRC_CUSTOM],
           sink.stat_written[BLE_LOG_SRC_LL_TASK],
           sink.stat_written[BLE_LOG_SRC_LL_HCI],
           sink.stat_written[BLE_LOG_SRC_LL_ISR],
           sink.stat_written[BLE_LOG_SRC_ENCODE],
           sink.stat_lost[BLE_LOG_SRC_CUSTOM],
           sink.stat_lost[BLE_LOG_SRC_LL_TASK],
           sink.stat_lost[BLE_LOG_SRC_LL_HCI],
           sink.stat_lost[BLE_LOG_SRC_LL_ISR],
           sink.stat_lost[BLE_LOG_SRC_ENCODE]);

    print_run_separator("END", cfg);
    printf("\n");

    TEST_ASSERT_EQUAL_UINT64(0, sink.walk_errors);
    TEST_ASSERT_GREATER_THAN_UINT64(0, ok_total);
    if (cfg->expect_no_loss) {
        TEST_ASSERT_EQUAL_UINT64(0, failed_total);
        TEST_ASSERT_EQUAL_UINT64(0, sink.stat_lost[BLE_LOG_SRC_CUSTOM]);
        TEST_ASSERT_EQUAL_UINT64(0, sink.stat_lost[BLE_LOG_SRC_LL_TASK]);
        TEST_ASSERT_EQUAL_UINT64(0, sink.stat_lost[BLE_LOG_SRC_LL_HCI]);
        TEST_ASSERT_EQUAL_UINT64(0, sink.stat_lost[BLE_LOG_SRC_LL_ISR]);
        TEST_ASSERT_EQUAL_UINT64(0, sink.stat_lost[BLE_LOG_SRC_ENCODE]);
    } else if (sink.stat_final_seen) {
        TEST_ASSERT_EQUAL_UINT64(failed_total, sink.stat_lost[BLE_LOG_SRC_CUSTOM]);
#if CONFIG_BLE_COMPRESSED_LOG_ENABLE
        if (compress) {
            /* Every hex_print call reaches write_hex(ENCODE) exactly once;
             * the LBM's own counters must account for it, written or lost. */
            TEST_ASSERT_EQUAL_UINT64(compressed_cycles.ok_frames,
                                     sink.stat_written[BLE_LOG_SRC_ENCODE] +
                                     sink.stat_lost[BLE_LOG_SRC_ENCODE]);
        }
#endif
    }

    vSemaphoreDelete(start);
    vSemaphoreDelete(sink.done);
    for (i = 0; i < writer_count; i++) {
        vSemaphoreDelete(writers[i].ready);
        vSemaphoreDelete(writers[i].done);
    }
#if CONFIG_BLE_LOG_LL_ENABLED
    if (isr_timer) {
        TEST_ESP_OK(esp_timer_delete(isr_timer));
    }
#endif
}

static void run_throughput_profile(const char *name, const uint16_t *profile,
                                   size_t count)
{
    uint32_t link_count = sizeof(s_throughput_link_Bps) / sizeof(s_throughput_link_Bps[0]);
    for (size_t i = 0; i < link_count; i++) {
        perf_run_cfg_t cfg = {
            .mode_name = "throughput",
            .profile_name = name,
            .run_idx = (uint32_t)i + 1,
            .run_total = link_count,
            .profile = profile,
            .profile_count = count,
            .bytes_per_second = s_throughput_link_Bps[i],
            .custom_writer_count = PERF_CUSTOM_WRITER_COUNT,
            .ll_task = true,
            .ll_hci = true,
            .compress = true,
            .isr = true,
            .compress_cfg = &s_cp_mixed_u32,
        };
        run_perf_case(&cfg);
    }
}

static void run_cycle_case(const char *name, const uint16_t *profile, size_t count,
                           uint32_t custom_writer_count, bool ll_task, bool ll_hci,
                           const perf_compress_cfg_t *compress_cfg)
{
    perf_run_cfg_t cfg = {
        .mode_name = "cycle",
        .profile_name = name,
        .run_idx = 1,
        .run_total = 1,
        .profile = profile,
        .profile_count = count,
        .bytes_per_second = 0,
        .custom_writer_count = custom_writer_count,
        .ll_task = ll_task,
        .ll_hci = ll_hci,
        .compress = compress_cfg != NULL,
        .measure_cycles = true,
        .isolate_write = true,
        .expect_no_loss = true,
        .compress_cfg = compress_cfg,
    };
    run_perf_case(&cfg);
}

TEST_CASE("BLE Log throughput mixed load fixed 32B", "[ble_log][perf][throughput][ignore]")
{
    run_throughput_profile("fixed32", s_fixed32, 1);
}

TEST_CASE("BLE Log throughput mixed load fixed 64B", "[ble_log][perf][throughput][ignore]")
{
    run_throughput_profile("fixed64", s_fixed64, 1);
}

TEST_CASE("BLE Log throughput mixed load fixed 128B", "[ble_log][perf][throughput][ignore]")
{
    run_throughput_profile("fixed128", s_fixed128, 1);
}

TEST_CASE("BLE Log throughput mixed load mixed 8/16/32/64B", "[ble_log][perf][throughput][ignore]")
{
    run_throughput_profile("mixed_8_16_32_64", s_mixed_8_16_32_64, 4);
}

TEST_CASE("BLE Log write_hex cycles (single writer, link=0)", "[ble_log][perf][cycle][ignore]")
{
    for (size_t i = 0; i < sizeof(s_cycle_sizes) / sizeof(s_cycle_sizes[0]); i++) {
        run_cycle_case("write_hex", &s_cycle_sizes[i], 1, 1, false, false, NULL);
    }
}

/* Saturated link: most writes fail. Measures the drop-path cost and
 * cross-checks client failed counts against the LBM's lost counters. */
TEST_CASE("BLE Log write_hex drop path cycles (link=2Mbps)", "[ble_log][perf][cycle][ignore]")
{
    perf_run_cfg_t cfg = {
        .mode_name = "cycle",
        .profile_name = "write_hex_drop",
        .run_idx = 1,
        .run_total = 1,
        .profile = s_fixed32,
        .profile_count = 1,
        .bytes_per_second = PERF_LINK_2MBPS_BPS,
        .custom_writer_count = 1,
        .measure_cycles = true,
        .isolate_write = true,
    };
    run_perf_case(&cfg);
}

#if CONFIG_BLE_LOG_LL_ENABLED
TEST_CASE("BLE Log write_hex_ll cycles (single writer, link=0)", "[ble_log][perf][cycle][ignore]")
{
    for (size_t i = 0; i < sizeof(s_cycle_sizes) / sizeof(s_cycle_sizes[0]); i++) {
        run_cycle_case("write_hex_ll", &s_cycle_sizes[i], 1, 0, true, false, NULL);
    }
}

/* The controller passes a real append buffer via the write_hex_ll
 * interface; this case covers the second-copy path. */
TEST_CASE("BLE Log write_hex_ll append cycles (32+32B)", "[ble_log][perf][cycle][ignore]")
{
    static uint8_t append_buf[64];
    memset(append_buf, 0x5A, sizeof(append_buf));
    perf_run_cfg_t cfg = {
        .mode_name = "cycle",
        .profile_name = "write_hex_ll_app",
        .run_idx = 1,
        .run_total = 1,
        .profile = s_fixed32,
        .profile_count = 1,
        .custom_writer_count = 0,
        .ll_task = true,
        .measure_cycles = true,
        .isolate_write = true,
        .expect_no_loss = true,
        .ll_append = append_buf,
        .ll_append_len = 32,
    };
    run_perf_case(&cfg);
}
#endif

#if CONFIG_BLE_COMPRESSED_LOG_ENABLE
/* Compressed records are log_index + 0..2 U32 args, not a raw payload
 * length. One case per arg count splits encode vs downstream write_hex
 * cost for each workload shape. */
TEST_CASE("BLE Log compressed write cycles 0 args", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_a0", s_ref3, 1, 1, false, false, &s_cp_u32_0);
}

TEST_CASE("BLE Log compressed write cycles 1 U32 arg", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_a1", s_ref8, 1, 1, false, false, &s_cp_u32_1);
}

TEST_CASE("BLE Log compressed write cycles 2 U32 args", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_a2", s_ref12, 1, 1, false, false, &s_cp_u32_2);
}

TEST_CASE("BLE Log compressed write cycles mixed 0/1/2 args", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_mix", s_fixed32, 1, 0, false, false, &s_cp_mixed_u32);
}

/* CPU-heavy scenarios: U64 leading-zero analysis (clz + partial pushes +
 * nibble packing) and string/buffer copies (strlen/memcpy over the whole
 * payload). The encode-vs-write_hex split shows the CPU-for-memory
 * tradeoff for each shape. */
TEST_CASE("BLE Log compressed write cycles U64 full 8B", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_u64full", s_ref12, 1, 1, false, false, &s_cp_u64_full);
}

TEST_CASE("BLE Log compressed write cycles U64 lz 1B", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_u64lz", s_ref6, 1, 1, false, false, &s_cp_u64_lz);
}

TEST_CASE("BLE Log compressed write cycles U64 zero", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_u64az", s_ref4, 1, 1, false, false, &s_cp_u64_az);
}

TEST_CASE("BLE Log compressed write cycles STR 8B", "[ble_log][perf][cycle][ignore]")
{
    memset(s_cp_str_buf, 'x', 8);
    s_cp_str_buf[8] = '\0';
    s_cp_str_cfg.mode = PERF_CP_MODE_STR;
    s_cp_str_cfg.str = s_cp_str_buf;
    run_cycle_case("compress_s8", s_ref13, 1, 1, false, false, &s_cp_str_cfg);
}

TEST_CASE("BLE Log compressed write cycles STR 128B", "[ble_log][perf][cycle][ignore]")
{
    memset(s_cp_str_buf, 'x', 128);
    s_cp_str_buf[128] = '\0';
    s_cp_str_cfg.mode = PERF_CP_MODE_STR;
    s_cp_str_cfg.str = s_cp_str_buf;
    run_cycle_case("compress_s128", s_ref133, 1, 1, false, false, &s_cp_str_cfg);
}

TEST_CASE("BLE Log compressed write cycles raw buffer 128B", "[ble_log][perf][cycle][ignore]")
{
    memset(s_cp_raw_buf, 0xA5, sizeof(s_cp_raw_buf));
    s_cp_buf_cfg.mode = PERF_CP_MODE_BUF;
    s_cp_buf_cfg.buf = s_cp_raw_buf;
    s_cp_buf_cfg.len = sizeof(s_cp_raw_buf);
    run_cycle_case("compress_buf128", s_ref133, 1, 1, false, false, &s_cp_buf_cfg);
}

TEST_CASE("BLE Log compressed write cycles hex_printv 0 args", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_v0", s_ref3, 1, 1, false, false, &s_cp_v0);
}

TEST_CASE("BLE Log compressed write cycles hex_printv U64 full", "[ble_log][perf][cycle][ignore]")
{
    run_cycle_case("compress_vu64", s_ref12, 1, 1, false, false, &s_cp_vu64);
}
#endif
