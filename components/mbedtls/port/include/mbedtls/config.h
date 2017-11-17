/* This shim header is added so that any application code
   which includes "mbedtls/config.h" directly gets the correct
   config. */
#pragma once
#if !defined(MBEDTLS_CONFIG_FILE)
#include_next "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
