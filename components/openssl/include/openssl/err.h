// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _OPENSSL_ERR_H
#define _OPENSSL_ERR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note This file contains a very simple implementation of error stack
 * for ESP APIs stubs to OpenSSL
 */

#define OPENSSL_PUT_SYSTEM_ERROR() \
  ERR_put_error(ERR_LIB_SYS, 0, 0, __FILE__, __LINE__);

#define OPENSSL_PUT_LIB_ERROR(lib, code) \
  ERR_put_error(lib, 0, code, __FILE__, __LINE__);

#define ERR_GET_LIB(packed_error) ((int)(((packed_error) >> 24) & 0xff))
#define ERR_GET_REASON(packed_error) ((int)((packed_error) & 0xffff))
#define ERR_R_PEM_LIB ERR_LIB_PEM
/* inherent openssl errors */
# define ERR_R_FATAL                             64
# define ERR_R_MALLOC_FAILURE                    (1|ERR_R_FATAL)
# define ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED       (2|ERR_R_FATAL)
# define ERR_R_PASSED_NULL_PARAMETER             (3|ERR_R_FATAL)
# define ERR_R_INTERNAL_ERROR                    (4|ERR_R_FATAL)
# define ERR_R_DISABLED                          (5|ERR_R_FATAL)
# define ERR_R_INIT_FAIL                         (6|ERR_R_FATAL)
# define ERR_R_PASSED_INVALID_ARGUMENT           (7)
# define ERR_R_OPERATION_FAIL                    (8|ERR_R_FATAL)
# define ERR_R_INVALID_PROVIDER_FUNCTIONS        (9|ERR_R_FATAL)
# define ERR_R_INTERRUPTED_OR_CANCELLED          (10)

enum {
    ERR_LIB_NONE = 1,
    ERR_LIB_SYS,
    ERR_LIB_BN,
    ERR_LIB_RSA,
    ERR_LIB_DH,
    ERR_LIB_EVP,
    ERR_LIB_BUF,
    ERR_LIB_OBJ,
    ERR_LIB_PEM,
    ERR_LIB_DSA,
    ERR_LIB_X509,
    ERR_LIB_ASN1,
    ERR_LIB_CONF,
    ERR_LIB_CRYPTO,
    ERR_LIB_EC,
    ERR_LIB_SSL,
    ERR_LIB_BIO,
    ERR_LIB_PKCS7,
    ERR_LIB_PKCS8,
    ERR_LIB_X509V3,
    ERR_LIB_RAND,
    ERR_LIB_ENGINE,
    ERR_LIB_OCSP,
    ERR_LIB_UI,
    ERR_LIB_COMP,
    ERR_LIB_ECDSA,
    ERR_LIB_ECDH,
    ERR_LIB_HMAC,
    ERR_LIB_DIGEST,
    ERR_LIB_CIPHER,
    ERR_LIB_HKDF,
    ERR_LIB_USER,
    ERR_NUM_LIBS
};

/**
 * @brief clear the SSL error code
 *
 * @param none
 *
 * @return none
 */
void ERR_clear_error(void);

/**
 * @brief get the current SSL error code
 *
 * @param none
 *
 * @return current SSL error number
 */
uint32_t ERR_get_error(void);

/**
 * @brief peek the current SSL error code, not clearing it
 *
 * @param none
 *
 * @return current SSL error number
 */
uint32_t ERR_peek_error(void);

/**
 * @brief peek the last SSL error code, not clearing it
 *
 * @param none
 *
 * @return current SSL error number
 */
uint32_t ERR_peek_last_error(void);

/**
 * @brief register the SSL error strings
 *
 * @param none
 *
 * @return none
 */
void ERR_load_SSL_strings(void);

/**
 * @brief clear the SSL error code
 *
 * @param none
 *
 * @return none
 */
void ERR_clear_error(void);

/**
 * @brief peek the current SSL error code, not clearing it
 *
 * @param none
 *
 * @return current SSL error number
 */
uint32_t ERR_peek_error(void);

/**
 * @brief peek the last SSL error code, not clearing it
 *
 * @param none
 *
 * @return current SSL error number
 */
uint32_t ERR_peek_last_error(void);

/**
 * @brief capture the current error to the error structure
 *
 * @param library Related library
 * @param unused Not used (used for compliant function prototype)
 * @param reason The actual error code
 * @param file File name of the error report
 * @param line Line number of the error report
 *
 */
void ERR_put_error(int library, int unused, int reason, const char *file, unsigned line);

/**
 * @brief Peek the current SSL error, not clearing it
 *
 * @param file file name of the reported error
 * @param line line number of the reported error
 * @param data Associated data to the reported error
 * @param flags Flags associated to the error
 *
 * @return current SSL error number
 */
uint32_t ERR_peek_error_line_data(const char **file, int *line,
                                  const char **data, int *flags);

/**
 * @brief Get the current SSL error
 *
 * @param file file name of the reported error
 * @param line line number of the reported error
 * @param data Associated data to the reported error
 * @param flags Flags associated to the error
 *
 * @return current SSL error number
 */
uint32_t ERR_get_error_line_data(const char **file, int *line,
                                  const char **data, int *flags);

/**
 * @brief API provided as a declaration only
 *
 */
void SSL_load_error_strings(void);

/**
 * @brief API provided as a declaration only
 *
 */
void ERR_free_strings(void);

/**
 * @brief API provided as a declaration only
 *
 */
void ERR_remove_state(unsigned long pid);

/**
 * @brief Returns error string -- Not implemented
 *
 * @param packed_error Packed error code
 *
 * @return NULL
 */
const char *ERR_reason_error_string(uint32_t packed_error);

#ifdef __cplusplus
}
#endif

#endif // _OPENSSL_ERR_H
