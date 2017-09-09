/* Shim needed to make sure the mbedTLS-specific
   sha256 & sha512 headers are included */
#pragma once
#include "crypto_hash_sha512.h"
#include "crypto_hash_sha256.h"
#include_next "sodium/crypto_auth.h"
