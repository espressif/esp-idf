#pragma once

#include "esp_compiler.h"

typedef struct {
    const char * char_star;
    const char char_array[10];
    int x;
    float y;
    struct var_struct_t {
    } var_struct;
} g_netif_test_struct_t;

#define NETIF_TEST_STRUCT_EMPTY() \
    {   \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(char_star) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(char_array) \
        .x = 0, \
        .y = 0.0, \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(var_struct) \
     }

#define NETIF_TEST_STRUCT_DEFAULT() \
    {   \
        .char_star = "Espressif", \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(char_array, "Espressif") \
        .x = 42, \
        .y = 42.192, \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(var_struct) \
     }
