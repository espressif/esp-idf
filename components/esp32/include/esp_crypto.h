#ifndef _MULTI_CRYPTO_H_
#define _MULTI_CRYPTO_H_

#include "c_types.h"
#include "rom/ets_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	AES_MUTEX = 0,
	BIGNUM_MUTEX,
	SHA_MUTEX,

	MUTEX_MAX_NUM,
};

int esp_crypto_init(void);

void esp_crypto_lock(unsigned int num);
void esp_crypto_unlock(unsigned int num);

void esp_crypto_take(unsigned int num);
void esp_crypto_give(unsigned int num);
bool esp_crypto_is_used(unsigned int num);

#define MUTEX_LOCK(num) esp_crypto_lock(num)
#define MUTEX_UNLOCK(num) esp_crypto_unlock(num)

#define SIG_TAKE(num) esp_crypto_take(num)
#define SIG_GIVE(num) esp_crypto_give(num)
#define SIG_IS_USED(num) esp_crypto_is_used(num)

#define AES_LOCK() MUTEX_LOCK(AES_MUTEX)
#define AES_UNLOCK() MUTEX_UNLOCK(AES_MUTEX)
#define BIGNUM_LOCK() MUTEX_LOCK(BIGNUM_MUTEX)
#define BIGNUM_UNLOCK() MUTEX_UNLOCK(BIGNUM_MUTEX)
#define SHA_LOCK() MUTEX_LOCK(SHA_MUTEX)
#define SHA_UNLOCK() MUTEX_UNLOCK(SHA_MUTEX)

#define AES_TAKE() SIG_TAKE(AES_MUTEX)
#define AES_GIVE() SIG_GIVE(AES_MUTEX)
#define AES_IS_USED() SIG_IS_USED(AES_MUTEX)
#define BIGNUM_TAKE() SIG_TAKE(BIGNUM_MUTEX)
#define BIGNUM_GIVE() SIG_GIVE(BIGNUM_MUTEX)
#define BIGNUM_IS_USED() SIG_IS_USED(BIGNUM_MUTEX)
#define SHA_TAKE() SIG_TAKE(SHA_MUTEX)
#define SHA_GIVE() SIG_GIVE(SHA_MUTEX)
#define SHA_IS_USED() SIG_IS_USED(SHA_MUTEX)

#ifdef __cplusplus
}
#endif

#endif /* esp_crypto.h */
