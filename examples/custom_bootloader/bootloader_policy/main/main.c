/*
 * SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_partition.h"
#include "esp_timer.h"
#include "policy_core.h"
#include "sdkconfig.h"

#define POLICY_JOURNAL_LABEL "bootpolicy"
#define POLICY_SECTOR_SIZE 0x1000
#define POLICY_RECORDS_PER_SECTOR (POLICY_SECTOR_SIZE / POLICY_RECORD_SIZE)

static const esp_partition_t *journal_partition(void)
{
    return esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, POLICY_JOURNAL_LABEL);
}

static bool read_record(const esp_partition_t *partition, uint32_t index, policy_record_t *record)
{
    uint8_t raw[POLICY_RECORD_SIZE];
    if (esp_partition_read(partition, (size_t)index * POLICY_RECORD_SIZE, raw, sizeof(raw)) != ESP_OK) {
        return false;
    }
    return policy_record_decode(raw, record);
}

static bool find_newest(const esp_partition_t *partition, uint32_t *index, policy_record_t *newest, uint32_t *count)
{
    uint32_t total = partition->size / POLICY_RECORD_SIZE;
    bool found = false;
    *count = 0;
    for (uint32_t i = 0; i < total; i++) {
        policy_record_t record;
        if (!read_record(partition, i, &record)) {
            continue;
        }
        (*count)++;
        if (!found || record.sequence > newest->sequence) {
            found = true;
            *index = i;
            *newest = record;
        }
    }
    return found;
}

static void report(void)
{
    const esp_partition_t *partition = journal_partition();
    if (partition == NULL) {
        printf("bootloader_policy: journal partition '%s' not found\n", POLICY_JOURNAL_LABEL);
        return;
    }

    uint32_t index = 0;
    uint32_t count = 0;
    policy_record_t newest;
    memset(&newest, 0, sizeof(newest));
    if (!find_newest(partition, &index, &newest, &count)) {
        printf("bootloader_policy: journal empty\n");
        return;
    }

    printf("bootloader_policy: records=%u latest_seq=%u attempts=%u reason=%u safe_mode=%u confirmed=%u\n",
           (unsigned)count, (unsigned)newest.sequence, (unsigned)newest.attempts, (unsigned)newest.reason,
           (unsigned)(newest.flags & POLICY_RECORD_FLAG_SAFE_MODE ? 1 : 0),
           (unsigned)(newest.flags & POLICY_RECORD_FLAG_CONFIRMED ? 1 : 0));
}

static void confirm_image(void)
{
    const esp_partition_t *partition = journal_partition();
    if (partition == NULL) {
        return;
    }

    uint32_t index = 0;
    uint32_t count = 0;
    policy_record_t newest;
    memset(&newest, 0, sizeof(newest));
    bool found = find_newest(partition, &index, &newest, &count);

    uint32_t next = found ? (index + 1) % (partition->size / POLICY_RECORD_SIZE) : 0;
    policy_record_t record;
    policy_record_init(&record);
    record.sequence = found ? newest.sequence + 1 : 1;
    record.attempts = 0;
    record.uptime_ms = (uint32_t)(esp_timer_get_time() / 1000);
    record.reason = POLICY_REASON_NORMAL;
    record.flags = POLICY_RECORD_FLAG_CONFIRMED;

    if (next % POLICY_RECORDS_PER_SECTOR == 0) {
        esp_partition_erase_range(partition, (size_t)next * POLICY_RECORD_SIZE, POLICY_SECTOR_SIZE);
    }
    uint8_t raw[POLICY_RECORD_SIZE];
    policy_record_encode(&record, raw);
    if (esp_partition_write(partition, (size_t)next * POLICY_RECORD_SIZE, raw, sizeof(raw)) == ESP_OK) {
        printf("bootloader_policy: image confirmed at uptime %u ms\n", (unsigned)record.uptime_ms);
    }
}

void app_main(void)
{
    report();
#if CONFIG_EXAMPLE_CONFIRM_IMAGE
    confirm_image();
#else
    printf("bootloader_policy: this image does not confirm itself, see main/Kconfig.projbuild\n");
#endif
}
