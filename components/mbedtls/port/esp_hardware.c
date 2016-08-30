#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
/**
 * \brief           Entropy poll callback for a hardware source
 *
 * \warning         This is not provided by mbed TLS!
 *                  See \c MBEDTLS_ENTROPY_HARDWARE_ALT in config.h.
 *
 * \note            This must accept NULL as its first argument.
 */
static int os_get_random(unsigned char *buf, size_t len)
{
	int i, j;
	unsigned long tmp;
	for (i = 0; i < ((len + 3) & ~3) / 4; i ++){
		tmp = rand();
		for (j = 0; j < 4; j ++){
			if ((i * 4 + j) < len){
				buf[i * 4 + j] = (unsigned char)(tmp >> (j * 8));
			}else{
				break;
			}
		}

	}
	return 0;
}

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
	os_get_random(output, len);
	*olen = len;
	return 0;
}
#endif

