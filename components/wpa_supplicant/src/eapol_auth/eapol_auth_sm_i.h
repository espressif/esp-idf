/*
 * IEEE 802.1X-2004 Authenticator - EAPOL state machine (internal definitions)
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAPOL_AUTH_SM_I_H
#define EAPOL_AUTH_SM_I_H

#include "common/defs.h"

/* IEEE Std 802.1X-2004, Ch. 8.2 */

typedef enum { ForceUnauthorized = 1, ForceAuthorized = 3, Auto = 2 }
	PortTypes;
typedef enum { Unauthorized = 2, Authorized = 1 } PortState;
typedef enum { Both = 0, In = 1 } ControlledDirection;
typedef unsigned int Counter;


/**
 * struct eapol_authenticator - Global EAPOL authenticator data
 */
struct eapol_authenticator {
	struct eapol_auth_config conf;
	struct eapol_auth_cb cb;

	u8 *default_wep_key;
	u8 default_wep_key_idx;
};


/**
 * struct eapol_state_machine - Per-Supplicant Authenticator state machines
 */
struct eapol_state_machine {
	/* timers */
	int aWhile;
	int quietWhile;
	int reAuthWhen;

	/* global variables */
	bool authAbort;
	bool authFail;
	PortState authPortStatus;
	bool authStart;
	bool authTimeout;
	bool authSuccess;
	bool eapolEap;
	bool initialize;
	bool keyDone;
	bool keyRun;
	bool keyTxEnabled;
	PortTypes portControl;
	bool portValid;
	bool reAuthenticate;

	/* Port Timers state machine */
	/* 'bool tick' implicitly handled as registered timeout */

	/* Authenticator PAE state machine */
	enum { AUTH_PAE_INITIALIZE, AUTH_PAE_DISCONNECTED, AUTH_PAE_CONNECTING,
	       AUTH_PAE_AUTHENTICATING, AUTH_PAE_AUTHENTICATED,
	       AUTH_PAE_ABORTING, AUTH_PAE_HELD, AUTH_PAE_FORCE_AUTH,
	       AUTH_PAE_FORCE_UNAUTH, AUTH_PAE_RESTART } auth_pae_state;
	/* variables */
	bool eapolLogoff;
	bool eapolStart;
	PortTypes portMode;
	unsigned int reAuthCount;
	/* constants */
	unsigned int quietPeriod; /* default 60; 0..65535 */
#define AUTH_PAE_DEFAULT_quietPeriod 60
	unsigned int reAuthMax; /* default 2 */
#define AUTH_PAE_DEFAULT_reAuthMax 2
	/* counters */
	Counter authEntersConnecting;
	Counter authEapLogoffsWhileConnecting;
	Counter authEntersAuthenticating;
	Counter authAuthSuccessesWhileAuthenticating;
	Counter authAuthTimeoutsWhileAuthenticating;
	Counter authAuthFailWhileAuthenticating;
	Counter authAuthEapStartsWhileAuthenticating;
	Counter authAuthEapLogoffWhileAuthenticating;
	Counter authAuthReauthsWhileAuthenticated;
	Counter authAuthEapStartsWhileAuthenticated;
	Counter authAuthEapLogoffWhileAuthenticated;

	/* Backend Authentication state machine */
	enum { BE_AUTH_REQUEST, BE_AUTH_RESPONSE, BE_AUTH_SUCCESS,
	       BE_AUTH_FAIL, BE_AUTH_TIMEOUT, BE_AUTH_IDLE, BE_AUTH_INITIALIZE,
	       BE_AUTH_IGNORE
	} be_auth_state;
	/* constants */
	unsigned int serverTimeout; /* default 30; 1..X */
#define BE_AUTH_DEFAULT_serverTimeout 30
	/* counters */
	Counter backendResponses;
	Counter backendAccessChallenges;
	Counter backendOtherRequestsToSupplicant;
	Counter backendAuthSuccesses;
	Counter backendAuthFails;

	/* Reauthentication Timer state machine */
	enum { REAUTH_TIMER_INITIALIZE, REAUTH_TIMER_REAUTHENTICATE
	} reauth_timer_state;
	/* constants */
	unsigned int reAuthPeriod; /* default 3600 s */
	bool reAuthEnabled;

	/* Authenticator Key Transmit state machine */
	enum { AUTH_KEY_TX_NO_KEY_TRANSMIT, AUTH_KEY_TX_KEY_TRANSMIT
	} auth_key_tx_state;

	/* Key Receive state machine */
	enum { KEY_RX_NO_KEY_RECEIVE, KEY_RX_KEY_RECEIVE } key_rx_state;
	/* variables */
	bool rxKey;

	/* Controlled Directions state machine */
	enum { CTRL_DIR_FORCE_BOTH, CTRL_DIR_IN_OR_BOTH } ctrl_dir_state;
	/* variables */
	ControlledDirection adminControlledDirections;
	ControlledDirection operControlledDirections;
	bool operEdge;

	/* Authenticator Statistics Table */
	Counter dot1xAuthEapolFramesRx;
	Counter dot1xAuthEapolFramesTx;
	Counter dot1xAuthEapolStartFramesRx;
	Counter dot1xAuthEapolLogoffFramesRx;
	Counter dot1xAuthEapolRespIdFramesRx;
	Counter dot1xAuthEapolRespFramesRx;
	Counter dot1xAuthEapolReqIdFramesTx;
	Counter dot1xAuthEapolReqFramesTx;
	Counter dot1xAuthInvalidEapolFramesRx;
	Counter dot1xAuthEapLengthErrorFramesRx;
	Counter dot1xAuthLastEapolFrameVersion;

	/* Other variables - not defined in IEEE 802.1X */
	u8 addr[ETH_ALEN]; /* Supplicant address */
	int flags; /* EAPOL_SM_* */

	/* EAPOL/AAA <-> EAP full authenticator interface */
	struct eap_eapol_interface *eap_if;

	int radius_identifier;
	/* TODO: check when the last messages can be released */
	struct radius_msg *last_recv_radius;
	u8 last_eap_id; /* last used EAP Identifier */
	u8 *identity;
	size_t identity_len;
	u8 eap_type_authsrv; /* EAP type of the last EAP packet from
			      * Authentication server */
	u8 eap_type_supp; /* EAP type of the last EAP packet from Supplicant */
#ifndef CONFIG_NO_RADIUS
	struct wpabuf *radius_cui; /* Chargeable-User-Identity */
#endif

	struct eap_sm *eap;

	bool initializing; /* in process of initializing state machines */
	bool changed;

	struct eapol_authenticator *eapol;

	void *sta; /* station context pointer to use in callbacks */

	int remediation;

	u64 acct_multi_session_id;
};

#endif /* EAPOL_AUTH_SM_I_H */
