/*
 * SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>

#include "bootloader_common.h"
#include "esp_private/bootloader_flash_internal.h"
#include "esp_flash_partitions.h"
#include "esp_rom_sys.h"
#include "policy_core.h"

#define POLICY_MAX_ATTEMPTS 3
#define POLICY_CRASHLOOP_WINDOW_MS 5000

#define POLICY_TABLE_ENTRIES_SCANNED 16
#define POLICY_MAX_BOOTABLE 4
#define POLICY_SECTOR_SIZE 0x1000
#define POLICY_RECORDS_PER_SECTOR (POLICY_SECTOR_SIZE / POLICY_RECORD_SIZE)
#define POLICY_JOURNAL_LABEL "bootpolicy"

typedef struct {
    policy_label_t bootable[POLICY_MAX_BOOTABLE];
    uint8_t bootable_count;
    bool have_journal;
    uint32_t journal_offset;
    uint32_t journal_sectors;
    bool have_otadata;
    esp_partition_pos_t otadata_pos;
} inventory_t;

/* Keeps the weak hooks in the link, see the README. */
void bootloader_hooks_include(void)
{
}

static uint32_t next_index(const inventory_t *inventory, uint32_t index)
{
    uint32_t total = inventory->journal_sectors * POLICY_RECORDS_PER_SECTOR;
    return (index + 1) % total;
}

static bool read_record(uint32_t journal_offset, uint32_t index, policy_record_t *record)
{
    uint8_t raw[POLICY_RECORD_SIZE];
    size_t address = journal_offset + (size_t)index * POLICY_RECORD_SIZE;
    if (bootloader_flash_read(address, raw, sizeof(raw), false) != ESP_OK) {
        return false;
    }
    return policy_record_decode(raw, record);
}

static bool append_record(const inventory_t *inventory, uint32_t index, const policy_record_t *record)
{
    size_t address = inventory->journal_offset + (size_t)index * POLICY_RECORD_SIZE;
    if (index % POLICY_RECORDS_PER_SECTOR == 0) {
        uint32_t sector = inventory->journal_offset / POLICY_SECTOR_SIZE + index / POLICY_RECORDS_PER_SECTOR;
        if (bootloader_flash_erase_sector(sector) != ESP_OK) {
            return false;
        }
    }
    uint8_t raw[POLICY_RECORD_SIZE];
    policy_record_encode(record, raw);
    return bootloader_flash_write(address, raw, sizeof(raw), false) == ESP_OK;
}

static bool recover(const inventory_t *inventory, uint32_t *head_index, policy_record_t *head)
{
    *head_index = 0;
    bool found_head = false;
    uint32_t best_sector = 0;
    uint32_t best_sequence = 0;

    for (uint32_t sector = 0; sector < inventory->journal_sectors; sector++) {
        policy_record_t first;
        if (!read_record(inventory->journal_offset, sector * POLICY_RECORDS_PER_SECTOR, &first)) {
            continue;
        }
        if (!found_head || first.sequence > best_sequence) {
            found_head = true;
            best_sector = sector;
            best_sequence = first.sequence;
        }
    }
    if (!found_head) {
        return false;
    }

    bool found = false;
    for (uint32_t step = 0; step < 2 && step < inventory->journal_sectors; step++) {
        uint32_t sector = (best_sector + step) % inventory->journal_sectors;
        for (uint32_t slot = 0; slot < POLICY_RECORDS_PER_SECTOR; slot++) {
            uint32_t index = sector * POLICY_RECORDS_PER_SECTOR + slot;
            policy_record_t candidate;
            if (!read_record(inventory->journal_offset, index, &candidate)) {
                continue;
            }
            if (!found || candidate.sequence > head->sequence) {
                found = true;
                *head_index = index;
                *head = candidate;
            }
        }
    }
    return found;
}

static void inventory_load(inventory_t *inventory)
{
    memset(inventory, 0, sizeof(*inventory));
    esp_partition_info_t entry;

    for (int index = 0; index < POLICY_TABLE_ENTRIES_SCANNED; index++) {
        size_t offset = CONFIG_PARTITION_TABLE_OFFSET + index * sizeof(entry);
        if (bootloader_flash_read(offset, &entry, sizeof(entry), false) != ESP_OK) {
            break;
        }
        if (entry.magic != ESP_PARTITION_MAGIC) {
            break;
        }
        if (entry.type == PART_TYPE_APP && inventory->bootable_count < POLICY_MAX_BOOTABLE) {
            memcpy(inventory->bootable[inventory->bootable_count], entry.label, POLICY_LABEL_LEN);
            inventory->bootable[inventory->bootable_count][POLICY_LABEL_LEN - 1] = '\0';
            inventory->bootable_count++;
        } else if (entry.type == PART_TYPE_DATA && entry.subtype == PART_SUBTYPE_DATA_OTA) {
            inventory->have_otadata = true;
            inventory->otadata_pos = entry.pos;
        } else if (entry.type == PART_TYPE_DATA &&
                   strncmp((const char *)entry.label, POLICY_JOURNAL_LABEL, POLICY_LABEL_LEN) == 0) {
            inventory->have_journal = true;
            inventory->journal_offset = entry.pos.offset;
            inventory->journal_sectors = entry.pos.size / POLICY_SECTOR_SIZE;
        }
    }
}

/* With application rollback enabled, an image is marked valid in otadata once it is
 * accepted, either during startup or by the application. Treating that as a confirmation
 * keeps this policy in agreement with the rollback state. */
static bool otadata_slot_confirmed(const inventory_t *inventory)
{
    if (!inventory->have_otadata) {
        return false;
    }
    esp_ota_select_entry_t entries[2];
    if (bootloader_common_read_otadata(&inventory->otadata_pos, entries) != ESP_OK) {
        return false;
    }
    int active = bootloader_common_get_active_otadata(entries);
    return active >= 0 && entries[active].ota_state == ESP_OTA_IMG_VALID;
}

void bootloader_after_init(void)
{
    inventory_t inventory;
    inventory_load(&inventory);

    policy_state_t state;
    memset(&state, 0, sizeof(state));
    state.bootable_count = inventory.bootable_count;
    for (uint8_t i = 0; i < inventory.bootable_count; i++) {
        memcpy(state.bootable[i], inventory.bootable[i], POLICY_LABEL_LEN);
    }

    uint32_t index = 0;
    uint32_t sequence = 1;
    bool recovered = false;
    policy_record_t head;
    memset(&head, 0, sizeof(head));

    bool have_journal = inventory.have_journal && inventory.journal_sectors > 0;
    if (have_journal) {
        uint32_t head_index = 0;
        recovered = recover(&inventory, &head_index, &head);
        if (recovered) {
            state.attempts = (uint8_t)head.attempts;
            state.last_boot_uptime_ms = head.uptime_ms;
            state.last_boot_confirmed = (head.flags & POLICY_RECORD_FLAG_CONFIRMED) != 0;
            index = next_index(&inventory, head_index);
            sequence = head.sequence + 1;
        }
    }

    if (otadata_slot_confirmed(&inventory)) {
        state.last_boot_confirmed = true;
    }

    const policy_config_t config = {
        .max_attempts = POLICY_MAX_ATTEMPTS,
        .crashloop_window_ms = POLICY_CRASHLOOP_WINDOW_MS,
        .enable_fallback = true,
        .enable_safe_mode = true,
    };

    policy_decision_t decision;
    policy_decide(&state, &config, &decision);

    bool journaled = false;
    if (have_journal) {
        policy_record_t record;
        policy_record_init(&record);
        record.sequence = sequence;
        record.attempts = decision.attempts_after;
        record.rtc_ms = 0;
        record.uptime_ms = state.last_boot_uptime_ms;
        record.reason = (uint8_t)decision.reason;
        /* The confirmation flag is the application's report and is only written by
         * confirm_image(). Propagating it here would make a single confirmation look like a
         * permanently healthy image and hide later failures. */
        record.flags = (decision.safe_mode ? POLICY_RECORD_FLAG_SAFE_MODE : 0) |
                       (decision.fallback_used ? POLICY_RECORD_FLAG_FALLBACK : 0) |
                       (decision.fallback_unavailable ? POLICY_RECORD_FLAG_NO_FALLBACK : 0);
        journaled = append_record(&inventory, index, &record);
    }

    esp_rom_printf("bootloader_policy: bootable=%u attempts=%u->%u target=%s reason=%u fallback=%u safe=%u journal=%s\n",
                   state.bootable_count, state.attempts, decision.attempts_after, decision.target,
                   (unsigned)decision.reason, (unsigned)decision.fallback_used, (unsigned)decision.safe_mode,
                   have_journal ? (journaled ? "written" : "failed") : "missing");
}
