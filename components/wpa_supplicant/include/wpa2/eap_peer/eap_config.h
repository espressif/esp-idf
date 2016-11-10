/*
 * EAP peer configuration data
 * Copyright (c) 2003-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_CONFIG_H
#define EAP_CONFIG_H

/**
 * struct eap_peer_config - EAP peer configuration/credentials
 */
struct eap_peer_config {
	/**
	 * identity - EAP Identity
	 *
	 * This field is used to set the real user identity or NAI (for
	 * EAP-PSK/PAX/SAKE/GPSK).
	 */
	u8 *identity;

	/**
	 * identity_len - EAP Identity length
	 */
	size_t identity_len;

	/**
	 * password - Password string for EAP
	 *
	 * This field can include either the plaintext password (default
	 * option) or a NtPasswordHash (16-byte MD4 hash of the unicode
	 * presentation of the password) if flags field has
	 * EAP_CONFIG_FLAGS_PASSWORD_NTHASH bit set to 1. NtPasswordHash can
	 * only be used with authentication mechanism that use this hash as the
	 * starting point for operation: MSCHAP and MSCHAPv2 (EAP-MSCHAPv2,
	 * EAP-TTLS/MSCHAPv2, EAP-TTLS/MSCHAP, LEAP).
	 *
	 * In addition, this field is used to configure a pre-shared key for
	 * EAP-PSK/PAX/SAKE/GPSK. The length of the PSK must be 16 for EAP-PSK
	 * and EAP-PAX and 32 for EAP-SAKE. EAP-GPSK can use a variable length
	 * PSK.
	 */
	u8 *password;

	/**
	 * password_len - Length of password field
	 */
	size_t password_len;

	/**
	 * ca_cert - File path to CA certificate file (PEM/DER)
	 *
	 * This file can have one or more trusted CA certificates. If ca_cert
	 * and ca_path are not included, server certificate will not be
	 * verified. This is insecure and a trusted CA certificate should
	 * always be configured when using EAP-TLS/TTLS/PEAP. Full path to the
	 * file should be used since working directory may change when
	 * wpa_supplicant is run in the background.
	 *
	 * Alternatively, a named configuration blob can be used by setting
	 * this to blob://blob_name.
	 *
	 * Alternatively, this can be used to only perform matching of the
	 * server certificate (SHA-256 hash of the DER encoded X.509
	 * certificate). In this case, the possible CA certificates in the
	 * server certificate chain are ignored and only the server certificate
	 * is verified. This is configured with the following format:
	 * hash:://server/sha256/cert_hash_in_hex
	 * For example: "hash://server/sha256/
	 * 5a1bc1296205e6fdbe3979728efe3920798885c1c4590b5f90f43222d239ca6a"
	 *
	 * On Windows, trusted CA certificates can be loaded from the system
	 * certificate store by setting this to cert_store://name, e.g.,
	 * ca_cert="cert_store://CA" or ca_cert="cert_store://ROOT".
	 * Note that when running wpa_supplicant as an application, the user
	 * certificate store (My user account) is used, whereas computer store
	 * (Computer account) is used when running wpasvc as a service.
	 */
	u8 *ca_cert;

	/**
	 * ca_path - Directory path for CA certificate files (PEM)
	 *
	 * This path may contain multiple CA certificates in OpenSSL format.
	 * Common use for this is to point to system trusted CA list which is
	 * often installed into directory like /etc/ssl/certs. If configured,
	 * these certificates are added to the list of trusted CAs. ca_cert
	 * may also be included in that case, but it is not required.
	 */
	u8 *ca_path;

	/**
	 * client_cert - File path to client certificate file (PEM/DER)
	 *
	 * This field is used with EAP method that use TLS authentication.
	 * Usually, this is only configured for EAP-TLS, even though this could
	 * in theory be used with EAP-TTLS and EAP-PEAP, too. Full path to the
	 * file should be used since working directory may change when
	 * wpa_supplicant is run in the background.
	 *
	 * Alternatively, a named configuration blob can be used by setting
	 * this to blob://blob_name.
	 */
	u8 *client_cert;

	/**
	 * private_key - File path to client private key file (PEM/DER/PFX)
	 *
	 * When PKCS#12/PFX file (.p12/.pfx) is used, client_cert should be
	 * commented out. Both the private key and certificate will be read
	 * from the PKCS#12 file in this case. Full path to the file should be
	 * used since working directory may change when wpa_supplicant is run
	 * in the background.
	 *
	 * Windows certificate store can be used by leaving client_cert out and
	 * configuring private_key in one of the following formats:
	 *
	 * cert://substring_to_match
	 *
	 * hash://certificate_thumbprint_in_hex
	 *
	 * For example: private_key="hash://63093aa9c47f56ae88334c7b65a4"
	 *
	 * Note that when running wpa_supplicant as an application, the user
	 * certificate store (My user account) is used, whereas computer store
	 * (Computer account) is used when running wpasvc as a service.
	 *
	 * Alternatively, a named configuration blob can be used by setting
	 * this to blob://blob_name.
	 */
	u8 *private_key;

	/**
	 * private_key_passwd - Password for private key file
	 *
	 * If left out, this will be asked through control interface.
	 */
	u8 *private_key_passwd;

	char *phase1;

	/**
	 * pin - PIN for USIM, GSM SIM, and smartcards
	 *
	 * This field is used to configure PIN for SIM and smartcards for
	 * EAP-SIM and EAP-AKA. In addition, this is used with EAP-TLS if a
	 * smartcard is used for private key operations.
	 *
	 * If left out, this will be asked through control interface.
	 */
	char *pin;

	/**
	 * fragment_size - Maximum EAP fragment size in bytes (default 1398)
	 *
	 * This value limits the fragment size for EAP methods that support
	 * fragmentation (e.g., EAP-TLS and EAP-PEAP). This value should be set
	 * small enough to make the EAP messages fit in MTU of the network
	 * interface used for EAPOL. The default value is suitable for most
	 * cases.
	 */
	int fragment_size;

#define EAP_CONFIG_FLAGS_PASSWORD_NTHASH BIT(0)
#define EAP_CONFIG_FLAGS_EXT_PASSWORD BIT(1)
	/**
	 * flags - Network configuration flags (bitfield)
	 *
	 * This variable is used for internal flags to describe further details
	 * for the network parameters.
	 * bit 0 = password is represented as a 16-byte NtPasswordHash value
	 *         instead of plaintext password
	 * bit 1 = password is stored in external storage; the value in the
	 *         password field is the name of that external entry
	 */
	u32 flags;

	/**
	 * ocsp - Whether to use/require OCSP to check server certificate
	 *
	 * 0 = do not use OCSP stapling (TLS certificate status extension)
	 * 1 = try to use OCSP stapling, but not require response
	 * 2 = require valid OCSP stapling response
	 */
	int ocsp;
};


/**
 * struct wpa_config_blob - Named configuration blob
 *
 * This data structure is used to provide storage for binary objects to store
 * abstract information like certificates and private keys inlined with the
 * configuration data.
 */
struct wpa_config_blob {
	/**
	 * name - Blob name
	 */
	char *name;

	/**
	 * data - Pointer to binary data
	 */
	u8 *data;

	/**
	 * len - Length of binary data
	 */
	size_t len;

	/**
	 * next - Pointer to next blob in the configuration
	 */
	struct wpa_config_blob *next;
};

#endif /* EAP_CONFIG_H */
