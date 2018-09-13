set(SRC libsodium/src/libsodium)

set(COMPONENT_REQUIRES "mbedtls")

set(COMPONENT_SRCS "${SRC}/crypto_aead/chacha20poly1305/sodium/aead_chacha20poly1305.c"
                   "${SRC}/crypto_aead/xchacha20poly1305/sodium/aead_xchacha20poly1305.c"
                   "${SRC}/crypto_auth/crypto_auth.c"
                   "${SRC}/crypto_auth/hmacsha256/auth_hmacsha256.c"
                   "${SRC}/crypto_auth/hmacsha512/auth_hmacsha512.c"
                   "${SRC}/crypto_auth/hmacsha512256/auth_hmacsha512256.c"
                   "${SRC}/crypto_box/crypto_box.c"
                   "${SRC}/crypto_box/crypto_box_easy.c"
                   "${SRC}/crypto_box/crypto_box_seal.c"
                   "${SRC}/crypto_box/curve25519xsalsa20poly1305/box_curve25519xsalsa20poly1305.c"
                   "${SRC}/crypto_core/curve25519/ref10/curve25519_ref10.c"
                   "${SRC}/crypto_core/hchacha20/core_hchacha20.c"
                   "${SRC}/crypto_core/hsalsa20/core_hsalsa20.c"
                   "${SRC}/crypto_core/hsalsa20/ref2/core_hsalsa20_ref2.c"
                   "${SRC}/crypto_core/salsa/ref/core_salsa_ref.c"
                   "${SRC}/crypto_generichash/crypto_generichash.c"
                   "${SRC}/crypto_generichash/blake2b/generichash_blake2.c"
                   "${SRC}/crypto_generichash/blake2b/ref/blake2b-compress-avx2.c"
                   "${SRC}/crypto_generichash/blake2b/ref/blake2b-compress-ref.c"
                   "${SRC}/crypto_generichash/blake2b/ref/blake2b-compress-sse41.c"
                   "${SRC}/crypto_generichash/blake2b/ref/blake2b-compress-ssse3.c"
                   "${SRC}/crypto_generichash/blake2b/ref/blake2b-ref.c"
                   "${SRC}/crypto_generichash/blake2b/ref/generichash_blake2b.c"
                   "${SRC}/crypto_hash/crypto_hash.c"
                   "${SRC}/crypto_hash/sha256/hash_sha256.c"
                   "${SRC}/crypto_hash/sha512/hash_sha512.c"
                   "${SRC}/crypto_kdf/crypto_kdf.c"
                   "${SRC}/crypto_kdf/blake2b/kdf_blake2b.c"
                   "${SRC}/crypto_kx/crypto_kx.c"
                   "${SRC}/crypto_onetimeauth/crypto_onetimeauth.c"
                   "${SRC}/crypto_onetimeauth/poly1305/onetimeauth_poly1305.c"
                   "${SRC}/crypto_onetimeauth/poly1305/donna/poly1305_donna.c"
                   "${SRC}/crypto_pwhash/crypto_pwhash.c"
                   "${SRC}/crypto_pwhash/argon2/argon2-core.c"
                   "${SRC}/crypto_pwhash/argon2/argon2-encoding.c"
                   "${SRC}/crypto_pwhash/argon2/argon2-fill-block-ref.c"
                   "${SRC}/crypto_pwhash/argon2/argon2-fill-block-ssse3.c"
                   "${SRC}/crypto_pwhash/argon2/argon2.c"
                   "${SRC}/crypto_pwhash/argon2/blake2b-long.c"
                   "${SRC}/crypto_pwhash/argon2/pwhash_argon2i.c"
                   "${SRC}/crypto_pwhash/scryptsalsa208sha256/crypto_scrypt-common.c"
                   "${SRC}/crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.c"
                   "${SRC}/crypto_pwhash/scryptsalsa208sha256/pwhash_scryptsalsa208sha256.c"
                   "${SRC}/crypto_pwhash/scryptsalsa208sha256/scrypt_platform.c"
                   "${SRC}/crypto_pwhash/scryptsalsa208sha256/nosse/pwhash_scryptsalsa208sha256_nosse.c"
                   "${SRC}/crypto_scalarmult/crypto_scalarmult.c"
                   "${SRC}/crypto_scalarmult/curve25519/scalarmult_curve25519.c"
                   "${SRC}/crypto_scalarmult/curve25519/ref10/x25519_ref10.c"
                   "${SRC}/crypto_secretbox/crypto_secretbox.c"
                   "${SRC}/crypto_secretbox/crypto_secretbox_easy.c"
                   "${SRC}/crypto_secretbox/xsalsa20poly1305/secretbox_xsalsa20poly1305.c"
                   "${SRC}/crypto_shorthash/crypto_shorthash.c"
                   "${SRC}/crypto_shorthash/siphash24/shorthash_siphash24.c"
                   "${SRC}/crypto_shorthash/siphash24/shorthash_siphashx24.c"
                   "${SRC}/crypto_shorthash/siphash24/ref/shorthash_siphash24_ref.c"
                   "${SRC}/crypto_shorthash/siphash24/ref/shorthash_siphashx24_ref.c"
                   "${SRC}/crypto_sign/crypto_sign.c"
                   "${SRC}/crypto_sign/ed25519/sign_ed25519.c"
                   "${SRC}/crypto_sign/ed25519/ref10/keypair.c"
                   "${SRC}/crypto_sign/ed25519/ref10/obsolete.c"
                   "${SRC}/crypto_sign/ed25519/ref10/open.c"
                   "${SRC}/crypto_sign/ed25519/ref10/sign.c"
                   "${SRC}/crypto_stream/crypto_stream.c"
                   "${SRC}/crypto_stream/chacha20/stream_chacha20.c"
                   "${SRC}/crypto_stream/chacha20/ref/chacha20_ref.c"
                   "${SRC}/crypto_stream/salsa20/stream_salsa20.c"
                   "${SRC}/crypto_stream/salsa20/ref/salsa20_ref.c"
                   "${SRC}/crypto_stream/xsalsa20/stream_xsalsa20.c"
                   "${SRC}/crypto_verify/sodium/verify.c"
                   "${SRC}/randombytes/randombytes.c"
                   "${SRC}/sodium/core.c"
                   "${SRC}/sodium/runtime.c"
                   "${SRC}/sodium/utils.c"
                   "${SRC}/sodium/version.c"
                   "port/randombytes_esp32.c")

if(CONFIG_LIBSODIUM_USE_MBEDTLS_SHA)
    list(APPEND COMPONENT_SRCS "port/crypto_hash_mbedtls/crypto_hash_sha256_mbedtls.c"
                   "port/crypto_hash_mbedtls/crypto_hash_sha512_mbedtls.c")
else()
    list(APPEND COMPONENT_SRCS "${SRC}/crypto_hash/sha256/cp/hash_sha256_cp.c"
                   "${SRC}/crypto_hash/sha512/cp/hash_sha512_cp.c")
endif()

set(COMPONENT_ADD_INCLUDEDIRS ${SRC}/include port_include)
set(COMPONENT_PRIV_INCLUDEDIRS ${SRC}/include/sodium port_include/sodium port)

register_component()

component_compile_definitions(
    CONFIGURED
    NATIVE_LITTLE_ENDIAN
    HAVE_WEAK_SYMBOLS
    __STDC_LIMIT_MACROS
    __STDC_CONSTANT_MACROS
    )

component_compile_options(-Wno-unknown-pragmas)

# patch around warnings in third-party files
set_source_files_properties(
    ${SRC}/crypto_pwhash/argon2/argon2-fill-block-ref.c
    ${SRC}/crypto_pwhash/argon2/pwhash_argon2i.c
    ${SRC}/crypto_pwhash/argon2/argon2-core.c
    ${SRC}/crypto_pwhash/scryptsalsa208sha256/pwhash_scryptsalsa208sha256.c
    PROPERTIES COMPILE_FLAGS
    -Wno-type-limits
    )
set_source_files_properties(
    ${SRC}/sodium/utils.c
    PROPERTIES COMPILE_FLAGS
    -Wno-unused-variable
    )

# Temporary suppress "fallthrough" warnings until they are fixed in libsodium repo
set_source_files_properties(
    ${SRC}/crypto_shorthash/siphash24/ref/shorthash_siphashx24_ref.c
    ${SRC}/crypto_shorthash/siphash24/ref/shorthash_siphash24_ref.c
    PROPERTIES COMPILE_FLAGS
    -Wno-implicit-fallthrough)
