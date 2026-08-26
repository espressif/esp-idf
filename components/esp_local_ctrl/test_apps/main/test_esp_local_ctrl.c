/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "unity.h"
#include "esp_local_ctrl.h"
#include "esp_local_ctrl_priv.h"   /* full definition of esp_local_ctrl_transport */
#include "esp_heap_caps.h"

/*
 * Minimal transport with all function pointers NULL.
 *
 * esp_local_ctrl_start() guards every transport callback with an `if` before
 * calling it, so a zero-initialised transport lets the service start with an
 * in-memory protocomm instance only — no BLE or HTTPD hardware required.
 */
static const esp_local_ctrl_transport_t s_null_transport = { 0 };

static esp_err_t dummy_get_prop_values(size_t props_count,
                                       const esp_local_ctrl_prop_t props[],
                                       esp_local_ctrl_prop_val_t prop_values[],
                                       void *usr_ctx)
{
    return ESP_OK;
}

static esp_err_t dummy_set_prop_values(size_t props_count,
                                       const esp_local_ctrl_prop_t props[],
                                       const esp_local_ctrl_prop_val_t prop_values[],
                                       void *usr_ctx)
{
    return ESP_OK;
}

static esp_local_ctrl_config_t make_config(size_t max_properties)
{
    esp_local_ctrl_config_t cfg = {
        .transport        = &s_null_transport,
        .transport_config = { .httpd = NULL },
        .proto_sec        = { .version = PROTOCOM_SEC0 },
        .handlers = {
            .get_prop_values = dummy_get_prop_values,
            .set_prop_values = dummy_set_prop_values,
        },
        .max_properties   = max_properties,
    };
    return cfg;
}

/*
 * Test: remove a non-last property, then stop.
 *
 *   1. Add prop_a (slot 0) and prop_b (slot 1).  props_count = 2.
 *   2. Remove prop_a:
 *        - prop_a is freed.
 *        - Compaction shifts prop_b into slot 0.
 *   3. esp_local_ctrl_stop() must free prop_b exactly once (slot 0).
 */
TEST_CASE("remove_property then stop does not double-free", "[esp_local_ctrl]")
{
    esp_local_ctrl_config_t cfg = make_config(5);
    TEST_ESP_OK(esp_local_ctrl_start(&cfg));

    const esp_local_ctrl_prop_t prop_a = { .name = "prop_a" };
    const esp_local_ctrl_prop_t prop_b = { .name = "prop_b" };
    TEST_ESP_OK(esp_local_ctrl_add_property(&prop_a));
    TEST_ESP_OK(esp_local_ctrl_add_property(&prop_b));

    /* Removing the first property triggers compaction. */
    TEST_ESP_OK(esp_local_ctrl_remove_property("prop_a"));

    /* prop_b must still be reachable after compaction */
    TEST_ASSERT_NOT_NULL(esp_local_ctrl_get_property("prop_b"));

    TEST_ESP_OK(esp_local_ctrl_stop());
}

/* Test: remove the only property then stop — no compaction, basic sanity. */
TEST_CASE("remove sole property then stop is clean", "[esp_local_ctrl]")
{
    esp_local_ctrl_config_t cfg = make_config(3);
    TEST_ESP_OK(esp_local_ctrl_start(&cfg));

    const esp_local_ctrl_prop_t prop = { .name = "only_prop" };
    TEST_ESP_OK(esp_local_ctrl_add_property(&prop));
    TEST_ESP_OK(esp_local_ctrl_remove_property("only_prop"));

    /* After removing the sole property, stop must find nothing to free. */
    TEST_ESP_OK(esp_local_ctrl_stop());
}

/* Test: multiple compaction passes (remove head repeatedly) then stop. */
TEST_CASE("repeated head removal then stop is clean", "[esp_local_ctrl]")
{
    esp_local_ctrl_config_t cfg = make_config(5);
    TEST_ESP_OK(esp_local_ctrl_start(&cfg));

    const esp_local_ctrl_prop_t p0 = { .name = "p0" };
    const esp_local_ctrl_prop_t p1 = { .name = "p1" };
    const esp_local_ctrl_prop_t p2 = { .name = "p2" };
    TEST_ESP_OK(esp_local_ctrl_add_property(&p0));
    TEST_ESP_OK(esp_local_ctrl_add_property(&p1));
    TEST_ESP_OK(esp_local_ctrl_add_property(&p2));

    /* First removal: p1→slot0, p2→slot1, slot2 stale (fixed to NULL). */
    TEST_ESP_OK(esp_local_ctrl_remove_property("p0"));
    /* Second removal: p2→slot0, slot1 stale (fixed to NULL). */
    TEST_ESP_OK(esp_local_ctrl_remove_property("p1"));

    TEST_ASSERT_NOT_NULL(esp_local_ctrl_get_property("p2"));
    TEST_ESP_OK(esp_local_ctrl_stop());
}

/* Test: add/remove/re-add cycle — the slot vacated by a remove must accept a
 * new property without corrupting internal bookkeeping. */
TEST_CASE("add remove add cycle is clean", "[esp_local_ctrl]")
{
    esp_local_ctrl_config_t cfg = make_config(3);
    TEST_ESP_OK(esp_local_ctrl_start(&cfg));

    const esp_local_ctrl_prop_t pa = { .name = "a" };
    const esp_local_ctrl_prop_t pb = { .name = "b" };
    const esp_local_ctrl_prop_t pc = { .name = "c" };

    TEST_ESP_OK(esp_local_ctrl_add_property(&pa));
    TEST_ESP_OK(esp_local_ctrl_add_property(&pb));
    TEST_ESP_OK(esp_local_ctrl_remove_property("a"));

    /* Reuse the freed slot capacity for a new property. */
    TEST_ESP_OK(esp_local_ctrl_add_property(&pc));

    TEST_ASSERT_NULL(esp_local_ctrl_get_property("a"));
    TEST_ASSERT_NOT_NULL(esp_local_ctrl_get_property("b"));
    TEST_ASSERT_NOT_NULL(esp_local_ctrl_get_property("c"));

    TEST_ESP_OK(esp_local_ctrl_stop());
}
