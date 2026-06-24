/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_bit_defs.h"
#include "esp_cpu.h"

static void dummy_handler(void *arg)
{
    volatile uint32_t *counter = (volatile uint32_t *)arg;
    if (counter != NULL) {
        (*counter)++;
    }
}

static bool is_valid_desc_type(esp_cpu_intr_type_t type)
{
    return type == ESP_CPU_INTR_TYPE_LEVEL
           || type == ESP_CPU_INTR_TYPE_EDGE
           || type == ESP_CPU_INTR_TYPE_NA;
}

static int find_test_interrupt(void)
{
    const int core_id = esp_cpu_get_core_id();

    for (int intr_num = 0; intr_num < SOC_CPU_INTR_NUM; intr_num++) {
        esp_cpu_intr_desc_t intr_desc;
        esp_cpu_intr_get_desc(core_id, intr_num, &intr_desc);

        if (intr_desc.flags & (ESP_CPU_INTR_DESC_FLAG_SPECIAL | ESP_CPU_INTR_DESC_FLAG_RESVD)) {
            continue;
        }
        if (esp_cpu_intr_has_handler(intr_num)) {
            continue;
        }
        return intr_num;
    }
    return -1;
}

TEST_CASE("CPU intr: descriptor is valid for all interrupt lines", "[cpu][cpu_intr]")
{
    const int core_id = esp_cpu_get_core_id();
    const uint32_t valid_flags = ESP_CPU_INTR_DESC_FLAG_SPECIAL | ESP_CPU_INTR_DESC_FLAG_RESVD;

    for (int intr_num = 0; intr_num < SOC_CPU_INTR_NUM; intr_num++) {
        esp_cpu_intr_desc_t intr_desc;
        esp_cpu_intr_get_desc(core_id, intr_num, &intr_desc);

        TEST_ASSERT_TRUE(is_valid_desc_type(intr_desc.type));
        TEST_ASSERT_TRUE(intr_desc.priority >= -1);
        TEST_ASSERT_EQUAL_HEX32(0, intr_desc.flags & ~valid_flags);
    }
}

TEST_CASE("CPU intr: handler and arg can be set and cleared", "[cpu][cpu_intr]")
{
    int intr_num = find_test_interrupt();
    if (intr_num < 0) {
        TEST_IGNORE_MESSAGE("No free interrupt line was found for the handler test");
    }

    volatile uint32_t counter = 0;

    esp_cpu_intr_set_handler(intr_num, dummy_handler, (void *)&counter);
    TEST_ASSERT_TRUE(esp_cpu_intr_has_handler(intr_num));
#if !__XTENSA__
    TEST_ASSERT_EQUAL_PTR(&counter, esp_cpu_intr_get_handler_arg(intr_num));
#endif

    esp_cpu_intr_set_handler(intr_num, NULL, NULL);
    TEST_ASSERT_FALSE(esp_cpu_intr_has_handler(intr_num));
}

TEST_CASE("CPU intr: enabled mask can be toggled for a free line", "[cpu][cpu_intr]")
{
    int intr_num = find_test_interrupt();
    if (intr_num < 0) {
        TEST_IGNORE_MESSAGE("No free interrupt line was found for the enabled mask test");
    }

    const uint32_t bit = BIT(intr_num);
    const uint32_t old_mask = esp_cpu_intr_get_enabled_mask();

    esp_cpu_intr_disable(bit);
    TEST_ASSERT_EQUAL_HEX32(0, esp_cpu_intr_get_enabled_mask() & bit);

    esp_cpu_intr_enable(bit);
    TEST_ASSERT_NOT_EQUAL(0, esp_cpu_intr_get_enabled_mask() & bit);

    if ((old_mask & bit) == 0) {
        esp_cpu_intr_disable(bit);
    } else {
        esp_cpu_intr_enable(bit);
    }
}

#if SOC_CPU_HAS_FLEXIBLE_INTC
TEST_CASE("CPU intr: type can be changed on a free line", "[cpu][cpu_intr]")
{
    int intr_num = find_test_interrupt();
    if (intr_num < 0) {
        TEST_IGNORE_MESSAGE("No free interrupt line was found for the type test");
    }

    esp_cpu_intr_type_t old_type = esp_cpu_intr_get_type(intr_num);
    esp_cpu_intr_type_t new_type = (old_type == ESP_CPU_INTR_TYPE_EDGE) ? ESP_CPU_INTR_TYPE_LEVEL : ESP_CPU_INTR_TYPE_EDGE;

    esp_cpu_intr_set_type(intr_num, new_type);
    TEST_ASSERT_EQUAL(new_type, esp_cpu_intr_get_type(intr_num));
    esp_cpu_intr_set_type(intr_num, old_type);
}

TEST_CASE("CPU intr: priority can be changed on a free line", "[cpu][cpu_intr]")
{
    int intr_num = find_test_interrupt();
    if (intr_num < 0) {
        TEST_IGNORE_MESSAGE("No free interrupt line was found for the priority test");
    }

    int old_priority = esp_cpu_intr_get_priority(intr_num);
    int new_priority = (old_priority == 1) ? 2 : 1;

    esp_cpu_intr_set_priority(intr_num, new_priority);
    TEST_ASSERT_EQUAL(new_priority, esp_cpu_intr_get_priority(intr_num));
    esp_cpu_intr_set_priority(intr_num, old_priority);
}
#endif

TEST_CASE("CPU intr: edge ack is callable", "[cpu][cpu_intr]")
{
    int intr_num = find_test_interrupt();
    if (intr_num < 0) {
        TEST_IGNORE_MESSAGE("No free interrupt line was found for the edge ack test");
    }

#if SOC_CPU_HAS_FLEXIBLE_INTC
    esp_cpu_intr_type_t old_type = esp_cpu_intr_get_type(intr_num);
    esp_cpu_intr_set_type(intr_num, ESP_CPU_INTR_TYPE_EDGE);
    esp_cpu_intr_edge_ack(intr_num);
    esp_cpu_intr_set_type(intr_num, old_type);
#else
    const int core_id = esp_cpu_get_core_id();
    esp_cpu_intr_desc_t intr_desc;
    esp_cpu_intr_get_desc(core_id, intr_num, &intr_desc);
    if (intr_desc.type != ESP_CPU_INTR_TYPE_EDGE) {
        TEST_IGNORE_MESSAGE("No free edge interrupt line was found for the edge ack test");
    }
    esp_cpu_intr_edge_ack(intr_num);
#endif
}
