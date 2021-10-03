COMPONENT_SUBMODULES += libsodium

# Common root directory for all source directories
LSRC := libsodium/src/libsodium

COMPONENT_SRCDIRS := port

# Derived from libsodium/src/libsodium/Makefile.am
# (ignoring the !MINIMAL set)
COMPONENT_SRCDIRS += \
	$(LSRC)/crypto_aead/chacha20poly1305/sodium \
	$(LSRC)/crypto_aead/xchacha20poly1305/sodium \
	$(LSRC)/crypto_auth \
	$(LSRC)/crypto_auth/hmacsha256 \
	$(LSRC)/crypto_auth/hmacsha512 \
	$(LSRC)/crypto_auth/hmacsha512256 \
	$(LSRC)/crypto_box \
	$(LSRC)/crypto_box/curve25519xsalsa20poly1305 \
	$(LSRC)/crypto_core/ed25519 \
	$(LSRC)/crypto_core/ed25519/ref10 \
	$(LSRC)/crypto_core/hchacha20 \
	$(LSRC)/crypto_core/hsalsa20/ref2 \
	$(LSRC)/crypto_core/hsalsa20 \
	$(LSRC)/crypto_core/salsa/ref \
	$(LSRC)/crypto_generichash \
	$(LSRC)/crypto_generichash/blake2b \
	$(LSRC)/crypto_generichash/blake2b/ref \
	$(LSRC)/crypto_hash \
	$(LSRC)/crypto_hash/sha256 \
	$(LSRC)/crypto_hash/sha512 \
	$(LSRC)/crypto_kdf/blake2b \
	$(LSRC)/crypto_kdf \
	$(LSRC)/crypto_kx \
	$(LSRC)/crypto_onetimeauth \
	$(LSRC)/crypto_onetimeauth/poly1305 \
	$(LSRC)/crypto_onetimeauth/poly1305/donna \
	$(LSRC)/crypto_pwhash/argon2 \
	$(LSRC)/crypto_pwhash \
	$(LSRC)/crypto_pwhash/scryptsalsa208sha256 \
	$(LSRC)/crypto_pwhash/scryptsalsa208sha256/nosse \
	$(LSRC)/crypto_scalarmult \
	$(LSRC)/crypto_scalarmult/curve25519 \
	$(LSRC)/crypto_scalarmult/curve25519/ref10 \
	$(LSRC)/crypto_scalarmult/curve25519/sandy2x \
	$(LSRC)/crypto_scalarmult/ed25519/ref10 \
	$(LSRC)/crypto_scalarmult/ristretto255/ref10 \
	$(LSRC)/crypto_secretbox \
	$(LSRC)/crypto_secretbox/xsalsa20poly1305 \
	$(LSRC)/crypto_shorthash \
	$(LSRC)/crypto_shorthash/siphash24 \
	$(LSRC)/crypto_shorthash/siphash24/ref \
	$(LSRC)/crypto_sign \
	$(LSRC)/crypto_sign/ed25519 \
	$(LSRC)/crypto_sign/ed25519/ref10 \
	$(LSRC)/crypto_stream/chacha20 \
	$(LSRC)/crypto_stream/chacha20/ref \
	$(LSRC)/crypto_stream \
	$(LSRC)/crypto_stream/salsa20 \
    $(LSRC)/crypto_stream/salsa20/ref \
	$(LSRC)/crypto_stream/xsalsa20 \
	$(LSRC)/crypto_verify/sodium \
	$(LSRC)/randombytes \
	$(LSRC)/sodium

ifdef CONFIG_LIBSODIUM_USE_MBEDTLS_SHA
COMPONENT_SRCDIRS += port/crypto_hash_mbedtls
else
COMPONENT_SRCDIRS += \
	$(LSRC)/crypto_hash/sha256/cp \
    $(LSRC)/crypto_hash/sha512/cp
endif

# Fix some warnings in current libsodium source files
# (not applied to whole component as we compile some of our own files, also.)
$(LSRC)/crypto_pwhash/argon2/argon2-fill-block-ref.o: CFLAGS += -Wno-unknown-pragmas
$(LSRC)/crypto_pwhash/argon2/pwhash_argon2i.o: CFLAGS += -Wno-type-limits
$(LSRC)/crypto_pwhash/argon2/pwhash_argon2id.o: CFLAGS += -Wno-type-limits
$(LSRC)/crypto_pwhash/argon2/argon2-core.o: CFLAGS += -Wno-type-limits
$(LSRC)/crypto_pwhash/scryptsalsa208sha256/pwhash_scryptsalsa208sha256.o: CFLAGS += -Wno-type-limits
$(LSRC)/sodium/utils.o: CFLAGS += -Wno-unused-variable

COMPONENT_ADD_INCLUDEDIRS := $(LSRC)/include port_include
COMPONENT_PRIV_INCLUDEDIRS := $(LSRC)/include/sodium port_include/sodium port


# Not using autoconf, but this needs to be set
CFLAGS += -DCONFIGURED -Wno-unused-function

# Add the options from configure.ac (this needs checking if new versions are added )
CFLAGS +=  -DNATIVE_LITTLE_ENDIAN -DHAVE_WEAK_SYMBOLS -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS

# randombytes.c needs to pull in platform-specific implementation
$(LSRC)/randombytes/randombytes.o: CFLAGS+=-DRANDOMBYTES_DEFAULT_IMPLEMENTATION

# Temporary suppress "fallthrough" warnings until they are fixed in libsodium repo
$(LSRC)/crypto_shorthash/siphash24/ref/shorthash_siphashx24_ref.o: CFLAGS += -Wno-implicit-fallthrough
$(LSRC)/crypto_shorthash/siphash24/ref/shorthash_siphash24_ref.o: CFLAGS += -Wno-implicit-fallthrough
