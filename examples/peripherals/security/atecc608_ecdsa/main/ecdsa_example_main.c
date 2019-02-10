/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/* This is mbedtls boilerplate for library configuration */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

/* System Includes*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

/* Cryptoauthlib includes */
#include "cryptoauthlib.h"
#include "mbedtls/atca_mbedtls_wrap.h"

/* mbedTLS includes */
#include "mbedtls/platform.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pk.h"

/* globals for mbedtls RNG */
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;

static int configure_mbedtls_rng(void)
{
    int ret;
    const char * seed = "some random seed string";
    mbedtls_ctr_drbg_init(&ctr_drbg);

    printf("\n  . Seeding the random number generator...");

    mbedtls_entropy_init(&entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
        (const unsigned char *)seed, strlen(seed))) != 0)
    {
        printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    }
    else
    {
        printf(" ok\n");
    }
    return ret;
}

static void close_mbedtls_rng(void)
{
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

/* An example hash */
static unsigned char hash[32] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

static const uint8_t public_key_x509_header[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04
};

static void print_public_key(uint8_t pubkey[ATCA_PUB_KEY_SIZE])
{
    uint8_t buf[128];
    uint8_t * tmp;
    size_t buf_len = sizeof(buf);

    /* Calculate where the raw data will fit into the buffer */
    tmp = buf + sizeof(buf) - ATCA_PUB_KEY_SIZE - sizeof(public_key_x509_header);

    /* Copy the header */
    memcpy(tmp, public_key_x509_header, sizeof(public_key_x509_header));

    /* Copy the key bytes */
    memcpy(tmp + sizeof(public_key_x509_header), pubkey, ATCA_PUB_KEY_SIZE);

    /* Convert to base 64 */
    (void)atcab_base64encode(tmp, ATCA_PUB_KEY_SIZE + sizeof(public_key_x509_header), (char*)buf, &buf_len);

    /* Add a null terminator */
    buf[buf_len] = 0;

    /* Print out the key */
    printf("-----BEGIN PUBLIC KEY-----\r\n%s\r\n-----END PUBLIC KEY-----\r\n", buf);
}


extern int atca_connect(const char * endpoint, const char * port);
extern int atca_provision(void);

static int atca_ecdsa_test(void)
{
    mbedtls_pk_context pkey;
    int ret;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    size_t olen = 0;

    /* ECDSA Sign/Verify */

#ifdef MBEDTLS_ECDSA_SIGN_ALT
    /* Convert to an mbedtls key */
    printf( "\n  . Using a hardware private key ..." );
    if (0 != (ret = atca_mbedtls_pk_init(&pkey, 0)))
    {
        printf(" failed\n  !  atca_mbedtls_pk_init returned %02x\n", ret);
        goto exit;
    }
    printf(" ok\n");
#else
    printf( "\n  . Generating a software private key ..." );
    mbedtls_pk_init(&pkey);
    if( ( ret = mbedtls_pk_setup( &pkey,
            mbedtls_pk_info_from_type( MBEDTLS_PK_ECDSA ) ) ) != 0 )
    {
        printf( " failed\n  !  mbedtls_pk_setup returned -0x%04x", -ret );
        goto exit;
    }

    ret = mbedtls_ecp_gen_key( MBEDTLS_ECP_DP_SECP256R1,
                                   mbedtls_pk_ec( pkey ),
                                   mbedtls_ctr_drbg_random, &ctr_drbg );
    if( ret != 0 )
    {
        printf( " failed\n  !  mbedtls_ecp_gen_key returned -0x%04x", -ret );
        goto exit;
    }
    printf(" ok\n");
#endif

    printf("  . Generating ECDSA Signature...");
    if ((ret = mbedtls_pk_sign(&pkey, MBEDTLS_MD_SHA256, hash, 0, buf, &olen,
        mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_sign returned -0x%04x\n", -ret);
        goto exit;
    }
    printf(" ok\n");

    printf("  . Verifying ECDSA Signature...");
    if ((ret = mbedtls_pk_verify(&pkey, MBEDTLS_MD_SHA256, hash, 0,
        buf, olen)) != 0)
    {
        printf(" failed\n  ! mbedtls_pk_verify returned -0x%04x\n", -ret);
        goto exit;
    }
    printf(" ok\n");

exit:
    fflush(stdout);
    return ret;
}


void app_main(void)
{
    int ret = 0;
    bool lock;
    uint8_t buf[ATCA_ECC_CONFIG_SIZE];
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];

    /* Initialize the mbedtls library */
    if (!ret)
    {
        ret = configure_mbedtls_rng();
    }

    printf("  . Initialize the ATECC interface...");
    if(0 != (ret = atcab_init(&cfg_ateccx08a_i2c_default)))
    {
        printf(" failed\n  ! atcab_init returned %02x\n", ret);
        goto exit;
    }
    printf(" ok\n");

    lock = 0;
    printf("  . Check the data zone lock status...");
    if (0 != (ret = atcab_is_locked(LOCK_ZONE_DATA, &lock)))
    {
        printf(" failed\n  ! atcab_is_locked returned %02x\n", ret);
        goto exit;
    }
    printf(" ok: %s\n", lock ? "locked" : "unlocked");

    printf("  . Get the device info (type)...");
    if (0 != (ret = atcab_info(buf)))
    {
        printf(" failed\n  ! atcab_info returned %02x\n", ret);
        goto exit;
    }
    printf(" ok: %02x %02x\n", buf[2], buf[3]);

    /* If the data zone is not locked (i.e. the configuration is not activated
        then if these were run they would simply return execution errors */
    if (!ret && lock)
    {
        printf("  . Get the public key...");
        if (0 != (ret = atcab_get_pubkey(0, pubkey)))
        {
            printf(" failed\n  ! atcab_get_pubkey returned %02x\n", ret);
            goto exit;
        }
        printf(" ok\n\n");
        print_public_key(pubkey);

        if(!ret)
        {
            /* Perform a Sign/Verify Test */
            ret = atca_ecdsa_test();
        }
    }

exit:
    fflush(stdout);
    close_mbedtls_rng();

}

