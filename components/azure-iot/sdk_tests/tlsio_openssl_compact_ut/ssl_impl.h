// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is made an integral part of tlsio_openssl_compact.c with a #include. It
// is broken out for readability. 

#ifndef SSL_ERRORS_H
#define SSL_ERRORS_H

// 
#define SSL_ERROR__plus__WANT_READ -2
#define SSL_ERROR__plus__WANT_WRITE -3
#define SSL_ERROR__plus__HARD_FAIL -4

#define SSL_ERROR_HARD_FAIL 99
#define SSL_Good_Ptr (void*)22
#define SSL_Good_Context_Ptr (SSL_CTX*)33
#define SSL_Good_Socket 44
#define SSL_CONNECT_SUCCESS 0
#define SSL_SET_FD_SUCCESS 1
#define SSL_SET_FD_FAILURE 0
#define SSL_READ_NO_DATA 0

#define SSL_Get_IPv4_OK (uint32_t)0x11223344
#define SSL_Get_IPv4_FAIL 0

#define SSL_good_port_number  447
#define SSL_port_number_too_low -1
#define SSL_port_number_too_high 0xffff + 1
#define SSL_good_host_name "fakehost.com"
#define SSL_good_old_host_name "fakehost.com"
uint8_t* SSL_send_buffer = (uint8_t*)"111111112222222233333333";
size_t SSL_send_message_size = sizeof(SSL_send_buffer) - 1;

#define DOWORK_RECV_XFER_BUFFER_SIZE 64
#define SSL_TEST_MESSAGE_SIZE 64
#define SSL_WRITE_MAX_TEST_SIZE 60
#define SSL_SHORT_SENT_MESSAGE_SIZE 30
#define SSL_FAIL_ME_SENT_MESSAGE_SIZE 1700
#define SSL_SHORT_RECEIVED_MESSAGE_SIZE 15
#define SSL_LONG_RECEIVED_MESSAGE_SIZE 1500

static size_t fake_read_byte_out_count = 0;
static size_t fake_read_current_byte_out_count = 0;
static size_t fake_read_bytes_received = 0;


// The fact that SSL_get_error requires the previous error allows a mocking strategy that
// permits encoding the extended error into the main failure
static int my_SSL_get_error(SSL* ssl, int callReturn)
{
    (void)ssl;
    switch (callReturn)
    {
    case SSL_ERROR__plus__WANT_READ:
        return SSL_ERROR_WANT_READ;
    case SSL_ERROR__plus__WANT_WRITE:
        return SSL_ERROR_WANT_WRITE;
    case SSL_ERROR__plus__HARD_FAIL:
        return SSL_ERROR_HARD_FAIL;
    }
    // This foolish-looking code dances around conflicting warnings
    // in the C and C++ compilers about no return path
    if (callReturn <= 0)
    {
        ASSERT_FAIL("bad enum");
    }
    return 0;
}

static void init_fake_read(size_t byte_count)
{
    fake_read_byte_out_count = byte_count;
    fake_read_current_byte_out_count = 0;
    fake_read_bytes_received = 0;
}

static void ASSERT_BYTE_RECEIVED(uint8_t byte)
{
    ASSERT_ARE_EQUAL(size_t, (size_t)byte, (size_t)(fake_read_bytes_received % 256));
    fake_read_bytes_received++;
}

int my_SSL_read(SSL* ssl, uint8_t* buffer, size_t size)
{
    size_t bytes_to_receive;
    size_t i;
    (void)size;
    ASSERT_ARE_EQUAL(size_t, (size_t)ssl, (size_t)SSL_Good_Ptr);
    bytes_to_receive = fake_read_byte_out_count - fake_read_current_byte_out_count;
    bytes_to_receive = bytes_to_receive <= size ? bytes_to_receive : size;
    for (i = 0; i < bytes_to_receive; i++)
    {
        buffer[i] = (uint8_t)(fake_read_current_byte_out_count % 256);
        fake_read_current_byte_out_count++;
    }
    return (int)bytes_to_receive;
}

int my_SSL_write(SSL* ssl, uint8_t* buffer, size_t size)
{
    int result;
    // "Send" no more than SSL_WRITE_MAX_TEST_SIZE bytes
    (void)buffer; // not used
    ASSERT_ARE_EQUAL(size_t, (size_t)ssl, (size_t)SSL_Good_Ptr);
    if (size == SSL_FAIL_ME_SENT_MESSAGE_SIZE)
    {
        result = SSL_ERROR__plus__HARD_FAIL;
    }
    else
    {
        if (size > SSL_WRITE_MAX_TEST_SIZE)
        {
            result = SSL_WRITE_MAX_TEST_SIZE;
        }
        else
        {
            result = (int)size;
        }
    }
    return result;
}

/////////////////////////////////////////////////////////////////////
//  Empty functions. These must be available to call, but they have no effect
int TLSv1_2_client_method() { return 0; }
void SSL_CTX_set_default_read_buffer_len(SSL_CTX* dummy, int dummy2) { (void)dummy; (void)dummy2; }

// End of empty functions
/////////////////////////////////////////////////////////////////////

#endif // SSL_ERRORS_H
