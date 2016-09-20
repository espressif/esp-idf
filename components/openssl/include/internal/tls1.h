#ifndef _TLS1_H_
#define _TLS1_H_

# define TLS1_AD_DECRYPTION_FAILED       21
# define TLS1_AD_RECORD_OVERFLOW         22
# define TLS1_AD_UNKNOWN_CA              48/* fatal */
# define TLS1_AD_ACCESS_DENIED           49/* fatal */
# define TLS1_AD_DECODE_ERROR            50/* fatal */
# define TLS1_AD_DECRYPT_ERROR           51
# define TLS1_AD_EXPORT_RESTRICTION      60/* fatal */
# define TLS1_AD_PROTOCOL_VERSION        70/* fatal */
# define TLS1_AD_INSUFFICIENT_SECURITY   71/* fatal */
# define TLS1_AD_INTERNAL_ERROR          80/* fatal */
# define TLS1_AD_INAPPROPRIATE_FALLBACK  86/* fatal */
# define TLS1_AD_USER_CANCELLED          90
# define TLS1_AD_NO_RENEGOTIATION        100
/* codes 110-114 are from RFC3546 */
# define TLS1_AD_UNSUPPORTED_EXTENSION   110
# define TLS1_AD_CERTIFICATE_UNOBTAINABLE 111
# define TLS1_AD_UNRECOGNIZED_NAME       112
# define TLS1_AD_BAD_CERTIFICATE_STATUS_RESPONSE 113
# define TLS1_AD_BAD_CERTIFICATE_HASH_VALUE 114
# define TLS1_AD_UNKNOWN_PSK_IDENTITY    115/* fatal */
# define TLS1_AD_NO_APPLICATION_PROTOCOL 120 /* fatal */

/* Special value for method supporting multiple versions */
#define TLS_ANY_VERSION                 0x10000

#define TLS1_VERSION                    0x0301
#define TLS1_1_VERSION                  0x0302
#define TLS1_2_VERSION                  0x0303

#endif
