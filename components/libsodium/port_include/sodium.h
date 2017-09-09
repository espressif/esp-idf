/* Shim needed to make sure the mbedTLS-specific
   sha256 & 512 headers are included */
#pragma once
#include "sodium/crypto_hash_sha512.h"
#include "sodium/crypto_hash_sha256.h"
#include_next "sodium.h"
