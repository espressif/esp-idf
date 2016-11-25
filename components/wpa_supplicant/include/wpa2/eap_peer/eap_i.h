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

#define CLIENT_CERT_NAME	"CLC"
#define CA_CERT_NAME		"CAC"
#define PRIVATE_KEY_NAME	"PVK"
#define BLOB_NAME_LEN		3
#define BLOB_NUM		2

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
#define SIG_WPA2_NUM 2
    	u8 wpa2_sig_cnt[SIG_WPA2_NUM];
#endif
	u8 finish_state;
};

struct eap_peer_config * eap_get_config(struct eap_sm *sm);
const struct wpa_config_blob * eap_get_config_blob(struct eap_sm *sm, const char *name);

#endif /* EAP_I_H */
