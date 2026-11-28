/*
 * SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    POLICY_REASON_FIRST_BOOT = 0,
    POLICY_REASON_NORMAL,
    POLICY_REASON_ATTEMPTS_EXHAUSTED,
    POLICY_REASON_CRASH_LOOP,
    POLICY_REASON_NO_FALLBACK,
} policy_reason_t;

#define POLICY_LABEL_LEN 16
typedef char policy_label_t[POLICY_LABEL_LEN];

typedef struct {
    uint8_t max_attempts; /* 0 disables attempt accounting */
    uint32_t crashloop_window_ms;
    bool enable_fallback;
    bool enable_safe_mode;
} policy_config_t;

typedef struct {
    uint8_t attempts;
    uint32_t last_boot_uptime_ms;
    bool last_boot_confirmed;
    uint8_t bootable_count;
    policy_label_t bootable[4];
} policy_state_t;

typedef struct {
    policy_label_t target;
    bool increment_attempts;
    bool clear_attempts;
    bool safe_mode;
    bool fallback_used;
    bool fallback_unavailable;
    uint8_t attempts_after;
    policy_reason_t reason;
} policy_decision_t;

void policy_decide(const policy_state_t *state, const policy_config_t *config, policy_decision_t *decision);

#define POLICY_RECORD_VERSION 1
#define POLICY_RECORD_SIZE 40

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t attempts;
    uint32_t rtc_ms;
    uint32_t uptime_ms;
    uint32_t reset_reason;
    uint8_t reason;
    uint8_t flags;
    uint8_t padding[6];
    uint32_t crc;
} policy_record_t;

#define POLICY_RECORD_FLAG_SAFE_MODE 0x01
#define POLICY_RECORD_FLAG_FALLBACK 0x02
#define POLICY_RECORD_FLAG_CONFIRMED 0x04
#define POLICY_RECORD_FLAG_NO_FALLBACK 0x08

_Static_assert(sizeof(policy_record_t) == POLICY_RECORD_SIZE, "policy_record_t must match the on-flash size");

/* CRC-32/ISO-HDLC without the final XOR, the variant esp_rom_crc32_le() produces. */
uint32_t policy_crc32(uint32_t crc, const void *data, uint32_t len);

void policy_record_init(policy_record_t *record);
uint32_t policy_record_encode(const policy_record_t *record, uint8_t out[POLICY_RECORD_SIZE]);
bool policy_record_decode(const uint8_t in[POLICY_RECORD_SIZE], policy_record_t *record);
