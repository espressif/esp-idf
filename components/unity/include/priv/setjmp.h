#include_next <setjmp.h>
#include "esp_debug_helpers.h"

/*
 * This is the middle layer of setjmp to be used with the unity.
 */

/** Insert backtrace before longjmp (TEST_ABORT).
 *
 * Currently we only do long jump before test is ignored or failed.
 * If this is also called when test pass, we may need to add some check before
 * backtrace is called.
 */
#define longjmp(buf, val) do {esp_backtrace_print(100); longjmp(buf, val);} while(0)
