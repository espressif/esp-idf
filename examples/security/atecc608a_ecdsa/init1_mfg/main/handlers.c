/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <string.h>
#include "stdio.h"
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_csr.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"
#include "rom/crc.h"

#include "soc/soc.h"
#include "soc/efuse_reg.h"
#include "esp_efuse.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "rom/secure_boot.h"

#include "handlers.h"
#include "secure_cert_config.h"
#include "security_utils.h"

#ifdef ESPWROOM32SE
/* Cryptoauthlib includes */
#include "cryptoauthlib.h"
#include "cert_def_3_device_csr.h"
#include "cert_def_2_device.h"
#include "cert_def_1_signer.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_pem.h"

#include "mbedtls/atca_mbedtls_wrap.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
static bool is_atcab_init=false;
#endif /* ESPWROOM32SE */

#ifdef CONFIG_MFG_DEBUGGING
#define ESP_MFG_DEBUG ESP_LOGI
#else
#define ESP_MFG_DEBUG(...)
#endif /* MFG_DEBUG */

#define MFG_DBG "Secure Cert"
static mbedtls_pk_context key;
extern QueueHandle_t uart_queue;

/* An example hash */
static unsigned char hash[32] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

static const uint8_t public_key_x509_header[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04
};

static esp_err_t initialise_atcab() 
{   int ret = 0;
    printf("  . Initialize the ATECC interface...");
    if(0 != (ret = atcab_init(&cfg_ateccx08a_i2c_default)))
    {
        printf(" failed\n  ! atcab_init returned %02x\n", ret);
        goto exit;
    }
    printf(" ok\n");
    return ESP_OK;
exit:
    return ESP_FAIL;
}

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

static esp_err_t secure_cert_write(size_t offset, unsigned char *buffer, size_t buffer_len ,uint8_t magic_byte)
{
    /* Align flash writing to 32 bytes */
    size_t byte_aligned_buffer = 0;
    if (strlen((char *)buffer) % 32 != 0) {
        byte_aligned_buffer = strlen((char *)buffer) + (32 - (strlen((char *)buffer) % 32));
        ESP_MFG_DEBUG(MFG_DBG,"Length of data written to flash after alignment %d", byte_aligned_buffer);

        if (buffer_len < byte_aligned_buffer) {
            ESP_MFG_DEBUG(MFG_DBG,"Not enough memory to align the buffer to 32 bytes for writing to flash.");
            return ESP_FAIL;
        }
    }

    esp_partition_iterator_t it = esp_partition_find(SECURE_CERT_PARTITION_TYPE, ESP_PARTITION_SUBTYPE_ANY, SECURE_CERT_PARTITION_NAME);
    if (it == NULL) {
        ESP_MFG_DEBUG(MFG_DBG,"Partition not found.");
        return ESP_FAIL;
    }

    const esp_partition_t * part = esp_partition_get(it);
    if (part == NULL) {
        ESP_MFG_DEBUG(MFG_DBG,"Could not get partition.");
        return ESP_FAIL;
    }

    esp_err_t err;
    err = esp_partition_erase_range(part, offset, SECURE_CERT_OFFSET);
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG,"Could not erase partition."); 
        return ESP_FAIL;
    }

    uint32_t crc = crc32_le(UINT32_MAX ,(const uint8_t * )buffer, strlen((char *)buffer));
    uint32_t len = strlen((char *)buffer);

    /* SPI flash writes on encrypted partitions have to be 32 byte aligned */
    uint8_t byte_aligned_metadata[METADATA_SIZE];

    bzero(byte_aligned_metadata, METADATA_SIZE);
    secure_cert_metadata metadata = {
        .secure_cert_crc = crc,
        .magic_byte = magic_byte,
        .secure_cert_len = len
    };    
    memcpy(byte_aligned_metadata, &metadata, sizeof(metadata)); /* sizeof(secure_cert_metadata) bytes copied, last bytes are empty */

    /* esp_partition_write handles the encrypted/non-encrypted writes wrt to flash encryption */
    err = esp_partition_write(part, offset, &byte_aligned_metadata, METADATA_SIZE);
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG,"Could not write to partition.");
        return ESP_FAIL;
    }

    err = esp_partition_write(part, offset + METADATA_SIZE, buffer, byte_aligned_buffer);
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG,"Could not write to partition.");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mfg_rsa_keygen(int key_size, unsigned char *private_key_buf, size_t private_key_buf_len)
{
#ifdef ESPWROOM32SE
    /* This is the function to generate a private key in one of the slots of ATECC608A which is specificcaly
     * configured to do so. 
     * Note:- The key generated here will be ECC 256 key which will be generated internally by a RNG placed 
     * inside the ATECC608A chip and will be stored in the designated slot, there is no way to read the key,
     * one can only sign the data with the key.
     * This function generates key in specefied slot which is 0 in our case */
    /* Config zone, Data Zone must be locked for this to proceed */
    int ret = 0; 
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];
    if (!is_atcab_init) {
        if(initialise_atcab() != ESP_OK)
            goto exit;
    }    
    //printf("\n Slot 0 already has the key\n");
    // Disabled for now , as we aleady have saved one private key on slot 0
    printf("Generating Priv key ..");
    ret = atcab_genkey(1, pubkey);
    if(ret != ATCA_SUCCESS) {
        printf("\n Failure");
        ESP_MFG_DEBUG(MFG_DBG,"atcab_genkey returned -0x%02x\n", -ret);
        goto exit;
    }
    printf("\t OK\n");
    ESP_MFG_DEBUG(MFG_DBG,"Private key successfully generated");
    /* To Do - put print_public_key under verbose/debug flag */
    ESP_LOGV(MFG_DBG," Public key is");
    print_public_key(pubkey);
    return ESP_OK; 
    
exit:
    printf("\nFailure\n");
    return ESP_FAIL;
#else
    const char *pers = "gen_key";
    mbedtls_mpi N, P, Q, D, E, DP, DQ, QP;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&D); 
    mbedtls_mpi_init(&E); mbedtls_mpi_init(&DP);
    mbedtls_mpi_init(&DQ); mbedtls_mpi_init(&QP);

    mbedtls_pk_free(&key);
    mbedtls_pk_init(&key);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    memset(private_key_buf, 0, private_key_buf_len);

    ESP_MFG_DEBUG(MFG_DBG, "Seeding the random number generator.");
    mbedtls_entropy_init(&entropy);
    int ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen(pers));
    if (ret != 0) {
        ESP_MFG_DEBUG(MFG_DBG,"mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret );
        goto exit;
    }

    ESP_MFG_DEBUG(MFG_DBG,"Generating the private key.." );
    ret = mbedtls_pk_setup(&key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (ret != 0) {
        ESP_MFG_DEBUG(MFG_DBG,"mbedtls_pk_setup returned -0x%04x", -ret );
        goto exit;
    }

    ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(key), mbedtls_ctr_drbg_random, &ctr_drbg, key_size, 65537); /* here, 65537 is the RSA exponent */
    if (ret != 0) {
        ESP_MFG_DEBUG(MFG_DBG,"mbedtls_rsa_gen_key returned -0x%04x", -ret );
        goto exit;
    }
    
    ESP_MFG_DEBUG(MFG_DBG, "Exporting the RSA parameters.");
    mbedtls_rsa_context *rsa = mbedtls_pk_rsa(key);
    if (mbedtls_rsa_export(rsa, &N, &P, &Q, &D, &E) != 0 || 
        mbedtls_rsa_export_crt(rsa, &DP, &DQ, &QP) != 0) {
            ESP_MFG_DEBUG(MFG_DBG,"Could not export RSA parameters");
            goto exit;
    }

    ESP_MFG_DEBUG(MFG_DBG, "Writing the key to flash");
    memset(private_key_buf, 0, private_key_buf_len);
    ret = mbedtls_pk_write_key_pem(&key, private_key_buf, private_key_buf_len);
    if (ret != 0) {
        ESP_MFG_DEBUG(MFG_DBG,"mbedtls_pk_write_key_pem returned -0x%04x\n", -ret );
        goto exit;
    }

    ESP_MFG_DEBUG(MFG_DBG, "Private Key written to flash");

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P); 
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&D); 
    mbedtls_mpi_free(&E); mbedtls_mpi_free(&DP);
    mbedtls_mpi_free(&DQ); mbedtls_mpi_free(&QP);

    // mbedtls_pk_free(&key);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    return ret;
#endif
}

esp_err_t mfg_csr_gen(const char * subject_name, unsigned char *csr_buf, size_t csr_buf_len)
{
#ifdef ESPWROOM32SE
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];
    int ret = 0;
    if (!is_atcab_init) {
        if( initialise_atcab() != ESP_OK)
            goto exit;
    }
    bzero(csr_buf, csr_buf_len);
    printf("Generating CSR ..");
    ret = atcacert_create_csr_pem(&g_csr_def_3_device, (char*)csr_buf, &csr_buf_len);
    if (ret != ATCA_SUCCESS) {
        printf("\n create csr pem failed, returned %02x \n", ret);
        goto exit;
    }
    printf("\t OK\n");
     
    ret = ESP_OK;
    return ret;
    
exit:
    printf("\nFailure, Exiting , ret is %02x\n", ret);
    return ret;

#else
    const char *pers = "gen_csr";
    mbedtls_x509write_csr csr;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;

    /* Generating CSR from the private key */
    mbedtls_x509write_csr_init(&csr);
    mbedtls_x509write_csr_set_md_alg(&csr, MBEDTLS_MD_SHA256);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ESP_MFG_DEBUG(MFG_DBG, "Seeding the random number generator.");
    mbedtls_entropy_init(&entropy);
    int ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen(pers));
    if (ret != 0) {
        ESP_MFG_DEBUG(MFG_DBG,"mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret );
        goto exit;
    }

    if (subject_name) {
        ret = mbedtls_x509write_csr_set_subject_name(&csr, subject_name);
        if (ret != 0) {
            ESP_MFG_DEBUG(MFG_DBG, "mbedtls_x509write_csr_set_subject_name returned %d", ret );
            goto exit;
        }
    }

    bzero(csr_buf, csr_buf_len);
    mbedtls_x509write_csr_set_key(&csr, &key);
    ret = mbedtls_x509write_csr_pem(&csr, csr_buf, csr_buf_len, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret < 0) {
        ESP_MFG_DEBUG(MFG_DBG,"mbedtls_x509write_csr_pem returned -0x%04x\n", -ret );
        goto exit;
    }
exit:

    mbedtls_x509write_csr_free(&csr);
    // mbedtls_pk_free(&key);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    return ret;
#endif
}

esp_err_t mfg_input_cert(unsigned char *cert_buf, size_t cert_len
#ifdef ESPWROOM32SE
,cert_type_t cert_type
#endif
)
{
    int uart_num = 0, i = 0;
    esp_err_t ret;
    uart_event_t event;

    memset(cert_buf, 0xff, cert_len);
    do {
        ret = xQueueReceive(uart_queue, (void * )&event, (portTickType) portMAX_DELAY);
        if (ret != pdPASS)
            continue;

        if (event.type == UART_DATA) {
            while (uart_read_bytes(uart_num, (uint8_t *) &cert_buf[i], 1, 0)) {
                if (cert_buf[i] == '\0') break;
                i++;
            }
        }  
    } while (i < cert_len-1 && cert_buf[i] != '\0');
    ESP_MFG_DEBUG(MFG_DBG, "Certificate :\n %s", cert_buf);
#ifdef ESPWROOM32SE
    /* We have to print the cert onto the ATECC608A,
     * Preperation - You should already have a CA which you will register with cloud, if not Create one sample 
     * CA certificate using openssl
     * commands -
     * 1)   Create CA key - `openssl ecparam -out CAkey.pem -name prime256v1 -genkey`
     *      Create CA cert - `openssl req -new -x509 -key CAkey.pem -out CAcert.pem -days XXX`
     *      fill in all the requested details for e.g.CN.
     * 
     * 2)  Generate a temporary CSR as follows, and fill in the required details which will 
     *      be used in the device certificate
     *      Gen sample device key - `openssl req -new -key dev_key.pem -out dev_cert.csr`
     *      Gen device cert CSR - `openssl req -new -key dev_key.pem -out dev_cert.csr`
     *
     * 3) Generate a temporary device cert just to create the device cert definition.
     *      Gen Cert - 
     *      `openssl x509 -req -days 365 -in dev_cert.csr -signkey CAkey.pem -sha256 -out dev_cert.pem`
     *      Verify The Contents of the cert using
     * 4)  (Optional) Verify the contents of the device
     *      `openssl x509 -in `name-of-cert` -noout -text`
     * 5)   Create The definitions of the certificates and CSR which will be used by the ATECC608A.
     *      use the python file cert2defcert.py , 
     *       
     *      i)first install all the requirements with 
     *      `pip install -r requirements.txt`
     *      
     *      ii) Create csr definition and update the example definition with latest definition by 
     *      following command
     *      Note: dev_cert.csr in following command represents the sample device cert csr generated
     *      in step 2 , you may change it to `your-csr-name.csr`
     *      `python cert2defcert.py --device-csr dev_cert.csr > components/cryptoauthlib/cert_def_3_device_csr.c`
     *      
     *      iii) Create device cert definition & update the example definition with latest definition by
     *      following command
     *      `python cert2defcert.py --device-cert dev_cert.pem >components/cryptoauthlib/cert_def_2_device.c`
     *      
     * Steps- 1) Get the device cert from host
     *        2) Write the cert in the slot mentioned in the cert def provided to the function.
     *        3) Get the CA cert from host
     *        4) Write the CA cert in the slot mentioned. */
    if (!is_atcab_init) {
        if( initialise_atcab() != ESP_OK)
            goto exit;
    }  
    printf("atcacert write cert");
    if (cert_type == CERT_TYPE_DEVICE) {
    ret = atcacert_write_cert(&g_cert_def_2_device, cert_buf, cert_len);
    if(ret != ATCA_SUCCESS) {
        printf("\n writecert failed , ret is %02x \n", ret);
        goto exit;
    }
    }
    else if (cert_type == CERT_TYPE_CA) {
    ret = atcacert_write_cert(&g_cert_def_1_signer, cert_buf, cert_len);
    if(ret != ATCA_SUCCESS) {
        printf("\n writecert failed , ret is %02x \n", ret);
        goto exit;
    }
    }
    else {
        printf("\nWrong Cert Type\n");
        goto exit;
    }
    printf("\ncert written successfully\n");
    ret = ESP_OK;
    return ret;
exit:
    printf("\nFailure Exiting\n");
    return ret;
#endif
    return ESP_OK;
}

#define STEP_SIZE 0x1000
uint8_t overwrite_boot_buff[STEP_SIZE];
esp_err_t mfg_overwrite_bootloader(unsigned char * private_key_buf, unsigned char * device_cert_buf, unsigned char * ca_cert_buf)
{

    int size = 0;
    uint32_t cust_boot =  0x10000;

#ifdef CONFIG_MFG_TEST_MODE
    ESP_MFG_DEBUG(MFG_DBG, "Test Mode. Skipping flash encryption keygen.");
#else
    if (flash_encryption_keygen() != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "Error in generating flash encryption key");
    }
#endif /* CONFIG_MFG_TEST_MODE */
    if (secure_cert_write(0, private_key_buf, 4096, SECURE_CERT_PKEY_MAGIC_BYTE) != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "Private Key could not be written to flash");
    }

    if (secure_cert_write(SECURE_CERT_OFFSET, device_cert_buf, 4096, SECURE_DEVICE_CERT_MAGIC_BYTE) != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG,"The device cert could not be written to flash!");
    }

    if (secure_cert_write(2 * SECURE_CERT_OFFSET, ca_cert_buf, 4096, SECURE_CA_CERT_MAGIC_BYTE) != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG,"The device cert could not be written to flash!");
    }

#ifdef CONFIG_MFG_TEST_MODE
    ESP_MFG_DEBUG(MFG_DBG, "Test Mode. Skipping secure boot.");
#else
    if (esp_cust_secure_boot_enable() != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "Error in generating secure boot key");
    }
#endif /* CONFIG_TEST_MODE */
 
    ESP_MFG_DEBUG(MFG_DBG,"Erasing the existing bootloader!");
    spi_flash_erase_range(0, CONFIG_PARTITION_TABLE_OFFSET);

    for(;size < CONFIG_PARTITION_TABLE_OFFSET; size += STEP_SIZE) {
        ESP_MFG_DEBUG(MFG_DBG,"Reading from cust_boot at %x!", cust_boot + size);
        spi_flash_read(cust_boot + size, overwrite_boot_buff, STEP_SIZE );

        ESP_MFG_DEBUG(MFG_DBG,"Writing to boot at %x!", size);
        spi_flash_write(size, overwrite_boot_buff, STEP_SIZE);

        ESP_MFG_DEBUG(MFG_DBG,"Erasing from flash at %x!", size); 
        spi_flash_erase_range(cust_boot + size, STEP_SIZE);
    }
    return ESP_OK;
}
