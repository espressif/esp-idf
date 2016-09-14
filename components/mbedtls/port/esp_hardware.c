#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

extern int os_get_random(unsigned char *buf, size_t len);
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    os_get_random(output, len);
    *olen = len;

    return 0;
}
#endif

