/* Shim needed to make sure the mbedTLS-specific
   sha512 header is included */
#pragma once
#include "crypto_hash_sha512.h"
#include_next "sodium/crypto_auth_hmacsha512.h"
