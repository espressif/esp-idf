/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "mqtt_client.h"

typedef enum {NONE, TCP, SSL, WS, WSS} transport_t;

typedef struct {
    esp_mqtt_client_handle_t mqtt_client;
    void * data;
} command_context_t;

typedef struct {
    transport_t selected_transport;
    char *pattern;
    int pattern_repetitions;
    int qos;
    char *expected;
    size_t expected_size;
    size_t nr_of_msg_received;
    size_t nr_of_msg_expected;
    char * received_data;
} publish_context_t ;

typedef struct {
    struct arg_str *uri;
    struct arg_int *test_case;
    struct arg_end *end;
} connection_args_t;

typedef struct {
    struct arg_int *expected_to_publish;
    struct arg_int *qos;
    struct arg_int *enqueue;
    struct arg_end *end;
} publish_args_t;

typedef struct {
    struct arg_str *transport;
    struct arg_str *pattern;
    struct arg_int *pattern_repetitions;
    struct arg_end *end;
} publish_setup_args_t;

void publish_init_flags(void);
void publish_setup(command_context_t * ctx, char const *  transport);
void publish_teardown(command_context_t * ctx);
void publish_test(command_context_t * ctx, int expect_to_publish, int qos, bool enqueue);
void connection_test(command_context_t * ctx, const char *uri, int test_case);
void connect_setup(command_context_t * ctx);
void connect_teardown(command_context_t * ctx);
