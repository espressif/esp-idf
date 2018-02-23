// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is made an integral part of tlsio_openssl_compact.c with a #include. It
// is broken out for readability. 

#ifndef TEST_DEFINES_H
#define TEST_DEFINES_H

#define GOOD_DNS_ASYNC_HANDLE (void*)0x12345678

#define SETOPTION_PV_COUNT 3
#define OPEN_PV_COUNT 4
#define SEND_PV_COUNT 4
#define CLOSE_PV_COUNT 2

static TLSIO_CONFIG good_config = { SSL_good_host_name, SSL_good_port_number, NULL, NULL };
static TLSIO_CONFIG tlsio_config = { NULL, SSL_good_port_number, NULL, NULL };

static bool bool_true = true;
static bool bool_false = false;
static size_t sizeof_bool = sizeof(bool);

typedef struct
{
    TLSIO_CONFIG* config;
    const char* fail_msg;
} create_parameters_t;

void populate_create_parameters(create_parameters_t* p, TLSIO_CONFIG* config, const char* hostname, int port, const char* fail_msg)
{
    p->config = config;
    if (config != NULL)
    {
        config->hostname = hostname;
        config->port = port;
        config->underlying_io_interface = NULL;
        config->underlying_io_parameters = NULL;
    }
    p->fail_msg = fail_msg;
}

#endif // TEST_DEFINES_H
