#ifndef _MULTI_THREAD_H_
#define _MULTI_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
	AES_MUTEX = 0,
	BIGNUM_MUTEX,
	SHA_MUTEX,

	MUTEX_MAX_NUM,
};

int multi_thread_init(void);

void multi_thread_lock(unsigned int num);
void multi_thread_unlock(unsigned int num);

void multi_thread_take(unsigned int num);
void multi_thread_give(unsigned int num);
bool multi_thread_is_used(num);

#define MUTEX_LOCK(num) multi_thread_lock(num)
#define MUTEX_UNLOCK(num) multi_thread_unlock(num)

#define SIG_TAKE(num) multi_thread_take(num)
#define SIG_GIVE(num) multi_thread_give(num)
#define SIG_IS_USED(num) multi_thread_is_used(num)

#define AES_LOCK() MUTEX_LOCK(AES_MUTEX)
#define AES_UNLOCK() MUTEX_UNLOCK(AES_MUTEX)
#define BIGNUM_LOCK() MUTEX_LOCK(BIGNUM_MUTEX)
#define BIGNUM_UNLOCK() MUTEX_UNLOCK(BIGNUM_MUTEX)
#define SHA1_LOCK() MUTEX_LOCK(SHA_MUTEX)
#define SHA1_UNLOCK() MUTEX_UNLOCK(SHA_MUTEX)
#define SHA256_LOCK() MUTEX_LOCK(SHA_MUTEX)
#define SHA256_UNLOCK() MUTEX_UNLOCK(SHA_MUTEX)
#define SHA512_LOCK() MUTEX_LOCK(SHA_MUTEX)
#define SHA512_UNLOCK() MUTEX_UNLOCK(SHA_MUTEX)

#define AES_TAKE() SIG_TAKE(AES_MUTEX)
#define AES_GIVE() SIG_GIVE(AES_MUTEX)
#define AES_IS_USED() SIG_IS_USED(AES_MUTEX)
#define BIGNUM_TAKE() SIG_TAKE(BIGNUM_MUTEX)
#define BIGNUM_GIVE() SIG_GIVE(BIGNUM_MUTEX)
#define BIGNUM_IS_USED() SIG_IS_USED(BIGNUM_MUTEX)
#define SHA1_TAKE() SIG_TAKE(SHA_MUTEX)
#define SHA1_GIVE() SIG_GIVE(SHA_MUTEX)
#define SHA1_IS_USED() SIG_IS_USED(SHA_MUTEX)
#define SHA256_TAKE() SIG_TAKE(SHA_MUTEX)
#define SHA256_GIVE() SIG_GIVE(SHA_MUTEX)
#define SHA256_IS_USED() SIG_IS_USED(SHA_MUTEX)
#define SHA512_TAKE() SIG_TAKE(SHA_MUTEX)
#define SHA512_GIVE() SIG_GIVE(SHA_MUTEX)
#define SHA512_IS_USED() SIG_IS_USED(SHA_MUTEX)

#ifdef __cplusplus
}
#endif

#endif /* multi_thread.h */
