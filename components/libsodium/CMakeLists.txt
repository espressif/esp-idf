set(SRC libsodium/src/libsodium)

set(COMPONENT_REQUIRES "mbedtls")

set(COMPONENT_SRCDIRS
    port

    # Derived from libsodium/src/libsodium/Makefile.am
    # (ignoring the !MINIMAL set)
    ${SRC}/crypto_aead/chacha20poly1305/sodium
    ${SRC}/crypto_aead/xchacha20poly1305/sodium
    ${SRC}/crypto_auth
    ${SRC}/crypto_auth/hmacsha256
    ${SRC}/crypto_auth/hmacsha512
    ${SRC}/crypto_auth/hmacsha512256
    ${SRC}/crypto_box
    ${SRC}/crypto_box/curve25519xsalsa20poly1305
    ${SRC}/crypto_core/curve25519/ref10
    ${SRC}/crypto_core/hchacha20
    ${SRC}/crypto_core/hsalsa20/ref2
    ${SRC}/crypto_core/hsalsa20
    ${SRC}/crypto_core/salsa/ref
    ${SRC}/crypto_generichash
    ${SRC}/crypto_generichash/blake2b
    ${SRC}/crypto_generichash/blake2b/ref
    ${SRC}/crypto_hash
    ${SRC}/crypto_hash/sha256
    ${SRC}/crypto_hash/sha512
    ${SRC}/crypto_kdf/blake2b
    ${SRC}/crypto_kdf
    ${SRC}/crypto_kx
    ${SRC}/crypto_onetimeauth
    ${SRC}/crypto_onetimeauth/poly1305
    ${SRC}/crypto_onetimeauth/poly1305/donna
    ${SRC}/crypto_pwhash/argon2
    ${SRC}/crypto_pwhash
    ${SRC}/crypto_pwhash/scryptsalsa208sha256
    ${SRC}/crypto_pwhash/scryptsalsa208sha256/nosse
    ${SRC}/crypto_scalarmult
    ${SRC}/crypto_scalarmult/curve25519
    ${SRC}/crypto_scalarmult/curve25519/ref10
    ${SRC}/crypto_secretbox
    ${SRC}/crypto_secretbox/xsalsa20poly1305
    ${SRC}/crypto_shorthash
    ${SRC}/crypto_shorthash/siphash24
    ${SRC}/crypto_shorthash/siphash24/ref
    ${SRC}/crypto_sign
    ${SRC}/crypto_sign/ed25519
    ${SRC}/crypto_sign/ed25519/ref10
    ${SRC}/crypto_stream/chacha20
    ${SRC}/crypto_stream/chacha20/ref
    ${SRC}/crypto_stream
    ${SRC}/crypto_stream/salsa20
    ${SRC}/crypto_stream/salsa20/ref
    ${SRC}/crypto_stream/xsalsa20
    ${SRC}/crypto_verify/sodium
    ${SRC}/randombytes
    ${SRC}/sodium
    )

if(CONFIG_LIBSODIUM_USE_MBEDTLS_SHA)
    set(COMPONENT_SRCDIRS ${COMPONENT_SRCDIRS}
        port/crypto_hash_mbedtls
        )
else()
    set(COMPONENT_SRCDIRS ${COMPONENT_SRCDIRS}
        ${SRC}/crypto_hash/sha256/cp
        ${SRC}/crypto_hash/sha512/cp
        )
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
