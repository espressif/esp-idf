/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_hmac.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP32C3_ERR_HW_CRYPTO_DS_HMAC_FAIL           ESP_ERR_HW_CRYPTO_BASE + 0x1 /*!< HMAC peripheral problem */
#define ESP32C3_ERR_HW_CRYPTO_DS_INVALID_KEY         ESP_ERR_HW_CRYPTO_BASE + 0x2 /*!< given HMAC key isn't correct,
                                                                                HMAC peripheral problem */
#define ESP32C3_ERR_HW_CRYPTO_DS_INVALID_DIGEST      ESP_ERR_HW_CRYPTO_BASE + 0x4 /*!< message digest check failed,
                                                                                result is invalid */
#define ESP32C3_ERR_HW_CRYPTO_DS_INVALID_PADDING     ESP_ERR_HW_CRYPTO_BASE + 0x5 /*!< padding check failed, but result
                                                                                   is produced anyway and can be read*/

#define ESP_DS_IV_BIT_LEN 128
#define ESP_DS_IV_LEN (ESP_DS_IV_BIT_LEN / 8)
#define ESP_DS_SIGNATURE_MAX_BIT_LEN 3072
#define ESP_DS_SIGNATURE_MD_BIT_LEN 256
#define ESP_DS_SIGNATURE_M_PRIME_BIT_LEN 32
#define ESP_DS_SIGNATURE_L_BIT_LEN 32
#define ESP_DS_SIGNATURE_PADDING_BIT_LEN 64

/* Length of parameter 'C' stored in flash, in bytes
   - Operands Y, M and r_bar; each 3072 bits
   - Operand MD (message digest); 256 bits
   - Operands M' and L; each 32 bits
   - Operand beta (padding value; 64 bits
*/
#define ESP_DS_C_LEN (((ESP_DS_SIGNATURE_MAX_BIT_LEN * 3 \
        + ESP_DS_SIGNATURE_MD_BIT_LEN   \
        + ESP_DS_SIGNATURE_M_PRIME_BIT_LEN   \
        + ESP_DS_SIGNATURE_L_BIT_LEN   \
        + ESP_DS_SIGNATURE_PADDING_BIT_LEN) / 8))

typedef struct esp_ds_context esp_ds_context_t;

typedef enum {
    ESP_DS_RSA_1024 = (1024 / 32) - 1,
    ESP_DS_RSA_2048 = (2048 / 32) - 1,
    ESP_DS_RSA_3072 = (3072 / 32) - 1
} esp_digital_signature_length_t;

/**
 * Encrypted private key data. Recommended to store in flash in this format.
 *
 * @note This struct has to match to one from the ROM code! This documentation is mostly taken from there.
 */
typedef struct esp_digital_signature_data {
    /**
     * RSA LENGTH register parameters
     * (number of words in RSA key & operands, minus one).
     *
     * Max value 127 (for RSA 3072).
     *
     * This value must match the length field encrypted and stored in 'c',
     * or invalid results will be returned. (The DS peripheral will
     * always use the value in 'c', not this value, so an attacker can't
     * alter the DS peripheral results this way, it will just truncate or
     * extend the message and the resulting signature in software.)
     *
     * @note In IDF, the enum type length is the same as of type unsigned, so they can be used interchangably.
     *       See the ROM code for the original declaration of struct \c ets_ds_data_t.
     */
    esp_digital_signature_length_t rsa_length;

    /**
     * IV value used to encrypt 'c'
     */
    uint32_t iv[ESP_DS_IV_BIT_LEN / 32];

    /**
     * Encrypted Digital Signature parameters. Result of AES-CBC encryption
     * of plaintext values. Includes an encrypted message digest.
     */
    uint8_t c[ESP_DS_C_LEN];
} esp_ds_data_t;

/**
 * Plaintext parameters used by Digital Signature.
 *
 * This is only used for encrypting the RSA parameters by calling esp_ds_encrypt_params().
 * Afterwards, the result can be stored in flash or in other persistent memory.
 * The encryption is a prerequisite step before any signature operation can be done.
 */
typedef struct {
    uint32_t  Y[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32]; //!< RSA exponent
    uint32_t  M[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32]; //!< RSA modulus
    uint32_t Rb[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32]; //!< RSA r inverse operand
    uint32_t M_prime;                               //!< RSA M prime operand
    uint32_t length;                                //!< RSA length in words (32 bit)
} esp_ds_p_data_t;

/**
 * @brief Sign the message with a hardware key from specific key slot.
 * The function calculates a plain RSA signature with help of the DS peripheral.
 * The RSA encryption operation is as follows:
 * Z = XY mod M where,
 * Z is the signature, X is the input message,
 * Y and M are the RSA private key parameters.
 *
 * This function is a wrapper around \c esp_ds_finish_sign() and \c esp_ds_start_sign(), so do not use them
 * in parallel.
 * It blocks until the signing is finished and then returns the signature.
 *
 * @note This function locks the HMAC, SHA, AES and RSA components during its entire execution time.
 *
 * @param message the message to be signed; its length should be (data->rsa_length + 1)*4 bytes
 * @param data the encrypted signing key data (AES encrypted RSA key + IV)
 * @param key_id the HMAC key ID determining the HMAC key of the HMAC which will be used to decrypt the
 *        signing key data
 * @param signature the destination of the signature, should be (data->rsa_length + 1)*4 bytes long
 *
 * @return
 *      - ESP_OK if successful, the signature was written to the parameter \c signature.
 *      - ESP_ERR_INVALID_ARG if one of the parameters is NULL or data->rsa_length is too long or 0
 *      - ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL if there was an HMAC failure during retrieval of the decryption key
 *      - ESP_ERR_NO_MEM if there hasn't been enough memory to allocate the context object
 *      - ESP_ERR_HW_CRYPTO_DS_INVALID_KEY if there's a problem with passing the HMAC key to the DS component
 *      - ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST if the message digest didn't match; the signature is invalid.
 *      - ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING if the message padding is incorrect, the signature can be read though
 *        since the message digest matches.
 */
esp_err_t esp_ds_sign(const void *message,
        const esp_ds_data_t *data,
        hmac_key_id_t key_id,
        void *signature);

/**
 * @brief Start the signing process.
 *
 * This function yields a context object which needs to be passed to \c esp_ds_finish_sign() to finish the signing
 * process.
 * The function calculates a plain RSA signature with help of the DS peripheral.
 * The RSA encryption operation is as follows:
 * Z = XY mod M where,
 * Z is the signature, X is the input message,
 * Y and M are the RSA private key parameters.
 *
 * @note This function locks the HMAC, SHA, AES and RSA components, so the user has to ensure to call
 *       \c esp_ds_finish_sign() in a timely manner.
 *
 * @param message the message to be signed; its length should be (data->rsa_length + 1)*4 bytes
 * @param data the encrypted signing key data (AES encrypted RSA key + IV)
 * @param key_id the HMAC key ID determining the HMAC key of the HMAC which will be used to decrypt the
 *        signing key data
 * @param esp_ds_ctx the context object which is needed for finishing the signing process later
 *
 * @return
 *      - ESP_OK if successful, the ds operation was started now and has to be finished with \c esp_ds_finish_sign()
 *      - ESP_ERR_INVALID_ARG if one of the parameters is NULL or data->rsa_length is too long or 0
 *      - ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL if there was an HMAC failure during retrieval of the decryption key
 *      - ESP_ERR_NO_MEM if there hasn't been enough memory to allocate the context object
 *      - ESP_ERR_HW_CRYPTO_DS_INVALID_KEY if there's a problem with passing the HMAC key to the DS component
 */
esp_err_t esp_ds_start_sign(const void *message,
        const esp_ds_data_t *data,
        hmac_key_id_t key_id,
        esp_ds_context_t **esp_ds_ctx);

/**
 * Return true if the DS peripheral is busy, otherwise false.
 *
 * @note Only valid if \c esp_ds_start_sign() was called before.
 */
bool esp_ds_is_busy(void);

/**
 * @brief Finish the signing process.
 *
 * @param signature the destination of the signature, should be (data->rsa_length + 1)*4 bytes long
 * @param esp_ds_ctx the context object retreived by \c esp_ds_start_sign()
 *
 * @return
 *      - ESP_OK if successful, the ds operation has been finished and the result is written to signature.
 *      - ESP_ERR_INVALID_ARG if one of the parameters is NULL
 *      - ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST if the message digest didn't match; the signature is invalid.
 *        This means that the encrypted RSA key parameters are invalid, indicating that they may have been tampered
 *        with or indicating a flash error, etc.
 *      - ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING if the message padding is incorrect, the signature can be read though
 *        since the message digest matches (see TRM for more details).
 */
esp_err_t esp_ds_finish_sign(void *signature, esp_ds_context_t *esp_ds_ctx);

/**
 * @brief Encrypt the private key parameters.
 *
 * The encryption is a prerequisite step before any signature operation can be done.
 * It is not strictly necessary to use this encryption function, the encryption could also happen on an external
 * device.
 *
 * @param data Output buffer to store encrypted data, suitable for later use generating signatures.
 *        The allocated memory must be in internal memory and word aligned since it's filled by DMA. Both is asserted
 *        at run time.
 * @param iv Pointer to 16 byte IV buffer, will be copied into 'data'. Should be randomly generated bytes each time.
 * @param p_data Pointer to input plaintext key data. The expectation is this data will be deleted after this process
 *        is done and 'data' is stored.
 * @param key Pointer to 32 bytes of key data. Type determined by key_type parameter. The expectation is the
 *        corresponding HMAC key will be stored to efuse and then permanently erased.
 *
 * @return
 *      - ESP_OK if successful, the ds operation has been finished and the result is written to signature.
 *      - ESP_ERR_INVALID_ARG if one of the parameters is NULL or p_data->rsa_length is too long
 */
esp_err_t esp_ds_encrypt_params(esp_ds_data_t *data,
        const void *iv,
        const esp_ds_p_data_t *p_data,
        const void *key);

#ifdef __cplusplus
}
#endif
