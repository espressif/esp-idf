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

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "utils/state_machine.h"
#include "rsn_supp/wpa.h"

#include "crypto/crypto.h"

#include "utils/ext_password.h"
#include "tls/tls.h"
#include "eap_peer/eap_i.h"
#include "eap_peer/eap_config.h"
#include "eap_peer/eap.h"
#include "eap_peer/eap_tls.h"
#include "esp_supplicant/esp_wifi_driver.h"
#ifdef EAP_PEER_METHOD
#include "eap_peer/eap_methods.h"
#endif

#include "supplicant_opt.h"


void eap_peer_config_deinit(struct eap_sm *sm);
void eap_peer_blob_deinit(struct eap_sm *sm);
void eap_deinit_prev_method(struct eap_sm *sm, const char *txt);

#ifdef EAP_PEER_METHOD
static struct eap_method *eap_methods = NULL;
static struct eap_method_type *config_methods;

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

/**
 * eap_config_allowed_method - Check whether EAP method is allowed
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @config: EAP configuration
 * @vendor: Vendor-Id for expanded types or 0 = IETF for legacy types
 * @method: EAP type
 * Returns: 1 = allowed EAP method, 0 = not allowed
 */
static int eap_config_allowed_method(struct eap_sm *sm,
				     struct eap_peer_config *config,
				     int vendor, u32 method)
{
	int i;
	struct eap_method_type *m;

	if (config == NULL || config->eap_methods == NULL)
		return 1;

	m = config->eap_methods;
	for (i = 0; m[i].vendor != EAP_VENDOR_IETF ||
		     m[i].method != EAP_TYPE_NONE; i++) {
		if (m[i].vendor == vendor && m[i].method == method)
			return 1;
	}
	return 0;
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

/**
 * eap_allowed_method - Check whether EAP method is allowed
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @vendor: Vendor-Id for expanded types or 0 = IETF for legacy types
 * @method: EAP type
 * Returns: 1 = allowed EAP method, 0 = not allowed
 */
int eap_allowed_method(struct eap_sm *sm, int vendor, u32 method)
{
	return eap_config_allowed_method(sm, eap_get_config(sm), vendor,
					 method);
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

bool eap_sm_allowMethod(struct eap_sm *sm, int vendor,
				  EapType method)
{
	if (!eap_allowed_method(sm, vendor, method)) {
		wpa_printf(MSG_DEBUG, "EAP: configuration does not allow: "
			   "vendor %u method %u", vendor, method);
		return FALSE;
	}
	if (eap_peer_get_eap_method(vendor, method))
		return TRUE;
	wpa_printf(MSG_DEBUG, "EAP: not included in build: "
		   "vendor %u method %u", vendor, method);
	return FALSE;
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

#ifndef USE_MBEDTLS_CRYPTO
#ifdef EAP_FAST
	if (ret == 0)
		ret = eap_peer_fast_register();
#endif
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

static int eap_sm_set_scard_pin(struct eap_sm *sm,
				struct eap_peer_config *conf)
{
	return -1;
}

static int eap_sm_get_scard_identity(struct eap_sm *sm,
				     struct eap_peer_config *conf)
{
	return -1;
}

/**
 * eap_sm_buildIdentity - Build EAP-Identity/Response for the current network
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @id: EAP identifier for the packet
 * @encrypted: Whether the packet is for encrypted tunnel (EAP phase 2)
 * Returns: Pointer to the allocated EAP-Identity/Response packet or %NULL on
 * failure
 *
 * This function allocates and builds an EAP-Identity/Response packet for the
 * current network. The caller is responsible for freeing the returned data.
 */
struct wpabuf * eap_sm_buildIdentity(struct eap_sm *sm, int id, int encrypted)
{
	struct eap_peer_config *config = eap_get_config(sm);
	struct wpabuf *resp;
	const u8 *identity;
	size_t identity_len;

	if (config == NULL) {
		wpa_printf(MSG_ERROR, "EAP: buildIdentity: configuration "
			   "was not available");
		return NULL;
	}

	if (sm->m && sm->m->get_identity &&
	    (identity = sm->m->get_identity(sm, sm->eap_method_priv,
					    &identity_len)) != NULL) {
		wpa_hexdump_ascii(MSG_DEBUG, "EAP: using method re-auth "
				  "identity", identity, identity_len);
	} else if (!encrypted && config->anonymous_identity) {
		identity = config->anonymous_identity;
		identity_len = config->anonymous_identity_len;
		wpa_hexdump_ascii(MSG_DEBUG, "EAP: using anonymous identity",
				  identity, identity_len);
	} else {
		identity = config->identity;
		identity_len = config->identity_len;
		wpa_hexdump_ascii(MSG_DEBUG, "EAP: using real identity",
				  identity, identity_len);
	}

	if (identity == NULL) {
		wpa_printf(MSG_ERROR, "EAP: buildIdentity: identity "
			   "configuration was not available");
		if (config->pcsc) {
			if (eap_sm_get_scard_identity(sm, config) < 0)
				return NULL;
			identity = config->identity;
			identity_len = config->identity_len;
			wpa_hexdump_ascii(MSG_DEBUG, "permanent identity from "
					  "IMSI", identity, identity_len);
		} else {
			eap_sm_request_identity(sm);
			return NULL;
		}
	} else if (config->pcsc) {
		if (eap_sm_set_scard_pin(sm, config) < 0)
			return NULL;
	}

	resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY, identity_len,
			     EAP_CODE_RESPONSE, id);
	if (resp == NULL)
		return NULL;

	wpabuf_put_data(resp, identity, identity_len);

	return resp;
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
		//do not propose insecure unencapsulated MSCHAPv2 as Phase 1 Method
		if(m->vendor == EAP_VENDOR_IETF && m->method == EAP_TYPE_MSCHAPV2)
			continue;

		//do not propose EAP_TYPE_TLS if no client cert/key are configured
		if(m->vendor == EAP_VENDOR_IETF && m->method == EAP_TYPE_TLS) {
			struct eap_peer_config *config = eap_get_config(sm);
			if (config == NULL || config->private_key == 0 || config->client_cert == 0)
				continue;
		}

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
	struct eap_sm *sm, const u8 *private_key_passwd,
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

	sm->config.pac_file = (char *) "blob://";

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

	/* To be used only for EAP-FAST */
	if (g_wpa_phase1_options) {
		sm->config.phase1 = g_wpa_phase1_options;
	}

	/* Allow methods with correct configuration only */
	size_t mcount;
	int allowed_method_count = 0;
	const struct eap_method *methods;

	if (!config_methods) {
		methods = eap_peer_get_methods(&mcount);
		if (methods == NULL) {
			wpa_printf(MSG_ERROR, "EAP: Set config init: EAP methods not registered.");
			return -1;
		}

		// Allocate a buffer large enough to accomodate all the registered methods.
		config_methods = os_malloc(mcount * sizeof(struct eap_method_type));
		if (config_methods == NULL) {
			wpa_printf(MSG_ERROR, "EAP: Set config init: Out of memory, set configured methods failed");
			return -1;
		}

		if (g_wpa_username) {
			//set EAP-PEAP
			config_methods[allowed_method_count].vendor = EAP_VENDOR_IETF;
			config_methods[allowed_method_count++].method = EAP_TYPE_PEAP;
			//set EAP-TTLS
			config_methods[allowed_method_count].vendor = EAP_VENDOR_IETF;
			config_methods[allowed_method_count++].method = EAP_TYPE_TTLS;
		}
		if (g_wpa_private_key) {
			//set EAP-TLS
			config_methods[allowed_method_count].vendor = EAP_VENDOR_IETF;
			config_methods[allowed_method_count++].method = EAP_TYPE_TLS;
		}
#ifndef USE_MBEDTLS_CRYPTO
		if (g_wpa_pac_file) {
			//set EAP-FAST
			config_methods[allowed_method_count].vendor = EAP_VENDOR_IETF;
			config_methods[allowed_method_count++].method = EAP_TYPE_FAST;
		}
#endif
		// Terminate the allowed method list
		config_methods[allowed_method_count].vendor = EAP_VENDOR_IETF;
		config_methods[allowed_method_count++].method = EAP_TYPE_NONE;
		// Set the allowed methods to config
		sm->config.eap_methods = config_methods;
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
	os_free(sm->config.eap_methods);
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
		os_strncpy(sm->blob[0].name, CLIENT_CERT_NAME, BLOB_NAME_LEN+1);
		sm->blob[0].len = g_wpa_client_cert_len;
		sm->blob[0].data = g_wpa_client_cert;
	}

	if (g_wpa_private_key && g_wpa_private_key_len) {
		sm->blob[1].name = (char *)os_zalloc(BLOB_NAME_LEN+1);
		if (sm->blob[1].name == NULL) {
			ret = -2;
			goto _out;
		}
		os_strncpy(sm->blob[1].name, PRIVATE_KEY_NAME, BLOB_NAME_LEN+1);
		sm->blob[1].len = g_wpa_private_key_len;
		sm->blob[1].data = g_wpa_private_key;
	}

	if (g_wpa_ca_cert && g_wpa_ca_cert_len) {
		sm->blob[2].name = (char *)os_zalloc(BLOB_NAME_LEN+1);
		if (sm->blob[2].name == NULL) {
			ret = -2;
			goto _out;
		}
		os_strncpy(sm->blob[2].name, CA_CERT_NAME, BLOB_NAME_LEN+1);
		sm->blob[2].len = g_wpa_ca_cert_len;
		sm->blob[2].data = g_wpa_ca_cert;
	}

	if (g_wpa_pac_file && g_wpa_pac_file_len) {
		sm->blob[3].name = (char *)os_zalloc(sizeof(char) * 8);
		if (sm->blob[3].name == NULL) {
			ret = -2;
			goto _out;
		}
		os_strncpy(sm->blob[3].name, "blob://", 8);
		sm->blob[3].len = g_wpa_pac_file_len;
		sm->blob[3].data = g_wpa_pac_file;
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

#if defined(CONFIG_CTRL_IFACE) || !defined(CONFIG_NO_STDOUT_DEBUG)
static void eap_sm_request(struct eap_sm *sm, enum wpa_ctrl_req_type field,
			   const char *msg, size_t msglen)
{
	struct eap_peer_config *config;

	if (sm == NULL)
		return;
	config = eap_get_config(sm);
	if (config == NULL)
		return;

	switch (field) {
	case WPA_CTRL_REQ_EAP_IDENTITY:
		config->pending_req_identity++;
		break;
	case WPA_CTRL_REQ_EAP_PASSWORD:
		config->pending_req_password++;
		break;
	case WPA_CTRL_REQ_EAP_NEW_PASSWORD:
		config->pending_req_new_password++;
		break;
	case WPA_CTRL_REQ_EAP_PIN:
		config->pending_req_pin++;
		break;
	case WPA_CTRL_REQ_EAP_PASSPHRASE:
		config->pending_req_passphrase++;
		break;
	default:
		return;
	}

}
#else /* CONFIG_CTRL_IFACE || !CONFIG_NO_STDOUT_DEBUG */
#define eap_sm_request(sm, type, msg, msglen) do { } while (0)
#endif /* CONFIG_CTRL_IFACE || !CONFIG_NO_STDOUT_DEBUG */

const char * eap_sm_get_method_name(struct eap_sm *sm)
{
	if (sm->m == NULL)
		return "UNKNOWN";
	return sm->m->name;
}

/**
 * eap_sm_request_identity - Request identity from user (ctrl_iface)
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 *
 * EAP methods can call this function to request identity information for the
 * current network. This is normally called when the identity is not included
 * in the network configuration. The request will be sent to monitor programs
 * through the control interface.
 */
void eap_sm_request_identity(struct eap_sm *sm)
{
	eap_sm_request(sm, WPA_CTRL_REQ_EAP_IDENTITY, NULL, 0);
}


/**
 * eap_sm_request_password - Request password from user (ctrl_iface)
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 *
 * EAP methods can call this function to request password information for the
 * current network. This is normally called when the password is not included
 * in the network configuration. The request will be sent to monitor programs
 * through the control interface.
 */
void eap_sm_request_password(struct eap_sm *sm)
{
	eap_sm_request(sm, WPA_CTRL_REQ_EAP_PASSWORD, NULL, 0);
}


/**
 * eap_sm_request_new_password - Request new password from user (ctrl_iface)
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 *
 * EAP methods can call this function to request new password information for
 * the current network. This is normally called when the EAP method indicates
 * that the current password has expired and password change is required. The
 * request will be sent to monitor programs through the control interface.
 */
void eap_sm_request_new_password(struct eap_sm *sm)
{
	eap_sm_request(sm, WPA_CTRL_REQ_EAP_NEW_PASSWORD, NULL, 0);
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
	sm->config.pac_file = NULL;
}

void eap_sm_abort(struct eap_sm *sm)
{
	wpabuf_free(sm->lastRespData);
	sm->lastRespData = NULL;
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


static int eap_copy_buf(u8 **dst, size_t *dst_len,
			     const u8 *src, size_t src_len)
{
	if (src) {
		*dst = os_memdup(src, src_len);
		if (*dst == NULL)
			return -1;
		*dst_len = src_len;
	}
	return 0;
}


/**
 * eap_set_config_blob - Set or add a named configuration blob
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @blob: New value for the blob
 *
 * Adds a new configuration blob or replaces the current value of an existing
 * blob.
 */
void eap_set_config_blob(struct eap_sm *sm, struct wpa_config_blob *blob)
{
    if (!sm)
        return;

    if (eap_copy_buf((u8 **)&sm->blob[3].data, (size_t *)&sm->blob[3].len, blob->data, blob->len) < 0) {
		wpa_printf(MSG_ERROR, "EAP: Set config blob: Unable to modify the configuration blob");
    }
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

