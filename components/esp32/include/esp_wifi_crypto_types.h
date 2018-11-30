// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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


#ifndef __ESP_WIFI_CRYPTO_TYPES_H__
#define __ESP_WIFI_CRYPTO_TYPES_H__

/* This is an internal API header for configuring the implementation used for WiFi cryptographic
   operations.

   During normal operation, you don't need to use any of these types or functions in this header.
   See esp_wifi.h & esp_wifi_types.h instead.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_WIFI_CRYPTO_VERSION 0x00000001

/*
 * Enumeration for hash operations.
 * When WPA2 is connecting, this enum is used to
 * request a hash algorithm via crypto_hash_xxx functions.
 */
typedef enum {
    ESP_CRYPTO_HASH_ALG_MD5, ESP_CRYPTO_HASH_ALG_SHA1,
    ESP_CRYPTO_HASH_ALG_HMAC_MD5, ESP_CRYPTO_HASH_ALG_HMAC_SHA1,
    ESP_CRYPTO_HASH_ALG_SHA256, ESP_CRYPTO_HASH_ALG_HMAC_SHA256
}esp_crypto_hash_alg_t;

/*
 * Enumeration for block cipher operations.
 * When WPA2 is connecting, this enum is used to request a block
 * cipher algorithm via crypto_cipher_xxx functions.
 */
typedef enum {
    ESP_CRYPTO_CIPHER_NULL, ESP_CRYPTO_CIPHER_ALG_AES, ESP_CRYPTO_CIPHER_ALG_3DES,
    ESP_CRYPTO_CIPHER_ALG_DES, ESP_CRYPTO_CIPHER_ALG_RC2, ESP_CRYPTO_CIPHER_ALG_RC4
} esp_crypto_cipher_alg_t;

/*
 * This structure is about the algorithm when do crypto_hash operation, for detail, 
 * please reference to the structure crypto_hash.
 */
typedef struct crypto_hash esp_crypto_hash_t;

/*
 * This structure is about the algorithm when do crypto_cipher operation, for detail, 
 * please reference to the structure crypto_cipher.
 */
typedef struct crypto_cipher esp_crypto_cipher_t;

/**
  * @brief The crypto callback function used in wpa enterprise hash operation when connect.
  *        Initialize a esp_crypto_hash_t structure.
  *
  * @param alg  Hash algorithm.
  * @param key  Key for keyed hash (e.g., HMAC) or %NULL if not needed.
  * @param key_len  Length of the key in bytes
  *
  */
typedef esp_crypto_hash_t * (*esp_crypto_hash_init_t)(esp_crypto_hash_alg_t alg, const unsigned char *key, int key_len);

/**
  * @brief The crypto callback function used in wpa enterprise hash operation when connect. 
  *        Add data to hash calculation.
  *
  * @param ctz  Context pointer from esp_crypto_hash_init_t function.
  * @param data  Data buffer to add.
  * @param len  Length of the buffer.
  *
  */
typedef void (*esp_crypto_hash_update_t)(esp_crypto_hash_t *ctx, const unsigned char *data, int len);

/**
  * @brief The crypto callback function used in wpa enterprise hash operation when connect. 
  *        Complete hash calculation.
  *
  * @param ctz  Context pointer from esp_crypto_hash_init_t function.
  * @param hash   Buffer for hash value or %NULL if caller is just freeing the hash
  *               context.
  * @param len  Pointer to length of the buffer or %NULL if caller is just freeing the
  *             hash context; on return, this is set to the actual length of the hash value
  *             Returns: 0 on success, -1 if buffer is too small (len set to needed length),
  *             or -2 on other failures (including failed crypto_hash_update() operations)
  *
  */
typedef int (*esp_crypto_hash_finish_t)(esp_crypto_hash_t *ctx, unsigned char *hash, int *len);

/**
  * @brief The AES callback function when do WPS connect. 
  *
  * @param key  Encryption key.
  * @param iv  Encryption IV for CBC mode (16 bytes).
  * @param data  Data to encrypt in-place.
  * @param data_len  Length of data in bytes (must be divisible by 16)
  */
typedef int (*esp_aes_128_encrypt_t)(const unsigned char *key, const unsigned char *iv, unsigned char *data, int data_len);

/**
  * @brief The AES callback function when do WPS connect. 
  *
  * @param key  Decryption key.
  * @param iv  Decryption IV for CBC mode (16 bytes).
  * @param data  Data to decrypt in-place.
  * @param data_len  Length of data in bytes (must be divisible by 16)
  *
  */
typedef int (*esp_aes_128_decrypt_t)(const unsigned char *key, const unsigned char *iv, unsigned char *data, int data_len);

/**
  * @brief The AES callback function when do STA connect.
  *
  * @param kek  16-octet Key encryption key (KEK).
  * @param n  Length of the plaintext key in 64-bit units;
  * @param plain  Plaintext key to be wrapped, n * 64 bits
  * @param cipher  Wrapped key, (n + 1) * 64 bits
  *
  */
typedef int (*esp_aes_wrap_t)(const unsigned char *kek, int n, const unsigned char *plain, unsigned char *cipher);

/**
  * @brief The AES callback function when do STA connect.
  *
  * @param kek  16-octet Key decryption key (KEK).
  * @param n  Length of the plaintext key in 64-bit units;
  * @param cipher  Wrapped key to be unwrapped, (n + 1) * 64 bits
  * @param plain  Plaintext key, n * 64 bits
  *
  */
typedef int (*esp_aes_unwrap_t)(const unsigned char *kek, int n, const unsigned char *cipher, unsigned char *plain);

/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Initialize a esp_crypto_cipher_t structure.
  *
  * @param alg  cipher algorithm.
  * @param iv  Initialization vector for block ciphers or %NULL for stream ciphers.
  * @param key  Cipher key
  * @param key_len  Length of key in bytes
  *
  */
typedef esp_crypto_cipher_t * (*esp_crypto_cipher_init_t)(esp_crypto_cipher_alg_t alg, const unsigned char *iv, const unsigned char *key, int key_len);

/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Cipher encrypt.
  *
  * @param ctx  Context pointer from esp_crypto_cipher_init_t callback function.
  * @param plain  Plaintext to cipher.
  * @param crypt  Resulting ciphertext.
  * @param len  Length of the plaintext.
  *
  */
typedef int (*esp_crypto_cipher_encrypt_t)(esp_crypto_cipher_t *ctx,
				             const unsigned char *plain, unsigned char *crypt, int len);
/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Cipher decrypt.
  *
  * @param ctx  Context pointer from esp_crypto_cipher_init_t callback function.
  * @param crypt  Ciphertext to decrypt.
  * @param plain  Resulting plaintext.
  * @param len  Length of the cipher text.
  *
  */
typedef int (*esp_crypto_cipher_decrypt_t)(esp_crypto_cipher_t *ctx,
				             const unsigned char *crypt, unsigned char *plain, int len);
/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Free cipher context.
  *
  * @param ctx  Context pointer from esp_crypto_cipher_init_t callback function.
  *
  */
typedef void (*esp_crypto_cipher_deinit_t)(esp_crypto_cipher_t *ctx);

/**
  * @brief The SHA256 callback function when do WPS connect. 
  *
  * @param key  Key for HMAC operations.
  * @param key_len  Length of the key in bytes.
  * @param data  Pointers to the data area.
  * @param data_len  Length of the data area.
  * @param mac  Buffer for the hash (20 bytes).
  *
  */
typedef void (*esp_hmac_sha256_t)(const unsigned char *key, int key_len, const unsigned char *data,
		                    int data_len, unsigned char *mac);

/**
  * @brief The SHA256 callback function when do WPS connect. 
  *
  * @param key  Key for HMAC operations.
  * @param key_len  Length of the key in bytes.
  * @param num_elem  Number of elements in the data vector.
  * @param addr  Pointers to the data areas.
  * @param len  Lengths of the data blocks.
  * @param mac  Buffer for the hash (32 bytes).
  *
  */
typedef void (*esp_hmac_sha256_vector_t)(const unsigned char *key, int key_len, int num_elem,
			                   const unsigned char *addr[], const int *len, unsigned char *mac);

/**
  * @brief The AES callback function when do STA connect.
  *
  * @param key  Key for PRF.
  * @param key_len  Length of the key in bytes.
  * @param label  A unique label for each purpose of the PRF.
  * @param data  Extra data to bind into the key.
  * @param data_len  Length of the data.
  * @param buf  Buffer for the generated pseudo-random key.  
  * @param buf_len  Number of bytes of key to generate.
  *
  */
typedef void (*esp_sha256_prf_t)(const unsigned char *key, int key_len, const char *label,
	                           const unsigned char *data, int data_len, unsigned char *buf, int buf_len);

/**
  * @brief The SHA256 callback function when do WPS connect. 
  *
  * @param num_elem  Number of elements in the data vector.
  * @param addr  Pointers to the data areas.
  * @param len  Lengths of the data blocks.
  * @paramac  Buffer for the hash.
  *
  */
typedef int (*esp_sha256_vector_t)(int num_elem, const unsigned char *addr[], const int *len,
		                     unsigned char *mac);

/**
  * @brief The bignum calculate callback function used when do connect. 
  *        In WPS process, it used to calculate public key and private key.
  *
  * @param base  Base integer (big endian byte array).
  * @param base_len  Length of base integer in bytes.
  * @param power  Power integer (big endian byte array).
  * @param power_len  Length of power integer in bytes.
  * @param modulus  Modulus integer (big endian byte array).
  * @param modulus_len  Length of modulus integer in bytes.
  * @param result  Buffer for the result.
  * @param result_len  Result length (max buffer size on input, real len on output).
  *
  */
typedef int (*esp_crypto_mod_exp_t)(const unsigned char *base, int base_len,
		                      const unsigned char *power, int power_len,
		                      const unsigned char *modulus, int modulus_len,
		                      unsigned char *result, unsigned int *result_len);

/**
 * @brief HMAC-MD5 over data buffer (RFC 2104)'
 * 
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (16 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_hmac_md5_t)(const unsigned char *key, unsigned int key_len, const unsigned char *data, 
                              unsigned int data_len, unsigned char *mac);

/**
 * @brief HMAC-MD5 over data vector (RFC 2104)
 * 
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (16 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_hmac_md5_vector_t)(const unsigned char *key, unsigned int key_len, unsigned int num_elem,
                              const unsigned char *addr[], const unsigned int *len, unsigned char *mac);

/**
 * @brief HMAC-SHA1 over data buffer (RFC 2104)
 * 
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 of failure
 */
typedef int (*esp_hmac_sha1_t)(const unsigned char *key, unsigned int key_len, const unsigned char *data, 
                              unsigned int data_len, unsigned char *mac);

/**
 * @brief HMAC-SHA1 over data vector (RFC 2104)
 * 
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_hmac_sha1_vector_t)(const unsigned char *key, unsigned int key_len, unsigned int num_elem,
                              const unsigned char *addr[], const unsigned int *len, unsigned char *mac);

/**
 * @brief SHA1-based Pseudo-Random Function (PRF) (IEEE 802.11i, 8.5.1.1)
 * 
 * @key: Key for PRF
 * @key_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @data: Extra data to bind into the key
 * @data_len: Length of the data
 * @buf: Buffer for the generated pseudo-random key
 * @buf_len: Number of bytes of key to generate
 * Returns: 0 on success, -1 of failure
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key (e.g., PMK in IEEE 802.11i).
 */
typedef int (*esp_sha1_prf_t)(const unsigned char *key, unsigned int key_len, const char *label,
                              const unsigned char *data, unsigned int data_len, unsigned char *buf, unsigned int buf_len);

/**
 * @brief SHA-1 hash for data vector
 * 
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_sha1_vector_t)(unsigned int num_elem, const unsigned char *addr[], const unsigned int *len, 
                              unsigned char *mac);

/**
 * @brief SHA1-based key derivation function (PBKDF2) for IEEE 802.11i
 * 
 * @passphrase: ASCII passphrase
 * @ssid: SSID
 * @ssid_len: SSID length in bytes
 * @iterations: Number of iterations to run
 * @buf: Buffer for the generated key
 * @buflen: Length of the buffer in bytes
 * Returns: 0 on success, -1 of failure
 *
 * This function is used to derive PSK for WPA-PSK. For this protocol,
 * iterations is set to 4096 and buflen to 32. This function is described in
 * IEEE Std 802.11-2004, Clause H.4. The main construction is from PKCS#5 v2.0.
 */
typedef int (*esp_pbkdf2_sha1_t)(const char *passphrase, const char *ssid, unsigned int ssid_len,
                              int iterations, unsigned char *buf, unsigned int buflen);

/**
 * @brief XOR RC4 stream to given data with skip-stream-start
 * 
 * @key: RC4 key
 * @keylen: RC4 key length
 * @skip: number of bytes to skip from the beginning of the RC4 stream
 * @data: data to be XOR'ed with RC4 stream
 * @data_len: buf length
 * Returns: 0 on success, -1 on failure
 *
 * Generate RC4 pseudo random stream for the given key, skip beginning of the
 * stream, and XOR the end result with the data buffer to perform RC4
 * encryption/decryption.
 */
typedef int (*esp_rc4_skip_t)(const unsigned char *key, unsigned int keylen, unsigned int skip,
                              unsigned char *data, unsigned int data_len);

/**
 * @brief MD5 hash for data vector
 * 
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_md5_vector_t)(unsigned int num_elem, const unsigned char *addr[], const unsigned int *len,
                              unsigned char *mac);

/**
 * @brief Encrypt one AES block
 * 
 * @ctx: Context pointer from aes_encrypt_init()
 * @plain: Plaintext data to be encrypted (16 bytes)
 * @crypt: Buffer for the encrypted data (16 bytes)
 */
typedef void (*esp_aes_encrypt_t)(void *ctx, const unsigned char *plain, unsigned char *crypt);

/**
 * @brief Initialize AES for encryption
 * 
 * @key: Encryption key
 * @len: Key length in bytes (usually 16, i.e., 128 bits)
 * Returns: Pointer to context data or %NULL on failure
 */
typedef void * (*esp_aes_encrypt_init_t)(const unsigned char *key,  unsigned int len);

/**
 * @brief Deinitialize AES encryption
 * 
 * @ctx: Context pointer from aes_encrypt_init()
 */
typedef void (*esp_aes_encrypt_deinit_t)(void *ctx);

/**
 * @brief Decrypt one AES block
 * 
 * @ctx: Context pointer from aes_encrypt_init()
 * @crypt: Encrypted data (16 bytes)
 * @plain: Buffer for the decrypted data (16 bytes)
 */
typedef void (*esp_aes_decrypt_t)(void *ctx, const unsigned char *crypt, unsigned char *plain);

/**
 * @brief Initialize AES for decryption
 * 
 * @key: Decryption key
 * @len: Key length in bytes (usually 16, i.e., 128 bits)
 * Returns: Pointer to context data or %NULL on failure
 */
typedef void * (*esp_aes_decrypt_init_t)(const unsigned char *key, unsigned int len);

/**
 * @brief Deinitialize AES decryption
 * 
 * @ctx: Context pointer from aes_encrypt_init()
 */
typedef void (*esp_aes_decrypt_deinit_t)(void *ctx);

/**
 * @brief Initialize TLS library
 * 
 * @conf: Configuration data for TLS library
 * Returns: Context data to be used as tls_ctx in calls to other functions,
 * or %NULL on failure.
 *
 * Called once during program startup and once for each RSN pre-authentication
 * session. In other words, there can be two concurrent TLS contexts. If global
 * library initialization is needed (i.e., one that is shared between both
 * authentication types), the TLS library wrapper should maintain a reference
 * counter and do global initialization only when moving from 0 to 1 reference.
 */
typedef void * (*esp_tls_init_t)(void);

/**
 * @brief Deinitialize TLS library
 * 
 * @tls_ctx: TLS context data from tls_init()
 *
 * Called once during program shutdown and once for each RSN pre-authentication
 * session. If global library deinitialization is needed (i.e., one that is
 * shared between both authentication types), the TLS library wrapper should
 * maintain a reference counter and do global deinitialization only when moving
 * from 1 to 0 references.
 */
typedef void (*esp_tls_deinit_t)(void *tls_ctx);

/**
 * @brief Add certificate and private key for connect
 
 * @sm: eap state machine
 * 
 * Returns: 0 for success, -1 state machine didn't exist, -2 short of certificate or key
 */
typedef int (*esp_eap_peer_blob_init_t)(void *sm);

/**
 * @brief delete the certificate and private
 * 
 * @sm: eap state machine
 * 
 */
typedef void (*esp_eap_peer_blob_deinit_t)(void *sm);

/**
 * @brief Initialize the eap state machine
 * 
 * @sm: eap state machine
 * @private_key_passwd: the start address of private_key_passwd
 * @private_key_passwd_len: length of private_key_password
 * 
 * Returns: 0 is success, -1 state machine didn't exist, -2 short of parameters
 * 
 */
typedef int (*esp_eap_peer_config_init_t)(void *sm, unsigned char *private_key_passwd,int private_key_passwd_len);

/**
 * @brief Deinit the eap state machine
 * 
 * @sm: eap state machine
 * 
 */
typedef void (*esp_eap_peer_config_deinit_t)(void *sm);

/**
 * @brief Register the eap method 
 * 
 * Note: ESP32 only support PEAP/TTLS/TLS three eap methods now.
 * 
 */
typedef int (*esp_eap_peer_register_methods_t)(void);

/**
 * @brief remove the eap method 
 * 
 * Note: ESP32 only support PEAP/TTLS/TLS three eap methods now.
 * 
 */
typedef void (*esp_eap_peer_unregister_methods_t)(void);

/**
 * @brief remove the eap method before build new connect
 * 
 * @sm: eap state machine
 * @txt: not used now
 */
typedef void (*esp_eap_deinit_prev_method_t)(void *sm, const char *txt);

/**
 * @brief Get EAP method based on type number
 * 
 * @vendor: EAP Vendor-Id (0 = IETF)
 * @method: EAP type number
 * Returns: Pointer to EAP method or %NULL if not found
 */
typedef const void * (*esp_eap_peer_get_eap_method_t)(int vendor, int method);

/**
 * @brief Abort EAP authentication
 * 
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 *
 * Release system resources that have been allocated for the authentication
 * session without fully deinitializing the EAP state machine.
 */
typedef void (*esp_eap_sm_abort_t)(void *sm);

/**
 * @brief Build EAP-NAK for the current network
 * 
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @type: EAP type of the fail reason
 * @id: EAP identifier for the packet
 *
 * This function allocates and builds a nak packet for the
 * current network. The caller is responsible for freeing the returned data.
 */
typedef void * (*esp_eap_sm_build_nak_t)(void *sm, int type, unsigned char id);

/**
 * @brief Build EAP-Identity/Response for the current network
 * 
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @id: EAP identifier for the packet
 * @encrypted: Whether the packet is for encrypted tunnel (EAP phase 2)
 * Returns: Pointer to the allocated EAP-Identity/Response packet or %NULL on
 * failure
 *
 * This function allocates and builds an EAP-Identity/Response packet for the
 * current network. The caller is responsible for freeing the returned data.
 */
typedef void * (*esp_eap_sm_build_identity_resp_t)(void *sm, unsigned char id, int encrypted);

/**
 * @brief Allocate a buffer for an EAP message
 * 
 * @vendor: Vendor-Id (0 = IETF)
 * @type: EAP type
 * @payload_len: Payload length in bytes (data after Type)
 * @code: Message Code (EAP_CODE_*)
 * @identifier: Identifier
 * Returns: Pointer to the allocated message buffer or %NULL on error
 *
 * This function can be used to allocate a buffer for an EAP message and fill
 * in the EAP header. This function is automatically using expanded EAP header
 * if the selected Vendor-Id is not IETF. In other words, most EAP methods do
 * not need to separately select which header type to use when using this
 * function to allocate the message buffers. The returned buffer has room for
 * payload_len bytes and has the EAP header and Type field already filled in.
 */
typedef void * (*esp_eap_msg_alloc_t)(int vendor, int type, unsigned int payload_len,
                             unsigned char code, unsigned char identifier);

/**
 * @brief get the enrollee mac address
 * @mac_addr: instore the mac address of enrollee
 * @uuid: Universally Unique Identifer of the enrollee
 * 
 */                        
typedef void (*esp_uuid_gen_mac_addr_t)(const unsigned char *mac_addr, unsigned char *uuid);

/**
 * @brief free the message after finish DH
 * 
 */
typedef void (*esp_dh5_free_t)(void *ctx);

/**
 * @brief Build WPS IE for (Re)Association Request
 * 
 * @req_type: Value for Request Type attribute
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
typedef void * (*esp_wps_build_assoc_req_ie_t)(int req_type);

/**
 * @brief Build WPS IE for (Re)Association Response
 * 
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
typedef void * (*esp_wps_build_assoc_resp_ie_t)(void);

/**
 * @brief Build WPS IE for Probe Request
 * 
 * @pw_id: Password ID (DEV_PW_PUSHBUTTON for active PBC and DEV_PW_DEFAULT for
 * most other use cases)
 * @dev: Device attributes
 * @uuid: Own UUID
 * @req_type: Value for Request Type attribute
 * @num_req_dev_types: Number of requested device types
 * @req_dev_types: Requested device types (8 * num_req_dev_types octets) or
 *	%NULL if none
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
typedef void * (*esp_wps_build_probe_req_ie_t)(uint16_t pw_id, void *dev, const unsigned char *uuid,
                             int req_type, unsigned int num_req_dev_types, const unsigned char *req_dev_types);

/**
 * @brief build public key for exchange in M1
 * 
 * 
 */
typedef int (*esp_wps_build_public_key_t)(void *wps, void *msg, int mode);


/**
 * @brief get the wps information in exchange password
 * 
 * 
 */
typedef void * (*esp_wps_enrollee_get_msg_t)(void *wps, void *op_code);

/**
 * @brief deal with the wps information in exchange password
 * 
 * 
 */
typedef int (*esp_wps_enrollee_process_msg_t)(void *wps, int op_code, const void *msg);

/**
 * @brief Generate a random PIN
 * 
 * Returns: Eight digit PIN (i.e., including the checksum digit)
 */
typedef unsigned int (*esp_wps_generate_pin_t)(void);

/**
 * @brief Check whether WPS IE indicates active PIN
 * 
 * @msg: WPS IE contents from Beacon or Probe Response frame
 * Returns: 1 if PIN Registrar is active, 0 if not
 */
typedef int (*esp_wps_is_selected_pin_registrar_t)(const void *msg, unsigned char *bssid);

/**
 * @brief Check whether WPS IE indicates active PBC
 * 
 * @msg: WPS IE contents from Beacon or Probe Response frame
 * Returns: 1 if PBC Registrar is active, 0 if not
 */
typedef int (*esp_wps_is_selected_pbc_registrar_t)(const void *msg, unsigned char *bssid);



/**
  * @brief The crypto callback function structure used when do station security connect.
  *        The structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct {
    uint32_t size;
    uint32_t version;
    esp_aes_wrap_t aes_wrap;                         /**< station connect function used when send EAPOL frame */
    esp_aes_unwrap_t aes_unwrap;                     /**< station connect function used when decrypt key data */
    esp_hmac_sha256_vector_t hmac_sha256_vector;     /**< station connect function used when check MIC */
    esp_sha256_prf_t sha256_prf;                     /**< station connect function used when check MIC */
    esp_hmac_md5_t hmac_md5;
    esp_hmac_md5_vector_t hamc_md5_vector;
    esp_hmac_sha1_t hmac_sha1;
    esp_hmac_sha1_vector_t hmac_sha1_vector;
    esp_sha1_prf_t sha1_prf;
    esp_sha1_vector_t sha1_vector;
    esp_pbkdf2_sha1_t pbkdf2_sha1;
    esp_rc4_skip_t rc4_skip;
    esp_md5_vector_t md5_vector;
    esp_aes_encrypt_t aes_encrypt;
    esp_aes_encrypt_init_t aes_encrypt_init;
    esp_aes_encrypt_deinit_t aes_encrypt_deinit;
    esp_aes_decrypt_t aes_decrypt;
    esp_aes_decrypt_init_t aes_decrypt_init;
    esp_aes_decrypt_deinit_t aes_decrypt_deinit;
}wpa_crypto_funcs_t;

/**
  * @brief The crypto callback function structure used when do WPS process. The 
  *        structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct{
    uint32_t size;
    uint32_t version;
    esp_aes_128_encrypt_t aes_128_encrypt;          /**< function used to process message when do WPS */
    esp_aes_128_decrypt_t aes_128_decrypt;          /**< function used to process message when do WPS */
    esp_crypto_mod_exp_t crypto_mod_exp;            /**< function used to calculate public key and private key */
    esp_hmac_sha256_t hmac_sha256;                  /**< function used to get attribute */
    esp_hmac_sha256_vector_t hmac_sha256_vector;    /**< function used to process message when do WPS */
    esp_sha256_vector_t sha256_vector;              /**< function used to process message when do WPS */
    esp_uuid_gen_mac_addr_t uuid_gen_mac_addr;
    esp_dh5_free_t dh5_free;
    esp_wps_build_assoc_req_ie_t wps_build_assoc_req_ie;
    esp_wps_build_assoc_resp_ie_t wps_build_assoc_resp_ie;
    esp_wps_build_probe_req_ie_t wps_build_probe_req_ie;
    esp_wps_build_public_key_t wps_build_public_key;
    esp_wps_enrollee_get_msg_t wps_enrollee_get_msg;
    esp_wps_enrollee_process_msg_t wps_enrollee_process_msg;
    esp_wps_generate_pin_t wps_generate_pin;
    esp_wps_is_selected_pin_registrar_t wps_is_selected_pin_registrar;
    esp_wps_is_selected_pbc_registrar_t wps_is_selected_pbc_registrar;
    esp_eap_msg_alloc_t eap_msg_alloc;
}wps_crypto_funcs_t;

/**
  * @brief The crypto callback function structure used when do WPA enterprise connect.
  *        The structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct {
    uint32_t size;
    uint32_t version;
    esp_crypto_hash_init_t crypto_hash_init;                  /**< function used to initialize a crypto_hash structure when use TLSV1 */
    esp_crypto_hash_update_t crypto_hash_update;              /**< function used to calculate hash data when use TLSV1 */
    esp_crypto_hash_finish_t crypto_hash_finish;              /**< function used to finish the hash calculate when use TLSV1 */
    esp_crypto_cipher_init_t crypto_cipher_init;              /**< function used to initialize a crypt_cipher structure when use TLSV1 */
    esp_crypto_cipher_encrypt_t crypto_cipher_encrypt;        /**< function used to encrypt cipher when use TLSV1 */
    esp_crypto_cipher_decrypt_t crypto_cipher_decrypt;        /**< function used to decrypt cipher when use TLSV1 */
    esp_crypto_cipher_deinit_t crypto_cipher_deinit;          /**< function used to free context when use TLSV1 */
    esp_crypto_mod_exp_t crypto_mod_exp;                      /**< function used to do key exchange when use TLSV1 */
    esp_sha256_vector_t sha256_vector;                        /**< function used to do X.509v3 certificate parsing and processing */
    esp_tls_init_t tls_init;
    esp_tls_deinit_t tls_deinit;
    esp_eap_peer_blob_init_t eap_peer_blob_init;
    esp_eap_peer_blob_deinit_t eap_peer_blob_deinit;
    esp_eap_peer_config_init_t eap_peer_config_init;
    esp_eap_peer_config_deinit_t eap_peer_config_deinit;
    esp_eap_peer_register_methods_t eap_peer_register_methods;
    esp_eap_peer_unregister_methods_t eap_peer_unregister_methods;
    esp_eap_deinit_prev_method_t eap_deinit_prev_method;
    esp_eap_peer_get_eap_method_t eap_peer_get_eap_method;
    esp_eap_sm_abort_t eap_sm_abort;
    esp_eap_sm_build_nak_t eap_sm_build_nak;
    esp_eap_sm_build_identity_resp_t eap_sm_build_identity_resp;
    esp_eap_msg_alloc_t eap_msg_alloc;
} wpa2_crypto_funcs_t;

/**
  * @brief The crypto callback function structure used in mesh vendor IE encryption. The
  *        structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct{
    esp_aes_128_encrypt_t aes_128_encrypt;          /**< function used in mesh vendor IE encryption */
    esp_aes_128_decrypt_t aes_128_decrypt;          /**< function used in mesh vendor IE decryption */
} mesh_crypto_funcs_t;

#ifdef __cplusplus
}
#endif
#endif
