/*
 * EAP peer state machines (RFC 4137)
 * Copyright (c) 2004-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file implements the Peer State Machine as defined in RFC 4137. The used
 * states and state transitions match mostly with the RFC. However, there are
 * couple of additional transitions for working around small issues noticed
 * during testing. These exceptions are explained in comments within the
 * functions in this file. The method functions, m.func(), are similar to the
 * ones used in RFC 4137, but some small changes have used here to optimize
 * operations and to add functionality needed for fast re-authentication
 * (session resumption).
 */
#include <string.h>
     
#include "esp_err.h"

#include "wpa/includes.h"
#include "wpa/common.h"
#include "wpa/wpa_debug.h"
#include "wpa/eapol_common.h"
#include "wpa/ieee802_11_defs.h"
#include "wpa/state_machine.h"
#include "wpa/wpa.h"

#include "crypto/crypto.h"

#include "wpa2/utils/ext_password.h"
#include "wpa2/tls/tls.h"
#include "wpa2/eap_peer/eap_i.h"
#include "wpa2/eap_peer/eap_config.h"
#include "wpa2/eap_peer/eap.h"
#include "wpa2/eap_peer/eap_tls.h"
#ifdef EAP_PEER_METHOD
#include "wpa2/eap_peer/eap_methods.h"
#endif


static bool gl_disable_time_check = true;
void eap_peer_config_deinit(struct eap_sm *sm);
void eap_peer_blob_deinit(struct eap_sm *sm);
void eap_deinit_prev_method(struct eap_sm *sm, const char *txt);

extern bool ieee80211_unregister_wpa2_cb(void);

#ifdef EAP_PEER_METHOD
static struct eap_method *eap_methods = NULL;

const struct eap_method * eap_peer_get_eap_method(int vendor, EapType method)
{
	struct eap_method *m;
	for (m = eap_methods; m; m = m->next) {
		if (m->vendor == vendor && m->method == method)
			return m;
	}
	return NULL;
}

const struct eap_method * eap_peer_get_methods(size_t *count)
{
	int c = 0;
	struct eap_method *m;

	for (m = eap_methods; m; m = m->next)
		c++;

	*count = c;
	return eap_methods;
}

EapType eap_peer_get_type(const char *name, int *vendor)
{
	struct eap_method *m;
	for (m = eap_methods; m; m = m->next) {
		if (os_strcmp(m->name, name) == 0) {
			*vendor = m->vendor;
			return m->method;
		}
	}
	*vendor = EAP_VENDOR_IETF;
	return EAP_TYPE_NONE;
}

static int 
eap_allowed_phase2_type(int vendor, int type)
{
	if (vendor != EAP_VENDOR_IETF)
		return 0;
	return type != EAP_TYPE_PEAP && type != EAP_TYPE_TTLS &&
		type != EAP_TYPE_FAST;
}

u32 eap_get_phase2_type(const char *name, int *vendor)
{
	int v;
	u8 type = eap_peer_get_type(name, &v);
	if (eap_allowed_phase2_type(v, type)) {
		*vendor = v;
		return type;
	}
	*vendor = EAP_VENDOR_IETF;
	return EAP_TYPE_NONE;
}

struct eap_method_type * eap_get_phase2_types(struct eap_peer_config *config,
		     size_t *count)
{
	struct eap_method_type *buf;
	u32 method;
	int vendor;
	size_t mcount;
	const struct eap_method *methods, *m;

	methods = eap_peer_get_methods(&mcount);
	if (methods == NULL)
		return NULL;
	*count = 0;
	buf = os_malloc(mcount * sizeof(struct eap_method_type));
	if (buf == NULL)
		return NULL;

	for (m = methods; m; m = m->next) {
		vendor = m->vendor;
		method = m->method;
		if (eap_allowed_phase2_type(vendor, method)) {
			if (vendor == EAP_VENDOR_IETF &&
			    method == EAP_TYPE_TLS && config &&
			    config->private_key2 == NULL)
				continue;
			buf[*count].vendor = vendor;
			buf[*count].method = method;
			(*count)++;
		}
	}

	return buf;
}

struct eap_method * eap_peer_method_alloc(int vendor, EapType method,
		      const char *name)
{
	struct eap_method *eap;
	eap = (struct eap_method *)os_zalloc(sizeof(*eap));
	if (eap == NULL)
		return NULL;
	eap->vendor = vendor;
	eap->method = method;
	eap->name = name;
	return eap;
}

void eap_peer_method_free(struct eap_method *method)
{
	os_free(method);
}

int eap_peer_method_register(struct eap_method *method)
{
	struct eap_method *m, *last = NULL;

	if (method == NULL || method->name == NULL)
		return -1;
	for (m = eap_methods; m; m = m->next) {
		if (m->vendor == method->vendor &&
		    m->method == method->method &&
		    os_strcmp(m->name, method->name))
			return -2;
		last = m;
	}
	if (last)
		last->next = method;
	else
		eap_methods = method;
	return 0;
}

void eap_peer_unregister_methods(void)
{
	struct eap_method *m;
	while (eap_methods) {
		m = eap_methods;
		eap_methods = eap_methods->next;

		if (m->free)
			m->free(m);
		else
			eap_peer_method_free(m);
	}
}

int eap_peer_register_methods(void)
{
	int ret = 0;

#ifdef EAP_MD5
	if (ret == 0)
		ret = eap_peer_md5_register();
#endif

#ifdef EAP_TLS
	if (ret == 0)
		ret = eap_peer_tls_register();
#endif

#ifdef EAP_MSCHAPv2
	if (ret == 0)
		ret = eap_peer_mschapv2_register();
#endif

#ifdef EAP_PEAP
	if (ret == 0)
		ret = eap_peer_peap_register();
#endif

#ifdef EAP_TTLS
	if (ret == 0)
		ret = eap_peer_ttls_register();
#endif

	return ret;
}

void eap_deinit_prev_method(struct eap_sm *sm, const char *txt)
{
	if (sm->m == NULL || sm->eap_method_priv == NULL)
		return;
	sm->m->deinit(sm, sm->eap_method_priv);
	sm->eap_method_priv = NULL;
	sm->m = NULL;
}

struct wpabuf * eap_sm_build_identity_resp(struct eap_sm *sm, u8 id, int encrypted)
{
	const u8 *identity;
	size_t identity_len;
	struct wpabuf *eap_buf = NULL;
	struct eap_peer_config *config = eap_get_config(sm);

	if (config == NULL) {
        wpa_printf(MSG_ERROR, "EAP: Build Identity Resp-> configuration was not available\n");
		return NULL;
	}

	if (sm->m && sm->m->get_identity) {
		identity = sm->m->get_identity(sm,
					sm->eap_method_priv,
					&identity_len);
	} else if (!encrypted && config->anonymous_identity) {
		identity = config->anonymous_identity;
		identity_len = config->anonymous_identity_len;
	} else {
		identity = config->identity;
		identity_len = config->identity_len;
	}

	if (identity == NULL) {
        wpa_printf(MSG_ERROR, "EAP: Build Identity Resp-> identity was not available\n");
		return NULL;
	}

	eap_buf = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY,
				identity_len, EAP_CODE_RESPONSE, id);
	if (!eap_buf) {
        return NULL;       
    }

	wpabuf_put_data(eap_buf, identity, identity_len);
	return eap_buf;
}

struct wpabuf * eap_sm_build_nak(struct eap_sm *sm, EapType type, u8 id)
{
	size_t count = 0;
	int found = 0;
	struct wpabuf *resp;
	const struct eap_method *methods, *m;

	methods = eap_peer_get_methods(&count);
	if (methods == NULL)
		return NULL;

	if (type == EAP_TYPE_EXPANDED) {
		/*Build Expanded NAK*/
		resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_EXPANDED,
				     8 + 8 * (count + 1), EAP_CODE_RESPONSE, id);
		if (resp == NULL)
			return NULL;
		wpabuf_put_be24(resp, EAP_VENDOR_IETF);
		wpabuf_put_be32(resp, EAP_TYPE_NAK);
	} else {
		resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NAK,
				     sizeof(struct eap_hdr) + 1 + count + 1,
				     EAP_CODE_RESPONSE, id);
		if (resp == NULL)
			return NULL;
		wpabuf_put(resp, 0);
	}

	for (m = methods; m; m = m->next) {
		if (type == EAP_TYPE_EXPANDED) {
			wpabuf_put_u8(resp, EAP_TYPE_EXPANDED);
			wpabuf_put_be24(resp, m->vendor);
			wpabuf_put_be32(resp, m->method);
		} else
			wpabuf_put_u8(resp, m->method);
		found++;
	}
	if (!found) {
		if (type == EAP_TYPE_EXPANDED) {
			wpabuf_put_u8(resp, EAP_TYPE_EXPANDED);
			wpabuf_put_be24(resp, EAP_VENDOR_IETF);
			wpabuf_put_be32(resp, EAP_TYPE_NONE);
		} else
			wpabuf_put_u8(resp, EAP_TYPE_NONE);
	}
	eap_update_len(resp);
	return resp;
}
#endif

int eap_peer_config_init(
	struct eap_sm *sm, u8 *private_key_passwd,
	int private_key_passwd_len)
{
	if (!sm)
		return -1;

	sm->config.anonymous_identity = NULL;
	sm->config.identity = NULL;
	sm->config.password = NULL;
	sm->config.new_password = NULL;

	sm->config.private_key_passwd = private_key_passwd;
	sm->config.client_cert = (u8 *)sm->blob[0].name;
	sm->config.private_key = (u8 *)sm->blob[1].name;
	sm->config.ca_cert = (u8 *)sm->blob[2].name;

	sm->config.ca_path = NULL;

	sm->config.fragment_size = 1400; /* fragment size */

	/* anonymous identity */
	if (g_wpa_anonymous_identity && g_wpa_anonymous_identity_len > 0) {
	    sm->config.anonymous_identity_len = g_wpa_anonymous_identity_len;
	    sm->config.anonymous_identity = (u8 *)os_zalloc(sm->config.anonymous_identity_len);
	    if (sm->config.anonymous_identity == NULL)
		    return -2;
	    os_memcpy(sm->config.anonymous_identity, g_wpa_anonymous_identity, g_wpa_anonymous_identity_len);
	}

	/* Configre identity */
	if (g_wpa_username && g_wpa_username_len > 0) {
		sm->config.identity_len = g_wpa_username_len;
		sm->config.identity = (u8 *)os_zalloc(sm->config.identity_len);
		if (sm->config.identity == NULL) {
			return -2;
		}
		os_memcpy(sm->config.identity, g_wpa_username, g_wpa_username_len);
	}

	if (g_wpa_password && g_wpa_password_len) {
		sm->config.password_len = g_wpa_password_len;
		sm->config.password = (u8 *)os_zalloc(sm->config.password_len);
		if (sm->config.password == NULL)
			return -2;
		os_memcpy(sm->config.password, g_wpa_password, sm->config.password_len);
	}

	if (g_wpa_new_password && g_wpa_new_password_len) {
		sm->config.new_password_len = g_wpa_new_password_len;
		sm->config.new_password = (u8 *)os_zalloc(sm->config.new_password_len);
		if (sm->config.new_password == NULL)
			return -2;
		os_memcpy(sm->config.new_password, g_wpa_new_password,
			  sm->config.new_password_len);
	}

	return 0;
	
}

void eap_peer_config_deinit(struct eap_sm *sm)
{
	if (!sm)
		return;

	os_free(sm->config.anonymous_identity);
	os_free(sm->config.identity);
	os_free(sm->config.password);
	os_free(sm->config.new_password);
	os_bzero(&sm->config, sizeof(struct eap_peer_config));
}

int eap_peer_blob_init(struct eap_sm *sm)
{
	int i, ret;

	if (!sm)
		return -1;

	if (g_wpa_client_cert && g_wpa_client_cert_len) {
		sm->blob[0].name = (char *)os_zalloc(BLOB_NAME_LEN+1);
		if (sm->blob[0].name == NULL) {
			ret = -2;
			goto _out;
		}
		os_strncpy(sm->blob[0].name, CLIENT_CERT_NAME, BLOB_NAME_LEN);
		sm->blob[0].len = g_wpa_client_cert_len;
		sm->blob[0].data = g_wpa_client_cert;
	}

	if (g_wpa_private_key && g_wpa_private_key_len) {
		sm->blob[1].name = (char *)os_zalloc(BLOB_NAME_LEN+1);
		if (sm->blob[1].name == NULL) {
			ret = -2;
			goto _out;
		}
		os_strncpy(sm->blob[1].name, PRIVATE_KEY_NAME, BLOB_NAME_LEN);
		sm->blob[1].len = g_wpa_private_key_len;
		sm->blob[1].data = g_wpa_private_key;
	}

	if (g_wpa_ca_cert && g_wpa_ca_cert_len) {
		sm->blob[2].name = (char *)os_zalloc(BLOB_NAME_LEN+1);
		if (sm->blob[2].name == NULL) {
			ret = -2;
			goto _out;
		}
		os_strncpy(sm->blob[2].name, CA_CERT_NAME, BLOB_NAME_LEN);
		sm->blob[2].len = g_wpa_ca_cert_len;
		sm->blob[2].data = g_wpa_ca_cert;
	}

	return 0;
_out:
	for (i = 0; i < BLOB_NUM; i++) {
		if (sm->blob[i].name) {
			os_free(sm->blob[i].name);
			sm->blob[i].name = NULL;
		}
	}
	os_bzero(&sm->blob[0], sizeof(struct wpa_config_blob)*BLOB_NUM);

	return ret;
}

void eap_peer_blob_deinit(struct eap_sm *sm)
{
	int i;
	for (i = 0; i < BLOB_NUM; i++) {
		if (sm->blob[i].name) {
			os_free(sm->blob[i].name);
			sm->blob[i].name = NULL;
		}
	}
	os_bzero(&sm->blob[0], sizeof(struct wpa_config_blob)*BLOB_NUM);

	sm->config.client_cert = NULL;
	sm->config.private_key = NULL;
	sm->config.ca_cert = NULL;
}

void eap_sm_abort(struct eap_sm *sm)
{
	wpabuf_free(sm->lastRespData);
	sm->lastRespData = NULL;
	//os_free(sm->eapKeyData);
	//sm->eapKeyData = NULL;
}

/**
 * eap_get_config - Get current network configuration
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * Returns: Pointer to the current network configuration or %NULL if not found
 *
 * EAP peer methods should avoid using this function if they can use other
 * access functions, like eap_get_config_identity() and
 * eap_get_config_password(), that do not require direct access to
 * struct eap_peer_config.
 */
struct eap_peer_config * eap_get_config(struct eap_sm *sm)
{
	return &sm->config;
}

const u8 * eap_get_config_identity(struct eap_sm *sm, size_t *len)
{
	struct eap_peer_config *config = eap_get_config(sm);
	if (config == NULL)
		return NULL;
	*len = config->identity_len;
	return config->identity;
}

const u8 * eap_get_config_password(struct eap_sm *sm, size_t *len)
{
	struct eap_peer_config *config = eap_get_config(sm);
	if (config == NULL)
		return NULL;
	*len = config->password_len;
	return config->password;
}

const u8 * eap_get_config_password2(struct eap_sm *sm, size_t *len, int *hash)
{
	struct eap_peer_config *config = eap_get_config(sm);
	if (config == NULL)
		return NULL;

	*len = config->password_len;
	if (hash)
		*hash = !!(config->flags & EAP_CONFIG_FLAGS_PASSWORD_NTHASH);
	return config->password;
}

const u8 * eap_get_config_new_password(struct eap_sm *sm, size_t *len)
{
	struct eap_peer_config *config = eap_get_config(sm);
	if (config == NULL)
		return NULL;
	*len = config->new_password_len;
	return config->new_password;
}
/**
 * eap_get_config_blob - Get a named configuration blob
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @name: Name of the blob
 * Returns: Pointer to blob data or %NULL if not found
 */
const struct wpa_config_blob * eap_get_config_blob(struct eap_sm *sm,
						   const char *name)
{
	int i;

	if (!sm)
		return NULL;

	for (i = 0; i < BLOB_NUM; i++) {
		if (sm->blob[i].name == NULL)
			continue;
		if (os_strncmp(name, sm->blob[i].name, BLOB_NAME_LEN) == 0) {
			return &sm->blob[i];
		}
	}
	return NULL;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_cert_key(const unsigned char *client_cert, int client_cert_len, const unsigned char *private_key, int private_key_len, const unsigned char *private_key_passwd, int private_key_passwd_len)
{
	if (client_cert && client_cert_len > 0) {
		g_wpa_client_cert = client_cert;
		g_wpa_client_cert_len = client_cert_len;
	}
	if (private_key && private_key_len > 0) {
		g_wpa_private_key = private_key;
		g_wpa_private_key_len = private_key_len;
	} 
	if (private_key_passwd && private_key_passwd_len > 0) {
		g_wpa_private_key_passwd = private_key_passwd;
		g_wpa_private_key_passwd_len = private_key_passwd_len;
	}

	return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_cert_key(void)
{
	ieee80211_unregister_wpa2_cb();

    g_wpa_client_cert = NULL;
    g_wpa_client_cert_len = 0;
    g_wpa_private_key = NULL;
    g_wpa_private_key_len = 0;
    g_wpa_private_key_passwd = NULL;
    g_wpa_private_key_passwd_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len)
{
	if (ca_cert && ca_cert_len > 0) {
		g_wpa_ca_cert = ca_cert;
		g_wpa_ca_cert_len = ca_cert_len;	
	}

	return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_ca_cert(void)
{
	g_wpa_ca_cert = NULL;
	g_wpa_ca_cert_len = 0;
}

#define ANONYMOUS_ID_LEN_MAX 128
esp_err_t esp_wifi_sta_wpa2_ent_set_identity(const unsigned char *identity, int len)
{
	if (len <= 0 || len > ANONYMOUS_ID_LEN_MAX) {
		return ESP_ERR_INVALID_ARG;
	}

	if (g_wpa_anonymous_identity) {
		os_free(g_wpa_anonymous_identity);
		g_wpa_anonymous_identity = NULL;
	}

	g_wpa_anonymous_identity = (u8 *)os_zalloc(len);
	if (g_wpa_anonymous_identity == NULL) {
		return ESP_ERR_NO_MEM;
	}

	os_memcpy(g_wpa_anonymous_identity, identity, len);
	g_wpa_anonymous_identity_len = len;

	return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_identity(void)
{
	if (g_wpa_anonymous_identity)
		os_free(g_wpa_anonymous_identity);

	g_wpa_anonymous_identity = NULL;
	g_wpa_anonymous_identity_len = 0;
}

#define USERNAME_LEN_MAX 128
esp_err_t esp_wifi_sta_wpa2_ent_set_username(const unsigned char *username, int len)
{
	if (len <= 0 || len > USERNAME_LEN_MAX)
		return ESP_ERR_INVALID_ARG;

	if (g_wpa_username) {
		os_free(g_wpa_username);
		g_wpa_username = NULL;
	}

	g_wpa_username = (u8 *)os_zalloc(len);
	if (g_wpa_username == NULL)
		return ESP_ERR_NO_MEM;

	os_memcpy(g_wpa_username, username, len);
	g_wpa_username_len = len;

	return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_username(void)
{
	if (g_wpa_username)
		os_free(g_wpa_username);

	g_wpa_username = NULL;
	g_wpa_username_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_password(const unsigned char *password, int len)
{
	if (len <= 0)
		return ESP_ERR_INVALID_ARG;

	if (g_wpa_password) {
		os_free(g_wpa_password);
		g_wpa_password = NULL;
	}

	g_wpa_password = (u8 *)os_zalloc(len);
	if (g_wpa_password == NULL)
		return ESP_ERR_NO_MEM;

	os_memcpy(g_wpa_password, password, len);
	g_wpa_password_len = len;

	return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_password(void)
{
	if (g_wpa_password)
		os_free(g_wpa_password);
	g_wpa_password = NULL;
	g_wpa_password_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_new_password(const unsigned char *new_password, int len)
{
	if (len <= 0)
		return ESP_ERR_INVALID_ARG;

	if (g_wpa_new_password) {
		os_free(g_wpa_new_password);
		g_wpa_new_password = NULL;
	}

	g_wpa_new_password = (u8 *)os_zalloc(len);
	if (g_wpa_new_password == NULL)
		return ESP_ERR_NO_MEM;

	os_memcpy(g_wpa_new_password, new_password, len);
	g_wpa_password_len = len;

	return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_new_password(void)
{
	if (g_wpa_new_password)
		os_free(g_wpa_new_password);
	g_wpa_new_password = NULL;
	g_wpa_new_password_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_disable_time_check(bool disable)
{
	gl_disable_time_check = disable;
	return ESP_OK;
}

bool wifi_sta_get_enterprise_disable_time_check(void)
{
	return gl_disable_time_check;
}

esp_err_t esp_wifi_sta_wpa2_ent_get_disable_time_check(bool *disable)
{
	*disable = wifi_sta_get_enterprise_disable_time_check();
	return ESP_OK;
}

