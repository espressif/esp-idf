// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>

#ifdef __cplusplus
#else
#endif

/**
 * Include the C standards here.
 */
#ifdef __cplusplus
#include <cstddef>
#include <cstdlib>
#include <ctime>
#else
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#endif

/**
 * The gballoc.h will replace the malloc, free, and realloc by the my_gballoc functions, in this case,
 *    if you define these mock functions after include the gballoc.h, you will create an infinity recursion,
 *    so, places the my_gballoc functions before the #include "azure_c_shared_utility/gballoc.h"
 */
#include "gballoc_ut_impl_1.h"

 /**
 * Include the mockable headers here.
 * These are the headers that contains the functions that you will replace to execute the test.
 */
#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "dns_async.h"
#include "socket_async.h"
#include "openssl/ssl.h"

static int my_socket_async_is_create_complete(SOCKET_ASYNC_HANDLE sock, bool* is_complete)
{
    (void)sock;
    *is_complete = true;
    return 0;
}
#undef ENABLE_MOCKS

/**
 * Include the test tools.
 */
#include "tlsio_pal.h"
#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"

const IO_INTERFACE_DESCRIPTION* tlsio_id;
// These "headers" are actually source files that are broken out of this file for readability
//#include "unit_test_api.h"
#include "ssl_impl.h"
#include "callbacks.h"
#include "test_defines.h"
#include "gballoc_ut_impl_2.h"

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

/**
 * This is necessary for the test suite, just keep as is.
 */
static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;
static bool negative_mocks_used = false;


BEGIN_TEST_SUITE(tlsio_openssl_compact_unittests)

    TEST_SUITE_INITIALIZE(a)
    {
        int result;
        size_t type_size;
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);
        tlsio_id = tlsio_pal_get_interface_description();

        (void)umock_c_init(on_umock_c_error);

        result = umocktypes_charptr_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);
        result = umocktypes_bool_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);
        umocktypes_stdint_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);

        REGISTER_UMOCK_ALIAS_TYPE(SSL, void*);
        REGISTER_UMOCK_ALIAS_TYPE(SSL_CTX, void*);
        REGISTER_UMOCK_ALIAS_TYPE(SOCKET_ASYNC_OPTIONS_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(SOCKET_ASYNC_HANDLE, int);
        REGISTER_UMOCK_ALIAS_TYPE(DNS_ASYNC_HANDLE, void*);
        type_size = sizeof(time_t);
        if (type_size == sizeof(uint64_t))
        {
            REGISTER_UMOCK_ALIAS_TYPE(time_t, uint64_t);
        }
        else if (type_size == sizeof(uint32_t))
        {
            REGISTER_UMOCK_ALIAS_TYPE(time_t, uint32_t);
        }
        else
        {
            ASSERT_FAIL("Bad size_t size");
        }

        REGISTER_GLOBAL_MOCK_RETURNS(dns_async_create, GOOD_DNS_ASYNC_HANDLE, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(dns_async_is_lookup_complete, true, false);
        REGISTER_GLOBAL_MOCK_RETURNS(dns_async_get_ipv4, SSL_Get_IPv4_OK, SSL_Get_IPv4_FAIL);

        REGISTER_GLOBAL_MOCK_RETURNS(socket_async_create, SSL_Good_Socket, -1);
        REGISTER_GLOBAL_MOCK_HOOK(socket_async_is_create_complete, my_socket_async_is_create_complete);

        REGISTER_GLOBAL_MOCK_RETURNS(SSL_new, SSL_Good_Ptr, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_CTX_new, SSL_Good_Context_Ptr, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_set_fd, SSL_SET_FD_SUCCESS, SSL_SET_FD_FAILURE);
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_connect, SSL_CONNECT_SUCCESS, SSL_ERROR__plus__HARD_FAIL);
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_get_error, SSL_ERROR_WANT_READ, SSL_ERROR_HARD_FAIL);
        REGISTER_GLOBAL_MOCK_HOOK(SSL_write, my_SSL_write);
        REGISTER_GLOBAL_MOCK_HOOK(SSL_read, my_SSL_read);
        REGISTER_GLOBAL_MOCK_HOOK(SSL_get_error, my_SSL_get_error);

        /**
         * Or you can combine, for example, in the success case malloc will call my_gballoc_malloc, and for
         *    the failed cases, it will return NULL.
         */
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

        tlsio_config.hostname = SSL_good_old_host_name;
    }

    static void use_negative_mocks()
    {
        int negativeTestsInitResult = umock_c_negative_tests_init();
        negative_mocks_used = true;
        ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);
    }

    /**
     * The test suite will call this function to cleanup your machine.
     * It is called only once, after all tests is done.
     */
    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    /**
     * The test suite will call this function to prepare the machine for the new test.
     * It is called before execute each test.
     */
    TEST_FUNCTION_INITIALIZE(initialize)
    {
        if (TEST_MUTEX_ACQUIRE(g_testByTest))
        {
            ASSERT_FAIL("Could not acquire test serialization mutex.");
        }

        umock_c_reset_all_calls();
        reset_callback_context_records();
		init_gballoc_checks();
    }

    /**
     * The test suite will call this function to cleanup your machine for the next test.
     * It is called after execute each test.
     */
    TEST_FUNCTION_CLEANUP(cleans)
    {
		if (negative_mocks_used)
        {
            negative_mocks_used = false;
            umock_c_negative_tests_deinit();
        }
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    static void open_helper(CONCRETE_IO_HANDLE tlsio)
    {
        int open_result;
        reset_callback_context_records();
        open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);

        // Pump dowork until it opens
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (finishes Open)
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);
    }

    /* Tests_SRS_TLSIO_30_201: [ The "high-level retry sequence" shall succeed after an injected fault which causes on_io_error to be called. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__retry_open_after_io_failure__succeeds)
    {
        int send_result;
        int open_result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        reset_callback_context_records();
        open_helper(tlsio);

        // Queue up the message to eventually fail on
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_FAIL_ME_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_FAIL_ME_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, 0, send_result);

        umock_c_reset_all_calls();
        reset_callback_context_records();
        // Make sure the io fails
        tlsio_id->concrete_io_dowork(tlsio);
        ASSERT_IO_ERROR_CALLBACK(true);

        // Close the error'd tlsio
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, IO_CLOSE_COMPLETE_CONTEXT);
        ASSERT_IO_CLOSE_CALLBACK(true);

        // Retry the open
        open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);

        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)

        reset_callback_context_records();

        ///act
        // dowork_poll_open_ssl (done)
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        // Check that we got the on_open callback for our retry
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
		tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_200: [ The "high-level retry sequence" shall succeed after an injected fault which causes on_io_open_complete to return with IO_OPEN_ERROR. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__retry_open_after_open_failure__succeeds)
    {
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        int open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
        umock_c_reset_all_calls();

        // dowork_poll_open_ssl (done) Fail the SSL_connect call
        STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_ERROR__plus__HARD_FAIL);

        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (done)
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_ERROR);

        // Close the error'd tlsio
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);

        // Retry the open
        umock_c_reset_all_calls();
        open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)
        reset_callback_context_records();

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        // Check that we got the on_open callback for our retry
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_050: [ If the tlsio_handle parameter is NULL, tlsio_close shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_055: [ If the on_io_close_complete parameter is NULL, tlsio_close shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_054: [ On failure, the adapter shall not call on_io_close_complete. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__close_parameter_validation__fails)
    {
        int k;
        bool p0[CLOSE_PV_COUNT];
        ON_IO_CLOSE_COMPLETE p1[CLOSE_PV_COUNT];
        const char* fm[CLOSE_PV_COUNT];
        int i;

        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);

        umock_c_reset_all_calls();

        k = 0;
        p0[k] = false; p1[k] = on_io_close_complete; fm[k] = "Unexpected close success when tlsio_handle is NULL"; /* */  k++;
        p0[k] = true; p1[k] = NULL; /*           */ fm[k] = "Unexpected close success when on_io_close_complete is NULL"; k++;

        // Cycle through each failing combo of parameters
        for (i = 0; i < CLOSE_PV_COUNT; i++)
        {
            int close_result;
            reset_callback_context_records();
            ///arrange

            ///act
            close_result = tlsio_id->concrete_io_close(p0[i] ? tlsio : NULL, p1[i], IO_CLOSE_COMPLETE_CONTEXT);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, close_result, fm[i]);
            ASSERT_IO_CLOSE_CALLBACK(false);
        }

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_009: [ The phrase "enter TLSIO_STATE_EXT_CLOSING" means the adapter shall iterate through any unsent messages in the queue and shall delete each message after calling its on_send_complete with the associated callback_context and IO_SEND_CANCELLED . ]*/
	/* Tests_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__close_with_unsent_messages__succeeds)
    {
        ///arrange
        int send_result;
        int close_result;
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        reset_callback_context_records();
        open_helper(tlsio);

        // Make sure the arrangement is correct
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);

        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);


        umock_c_reset_all_calls();
        STRICT_EXPECTED_CALL(SSL_shutdown(SSL_Good_Ptr));
        STRICT_EXPECTED_CALL(SSL_free(SSL_Good_Ptr));
        STRICT_EXPECTED_CALL(SSL_CTX_free(SSL_Good_Context_Ptr));
        STRICT_EXPECTED_CALL(socket_async_destroy(SSL_Good_Socket));
        // Message 1 delete
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        // Message 2 delete
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        reset_callback_context_records();
        // End of arrange

        ///act
		close_result = tlsio_id->concrete_io_close(tlsio, on_io_close_complete, IO_CLOSE_COMPLETE_CONTEXT);

        ///assert
		ASSERT_ARE_EQUAL(int, 0, close_result);
        ASSERT_IO_CLOSE_CALLBACK(true);
        ASSERT_IO_SEND_ABANDONED(2); // 2 messages in this test
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());


        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SSRS_TLSIO_30_053: [ If the adapter is in any state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR then tlsio_close_async shall log that tlsio_close_async has been called and then continue normally. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__close_while_closed__succeeds)
    {
        CONCRETE_IO_HANDLE tlsio;
        int close_result;
        ///arrange
        reset_callback_context_records();
        tlsio_id = tlsio_pal_get_interface_description();
        tlsio = tlsio_id->concrete_io_create(&good_config);
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_OK);
        umock_c_reset_all_calls();
        reset_callback_context_records();

        ///act
		close_result = tlsio_id->concrete_io_close(tlsio, on_io_close_complete, IO_CLOSE_COMPLETE_CONTEXT);

        ///assert
		ASSERT_ARE_EQUAL(int, 0, close_result);
		ASSERT_IO_CLOSE_CALLBACK(true);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_009: [ The phrase "enter TLSIO_STATE_EXT_CLOSING" means the adapter shall iterate through any unsent messages in the queue and shall delete each message after calling its on_send_complete with the associated callback_context and IO_SEND_CANCELLED. ]*/
	/* Tests_SRS_TLSIO_30_006: [ The phrase "enter TLSIO_STATE_EXT_CLOSED" means the adapter shall forcibly close any existing connections then call the on_io_close_complete function and pass the on_io_close_complete_context that was supplied in tlsio_close_async. ]*/
	/* Tests_SRS_TLSIO_30_056: [ On success the adapter shall enter TLSIO_STATE_EX_CLOSING. ]*/
	/* Tests_SRS_TLSIO_30_051: [ On success, if the underlying TLS does not support asynchronous closing, then the adapter shall enter TLSIO_STATE_EX_CLOSED immediately after entering TLSIO_STATE_EX_CLOSING. ]*/
    /* Tests_SRS_TLSIO_30_052: [ On success tlsio_close  shall return 0. ]*/
	// For this case, tlsio_openssl_compact_open has been called previously
    TEST_FUNCTION(tlsio_openssl_compact__close_after_open__succeeds)
    {
        int close_result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        reset_callback_context_records();
        open_helper(tlsio);

        // Make sure the arrangement is correct
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);

        umock_c_reset_all_calls();
        STRICT_EXPECTED_CALL(SSL_shutdown(SSL_Good_Ptr));
        STRICT_EXPECTED_CALL(SSL_free(SSL_Good_Ptr));
        STRICT_EXPECTED_CALL(SSL_CTX_free(SSL_Good_Context_Ptr));
        STRICT_EXPECTED_CALL(socket_async_destroy(SSL_Good_Socket));
        reset_callback_context_records();
        // End of arrange

        ///act
        close_result = tlsio_id->concrete_io_close(tlsio, on_io_close_complete, IO_CLOSE_COMPLETE_CONTEXT);

        ///assert
		ASSERT_ARE_EQUAL(int, 0, close_result);
		ASSERT_IO_CLOSE_CALLBACK(true);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_053: [ If the adapter is in any state other than TLSIO_STATE_EXT_OPEN or TLSIO_STATE_EXT_ERROR then tlsio_close_async shall log that tlsio_close_async has been called and then continue normally. ]*/
    /* Tests_SRS_TLSIO_30_057: [ On success, if the adapter is in TLSIO_STATE_EXT_OPENING, it shall call on_io_open_complete with the on_io_open_complete_context supplied in tlsio_open_async and IO_OPEN_CANCELLED. This callback shall be made before changing the internal state of the adapter. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__close_while_opening__succeeds)
    {
        int open_result;
        CONCRETE_IO_HANDLE tlsio; 
        int close_result;
        ///arrange
        reset_callback_context_records();
        tlsio = tlsio_id->concrete_io_create(&good_config);
        open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
        umock_c_reset_all_calls();

        // dowork_poll_dns (waiting)
        STRICT_EXPECTED_CALL(dns_async_is_lookup_complete(GOOD_DNS_ASYNC_HANDLE)).SetReturn(false);

        // dowork_poll_dns (done)
        STRICT_EXPECTED_CALL(dns_async_is_lookup_complete(GOOD_DNS_ASYNC_HANDLE));
        STRICT_EXPECTED_CALL(dns_async_get_ipv4(GOOD_DNS_ASYNC_HANDLE));
        STRICT_EXPECTED_CALL(dns_async_destroy(GOOD_DNS_ASYNC_HANDLE));
        STRICT_EXPECTED_CALL(socket_async_create(SSL_Get_IPv4_OK, SSL_good_port_number, false, NULL));

        // dowork_poll_socket (waiting)
        STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_false, sizeof_bool);

        // dowork_poll_socket (done)
        STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_true, sizeof_bool);
        STRICT_EXPECTED_CALL(SSL_CTX_new(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_new(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(SSL_set_fd(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

        // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_READ)
        STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_ERROR__plus__WANT_READ);
        STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, SSL_ERROR__plus__WANT_READ));

        // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_WRITE)
        STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, SSL_ERROR__plus__WANT_WRITE));

        // dowork_poll_open_ssl (done)

        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (waiting)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (waiting)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_READ)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_WRITE)

        // Make sure the arrangement is correct so far
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_OK);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        umock_c_reset_all_calls();
        STRICT_EXPECTED_CALL(SSL_free(SSL_Good_Ptr));
        STRICT_EXPECTED_CALL(SSL_CTX_free(SSL_Good_Context_Ptr));
        STRICT_EXPECTED_CALL(socket_async_destroy(SSL_Good_Socket));
        reset_callback_context_records();
        // End of arrange

        ///act
        close_result = tlsio_id->concrete_io_close(tlsio, on_io_close_complete, IO_CLOSE_COMPLETE_CONTEXT);

        ///assert
		ASSERT_ARE_EQUAL(int, 0, close_result);
		ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_CANCELLED);
        ASSERT_IO_CLOSE_CALLBACK(true);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_063: [ The tlsio_openssl_compact_send shall enqueue for transmission the on_send_complete, the callback_context, the size, and the contents of buffer. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__send__succeeds)
	{
        int send_result;
		///arrange
		CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
		open_helper(tlsio);
		reset_callback_context_records();
		umock_c_reset_all_calls();

		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // PENDING_SOCKET_IO
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // message bytes
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // singlylinkedlist_add

		///act
		send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
			SSL_send_message_size, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);

		///assert
		ASSERT_ARE_EQUAL_WITH_MSG(int, send_result, 0, "Unexpected send failure");

		///cleanup
		tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
		tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_064: [ If the supplied message cannot be enqueued for transmission, tlsio_openssl_compact_send shall log an error and return FAILURE. ]*/
	/* Tests_SRS_TLSIO_30_066: [ On failure, on_send_complete shall not be called. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__send_unhappy_paths__fails)
	{
		///arrange
        size_t i;
		use_negative_mocks();

		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // PENDING_TRANSMISSION
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // message bytes
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // singlylinkedlist_add
		umock_c_negative_tests_snapshot();

		for (i = 0; i < umock_c_negative_tests_call_count(); i++)
		{
            int send_result;
			///arrange
			CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
			open_helper(tlsio);
			reset_callback_context_records();

			umock_c_reset_all_calls();

			umock_c_negative_tests_reset();
			umock_c_negative_tests_fail_call(i);

			///act
			send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
				SSL_send_message_size, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);

			///assert
			ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, send_result, 0, "Unexpected send success on unhappy path");
            ASSERT_IO_SEND_CALLBACK(false, IO_SEND_ERROR);

			///cleanup
			tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
			tlsio_id->concrete_io_destroy(tlsio);
		}

		///cleanup
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_060: [ If the tlsio_handle parameter is NULL, tlsio_openssl_compact_send shall log an error and return FAILURE. ]*/
	/* Tests_SRS_TLSIO_30_061: [ If the buffer is NULL, tlsio_openssl_compact_send shall log the error and return FAILURE. ]*/
	/* Tests_SRS_TLSIO_30_062: [ If the on_send_complete is NULL, tlsio_openssl_compact_send shall log the error and return FAILURE. ]*/
	/* Tests_SRS_TLSIO_30_067: [ If the  size  is 0,  tlsio_send  shall log the error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_066: [ On failure, on_send_complete shall not be called. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__send_parameter_validation__fails)
	{
		// Parameters arrays
		bool p0[SEND_PV_COUNT];
		const void* p1[SEND_PV_COUNT];
		size_t p2[SEND_PV_COUNT];
		ON_SEND_COMPLETE p3[SEND_PV_COUNT];
		const char* fm[SEND_PV_COUNT];
        int k;
        int i;

		///arrange
		CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
		open_helper(tlsio);

		k = 0;
		p0[k] = false; p1[k] = SSL_send_buffer; p2[k] = SSL_send_message_size; p3[k] = on_io_send_complete; fm[k] = "Unexpected send success when tlsio_handle is NULL"; k++;
		p0[k] = true; p1[k] = NULL; /*       */ p2[k] = SSL_send_message_size; p3[k] = on_io_send_complete; fm[k] = "Unexpected send success when send buffer is NULL"; k++;
		p0[k] = true; p1[k] = SSL_send_buffer; p2[k] = 0; /*                */ p3[k] = on_io_send_complete; fm[k] = "Unexpected send success when size is 0"; k++;
		p0[k] = true; p1[k] = SSL_send_buffer; p2[k] = SSL_send_message_size; p3[k] = NULL; /*           */ fm[k] = "Unexpected send success when on_send_complete is NULL"; k++;

		// Cycle through each failing combo of parameters
		for (i = 0; i < SEND_PV_COUNT; i++)
		{
            int send_result;
			///arrange
			reset_callback_context_records();

			///act
			send_result = tlsio_id->concrete_io_send(p0[i] ? tlsio : NULL, p1[i], p2[i], p3[i], IO_SEND_COMPLETE_CONTEXT);

			///assert
			ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, send_result, 0, fm[i]);
			ASSERT_IO_SEND_CALLBACK(false, IO_SEND_ERROR);

			///cleanup
		}

		///cleanup
		tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
		tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_065: [ If tlsio_openssl_compact_open has not been called or the opening process has not been completed, tlsio_openssl_compact_send shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_066: [ On failure, on_send_complete shall not be called. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__send_not_open__fails)
	{
        int send_result;
		///arrange
		CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
		reset_callback_context_records();

		///act
		send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
			SSL_send_message_size, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);

		///assert
		ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, send_result, 0, "Unexpected success in sending from wrong state");
		ASSERT_IO_SEND_CALLBACK(false, IO_SEND_ERROR);

		///cleanup
		tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_071: [ If the adapter is in TLSIO_STATE_EXT_ERROR then tlsio_dowork shall do nothing. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_send_post_error_do_nothing__succeeds)
    {
        int send_result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        // Queue up two messages, one to fail and one that should be ignored by the dowork
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);

        reset_callback_context_records();
        init_fake_read(0);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__HARD_FAIL);
        STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

        tlsio_id->concrete_io_dowork(tlsio);

        ASSERT_IO_SEND_CALLBACK(true, IO_SEND_ERROR);
        ASSERT_IO_ERROR_CALLBACK(true);

        reset_callback_context_records();
        umock_c_reset_all_calls();

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
		ASSERT_NO_CALLBACKS();
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_002: [ The phrase "destroy the failed message" means that the adapter shall remove the message from the queue and destroy it after calling the message's on_send_complete along with its associated callback_context and IO_SEND_ERROR. ]*/
	/* Tests_SRS_TLSIO_30_005: [ When the adapter enters TLSIO_STATE_EXT_ERROR it shall call the  on_io_error function and pass the on_io_error_context that were supplied in  tlsio_open . ]*/
	/* Tests_SRS_TLSIO_30_095: [ If the send process fails before sending all of the bytes in an enqueued message, tlsio_dowork shall destroy the failed message and enter TLSIO_STATE_EX_ERROR. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_send_unhappy_path__fails)
    {
        int send_result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);

        init_fake_read(0);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__HARD_FAIL);
        STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        ASSERT_IO_SEND_CALLBACK(true, IO_SEND_ERROR);
        ASSERT_IO_ERROR_CALLBACK(true);
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_093: [ If the TLS connection was not able to send an entire enqueued message at once, subsequent calls to tlsio_dowork shall continue to send the remaining bytes. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_send_big_message__succeeds)
    {
        int send_result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_TEST_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_TEST_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);
        ASSERT_IO_ERROR_CALLBACK(false);

        init_fake_read(0);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_TEST_MESSAGE_SIZE));

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_TEST_MESSAGE_SIZE - SSL_WRITE_MAX_TEST_SIZE));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        tlsio_id->concrete_io_dowork(tlsio);
        ASSERT_IO_ERROR_CALLBACK(false);

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        ASSERT_IO_SEND_CALLBACK(true, IO_SEND_OK);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IO_ERROR_CALLBACK(false);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_091: [ If tlsio_openssl_compact_dowork is able to send all the bytes in an enqueued message, it shall call the messages's on_send_complete along with its associated callback_context and IO_SEND_OK. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_send__succeeds)
    {
        int send_result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
            SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
        ASSERT_ARE_EQUAL(int, send_result, 0);
        ASSERT_IO_ERROR_CALLBACK(false);

        init_fake_read(0);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        ASSERT_IO_SEND_CALLBACK(true, IO_SEND_OK);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IO_ERROR_CALLBACK(false);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_096: [ If there are no enqueued messages available, tlsio_openssl_compact_dowork shall do nothing. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_send_empty_queue__succeeds)
    {
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        init_fake_read(0);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        // We do expect an empty read when we call dowork
        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        // Verify we got no callback for 0 messages
        ASSERT_IO_SEND_CALLBACK(false, IO_SEND_OK);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IO_ERROR_CALLBACK(false);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_102: [ If the TLS connection receives no data then tlsio_dowork shall not call the on_bytes_received callback. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_receive_no_data__succeeds)
    {
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        init_fake_read(0);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG));

        ///act
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)

        ///assert
        // Verify we got no callback for 0 bytes
        ASSERT_BYTES_RECEIVED_CALLBACK(false, 0);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IO_ERROR_CALLBACK(false);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_100: [ As long as the TLS connection is able to provide received data, tlsio_dowork shall repeatedly read this data and call on_bytes_received with the pointer to the buffer containing the data, the number of bytes received, and the on_bytes_received_context. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_receive_short_message__succeeds)
    {
        ///arrange
        // Create
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        init_fake_read(SSL_SHORT_RECEIVED_MESSAGE_SIZE);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG)); // Gets some data
        STRICT_EXPECTED_CALL(SSL_read(SSL_Good_Ptr, IGNORED_PTR_ARG, IGNORED_NUM_ARG)); // Gets zero bytes

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        // Verify we got the bytes and their callback context
        ASSERT_BYTES_RECEIVED_CALLBACK(true, SSL_SHORT_RECEIVED_MESSAGE_SIZE);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IO_ERROR_CALLBACK(false);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
        assert_gballoc_checks();
    }

    /* Tests_SRS_TLSIO_30_100: [ As long as the TLS connection is able to provide received data, tlsio_dowork shall repeatedly read this data and call on_bytes_received with the pointer to the buffer containing the data, the number of bytes received, and the on_bytes_received_context. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_receive_long_message__succeeds)
    {
        ///arrange
        // Create
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        open_helper(tlsio);

        init_fake_read(SSL_LONG_RECEIVED_MESSAGE_SIZE);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        // Verify we got the bytes and their callback context
        ASSERT_BYTES_RECEIVED_CALLBACK(true, SSL_LONG_RECEIVED_MESSAGE_SIZE);
        ASSERT_IO_ERROR_CALLBACK(false);

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
        assert_gballoc_checks();
    }

	/* Tests_SRS_TLSIO_30_075: [ If the adapter is in TLSIO_STATE_EXT_CLOSED then tlsio_dowork shall do nothing. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__dowork_post_close__succeeds)
    {
        CONCRETE_IO_HANDLE tlsio;
        int open_result;
        ///arrange
        // Create
        reset_callback_context_records();
        tlsio = tlsio_id->concrete_io_create(&good_config);
        open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);

        // Pump dowork until it opens
        STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_true, sizeof_bool);
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (done)
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);

        // close it
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, IO_CLOSE_COMPLETE_CONTEXT);
        ASSERT_IO_CLOSE_CALLBACK(true);
        reset_callback_context_records();
        umock_c_reset_all_calls();

        ///act
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)

        ///assert
        // Verify that the dowork did nothing
        ASSERT_NO_CALLBACKS();
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}


    /* Tests_SRS_TLSIO_30_082: [ If the connection process fails for any reason, tlsio_dowork shall log an error, call on_io_open_complete with the on_io_open_complete_context parameter provided in tlsio_open and IO_OPEN_ERROR, and enter TLSIO_STATE_EX_ERROR. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__dowork_open_unhappy_paths__fails)
    {
        int k;
        bool fails[100];
        size_t i;
        ///arrange
        k = 0;
        use_negative_mocks();

        // dowork_poll_dns (waiting)
        fails[k++] = false; STRICT_EXPECTED_CALL(dns_async_is_lookup_complete(GOOD_DNS_ASYNC_HANDLE)).SetReturn(false);

        // dowork_poll_dns (done)
        fails[k++] = false; STRICT_EXPECTED_CALL(dns_async_is_lookup_complete(GOOD_DNS_ASYNC_HANDLE));
        fails[k++] = true; STRICT_EXPECTED_CALL(dns_async_get_ipv4(GOOD_DNS_ASYNC_HANDLE));
        fails[k++] = false; STRICT_EXPECTED_CALL(dns_async_destroy(GOOD_DNS_ASYNC_HANDLE));
        fails[k++] = true; STRICT_EXPECTED_CALL(socket_async_create(SSL_Get_IPv4_OK, SSL_good_port_number, false, NULL));

        // dowork_poll_socket (waiting)
        fails[k++] = false; STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_false, sizeof_bool);

        // dowork_poll_socket (done)
        fails[k++] = false; STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_true, sizeof_bool);
        fails[k++] = true; STRICT_EXPECTED_CALL(SSL_CTX_new(IGNORED_NUM_ARG));
        fails[k++] = true; STRICT_EXPECTED_CALL(SSL_new(IGNORED_PTR_ARG));
        fails[k++] = true; STRICT_EXPECTED_CALL(SSL_set_fd(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

        // dowork_poll_open_ssl (timeout)
        fails[k++] = false; STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_ERROR__plus__WANT_READ);
        fails[k++] = false; STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, SSL_ERROR__plus__WANT_READ));

        // dowork_poll_open_ssl (hard failure)
        fails[k++] = true; STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr));

        umock_c_negative_tests_snapshot();

        for (i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            CONCRETE_IO_HANDLE tlsio;
            int open_result;
            reset_callback_context_records();
            tlsio = tlsio_id->concrete_io_create(&good_config);
            open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
                IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
            ASSERT_ARE_EQUAL(int, open_result, 0);
            ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
            umock_c_reset_all_calls();

            umock_c_negative_tests_reset();
			if (fails[i])
            {
                umock_c_negative_tests_fail_call(i);
            }
            tlsio_id->concrete_io_dowork(tlsio); 
            tlsio_id->concrete_io_dowork(tlsio);
            tlsio_id->concrete_io_dowork(tlsio);
            tlsio_id->concrete_io_dowork(tlsio); 
            tlsio_id->concrete_io_dowork(tlsio);

            ///act
            tlsio_id->concrete_io_dowork(tlsio);

            ///assert
            // A few of the iterations have no failures
            ASSERT_IO_OPEN_CALLBACK(true, fails[i] ? IO_OPEN_ERROR : IO_OPEN_OK);

            ///cleanup
            tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
            tlsio_id->concrete_io_destroy(tlsio);
        }

        ///cleanup
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_080: [ The tlsio_dowork shall establish a TLS connection using the hostName and port provided during tlsio_open. ]*/
	/* Tests_SRS_TLSIO_30_007: [ The phrase "enter TLSIO_STATE_EXT_OPEN" means the adapter shall call the on_io_open_complete function and pass IO_OPEN_OK and the on_io_open_complete_context that was supplied in tlsio_open . ]*/
	/* Tests_SRS_TLSIO_30_083: [ If tlsio_dowork successfully opens the TLS connection it shall enter TLSIO_STATE_EX_OPEN. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__dowork_open__succeeds)
    {
        CONCRETE_IO_HANDLE tlsio;
        int open_result;
        ///arrange
        reset_callback_context_records();
        tlsio = tlsio_id->concrete_io_create(&good_config);
        open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
        umock_c_reset_all_calls();

        // dowork_poll_dns (waiting)
        STRICT_EXPECTED_CALL(dns_async_is_lookup_complete(GOOD_DNS_ASYNC_HANDLE)).SetReturn(false);

        // dowork_poll_dns (done)
        STRICT_EXPECTED_CALL(dns_async_is_lookup_complete(GOOD_DNS_ASYNC_HANDLE));
        STRICT_EXPECTED_CALL(dns_async_get_ipv4(GOOD_DNS_ASYNC_HANDLE));
        STRICT_EXPECTED_CALL(dns_async_destroy(GOOD_DNS_ASYNC_HANDLE));
        STRICT_EXPECTED_CALL(socket_async_create(SSL_Get_IPv4_OK, SSL_good_port_number, false, NULL));

        // dowork_poll_socket (waiting)
        STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_false, sizeof_bool);

        // dowork_poll_socket (done)
        STRICT_EXPECTED_CALL(socket_async_is_create_complete(SSL_Good_Socket, IGNORED_PTR_ARG)).CopyOutArgumentBuffer_is_complete(&bool_true, sizeof_bool);
        STRICT_EXPECTED_CALL(SSL_CTX_new(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(SSL_new(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(SSL_set_fd(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

        // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_READ)
        STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_ERROR__plus__WANT_READ);
        STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, SSL_ERROR__plus__WANT_READ));

        // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_WRITE)
        STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        STRICT_EXPECTED_CALL(SSL_get_error(SSL_Good_Ptr, SSL_ERROR__plus__WANT_WRITE));

        // dowork_poll_open_ssl (done)
        STRICT_EXPECTED_CALL(SSL_connect(SSL_Good_Ptr)).SetReturn(SSL_CONNECT_SUCCESS);
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (waiting)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_dns (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (waiting)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_socket (done)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_READ)
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (waiting SSL_ERROR_WANT_WRITE)
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_OK);

        ///act
        tlsio_id->concrete_io_dowork(tlsio); // dowork_poll_open_ssl (done)

        ///assert
        // Check that we got the on_open callback
        ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_close(tlsio, on_io_close_complete, NULL);
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_075: [ If the adapter is in TLSIO_STATE_EXT_CLOSED then tlsio_dowork shall do nothing. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_pre_open__succeeds)
    {
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        umock_c_reset_all_calls();
        reset_callback_context_records();

        ///act
        tlsio_id->concrete_io_dowork(tlsio);

        ///assert
        ASSERT_NO_CALLBACKS();
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_070: [ If the tlsio_handle parameter is NULL, tlsio_dowork shall do nothing except log an error. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__dowork_parameter_validation__fails)
    {
        ///arrange

        ///act
        tlsio_id->concrete_io_dowork(NULL);

        ///assert
        ASSERT_NO_CALLBACKS();

        ///cleanup
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_035: [ On tlsio_open success the adapter shall enter TLSIO_STATE_EX_OPENING and return 0. ]*/
	/* Tests_SRS_TLSIO_30_034: [ The tlsio_open shall store the provided on_bytes_received, on_bytes_received_context, on_io_error, on_io_error_context, on_io_open_complete, and on_io_open_complete_context parameters for later use as specified and tested per other line entries in this document. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__open__succeeds)
	{
        int open_result;
		///arrange
		CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
		umock_c_reset_all_calls();
		reset_callback_context_records();

		STRICT_EXPECTED_CALL(dns_async_create(IGNORED_PTR_ARG, NULL));

		///act
		open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
			IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);

		///assert
		ASSERT_ARE_EQUAL(int, open_result, 0);
		// Should not have made any callbacks yet
		ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
		// Ensure that the callbacks were stored properly in the internal instance

		///cleanup
		tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

	/* Tests_SRS_TLSIO_30_038: [ If  tlsio_open  fails to enter TLSIO_STATE_EX_OPENING it shall return FAILURE. ]*/
	/* Tests_SRS_TLSIO_30_039: [ On failure, tlsio_open_async shall not call on_io_open_complete. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__open_unhappy_path__fails)
	{
        int open_result;
		///arrange
		CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
		umock_c_reset_all_calls();
		reset_callback_context_records();

		STRICT_EXPECTED_CALL(dns_async_create(IGNORED_PTR_ARG, NULL)).SetReturn(NULL);

		///act
		open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
			IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);

		///assert
		ASSERT_ARE_NOT_EQUAL(int, open_result, 0);
		// Should not get a callback
		ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		///cleanup
		tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_037: [ If the adapter is in any state other than TLSIO_STATE_EXT_CLOSED when tlsio_open is called, it shall log an error, and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_039: [ On failure, tlsio_open_async shall not call on_io_open_complete. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__open_wrong_state__fails)
    {
        int open_result_2;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        int open_result = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);
        ASSERT_ARE_EQUAL(int, open_result, 0);
        reset_callback_context_records();

        ///act
        open_result_2 = tlsio_id->concrete_io_open(tlsio, on_io_open_complete, IO_OPEN_COMPLETE_CONTEXT, on_bytes_received,
            IO_BYTES_RECEIVED_CONTEXT, on_io_error, IO_ERROR_CONTEXT);

        ///assert
        ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, open_result_2, 0, "Unexpected 2nd open success");
        ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_030: [ If the tlsio_handle parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_031: [ If the on_io_open_complete parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_032: [ If the on_bytes_received parameter is NULL, tlsio_open shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_033: [ If the on_io_error parameter is NULL, tlsio_openss_open shall log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_039: [ On failure, tlsio_open_async shall not call on_io_open_complete. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__open_parameter_validation_fails__fails)
    {
        ///arrange

        // Parameters arrays
        bool p0[OPEN_PV_COUNT];
        ON_IO_OPEN_COMPLETE p1[OPEN_PV_COUNT];
        ON_BYTES_RECEIVED p2[OPEN_PV_COUNT];
        ON_IO_ERROR p3[OPEN_PV_COUNT];
        const char* fm[OPEN_PV_COUNT]; 
        int i;

        int k = 0;
        p0[k] = false; p1[k] = on_io_open_complete; p2[k] = on_bytes_received; p3[k] = on_io_error; fm[k] = "Unexpected open success when tlsio_handle is NULL"; /* */  k++;
        p0[k] = true; p1[k] = NULL; /*           */ p2[k] = on_bytes_received; p3[k] = on_io_error; fm[k] = "Unexpected open success when on_io_open_complete is NULL"; k++;
        p0[k] = true; p1[k] = on_io_open_complete; p2[k] = NULL; /*         */ p3[k] = on_io_error; fm[k] = "Unexpected open success when on_bytes_received is NULL"; k++;
        p0[k] = true; p1[k] = on_io_open_complete; p2[k] = on_bytes_received;  p3[k] = NULL; /*  */ fm[k] = "Unexpected open success when on_io_error is NULL"; /*   */ k++;

        // Cycle through each failing combo of parameters
        for (i = 0; i < OPEN_PV_COUNT; i++)
        {
            CONCRETE_IO_HANDLE tlsio;
            int open_result;
            ///arrange
            reset_callback_context_records();
            tlsio = tlsio_id->concrete_io_create(&good_config);

            ///act
            open_result = tlsio_id->concrete_io_open(p0[i] ? tlsio : NULL, p1[i], IO_OPEN_COMPLETE_CONTEXT, p2[i],
                IO_BYTES_RECEIVED_CONTEXT, p3[i], IO_ERROR_CONTEXT);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, open_result, 0, fm[i]);
            ASSERT_IO_OPEN_CALLBACK(false, IO_OPEN_ERROR);

            ///cleanup
            tlsio_id->concrete_io_destroy(tlsio);
        }
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_OPENSSL_COMPACT_30_520 [ The  tlsio_setoption  shall do nothing and return __FAILURE__. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__setoption__succeeds)
    {
        int result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        ASSERT_IS_NOT_NULL(tlsio);
        umock_c_reset_all_calls();

        ///act
        result = tlsio_id->concrete_io_setoption(tlsio, "fake name", "fake value");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_120: [ If the tlsio_handle parameter is NULL, tlsio_openssl_compact_setoption shall do nothing except log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_121: [ If the optionName parameter is NULL, tlsio_openssl_compact_setoption shall do nothing except log an error and return FAILURE. ]*/
    /* Tests_SRS_TLSIO_30_122: [ If the value parameter is NULL, tlsio_openssl_compact_setoption shall do nothing except log an error and return FAILURE. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__setoption_parameter_validation__fails)
    {
        int k;
        // Parameters arrays
        bool p0[SETOPTION_PV_COUNT];
        const char* p1[SETOPTION_PV_COUNT];
        const char*  p2[SETOPTION_PV_COUNT];
        const char* fm[SETOPTION_PV_COUNT]; 
        int i;

        ///arrange
        umock_c_reset_all_calls();

        k = 0;
        p0[k] = false; p1[k] = "fake name"; p2[k] = "fake value"; fm[k] = "Unexpected setoption success when tlsio_handle is NULL"; /* */  k++;
        p0[k] = true; p1[k] = NULL; /*   */ p2[k] = "fake value"; fm[k] = "Unexpected setoption success when option_name is NULL"; /*  */  k++;
        p0[k] = true; p1[k] = "fake name"; p2[k] = NULL; /*    */ fm[k] = "Unexpected setoption success when option_value is NULL"; /* */  k++;


        // Cycle through each failing combo of parameters
        for (i = 0; i < SETOPTION_PV_COUNT; i++)
        {
            int result;
            ///arrange
            CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
            ASSERT_IS_NOT_NULL(tlsio);

            ///act
            result = tlsio_id->concrete_io_setoption(p0[i] ? tlsio : NULL, p1[i], p2[i]);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, fm[i]);

            ///cleanup
            tlsio_id->concrete_io_destroy(tlsio);
        }
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_160: [ If the tlsio_handle parameter is NULL, tlsio_openssl_compact_retrieveoptions shall do nothing except log an error and return FAILURE. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__retrieveoptions_parameter_validation__fails)
    {
        ///arrange

        ///act
        OPTIONHANDLER_HANDLE result = tlsio_id->concrete_io_retrieveoptions(NULL);

        ///assert
        ASSERT_IS_NULL((void*)result);

        ///cleanup
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_OPENSSL_COMPACT_30_560: [ The  tlsio_retrieveoptions  shall return an empty options handler. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__retrieveoptions__succeeds)
    {
        OPTIONHANDLER_HANDLE result;
        ///arrange
        CONCRETE_IO_HANDLE tlsio = tlsio_id->concrete_io_create(&good_config);
        ASSERT_IS_NOT_NULL(tlsio);
        umock_c_reset_all_calls();

        ///act
        result = tlsio_id->concrete_io_retrieveoptions(tlsio);

        ///assert
        ASSERT_IS_NOT_NULL((void*)result);

        ///cleanup
        tlsio_id->concrete_io_destroy(tlsio);
        OptionHandler_Destroy(result);
        assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_013: [ If the io_create_parameters value is NULL, tlsio_openssl_compact_create shall log an error and return NULL. ]*/
    /* Tests_SRS_TLSIO_30_014: [ If the hostname member of io_create_parameters value is NULL, tlsio_create shall log an error and return NULL. ]*/
    /* Tests_SRS_TLSIO_30_015: [ If the port member of io_create_parameters value is less than 0 or greater than 0xffff, tlsio_create shall log an error and return NULL. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__create_parameter_validation_fails__fails)
    {
        ///arrange
        TLSIO_CONFIG config[4];
        create_parameters_t p[4];
        int i;
        //                               config       hostname            port number                failure message
        populate_create_parameters(p + 0, NULL, /* */ SSL_good_host_name, SSL_good_port_number, "Should fail with NULL config");
        populate_create_parameters(p + 1, config + 1, NULL, /*         */ SSL_good_port_number, "Should fail with NULL hostname");
        populate_create_parameters(p + 2, config + 2, SSL_good_host_name, SSL_port_number_too_low, "Should fail with port number too low");
        populate_create_parameters(p + 3, config + 3, SSL_good_host_name, SSL_port_number_too_high, "Should fail with port number too high");

        // Cycle through each failing combo of parameters
        for (i = 0; i < sizeof(config) / sizeof(TLSIO_CONFIG); i++)
        {
            ///act
            CONCRETE_IO_HANDLE result = tlsio_id->concrete_io_create(p[i].config);

            ///assert
            ASSERT_IS_NULL_WITH_MSG(result, p[i].fail_msg);
        }
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_011: [ If any resource allocation fails, tlsio_create shall return NULL. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__create_unhappy_paths__fails)
    {
        size_t i;
        ///arrange
        use_negative_mocks();

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // concrete_io struct
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // copy hostname
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // singlylinkedlist_create
        umock_c_negative_tests_snapshot();

        for (i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            CONCRETE_IO_HANDLE result;
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            ///act
            result = tlsio_id->concrete_io_create(&good_config);

            ///assert
            ASSERT_IS_NULL(result);
        }

        ///cleanup
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_010: [ The tlsio_create shall allocate and initialize all necessary resources and return an instance of the tlsio_openssl_compact. ]*/
	/* Tests_SRS_TLSIO_30_016: [ tlsio_create shall make a copy of the hostname member of io_create_parameters to allow deletion of hostname immediately after the call. ]*/
	/* Tests_SRS_TLSIO_30_012: [ The tlsio_create shall receive the connection configuration as a TLSIO_CONFIG* in io_create_parameters. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__create__succeeds)
    {
        CONCRETE_IO_HANDLE result;
        ///arrange

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // concrete_io struct
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // copy hostname
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));  // singlylinkedlist_create
        //

        ///act
        result = tlsio_id->concrete_io_create(&good_config);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        tlsio_id->concrete_io_destroy(result);
		assert_gballoc_checks();
	}


	/* Tests_SRS_TLSIO_30_022: [ If the adapter is in any state other than TLSIO_STATE_EX_CLOSED when tlsio_destroy is called, the adapter shall enter TLSIO_STATE_EX_CLOSING and then enter TLSIO_STATE_EX_CLOSED before completing the destroy process. ]*/
	TEST_FUNCTION(tlsio_openssl_compact__destroy_with_unsent_messages__succeeds)
	{
        CONCRETE_IO_HANDLE tlsio;
        int send_result;
		///arrange
		reset_callback_context_records();
		tlsio = tlsio_id->concrete_io_create(&good_config);
		open_helper(tlsio);

		// Make sure the arrangement is correct
		ASSERT_IO_OPEN_CALLBACK(true, IO_OPEN_OK);

        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        STRICT_EXPECTED_CALL(SSL_write(SSL_Good_Ptr, IGNORED_PTR_ARG, SSL_SHORT_SENT_MESSAGE_SIZE)).SetReturn(SSL_ERROR__plus__WANT_WRITE);
        send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
			SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
		ASSERT_ARE_EQUAL(int, send_result, 0);
		send_result = tlsio_id->concrete_io_send(tlsio, SSL_send_buffer,
			SSL_SHORT_SENT_MESSAGE_SIZE, on_io_send_complete, IO_SEND_COMPLETE_CONTEXT);
		ASSERT_ARE_EQUAL(int, send_result, 0);


		umock_c_reset_all_calls();
		STRICT_EXPECTED_CALL(SSL_shutdown(SSL_Good_Ptr));
		STRICT_EXPECTED_CALL(SSL_free(SSL_Good_Ptr));
		STRICT_EXPECTED_CALL(SSL_CTX_free(SSL_Good_Context_Ptr));
		STRICT_EXPECTED_CALL(socket_async_destroy(SSL_Good_Socket));
		// Message 1 delete
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		// Message 2 delete
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		// Close and delete tlsio
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
        // End of arrange

		///act
		tlsio_id->concrete_io_destroy(tlsio);

		///assert
		ASSERT_IO_SEND_ABANDONED(2); // 2 messages in this test
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());


		///cleanup
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_020: [ If tlsio_handle is NULL, tlsio_destroy shall do nothing. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__destroy_parameter_validation__fails)
    {
        ///arrange

        ///act
        tlsio_id->concrete_io_destroy(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_021: [ The tlsio__destroy shall release all allocated resources and then release tlsio_handle. ]*/
    TEST_FUNCTION(tlsio_openssl_compact__destroy_unopened__succeeds)
    {
        ///arrange
        CONCRETE_IO_HANDLE result = tlsio_id->concrete_io_create(&good_config);
        ASSERT_IS_NOT_NULL(result);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));  // copy hostname
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));  // singlylinkedlist_create
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));  // concrete_io struct
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));  // tlsio_options
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));  // tlsio_options
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));  // tlsio_options

        ///act
        tlsio_id->concrete_io_destroy(result);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
		assert_gballoc_checks();
	}

    /* Tests_SRS_TLSIO_30_008: [ The tlsio_get_interface_description shall return the VTable IO_INTERFACE_DESCRIPTION. ]*/
    /* Tests_SRS_TLSIO_30_001: [ The tlsio_openssl_compact shall implement and export all the Concrete functions in the VTable IO_INTERFACE_DESCRIPTION defined in the xio.h. ] */
    TEST_FUNCTION(tlsio_openssl_compact__tlsio_get_interface_description)
    {
        ///act
        const IO_INTERFACE_DESCRIPTION* tlsio_id_ut = tlsio_pal_get_interface_description();

        ///assert
        // Later specific tests will verify the identity of each function
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_close);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_create);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_destroy);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_dowork);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_open);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_retrieveoptions);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_send);
        ASSERT_IS_NOT_NULL(tlsio_id_ut->concrete_io_setoption);
		assert_gballoc_checks();
	}

END_TEST_SUITE(tlsio_openssl_compact_unittests)
