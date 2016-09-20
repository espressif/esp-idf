#ifndef _SSL3_H_
#define _SSL3_H_

# define SSL3_AD_CLOSE_NOTIFY             0
# define SSL3_AD_UNEXPECTED_MESSAGE      10/* fatal */
# define SSL3_AD_BAD_RECORD_MAC          20/* fatal */
# define SSL3_AD_DECOMPRESSION_FAILURE   30/* fatal */
# define SSL3_AD_HANDSHAKE_FAILURE       40/* fatal */
# define SSL3_AD_NO_CERTIFICATE          41
# define SSL3_AD_BAD_CERTIFICATE         42
# define SSL3_AD_UNSUPPORTED_CERTIFICATE 43
# define SSL3_AD_CERTIFICATE_REVOKED     44
# define SSL3_AD_CERTIFICATE_EXPIRED     45
# define SSL3_AD_CERTIFICATE_UNKNOWN     46
# define SSL3_AD_ILLEGAL_PARAMETER       47/* fatal */

# define SSL3_AL_WARNING                  1
# define SSL3_AL_FATAL                    2

#define SSL3_VERSION                 0x0300

#endif
