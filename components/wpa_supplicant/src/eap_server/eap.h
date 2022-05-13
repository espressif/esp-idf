/*
 * hostapd / EAP Full Authenticator state machine (RFC 4137)
 * Copyright (c) 2004-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_H
#define EAP_H

#include "common/defs.h"
#include "utils/list.h"
#include "eap_common/eap_defs.h"
#include "eap_server/eap_methods.h"
#include "wpabuf.h"

struct eap_sm;

#define EAP_TTLS_AUTH_PAP 1
#define EAP_TTLS_AUTH_CHAP 2
#define EAP_TTLS_AUTH_MSCHAP 4
#define EAP_TTLS_AUTH_MSCHAPV2 8

struct eap_user {
	struct {
		int vendor;
		u32 method;
	} methods[EAP_MAX_METHODS];
	u8 *password;
	size_t password_len;
	int password_hash; /* whether password is hashed with
			    * nt_password_hash() */
	u8 *salt;
	size_t salt_len;
	int phase2;
	int force_version;
	unsigned int remediation:1;
	unsigned int macacl:1;
	int ttls_auth; /* bitfield of
			* EAP_TTLS_AUTH_{PAP,CHAP,MSCHAP,MSCHAPV2} */
	struct hostapd_radius_attr *accept_attr;
	u32 t_c_timestamp;
};

struct eap_eapol_interface {
	/* Lower layer to full authenticator variables */
	bool eapResp; /* shared with EAPOL Backend Authentication */
	struct wpabuf *eapRespData;
	bool portEnabled;
	int retransWhile;
	bool eapRestart; /* shared with EAPOL Authenticator PAE */
	int eapSRTT;
	int eapRTTVAR;

	/* Full authenticator to lower layer variables */
	bool eapReq; /* shared with EAPOL Backend Authentication */
	bool eapNoReq; /* shared with EAPOL Backend Authentication */
	bool eapSuccess;
	bool eapFail;
	bool eapTimeout;
	struct wpabuf *eapReqData;
	u8 *eapKeyData;
	size_t eapKeyDataLen;
	u8 *eapSessionId;
	size_t eapSessionIdLen;
	bool eapKeyAvailable; /* called keyAvailable in IEEE 802.1X-2004 */

	/* AAA interface to full authenticator variables */
	bool aaaEapReq;
	bool aaaEapNoReq;
	bool aaaSuccess;
	bool aaaFail;
	struct wpabuf *aaaEapReqData;
	u8 *aaaEapKeyData;
	size_t aaaEapKeyDataLen;
	bool aaaEapKeyAvailable;
	int aaaMethodTimeout;

	/* Full authenticator to AAA interface variables */
	bool aaaEapResp;
	struct wpabuf *aaaEapRespData;
	/* aaaIdentity -> eap_get_identity() */
	bool aaaTimeout;
};

struct eap_server_erp_key {
	struct dl_list list;
	size_t rRK_len;
	size_t rIK_len;
	u8 rRK[ERP_MAX_KEY_LEN];
	u8 rIK[ERP_MAX_KEY_LEN];
	u32 recv_seq;
	u8 cryptosuite;
	char keyname_nai[];
};

struct eapol_callbacks {
	int (*get_eap_user)(void *ctx, const u8 *identity, size_t identity_len,
			    int phase2, struct eap_user *user);
	const char * (*get_eap_req_id_text)(void *ctx, size_t *len);
	void (*log_msg)(void *ctx, const char *msg);
	int (*get_erp_send_reauth_start)(void *ctx);
	const char * (*get_erp_domain)(void *ctx);
	struct eap_server_erp_key * (*erp_get_key)(void *ctx,
						   const char *keyname);
	int (*erp_add_key)(void *ctx, struct eap_server_erp_key *erp);
};

struct eap_config {
	/**
	 * ssl_ctx - TLS context
	 *
	 * This is passed to the EAP server implementation as a callback
	 * context for TLS operations.
	 */
	void *ssl_ctx;
	void *msg_ctx;

	/**
	 * eap_sim_db_priv - EAP-SIM/AKA database context
	 *
	 * This is passed to the EAP-SIM/AKA server implementation as a
	 * callback context.
	 */
	void *eap_sim_db_priv;
	bool backend_auth;
	int eap_server;

	/**
	 * pwd_group - The D-H group assigned for EAP-pwd
	 *
	 * If EAP-pwd is not used it can be set to zero.
	 */
	u16 pwd_group;

	/**
	 * pac_opaque_encr_key - PAC-Opaque encryption key for EAP-FAST
	 *
	 * This parameter is used to set a key for EAP-FAST to encrypt the
	 * PAC-Opaque data. It can be set to %NULL if EAP-FAST is not used. If
	 * set, must point to a 16-octet key.
	 */
	u8 *pac_opaque_encr_key;

	/**
	 * eap_fast_a_id - EAP-FAST authority identity (A-ID)
	 *
	 * If EAP-FAST is not used, this can be set to %NULL. In theory, this
	 * is a variable length field, but due to some existing implementations
	 * requiring A-ID to be 16 octets in length, it is recommended to use
	 * that length for the field to provide interoperability with deployed
	 * peer implementations.
	 */
	u8 *eap_fast_a_id;

	/**
	 * eap_fast_a_id_len - Length of eap_fast_a_id buffer in octets
	 */
	size_t eap_fast_a_id_len;
	/**
	 * eap_fast_a_id_info - EAP-FAST authority identifier information
	 *
	 * This A-ID-Info contains a user-friendly name for the A-ID. For
	 * example, this could be the enterprise and server names in
	 * human-readable format. This field is encoded as UTF-8. If EAP-FAST
	 * is not used, this can be set to %NULL.
	 */
	char *eap_fast_a_id_info;

	/**
	 * eap_fast_prov - EAP-FAST provisioning modes
	 *
	 * 0 = provisioning disabled, 1 = only anonymous provisioning allowed,
	 * 2 = only authenticated provisioning allowed, 3 = both provisioning
	 * modes allowed.
	 */
	enum {
		NO_PROV, ANON_PROV, AUTH_PROV, BOTH_PROV
	} eap_fast_prov;

	/**
	 * pac_key_lifetime - EAP-FAST PAC-Key lifetime in seconds
	 *
	 * This is the hard limit on how long a provisioned PAC-Key can be
	 * used.
	 */
	int pac_key_lifetime;

	/**
	 * pac_key_refresh_time - EAP-FAST PAC-Key refresh time in seconds
	 *
	 * This is a soft limit on the PAC-Key. The server will automatically
	 * generate a new PAC-Key when this number of seconds (or fewer) of the
	 * lifetime remains.
	 */
	int pac_key_refresh_time;
	int eap_teap_auth;
	int eap_teap_pac_no_inner;
	int eap_teap_separate_result;
	enum eap_teap_id {
		EAP_TEAP_ID_ALLOW_ANY = 0,
		EAP_TEAP_ID_REQUIRE_USER = 1,
		EAP_TEAP_ID_REQUIRE_MACHINE = 2,
		EAP_TEAP_ID_REQUEST_USER_ACCEPT_MACHINE = 3,
		EAP_TEAP_ID_REQUEST_MACHINE_ACCEPT_USER = 4,
		EAP_TEAP_ID_REQUIRE_USER_AND_MACHINE = 5,
	} eap_teap_id;

	/**
	 * eap_sim_aka_result_ind - EAP-SIM/AKA protected success indication
	 *
	 * This controls whether the protected success/failure indication
	 * (AT_RESULT_IND) is used with EAP-SIM and EAP-AKA.
	 */
	int eap_sim_aka_result_ind;
	int eap_sim_id;

	/**
	 * tnc - Trusted Network Connect (TNC)
	 *
	 * This controls whether TNC is enabled and will be required before the
	 * peer is allowed to connect. Note: This is only used with EAP-TTLS
	 * and EAP-FAST. If any other EAP method is enabled, the peer will be
	 * allowed to connect without TNC.
	 */
	int tnc;

	/**
	 * wps - Wi-Fi Protected Setup context
	 *
	 * If WPS is used with an external RADIUS server (which is quite
	 * unlikely configuration), this is used to provide a pointer to WPS
	 * context data. Normally, this can be set to %NULL.
	 */
	struct wps_context *wps;
	int fragment_size;

	int pbc_in_m1;

	/**
	 * server_id - Server identity
	 */
	u8 *server_id;
	size_t server_id_len;

	/**
	 * erp - Whether EAP Re-authentication Protocol (ERP) is enabled
	 *
	 * This controls whether the authentication server derives ERP key
	 * hierarchy (rRK and rIK) from full EAP authentication and allows
	 * these keys to be used to perform ERP to derive rMSK instead of full
	 * EAP authentication to derive MSK.
	 */
	int erp;
	unsigned int tls_session_lifetime;
	unsigned int tls_flags;

	unsigned int max_auth_rounds;
	unsigned int max_auth_rounds_short;
};

struct eap_session_data {
	const struct wpabuf *assoc_wps_ie;
	const struct wpabuf *assoc_p2p_ie;
	const u8 *peer_addr;
#ifdef CONFIG_TESTING_OPTIONS
	u32 tls_test_flags;
#endif /* CONFIG_TESTING_OPTIONS */
};


struct eap_sm * eap_server_sm_init(void *eapol_ctx,
				   const struct eapol_callbacks *eapol_cb,
				   const struct eap_config *conf,
				   const struct eap_session_data *sess);
void eap_server_sm_deinit(struct eap_sm *sm);
int eap_server_sm_step(struct eap_sm *sm);
void eap_sm_notify_cached(struct eap_sm *sm);
void eap_sm_pending_cb(struct eap_sm *sm);
int eap_sm_method_pending(struct eap_sm *sm);
const u8 * eap_get_identity(struct eap_sm *sm, size_t *len);
const char * eap_get_serial_num(struct eap_sm *sm);
const char * eap_get_method(struct eap_sm *sm);
const char * eap_get_imsi(struct eap_sm *sm);
struct eap_eapol_interface * eap_get_interface(struct eap_sm *sm);
void eap_server_clear_identity(struct eap_sm *sm);
void eap_server_mschap_rx_callback(struct eap_sm *sm, const char *source,
				   const u8 *username, size_t username_len,
				   const u8 *challenge, const u8 *response);
void eap_erp_update_identity(struct eap_sm *sm, const u8 *eap, size_t len);
void eap_user_free(struct eap_user *user);
void eap_server_config_free(struct eap_config *cfg);

#endif /* EAP_H */
