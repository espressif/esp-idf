#ifndef _SSL_STACK_H_
#define _SSL_STACK_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "ssl_types.h"

/**
 * @brief create a openssl stack object
 *
 * @param c - stack function
 *
 * @return openssl stack object point
 */
OPENSSL_STACK* OPENSSL_sk_new(OPENSSL_sk_compfunc c);

/**
 * @brief create a NULL function openssl stack object
 *
 * @param none
 *
 * @return openssl stack object point
 */
OPENSSL_STACK *OPENSSL_sk_new_null(void);

/**
 * @brief free openssl stack object
 *
 * @param openssl stack object point
 *
 * @return none
 */
void OPENSSL_sk_free(OPENSSL_STACK *stack);

#ifdef __cplusplus
}
#endif

#endif
