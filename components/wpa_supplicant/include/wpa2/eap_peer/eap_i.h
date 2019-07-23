/*
 * EAP peer state machines internal structures (RFC 4137)
 * Copyright (c) 2004-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_I_H
#define EAP_I_H

#include "wpa/wpabuf.h"
#include "eap.h"
#include "eap_common.h"
#include "eap_config.h"
#include "esp_wpa2.h"

/* RFC 4137 - EAP Peer state machine */

typedef enum {
	DECISION_FAIL, DECISION_COND_SUCC, DECISION_UNCOND_SUCC
} EapDecision;

typedef enum {
	METHOD_NONE, METHOD_INIT, METHOD_CONT, METHOD_MAY_CONT, METHOD_DONE
} EapMethodState;

/**
 * struct eap_method_ret - EAP return values from struct eap_method::process()
 *
 * These structure contains OUT variables for the interface between peer state
 * machine and methods (RFC 4137, Sect. 4.2). eapRespData will be returned as
 * the return value of struct eap_method::process() so it is not included in
 * this structure.
 */
struct eap_method_ret {
	/**
	 * ignore - Whether method decided to drop the current packed (OUT)
	 */
	Boolean ignore;

	/**
	 * methodState - Method-specific state (IN/OUT)
	 */
	EapMethodState methodState;

	/**
	 * decision - Authentication decision (OUT)
	 */
	EapDecision decision;

	/**
	 * allowNotifications - Whether method allows notifications (OUT)
	 */
	Boolean allowNotifications;
};

struct eap_sm;

struct eap_method {
	/**
	 * vendor -EAP Vendor-ID
	 */
	int vendor;

	/**
	 * method - EAP type number
	 */
	EapType method;	

	/**
	 * name - Name of the method (e.g., "TLS")
	 */
	const char *name;

	struct eap_method *next;

	void * (*init)(struct eap_sm *sm);
	void (*deinit)(struct eap_sm *sm, void *priv);
	struct wpabuf * (*process)(struct eap_sm *sm, void *priv,
				   struct eap_method_ret *ret,
				   const struct wpabuf *reqData);
	bool (*isKeyAvailable)(struct eap_sm *sm, void *priv);
	u8 * (*getKey)(struct eap_sm *sm, void *priv, size_t *len);
	int (*get_status)(struct eap_sm *sm, void *priv, char *buf,
			  size_t buflen, int verbose);
	const u8 * (*get_identity)(struct eap_sm *sm, void *priv, size_t *len);
	void (*free)(struct eap_method *method);
	bool (*has_reauth_data)(struct eap_sm *sm, void *priv);
	void (*deinit_for_reauth)(struct eap_sm *sm, void *priv);
	void * (*init_for_reauth)(struct eap_sm *sm, void *priv);
	u8 * (*getSessionId)(struct eap_sm *sm, void *priv, size_t *len);
};

#define CLIENT_CERT_NAME	"CLC"
#define CA_CERT_NAME		"CAC"
#define PRIVATE_KEY_NAME	"PVK"
#define BLOB_NAME_LEN		3
#define BLOB_NUM		3

enum SIG_WPA2 {		
    SIG_WPA2_START = 0,
	SIG_WPA2_RX,
	SIG_WPA2_TASK_DEL,
	SIG_WPA2_MAX,
};

/**
 * struct eap_sm - EAP state machine data
 */
struct eap_sm {
	void *eap_method_priv;

	void *ssl_ctx;

	unsigned int workaround;
/////////////////////////////////////////////////
        struct pbuf *outbuf;
	struct wpa_config_blob blob[BLOB_NUM];
	struct eap_peer_config config;
	u8 current_identifier;
	u8 ownaddr[ETH_ALEN];
#ifdef USE_WPA2_TASK
    	u8 wpa2_sig_cnt[SIG_WPA2_MAX];
#endif
	u8 finish_state;

	int init_phase2;
	bool peap_done;

	u8 *eapKeyData;
	size_t eapKeyDataLen;
	struct wpabuf *lastRespData;
	const struct eap_method *m;
};

wpa2_crypto_funcs_t wpa2_crypto_funcs;

const u8 * eap_get_config_identity(struct eap_sm *sm, size_t *len);
const u8 * eap_get_config_password(struct eap_sm *sm, size_t *len);
const u8 * eap_get_config_password2(struct eap_sm *sm, size_t *len, int *hash);
const u8 * eap_get_config_new_password(struct eap_sm *sm, size_t *len);
struct eap_peer_config * eap_get_config(struct eap_sm *sm);
const struct wpa_config_blob * eap_get_config_blob(struct eap_sm *sm, const char *name);
bool wifi_sta_get_enterprise_disable_time_check(void);

struct wpabuf * eap_sm_build_identity_resp(struct eap_sm *sm, u8 id, int encrypted);

#endif /* EAP_I_H */
