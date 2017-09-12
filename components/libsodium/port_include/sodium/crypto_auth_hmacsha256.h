/* Shim needed to make sure the mbedTLS-specific
   sha256 header is included */
#pragma once
#include "crypto_hash_sha256.h"
#include_next "sodium/crypto_auth_hmacsha256.h"

