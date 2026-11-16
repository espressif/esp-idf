/*
 * SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>

#include "policy_core.h"

#define POLICY_RECORD_MAGIC 0x4c4f5042u

uint32_t policy_crc32(uint32_t crc, const void *data, uint32_t len)
{
    const uint8_t *bytes = (const uint8_t *)data;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= bytes[i];
        for (int bit = 0; bit < 8; bit++) {
            uint32_t mask = (uint32_t)(-(int32_t)(crc & 1u));
            crc = (crc >> 1) ^ (0xedb88320u & mask);
        }
    }
    return crc;
}

static void set_label(policy_label_t destination, const char *source)
{
    memset(destination, 0, POLICY_LABEL_LEN);
    if (source == NULL) {
        return;
    }
    for (size_t i = 0; i < POLICY_LABEL_LEN - 1 && source[i] != '\0'; i++) {
        destination[i] = source[i];
    }
}

static bool within_crashloop_window(const policy_state_t *state, const policy_config_t *config)
{
    if (config->crashloop_window_ms == 0 || state->last_boot_confirmed) {
        return false;
    }
    return state->last_boot_uptime_ms <= config->crashloop_window_ms;
}

void policy_decide(const policy_state_t *state, const policy_config_t *config, policy_decision_t *decision)
{
    memset(decision, 0, sizeof(*decision));
    const char *current = state->bootable_count > 0 ? state->bootable[0] : "";

    bool exhausted = config->max_attempts > 0 && state->attempts >= config->max_attempts;
    bool crashloop = exhausted && within_crashloop_window(state, config);
    bool counting = config->max_attempts > 0;

    if (exhausted) {
        bool can_fall_back = config->enable_fallback && state->bootable_count > 1;
        decision->fallback_unavailable = !can_fall_back;
        decision->fallback_used = can_fall_back;
        set_label(decision->target, can_fall_back ? state->bootable[1] : current);
        decision->clear_attempts = true;
        decision->safe_mode = config->enable_safe_mode && (crashloop || !can_fall_back);
        if (crashloop) {
            decision->reason = POLICY_REASON_CRASH_LOOP;
        } else if (!can_fall_back) {
            decision->reason = POLICY_REASON_NO_FALLBACK;
        } else {
            decision->reason = POLICY_REASON_ATTEMPTS_EXHAUSTED;
        }
        return;
    }

    set_label(decision->target, current);
    if (state->last_boot_confirmed) {
        decision->clear_attempts = state->attempts > 0;
        decision->reason = POLICY_REASON_NORMAL;
        return;
    }

    decision->increment_attempts = counting;
    decision->reason = state->attempts == 0 ? POLICY_REASON_FIRST_BOOT : POLICY_REASON_NORMAL;
    decision->attempts_after = counting ? (uint8_t)(state->attempts + 1) : 0;
}

void policy_record_init(policy_record_t *record)
{
    memset(record, 0, sizeof(*record));
    record->magic = POLICY_RECORD_MAGIC;
    record->version = POLICY_RECORD_VERSION;
    record->size = POLICY_RECORD_SIZE;
}

uint32_t policy_record_encode(const policy_record_t *record, uint8_t out[POLICY_RECORD_SIZE])
{
    policy_record_t copy = *record;
    copy.magic = POLICY_RECORD_MAGIC;
    copy.version = POLICY_RECORD_VERSION;
    copy.size = POLICY_RECORD_SIZE;
    copy.crc = policy_crc32(UINT32_MAX, &copy, POLICY_RECORD_SIZE - sizeof(copy.crc));
    memcpy(out, &copy, POLICY_RECORD_SIZE);
    return copy.crc;
}

bool policy_record_decode(const uint8_t in[POLICY_RECORD_SIZE], policy_record_t *record)
{
    memcpy(record, in, POLICY_RECORD_SIZE);
    if (record->magic != POLICY_RECORD_MAGIC || record->version != POLICY_RECORD_VERSION ||
        record->size != POLICY_RECORD_SIZE) {
        return false;
    }
    return policy_crc32(UINT32_MAX, in, POLICY_RECORD_SIZE - sizeof(record->crc)) == record->crc;
}
