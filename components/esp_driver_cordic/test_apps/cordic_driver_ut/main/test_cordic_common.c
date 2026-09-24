/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "driver/cordic.h"

TEST_CASE("cordic acquire-release test", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_handle_t engine2 = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_acquire_engine(&engine_config, &engine));

    // Multiple users share the one and only hardware, everyone gets the same handle
    TEST_ESP_OK(cordic_acquire_engine(&engine_config, &engine2));
    TEST_ASSERT_EQUAL(engine, engine2);

    // Hardware is only torn down when the last reference is gone
    TEST_ESP_OK(cordic_release_engine(engine2));
    TEST_ESP_OK(cordic_release_engine(engine));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, cordic_release_engine(engine));

    // There is a single CORDIC instance on this chip
    cordic_engine_config_t invalid_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
        .instance_id = 100, // Invalid instance ID
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, cordic_acquire_engine(&invalid_config, &engine));
}

TEST_CASE("new/delete must not mix with acquire/release", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_handle_t other = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    TEST_ESP_OK(cordic_acquire_engine(&engine_config, &engine));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, cordic_new_engine(&engine_config, &other));
    TEST_ESP_OK(cordic_release_engine(engine));

    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, cordic_acquire_engine(&engine_config, &other));
    TEST_ESP_OK(cordic_delete_engine(engine));
#pragma GCC diagnostic pop
}
