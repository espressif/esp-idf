// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is made an integral part of tlsio_openssl_compact.c with a #include. It
// is broken out for readability. 

#ifndef CALLBACKS_H
#define CALLBACKS_H


#define MAX_MESSAGE_COUNT 3

// Keep track of whether callbacks were performed as expected
static int on_io_open_complete_call_count;
static bool on_io_open_complete_context_ok;
static IO_OPEN_RESULT on_io_open_complete_result;

static int on_io_error_call_count;
static bool on_io_error_context_ok;

static int on_io_close_call_count;
static bool on_io_close_context_ok;

static int on_io_send_complete_call_count;
static bool on_io_send_complete_context_ok;
static IO_SEND_RESULT on_io_send_complete_result[MAX_MESSAGE_COUNT];

static int on_bytes_received_call_count;
static bool on_bytes_received_context_ok;

// Context pointers for the callbacks
#define IO_OPEN_COMPLETE_CONTEXT (void*)55
#define IO_ERROR_CONTEXT (void*)66
#define IO_BYTES_RECEIVED_CONTEXT (void*)77
#define IO_CLOSE_COMPLETE_CONTEXT (void*)231
#define IO_SEND_COMPLETE_CONTEXT (void*)7658

static void reset_callback_context_records()
{
    int i;
    on_io_open_complete_call_count = 0;
    on_io_open_complete_context_ok = true;
    on_io_open_complete_result = (IO_OPEN_RESULT)-1;
    on_io_error_call_count = 0;
    on_io_error_context_ok = true;
    on_io_close_call_count = 0;
    on_io_close_context_ok = true;
    on_io_send_complete_call_count = 0;
    on_io_send_complete_context_ok = true;
    for (i = 0; i < MAX_MESSAGE_COUNT; i++)
    {
        on_io_send_complete_result[i] = (IO_SEND_RESULT)-1;
    }
    on_bytes_received_call_count = 0;
    on_bytes_received_context_ok = true;
}

// Callbacks used by the tlsio adapter

static void on_io_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    bool result_valid = open_result == IO_OPEN_OK || open_result == IO_OPEN_ERROR || open_result == IO_OPEN_CANCELLED;
    ASSERT_IS_TRUE_WITH_MSG(result_valid, "Invalid IO_OPEN_RESULT");
    on_io_open_complete_call_count++;
    on_io_open_complete_result = open_result;
    if (context != IO_OPEN_COMPLETE_CONTEXT)
    {
        on_io_open_complete_context_ok = false;
    }
}

static void on_io_send_complete(void* context, IO_SEND_RESULT send_result)
{
    on_io_send_complete_result[on_io_send_complete_call_count] = send_result;
    on_io_send_complete_call_count++;
    if (context != IO_SEND_COMPLETE_CONTEXT)
    {
        on_io_send_complete_context_ok = false;
    }
}

static void on_io_close_complete(void* context)
{
    on_io_close_call_count++;
    if (context != IO_CLOSE_COMPLETE_CONTEXT)
    {
        on_io_close_context_ok = false;
    }
}

static void on_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    size_t i;
    on_bytes_received_call_count++;

    for (i = 0; i < size; i++)
    {
        ASSERT_BYTE_RECEIVED(buffer[i]);
    }
    if (context != IO_BYTES_RECEIVED_CONTEXT)
    {
        on_bytes_received_context_ok = false;
    }
}

static void on_io_error(void* context)
{
    on_io_error_call_count = true;
    if (context != IO_ERROR_CONTEXT)
    {
        on_io_error_context_ok = false;
    }
}

static void ASSERT_IO_ERROR_CALLBACK(bool called)
{
    int count = called ? 1 : 0;
    ASSERT_ARE_EQUAL_WITH_MSG(int, count, on_io_error_call_count, "io_error_callback count mismatch");
    if (count > 0)
    {
        ASSERT_IS_TRUE_WITH_MSG(on_io_error_context_ok, "io_error_callback missing context");
    }
}

static void ASSERT_IO_OPEN_CALLBACK(bool called, int open_result)
{
    if (called)
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 1, on_io_open_complete_call_count, "on_io_open_complete_callback count mismatch");
        ASSERT_ARE_EQUAL_WITH_MSG(int, on_io_open_complete_result, open_result, "on_io_open_complete result mismatch");
        ASSERT_IS_TRUE_WITH_MSG(on_io_open_complete_context_ok, "io_open_complete_context not passed");
    }
    else
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 0, on_io_open_complete_call_count, "unexpected on_io_open_complete_callback");
    }
}

static void ASSERT_IO_SEND_CALLBACK(bool called, int send_result)
{
    if (called)
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 1, on_io_send_complete_call_count, "on_io_send_complete_callback count mismatch");
        ASSERT_ARE_EQUAL_WITH_MSG(int, on_io_send_complete_result[0], send_result, "on_io_send_complete result mismatch");
        ASSERT_IS_TRUE_WITH_MSG(on_io_send_complete_context_ok, "io_send_complete_context not passed");
    }
    else
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 0, on_io_open_complete_call_count, "unexpected on_io_open_complete_callback");
    }
}

static void ASSERT_IO_SEND_ABANDONED(int count)
{
    int i;
    ASSERT_ARE_EQUAL_WITH_MSG(int, count, on_io_send_complete_call_count, "on_io_send_complete_callback count mismatch");
    ASSERT_IS_TRUE_WITH_MSG(on_io_send_complete_context_ok, "io_send_complete_context not passed");
    for (i = 0; i < on_io_send_complete_call_count; i++)
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, IO_SEND_CANCELLED, on_io_send_complete_result[i], "send result should be IO_SEND_CANCELLED");
    }
}

static void ASSERT_IO_CLOSE_CALLBACK(bool called)
{
    if (called)
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 1, on_io_close_call_count, "on_io_close_complete_callback count mismatch");
        ASSERT_IS_TRUE_WITH_MSG(on_io_close_context_ok, "io_close_complete_context not passed");
    }
    else
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 0, on_io_close_call_count, "unexpected on_io_close_complete_callback");
    }
}

static void ASSERT_BYTES_RECEIVED_CALLBACK(bool called, size_t message_size)
{
    if (called)
    {
        ASSERT_ARE_EQUAL_WITH_MSG(size_t, message_size, fake_read_bytes_received, "bytes_received count mismatch");
        ASSERT_IS_TRUE_WITH_MSG(on_bytes_received_context_ok, "bytes_received_context not passed");
    }
    else
    {
        ASSERT_ARE_EQUAL_WITH_MSG(int, 0, on_bytes_received_call_count, "unexpected bytes_received_callback");
    }
}

static void ASSERT_NO_CALLBACKS()
{
    ASSERT_IO_ERROR_CALLBACK(false);
    ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
    ASSERT_IO_SEND_CALLBACK(false, IO_SEND_ERROR);
    ASSERT_IO_CLOSE_CALLBACK(false);
    ASSERT_BYTES_RECEIVED_CALLBACK(false, 0);
}

#endif // CALLBACKS_H
