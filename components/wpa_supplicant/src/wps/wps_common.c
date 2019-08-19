/*
 * Wi-Fi Protected Setup - common functionality
 * Copyright (c) 2008-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include <string.h>

#include "utils/includes.h"
#include "utils/common.h"

#include "crypto/aes_wrap.h"
#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/sha256.h"
#include "crypto/dh_group5.h"
#include "crypto/random.h"

#include "wps/wps_i.h"

void wps_kdf(const u8 *key, const u8 *label_prefix, size_t label_prefix_len,
	     const char *label, u8 *res, size_t res_len)
{
	u8 i_buf[4], key_bits[4];
	const u8 *addr[4];
	size_t len[4];
	int i, iter;
	u8 hash[SHA256_MAC_LEN], *opos;
	size_t left;

	WPA_PUT_BE32(key_bits, res_len * 8);

	addr[0] = i_buf;
	len[0] = sizeof(i_buf);
	addr[1] = label_prefix;
	len[1] = label_prefix_len;
	addr[2] = (const u8 *) label;
	len[2] = os_strlen(label);
	addr[3] = key_bits;
	len[3] = sizeof(key_bits);

	iter = (res_len + SHA256_MAC_LEN - 1) / SHA256_MAC_LEN;
	opos = res;
	left = res_len;

	for (i = 1; i <= iter; i++) {
		WPA_PUT_BE32(i_buf, i);
		hmac_sha256_vector(key, SHA256_MAC_LEN, 4, addr, len, hash);
		if (i < iter) {
			os_memcpy(opos, hash, SHA256_MAC_LEN);
			opos += SHA256_MAC_LEN;
			left -= SHA256_MAC_LEN;
		} else
			os_memcpy(opos, hash, left);
	}
}


int wps_derive_keys(struct wps_data *wps)
{
	struct wpabuf *pubkey, *dh_shared;
	u8 dhkey[SHA256_MAC_LEN], kdk[SHA256_MAC_LEN];
	const u8 *addr[3];
	size_t len[3];
	u8 keys[WPS_AUTHKEY_LEN + WPS_KEYWRAPKEY_LEN + WPS_EMSK_LEN];

	if (wps->dh_privkey == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: Own DH private key not available");
		return -1;
	}

	pubkey = wps->registrar ? wps->dh_pubkey_e : wps->dh_pubkey_r;
	if (pubkey == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: Peer DH public key not available");
		return -1;
	}

	wpa_hexdump_buf_key(MSG_DEBUG, "WPS: DH Private Key", wps->dh_privkey);
	wpa_hexdump_buf(MSG_DEBUG, "WPS: DH peer Public Key", pubkey);
	dh_shared = dh5_derive_shared(wps->dh_ctx, pubkey, wps->dh_privkey);
	dh5_free(wps->dh_ctx);
	wps->dh_ctx = NULL;
	dh_shared = wpabuf_zeropad(dh_shared, 192);
	if (dh_shared == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: Failed to derive DH shared key");
		return -1;
	}

	/* Own DH private key is not needed anymore */
/*
 * due to the public key calculated when wps start, it will not calculate anymore even when we build M1 message, also calculate the key need take a long time
 * which would cause WPS fail, so we clean the key after WPS finished .
 */
#ifndef ESP32_WORKAROUND
	wpabuf_free(wps->dh_privkey);
	wps->dh_privkey = NULL;
#endif //ESP32_WORKAROUND

	wpa_hexdump_buf_key(MSG_DEBUG, "WPS: DH shared key", dh_shared);

	/* DHKey = SHA-256(g^AB mod p) */
	addr[0] = wpabuf_head(dh_shared);
	len[0] = wpabuf_len(dh_shared);

	sha256_vector(1, addr, len, dhkey);
	wpa_hexdump_key(MSG_DEBUG, "WPS: DHKey", dhkey, sizeof(dhkey));
	wpabuf_free(dh_shared);

	/* KDK = HMAC-SHA-256_DHKey(N1 || EnrolleeMAC || N2) */
	addr[0] = wps->nonce_e;
	len[0] = WPS_NONCE_LEN;
	addr[1] = wps->mac_addr_e;
	len[1] = ETH_ALEN;
	addr[2] = wps->nonce_r;
	len[2] = WPS_NONCE_LEN;
	hmac_sha256_vector(dhkey, sizeof(dhkey), 3, addr, len, kdk);
	wpa_hexdump_key(MSG_DEBUG, "WPS: KDK", kdk, sizeof(kdk));

	wps_kdf(kdk, NULL, 0, "Wi-Fi Easy and Secure Key Derivation",
		keys, sizeof(keys));
	os_memcpy(wps->authkey, keys, WPS_AUTHKEY_LEN);
	os_memcpy(wps->keywrapkey, keys + WPS_AUTHKEY_LEN, WPS_KEYWRAPKEY_LEN);
	os_memcpy(wps->emsk, keys + WPS_AUTHKEY_LEN + WPS_KEYWRAPKEY_LEN,
		  WPS_EMSK_LEN);

	wpa_hexdump_key(MSG_DEBUG, "WPS: AuthKey",
			wps->authkey, WPS_AUTHKEY_LEN);
	wpa_hexdump_key(MSG_DEBUG, "WPS: KeyWrapKey",
			wps->keywrapkey, WPS_KEYWRAPKEY_LEN);
	wpa_hexdump_key(MSG_DEBUG, "WPS: EMSK", wps->emsk, WPS_EMSK_LEN);

	return 0;
}


void wps_derive_psk(struct wps_data *wps, const u8 *dev_passwd,
		    size_t dev_passwd_len)
{
	u8 hash[SHA256_MAC_LEN];

	hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN, dev_passwd,
		                             (dev_passwd_len + 1) / 2, hash);
	os_memcpy(wps->psk1, hash, WPS_PSK_LEN);
	hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN,
		            dev_passwd + (dev_passwd_len + 1) / 2,
		            dev_passwd_len / 2, hash);
	os_memcpy(wps->psk2, hash, WPS_PSK_LEN);

	wpa_hexdump_ascii_key(MSG_DEBUG, "WPS: Device Password",
			      dev_passwd, dev_passwd_len);
	wpa_hexdump_key(MSG_DEBUG, "WPS: PSK1", wps->psk1, WPS_PSK_LEN);
	wpa_hexdump_key(MSG_DEBUG, "WPS: PSK2", wps->psk2, WPS_PSK_LEN);
}


struct wpabuf * wps_decrypt_encr_settings(struct wps_data *wps, const u8 *encr,
					  size_t encr_len)
{
	struct wpabuf *decrypted;
	const size_t block_size = 16;
	size_t i;
	u8 pad;
	const u8 *pos;

	/* AES-128-CBC */
	if (encr == NULL || encr_len < 2 * block_size || encr_len % block_size)
	{
		wpa_printf(MSG_DEBUG,  "WPS: No Encrypted Settings received");
		return NULL;
	}

	decrypted = wpabuf_alloc(encr_len - block_size);
	if (decrypted == NULL)
		return NULL;

	wpa_hexdump(MSG_MSGDUMP, "WPS: Encrypted Settings", encr, encr_len);
	wpabuf_put_data(decrypted, encr + block_size, encr_len - block_size);
	wpa_printf(MSG_DEBUG,  "WPS: AES Decrypt setting");
	if (aes_128_cbc_decrypt(wps->keywrapkey, encr, wpabuf_mhead(decrypted),
				        wpabuf_len(decrypted))) {
		wpabuf_free(decrypted);
		return NULL;
	}

	wpa_hexdump_buf_key(MSG_MSGDUMP, "WPS: Decrypted Encrypted Settings",
			    decrypted);

	pos = wpabuf_head_u8(decrypted) + wpabuf_len(decrypted) - 1;
	pad = *pos;
	if (pad > wpabuf_len(decrypted)) {
		wpa_printf(MSG_DEBUG,  "WPS: Invalid PKCS#5 v2.0 pad value");
		wpabuf_free(decrypted);
		return NULL;
	}
	for (i = 0; i < pad; i++) {
		if (*pos-- != pad) {
			wpa_printf(MSG_DEBUG,  "WPS: Invalid PKCS#5 v2.0 pad "
				   "string");
			wpabuf_free(decrypted);
			return NULL;
		}
	}
	decrypted->used -= pad;

	return decrypted;
}

#ifdef CONFIG_WPS_PIN
/**
 * wps_pin_checksum - Compute PIN checksum
 * @pin: Seven digit PIN (i.e., eight digit PIN without the checksum digit)
 * Returns: Checksum digit
 */
unsigned int wps_pin_checksum(unsigned int pin)
{
	unsigned int accum = 0;
	while (pin) {
		accum += 3 * (pin % 10);
		pin /= 10;
		accum += pin % 10;
		pin /= 10;
	}

	return (10 - accum % 10) % 10;
}


/**
 * wps_pin_valid - Check whether a PIN has a valid checksum
 * @pin: Eight digit PIN (i.e., including the checksum digit)
 * Returns: 1 if checksum digit is valid, or 0 if not
 */
unsigned int wps_pin_valid(unsigned int pin)
{
	return wps_pin_checksum(pin / 10) == (pin % 10);
}


/**
 * wps_generate_pin - Generate a random PIN
 * Returns: Eight digit PIN (i.e., including the checksum digit)
 */
unsigned int wps_generate_pin(void)
{
	unsigned int val;

	/* Generate seven random digits for the PIN */
	if (random_get_bytes((unsigned char *) &val, sizeof(val)) < 0) {
		return -1;
	}
	val %= 10000000;

	/* Append checksum digit */
	return val * 10 + wps_pin_checksum(val);
}


int wps_pin_str_valid(const char *pin)
{
	const char *p;
	size_t len;

	p = pin;
	while (*p >= '0' && *p <= '9')
		p++;
	if (*p != '\0')
		return 0;

	len = p - pin;
	return len == 4 || len == 8;
}
#endif

void wps_fail_event(struct wps_context *wps, enum wps_msg_type msg,
		    u16 config_error, u16 error_indication)
{
	union wps_event_data *data;

        data = (union wps_event_data *)os_zalloc(sizeof(union wps_event_data));
	if (data == NULL)
		return;

	if (wps->event_cb == NULL) {
		os_free(data);
		return;
	}

	os_memset(data, 0, sizeof(union wps_event_data));
	data->fail.msg = msg;
	data->fail.config_error = config_error;
	data->fail.error_indication = error_indication;
	wps->event_cb(wps->cb_ctx, WPS_EV_FAIL, data);
	os_free(data);
}


void wps_success_event(struct wps_context *wps)
{
	if (wps->event_cb == NULL)
		return;

	wps->event_cb(wps->cb_ctx, WPS_EV_SUCCESS, NULL);
}


void wps_pwd_auth_fail_event(struct wps_context *wps, int enrollee, int part)
{
	union wps_event_data *data;

        data = (union wps_event_data *)os_zalloc(sizeof(union wps_event_data));
	if (data == NULL)
		return;

	if (wps->event_cb == NULL) {
		os_free(data);
		return;
	}

	os_memset(data, 0, sizeof(union wps_event_data));
	data->pwd_auth_fail.enrollee = enrollee;
	data->pwd_auth_fail.part = part;
	wps->event_cb(wps->cb_ctx, WPS_EV_PWD_AUTH_FAIL, data);
	os_free(data);
}


void wps_pbc_overlap_event(struct wps_context *wps)
{
	if (wps->event_cb == NULL)
		return;

	wps->event_cb(wps->cb_ctx, WPS_EV_PBC_OVERLAP, NULL);
}


void wps_pbc_timeout_event(struct wps_context *wps)
{
	if (wps->event_cb == NULL)
		return;

	wps->event_cb(wps->cb_ctx, WPS_EV_PBC_TIMEOUT, NULL);
}


#ifdef CONFIG_WPS_OOB

struct wpabuf * wps_get_oob_cred(struct wps_context *wps)
{
	struct wps_data *data;
	struct wpabuf *plain;

	data = (struct wps_data *)os_zalloc(sizeof(struct wps_data));
	if (data == NULL)
		return NULL;

	plain = wpabuf_alloc(500);
	if (plain == NULL) {
		os_free(data);
		wpa_printf(MSG_ERROR,  "WPS: Failed to allocate memory for OOB "
			   "credential");
		return NULL;
	}

	os_memset(data, 0, sizeof(struct wps_data));
	data->wps = wps;
	data->auth_type = wps->auth_types;
	data->encr_type = wps->encr_types;
	if (wps_build_version(plain) ||
	    wps_build_cred(data, plain) ||
	    wps_build_wfa_ext(plain, 0, NULL, 0)) {
		wpabuf_free(plain);
		os_free(data);
		return NULL;
	}

	os_free(data);
	return plain;
}

#ifdef CONFIG_WPS_NFC

struct wpabuf * wps_build_nfc_pw_token(u16 dev_pw_id,
				       const struct wpabuf *pubkey,
				       const struct wpabuf *dev_pw)
{
	struct wpabuf *data;

	data = wpabuf_alloc(200);
	if (data == NULL)
		return NULL;

	if (wps_build_version(data) ||
	    wps_build_oob_dev_pw(data, dev_pw_id, pubkey,
				 wpabuf_head(dev_pw), wpabuf_len(dev_pw)) ||
	    wps_build_wfa_ext(data, 0, NULL, 0)) {
		wpa_printf(MSG_ERROR,  "WPS: Failed to build NFC password "
			   "token");
		wpabuf_free(data);
		return NULL;
	}

	return data;
}

#endif

int wps_oob_use_cred(struct wps_context *wps, struct wps_parse_attr *attr)
{
	struct wpabuf msg;
	size_t i;

	for (i = 0; i < attr->num_cred; i++) {
		struct wps_credential local_cred;
		struct wps_parse_attr cattr;

		os_memset(&local_cred, 0, sizeof(local_cred));
		wpabuf_set(&msg, attr->cred[i], attr->cred_len[i]);
		if (wps_parse_msg(&msg, &cattr) < 0 ||
		    wps_process_cred(&cattr, &local_cred)) {
			wpa_printf(MSG_ERROR,  "WPS: Failed to parse OOB "
				   "credential");
			return -1;
		}
		wps->cred_cb(wps->cb_ctx, &local_cred);
	}

	return 0;
}


#endif /* CONFIG_WPS_OOB */


int wps_dev_type_str2bin(const char *str, u8 dev_type[WPS_DEV_TYPE_LEN])
{
	const char *pos;

	/* <categ>-<OUI>-<subcateg> */
	WPA_PUT_BE16(dev_type, atoi(str));
	pos = (char *)os_strchr(str, '-');
	if (pos == NULL)
		return -1;
	pos++;
	if (hexstr2bin(pos, &dev_type[2], 4))
		return -1;
	pos = (char *)os_strchr(pos, '-');
	if (pos == NULL)
		return -1;
	pos++;
	WPA_PUT_BE16(&dev_type[6], atoi(pos));


	return 0;
}


char * wps_dev_type_bin2str(const u8 dev_type[WPS_DEV_TYPE_LEN], char *buf,
			    size_t buf_len)
{
	int ret;

	ret = snprintf(buf, buf_len, "%u-%08X-%u",
			  WPA_GET_BE16(dev_type), WPA_GET_BE32(&dev_type[2]),
			  WPA_GET_BE16(&dev_type[6]));
	if (ret < 0 || (unsigned int) ret >= buf_len)
		return NULL;

	return buf;
}


void uuid_gen_mac_addr(const u8 *mac_addr, u8 *uuid)
{
	const u8 *addr[2];
	size_t len[2];
	u8 hash[SHA1_MAC_LEN];
	u8 nsid[16] = {
		0x52, 0x64, 0x80, 0xf8,
		0xc9, 0x9b,
		0x4b, 0xe5,
		0xa6, 0x55,
		0x58, 0xed, 0x5f, 0x5d, 0x60, 0x84
	};

	addr[0] = nsid;
	len[0] = sizeof(nsid);
	addr[1] = mac_addr;
	len[1] = 6;
	sha1_vector(2, addr, len, hash);
	os_memcpy(uuid, hash, 16);

	/* Version: 5 = named-based version using SHA-1 */
	uuid[6] = (5 << 4) | (uuid[6] & 0x0f);

	/* Variant specified in RFC 4122 */
	uuid[8] = 0x80 | (uuid[8] & 0x3f);
}


u16 wps_config_methods_str2bin(const char *str)
{
	u16 methods = 0;

	if (str == NULL) {
		/* Default to enabling methods based on build configuration */
		methods |= WPS_CONFIG_DISPLAY | WPS_CONFIG_KEYPAD;
#ifdef CONFIG_WPS2
		methods |= WPS_CONFIG_VIRT_DISPLAY;
#endif /* CONFIG_WPS2 */
#ifdef CONFIG_WPS_NFC
		methods |= WPS_CONFIG_NFC_INTERFACE;
#endif /* CONFIG_WPS_NFC */
	} else {
		if (os_strstr(str, "ethernet"))
			methods |= WPS_CONFIG_ETHERNET;
		if (os_strstr(str, "label"))
			methods |= WPS_CONFIG_LABEL;
		if (os_strstr(str, "display"))
			methods |= WPS_CONFIG_DISPLAY;
		if (os_strstr(str, "ext_nfc_token"))
			methods |= WPS_CONFIG_EXT_NFC_TOKEN;
		if (os_strstr(str, "int_nfc_token"))
			methods |= WPS_CONFIG_INT_NFC_TOKEN;
		if (os_strstr(str, "nfc_interface"))
			methods |= WPS_CONFIG_NFC_INTERFACE;
		if (os_strstr(str, "push_button"))
			methods |= WPS_CONFIG_PUSHBUTTON;
		if (os_strstr(str, "keypad"))
			methods |= WPS_CONFIG_KEYPAD;
#ifdef CONFIG_WPS2
		if (os_strstr(str, "virtual_display"))
			methods |= WPS_CONFIG_VIRT_DISPLAY;
		if (os_strstr(str, "physical_display"))
			methods |= WPS_CONFIG_PHY_DISPLAY;
		if (os_strstr(str, "virtual_push_button"))
			methods |= WPS_CONFIG_VIRT_PUSHBUTTON;
		if (os_strstr(str, "physical_push_button"))
			methods |= WPS_CONFIG_PHY_PUSHBUTTON;
#endif /* CONFIG_WPS2 */
	}

	return methods;
}


struct wpabuf * wps_build_wsc_ack(struct wps_data *wps)
{
	struct wpabuf *msg;

	wpa_printf(MSG_DEBUG,  "WPS: Building Message WSC_ACK");

	msg = wpabuf_alloc(1000);
	if (msg == NULL)
		return NULL;

	if (wps_build_version(msg) ||
	    wps_build_msg_type(msg, WPS_WSC_ACK) ||
	    wps_build_enrollee_nonce(wps, msg) ||
	    wps_build_registrar_nonce(wps, msg) ||
	    wps_build_wfa_ext(msg, 0, NULL, 0)) {
		wpabuf_free(msg);
		return NULL;
	}

	return msg;
}


struct wpabuf * wps_build_wsc_nack(struct wps_data *wps)
{
	struct wpabuf *msg;

	wpa_printf(MSG_DEBUG,  "WPS: Building Message WSC_NACK");

	msg = wpabuf_alloc(1000);
	if (msg == NULL)
		return NULL;

	if (wps_build_version(msg) ||
	    wps_build_msg_type(msg, WPS_WSC_NACK) ||
	    wps_build_enrollee_nonce(wps, msg) ||
	    wps_build_registrar_nonce(wps, msg) ||
	    wps_build_config_error(msg, wps->config_error) ||
	    wps_build_wfa_ext(msg, 0, NULL, 0)) {
		wpabuf_free(msg);
		return NULL;
	}

	return msg;
}


#ifdef CONFIG_WPS_NFC
struct wpabuf * wps_nfc_token_gen(int ndef, int *id, struct wpabuf **pubkey,
				  struct wpabuf **privkey,
				  struct wpabuf **dev_pw)
{
	struct wpabuf *priv = NULL, *pub = NULL, *pw, *ret;
	void *dh_ctx;
	u16 val;

	pw = wpabuf_alloc(WPS_OOB_DEVICE_PASSWORD_LEN);
	if (pw == NULL)
		return NULL;

	if (random_get_bytes(wpabuf_put(pw, WPS_OOB_DEVICE_PASSWORD_LEN),
			     WPS_OOB_DEVICE_PASSWORD_LEN) ||
	    random_get_bytes((u8 *) &val, sizeof(val))) {
		wpabuf_free(pw);
		return NULL;
	}

	dh_ctx = dh5_init(&priv, &pub);
	if (dh_ctx == NULL) {
		wpabuf_free(pw);
		return NULL;
	}
	dh5_free(dh_ctx);

	*id = 0x10 + val % 0xfff0;
	wpabuf_free(*pubkey);
	*pubkey = pub;
	wpabuf_free(*privkey);
	*privkey = priv;
	wpabuf_free(*dev_pw);
	*dev_pw = pw;

	ret = wps_build_nfc_pw_token(*id, *pubkey, *dev_pw);
	if (ndef && ret) {
		struct wpabuf *tmp;
		tmp = ndef_build_wifi(ret);
		wpabuf_free(ret);
		if (tmp == NULL)
			return NULL;
		ret = tmp;
	}

	return ret;
}
#endif /* CONFIG_WPS_NFC */
