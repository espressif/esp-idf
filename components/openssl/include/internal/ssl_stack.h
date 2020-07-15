#ifndef _SSL_STACK_H_
#define _SSL_STACK_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "ssl_types.h"

#define STACK_OF(type)  struct stack_st_##type

#define SKM_DEFINE_STACK_OF(t1, t2, t3) \
    STACK_OF(t1); \
    static ossl_inline STACK_OF(t1) *sk_##t1##_new_null(void) \
    { \
        return (STACK_OF(t1) *)OPENSSL_sk_new_null(); \
    } \

#define DEFINE_STACK_OF(t) SKM_DEFINE_STACK_OF(t, t, t)
typedef struct asn1_string_st ASN1_OCTET_STRING;

struct stack_st_GENERAL_NAME;
typedef struct GENERAL_NAME_st {
    int type;
    union {
        char *ptr;
        struct asn1_string_st* dNSName;
        ASN1_OCTET_STRING* iPAddress;
    } d;
} GENERAL_NAME;

typedef struct asn1_string_st ASN1_OCTET_STRING;
typedef struct X509_name_st X509_NAME;
typedef struct asn1_string_st ASN1_STRING;
typedef struct X509_name_entry_st X509_NAME_ENTRY;

typedef struct asn1_string_st {
    int type;
    int length;
    void *data;
} ASN1_IA5STRING;

typedef STACK_OF(GENERAL_NAME) GENERAL_NAMES;

/**
 * @brief get nr of stack items
 *
 * @param sk Stack structure pointer
 *
 * @return number of items in the stack
 */
size_t sk_GENERAL_NAME_num(const struct stack_st_GENERAL_NAME *sk);

/**
 * @brief get GENERAL_NAME value from the stack
 *
 * @param sk Stack structure pointer
 * @param i Index to stack item
 *
 * @return GENERAL_NAME object pointer
 */
GENERAL_NAME *sk_GENERAL_NAME_value(const struct stack_st_GENERAL_NAME *sk, size_t i);

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
