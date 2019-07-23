/*
 * Wi-Fi Protected Setup
 * Copyright (c) 2007-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPS_H
#define WPS_H

#include "rom/ets_sys.h"
#include "wps_defs.h"
#include "esp_wifi_types.h"

/**
 * enum wsc_op_code - EAP-WSC OP-Code values
 */
enum wsc_op_code {
	WSC_UPnP = 0 /* No OP Code in UPnP transport */,
	WSC_Start = 0x01,
	WSC_ACK = 0x02,
	WSC_NACK = 0x03,
	WSC_MSG = 0x04,
	WSC_Done = 0x05,
	WSC_FRAG_ACK = 0x06
};

struct wps_registrar;
//struct upnp_wps_device_sm;
struct wps_er;
struct wps_parse_attr;

/**
 * struct wps_credential - WPS Credential
 * @ssid: SSID
 * @ssid_len: Length of SSID
 * @auth_type: Authentication Type (WPS_WIFI_AUTH_OPEN, .. flags)
 * @encr_type: Encryption Type (WPS_ENCR_NONE, .. flags)
 * @key_idx: Key index
 * @key: Key
 * @key_len: Key length in octets
 * @mac_addr: MAC address of the Credential receiver
 * @cred_attr: Unparsed Credential attribute data (used only in cred_cb());
 *	this may be %NULL, if not used
 * @cred_attr_len: Length of cred_attr in octets
 * @ap_channel: AP channel
 */
struct wps_credential {
	u8 ssid[32];
	size_t ssid_len;
	u16 auth_type;
	u16 encr_type;
	u8 key_idx;
	u8 key[64];
	size_t key_len;
	u8 mac_addr[ETH_ALEN];
	const u8 *cred_attr;
	size_t cred_attr_len;
	u16 ap_channel;
};

#define WPS_DEV_TYPE_LEN 8
#define WPS_DEV_TYPE_BUFSIZE 21
#define WPS_SEC_DEV_TYPE_MAX_LEN 128
/* maximum number of advertised WPS vendor extension attributes */
#define MAX_WPS_VENDOR_EXTENSIONS 10
/* maximum size of WPS Vendor extension attribute */
#define WPS_MAX_VENDOR_EXT_LEN 1024
/* maximum number of parsed WPS vendor extension attributes */
#define MAX_WPS_PARSE_VENDOR_EXT 10

/**
 * struct wps_device_data - WPS Device Data
 * @mac_addr: Device MAC address
 * @device_name: Device Name (0..32 octets encoded in UTF-8)
 * @manufacturer: Manufacturer (0..64 octets encoded in UTF-8)
 * @model_name: Model Name (0..32 octets encoded in UTF-8)
 * @model_number: Model Number (0..32 octets encoded in UTF-8)
 * @serial_number: Serial Number (0..32 octets encoded in UTF-8)
 * @pri_dev_type: Primary Device Type
 * @sec_dev_type: Array of secondary device types
 * @num_sec_dev_type: Number of secondary device types
 * @os_version: OS Version
 * @rf_bands: RF bands (WPS_RF_24GHZ, WPS_RF_50GHZ flags)
 * @p2p: Whether the device is a P2P device
 */
struct wps_device_data {
	u8 mac_addr[ETH_ALEN];
	char *device_name;
	char *manufacturer;
	char *model_name;
	char *model_number;
	char *serial_number;
	u8 pri_dev_type[WPS_DEV_TYPE_LEN];
#define WPS_SEC_DEVICE_TYPES 5
	u8 sec_dev_type[WPS_SEC_DEVICE_TYPES][WPS_DEV_TYPE_LEN];
	u8 num_sec_dev_types;
	u32 os_version;
	u8 rf_bands;
	u16 config_methods;
	struct wpabuf *vendor_ext_m1;
	struct wpabuf *vendor_ext[MAX_WPS_VENDOR_EXTENSIONS];

	int p2p;
};

/**
 * struct wps_config - WPS configuration for a single registration protocol run
 */
struct wps_config {
	/**
	 * wps - Pointer to long term WPS context
	 */
	struct wps_context *wps;

	/**
	 * registrar - Whether this end is a Registrar
	 */
	int registrar;

	/**
	 * pin - Enrollee Device Password (%NULL for Registrar or PBC)
	 */
	const u8 *pin;

	/**
	 * pin_len - Length on pin in octets
	 */
	size_t pin_len;

	/**
	 * pbc - Whether this is protocol run uses PBC
	 */
	int pbc;

	/**
	 * assoc_wps_ie: (Re)AssocReq WPS IE (in AP; %NULL if not AP)
	 */
	const struct wpabuf *assoc_wps_ie;

	/**
	 * new_ap_settings - New AP settings (%NULL if not used)
	 *
	 * This parameter provides new AP settings when using a wireless
	 * stations as a Registrar to configure the AP. %NULL means that AP
	 * will not be reconfigured, i.e., the station will only learn the
	 * current AP settings by using AP PIN.
	 */
	const struct wps_credential *new_ap_settings;

	/**
	 * peer_addr: MAC address of the peer in AP; %NULL if not AP
	 */
	const u8 *peer_addr;

	/**
	 * use_psk_key - Use PSK format key in Credential
	 *
	 * Force PSK format to be used instead of ASCII passphrase when
	 * building Credential for an Enrollee. The PSK value is set in
	 * struct wpa_context::psk.
	 */
	int use_psk_key;

	/**
	 * dev_pw_id - Device Password ID for Enrollee when PIN is used
	 */
	u16 dev_pw_id;

	/**
	 * p2p_dev_addr - P2P Device Address from (Re)Association Request
	 *
	 * On AP/GO, this is set to the P2P Device Address of the associating
	 * P2P client if a P2P IE is included in the (Re)Association Request
	 * frame and the P2P Device Address is included. Otherwise, this is set
	 * to %NULL to indicate the station does not have a P2P Device Address.
	 */
	const u8 *p2p_dev_addr;

	/**
	 * pbc_in_m1 - Do not remove PushButton config method in M1 (AP)
	 *
	 * This can be used to enable a workaround to allow Windows 7 to use
	 * PBC with the AP.
	 */
	int pbc_in_m1;
};

/* Bssid of the discard AP which is discarded for not select reg or other reason */
struct discard_ap_list_t{
	u8 bssid[6];
};

//struct wps_data * wps_init(const struct wps_config *cfg);
struct wps_data * wps_init(void);

//void wps_deinit(struct wps_data *data);
void wps_deinit(void);

/**
 * enum wps_process_res - WPS message processing result
 */
enum wps_process_res {
	/**
	 * WPS_DONE - Processing done
	 */
	WPS_DONE,

	/**
	 * WPS_CONTINUE - Processing continues
	 */
	WPS_CONTINUE,

	/**
	 * WPS_FAILURE - Processing failed
	 */
	WPS_FAILURE,

	/**
	 * WPS_PENDING - Processing continues, but waiting for an external
	 *	event (e.g., UPnP message from an external Registrar)
	 */
	WPS_PENDING,
	WPS_IGNORE,     /* snake, ignore the re-packge */

	WPS_FRAGMENT    /* Tim, send wsc fragment ack */
};
enum wps_process_res wps_process_msg(struct wps_data *wps,
				     enum wsc_op_code op_code,
				     const struct wpabuf *msg);

struct wpabuf * wps_get_msg(struct wps_data *wps, enum wsc_op_code *op_code);

int wps_is_selected_pbc_registrar(const struct wpabuf *msg, u8 *bssid);
int wps_is_selected_pin_registrar(const struct wpabuf *msg, u8 *bssid);
int wps_ap_priority_compar(const struct wpabuf *wps_a,
			   const struct wpabuf *wps_b);
int wps_is_addr_authorized(const struct wpabuf *msg, const u8 *addr,
			   int ver1_compat);
const u8 * wps_get_uuid_e(const struct wpabuf *msg);
int wps_is_20(const struct wpabuf *msg);

struct wpabuf * wps_build_assoc_req_ie(enum wps_request_type req_type);
struct wpabuf * wps_build_assoc_resp_ie(void);
struct wpabuf * wps_build_probe_req_ie(u16 pw_id, struct wps_device_data *dev,
				       const u8 *uuid,
				       enum wps_request_type req_type,
				       unsigned int num_req_dev_types,
				       const u8 *req_dev_types);


/**
 * struct wps_registrar_config - WPS Registrar configuration
 */
struct wps_registrar_config {
	/**
	 * new_psk_cb - Callback for new PSK
	 * @ctx: Higher layer context data (cb_ctx)
	 * @mac_addr: MAC address of the Enrollee
	 * @psk: The new PSK
	 * @psk_len: The length of psk in octets
	 * Returns: 0 on success, -1 on failure
	 *
	 * This callback is called when a new per-device PSK is provisioned.
	 */
	int (*new_psk_cb)(void *ctx, const u8 *mac_addr, const u8 *psk,
			  size_t psk_len);

	/**
	 * set_ie_cb - Callback for WPS IE changes
	 * @ctx: Higher layer context data (cb_ctx)
	 * @beacon_ie: WPS IE for Beacon
	 * @probe_resp_ie: WPS IE for Probe Response
	 * Returns: 0 on success, -1 on failure
	 *
	 * This callback is called whenever the WPS IE in Beacon or Probe
	 * Response frames needs to be changed (AP only). Callee is responsible
	 * for freeing the buffers.
	 */
	int (*set_ie_cb)(void *ctx, struct wpabuf *beacon_ie,
			 struct wpabuf *probe_resp_ie);

	/**
	 * pin_needed_cb - Callback for requesting a PIN
	 * @ctx: Higher layer context data (cb_ctx)
	 * @uuid_e: UUID-E of the unknown Enrollee
	 * @dev: Device Data from the unknown Enrollee
	 *
	 * This callback is called whenever an unknown Enrollee requests to use
	 * PIN method and a matching PIN (Device Password) is not found in
	 * Registrar data.
	 */
	void (*pin_needed_cb)(void *ctx, const u8 *uuid_e,
			      const struct wps_device_data *dev);

	/**
	 * reg_success_cb - Callback for reporting successful registration
	 * @ctx: Higher layer context data (cb_ctx)
	 * @mac_addr: MAC address of the Enrollee
	 * @uuid_e: UUID-E of the Enrollee
	 * @dev_pw: Device Password (PIN) used during registration
	 * @dev_pw_len: Length of dev_pw in octets
	 *
	 * This callback is called whenever an Enrollee completes registration
	 * successfully.
	 */
	void (*reg_success_cb)(void *ctx, const u8 *mac_addr,
			       const u8 *uuid_e, const u8 *dev_pw,
			       size_t dev_pw_len);

	/**
	 * set_sel_reg_cb - Callback for reporting selected registrar changes
	 * @ctx: Higher layer context data (cb_ctx)
	 * @sel_reg: Whether the Registrar is selected
	 * @dev_passwd_id: Device Password ID to indicate with method or
	 *	specific password the Registrar intends to use
	 * @sel_reg_config_methods: Bit field of active config methods
	 *
	 * This callback is called whenever the Selected Registrar state
	 * changes (e.g., a new PIN becomes available or PBC is invoked). This
	 * callback is only used by External Registrar implementation;
	 * set_ie_cb() is used by AP implementation in similar caes, but it
	 * provides the full WPS IE data instead of just the minimal Registrar
	 * state information.
	 */
	void (*set_sel_reg_cb)(void *ctx, int sel_reg, u16 dev_passwd_id,
			       u16 sel_reg_config_methods);

	/**
	 * enrollee_seen_cb - Callback for reporting Enrollee based on ProbeReq
	 * @ctx: Higher layer context data (cb_ctx)
	 * @addr: MAC address of the Enrollee
	 * @uuid_e: UUID of the Enrollee
	 * @pri_dev_type: Primary device type
	 * @config_methods: Config Methods
	 * @dev_password_id: Device Password ID
	 * @request_type: Request Type
	 * @dev_name: Device Name (if available)
	 */
	void (*enrollee_seen_cb)(void *ctx, const u8 *addr, const u8 *uuid_e,
				 const u8 *pri_dev_type, u16 config_methods,
				 u16 dev_password_id, u8 request_type,
				 const char *dev_name);

	/**
	 * cb_ctx: Higher layer context data for Registrar callbacks
	 */
	void *cb_ctx;

	/**
	 * skip_cred_build: Do not build credential
	 *
	 * This option can be used to disable internal code that builds
	 * Credential attribute into M8 based on the current network
	 * configuration and Enrollee capabilities. The extra_cred data will
	 * then be used as the Credential(s).
	 */
	int skip_cred_build;

	/**
	 * extra_cred: Additional Credential attribute(s)
	 *
	 * This optional data (set to %NULL to disable) can be used to add
	 * Credential attribute(s) for other networks into M8. If
	 * skip_cred_build is set, this will also override the automatically
	 * generated Credential attribute.
	 */
	const u8 *extra_cred;

	/**
	 * extra_cred_len: Length of extra_cred in octets
	 */
	size_t extra_cred_len;

	/**
	 * disable_auto_conf - Disable auto-configuration on first registration
	 *
	 * By default, the AP that is started in not configured state will
	 * generate a random PSK and move to configured state when the first
	 * registration protocol run is completed successfully. This option can
	 * be used to disable this functionality and leave it up to an external
	 * program to take care of configuration. This requires the extra_cred
	 * to be set with a suitable Credential and skip_cred_build being used.
	 */
	int disable_auto_conf;

	/**
	 * static_wep_only - Whether the BSS supports only static WEP
	 */
	int static_wep_only;

	/**
	 * dualband - Whether this is a concurrent dualband AP
	 */
	int dualband;
};


/**
 * enum wps_event - WPS event types
 */
enum wps_event {
	/**
	 * WPS_EV_M2D - M2D received (Registrar did not know us)
	 */
	WPS_EV_M2D,

	/**
	 * WPS_EV_FAIL - Registration failed
	 */
	WPS_EV_FAIL,

	/**
	 * WPS_EV_SUCCESS - Registration succeeded
	 */
	WPS_EV_SUCCESS,

	/**
	 * WPS_EV_PWD_AUTH_FAIL - Password authentication failed
	 */
	WPS_EV_PWD_AUTH_FAIL,

	/**
	 * WPS_EV_PBC_OVERLAP - PBC session overlap detected
	 */
	WPS_EV_PBC_OVERLAP,

	/**
	 * WPS_EV_PBC_TIMEOUT - PBC walktime expired before protocol run start
	 */
	WPS_EV_PBC_TIMEOUT,

	/**
	 * WPS_EV_ER_AP_ADD - ER: AP added
	 */
	WPS_EV_ER_AP_ADD,

	/**
	 * WPS_EV_ER_AP_REMOVE - ER: AP removed
	 */
	WPS_EV_ER_AP_REMOVE,

	/**
	 * WPS_EV_ER_ENROLLEE_ADD - ER: Enrollee added
	 */
	WPS_EV_ER_ENROLLEE_ADD,

	/**
	 * WPS_EV_ER_ENROLLEE_REMOVE - ER: Enrollee removed
	 */
	WPS_EV_ER_ENROLLEE_REMOVE,

	/**
	 * WPS_EV_ER_AP_SETTINGS - ER: AP Settings learned
	 */
	WPS_EV_ER_AP_SETTINGS,

	/**
	 * WPS_EV_ER_SET_SELECTED_REGISTRAR - ER: SetSelectedRegistrar event
	 */
	WPS_EV_ER_SET_SELECTED_REGISTRAR,

	/**
	 * WPS_EV_AP_PIN_SUCCESS - External Registrar used correct AP PIN
	 */
	WPS_EV_AP_PIN_SUCCESS
};

/**
 * union wps_event_data - WPS event data
 */
union wps_event_data {
	/**
	 * struct wps_event_m2d - M2D event data
	 */
	struct wps_event_m2d {
		u16 config_methods;
		const u8 *manufacturer;
		size_t manufacturer_len;
		const u8 *model_name;
		size_t model_name_len;
		const u8 *model_number;
		size_t model_number_len;
		const u8 *serial_number;
		size_t serial_number_len;
		const u8 *dev_name;
		size_t dev_name_len;
		const u8 *primary_dev_type; /* 8 octets */
		u16 config_error;
		u16 dev_password_id;
	} m2d;

	/**
	 * struct wps_event_fail - Registration failure information
	 * @msg: enum wps_msg_type
	 */
	struct wps_event_fail {
		int msg;
		u16 config_error;
		u16 error_indication;
	} fail;

	struct wps_event_pwd_auth_fail {
		int enrollee;
		int part;
	} pwd_auth_fail;

	struct wps_event_er_ap {
		const u8 *uuid;
		const u8 *mac_addr;
		const char *friendly_name;
		const char *manufacturer;
		const char *manufacturer_url;
		const char *model_description;
		const char *model_name;
		const char *model_number;
		const char *model_url;
		const char *serial_number;
		const char *upc;
		const u8 *pri_dev_type;
		u8 wps_state;
	} ap;

	struct wps_event_er_enrollee {
		const u8 *uuid;
		const u8 *mac_addr;
		int m1_received;
		u16 config_methods;
		u16 dev_passwd_id;
		const u8 *pri_dev_type;
		const char *dev_name;
		const char *manufacturer;
		const char *model_name;
		const char *model_number;
		const char *serial_number;
	} enrollee;

	struct wps_event_er_ap_settings {
		const u8 *uuid;
		const struct wps_credential *cred;
	} ap_settings;

	struct wps_event_er_set_selected_registrar {
		const u8 *uuid;
		int sel_reg;
		u16 dev_passwd_id;
		u16 sel_reg_config_methods;
		enum {
			WPS_ER_SET_SEL_REG_START,
			WPS_ER_SET_SEL_REG_DONE,
			WPS_ER_SET_SEL_REG_FAILED
		} state;
	} set_sel_reg;
};
#ifdef CONFIG_WPS_UPNP
/**
 * struct upnp_pending_message - Pending PutWLANResponse messages
 * @next: Pointer to next pending message or %NULL
 * @addr: NewWLANEventMAC
 * @msg: NewMessage
 * @type: Message Type
 */
struct upnp_pending_message {
	struct upnp_pending_message *next;
	u8 addr[ETH_ALEN];
	struct wpabuf *msg;
	enum wps_msg_type type;
};
void wps_free_pending_msgs(struct upnp_pending_message *msgs);
#endif
/**
 * struct wps_context - Long term WPS context data
 *
 * This data is stored at the higher layer Authenticator or Supplicant data
 * structures and it is maintained over multiple registration protocol runs.
 */
struct wps_context {
	/**
	 * ap - Whether the local end is an access point
	 */
	int ap;

	/**
	 * registrar - Pointer to WPS registrar data from wps_registrar_init()
	 */
	struct wps_registrar *registrar;

	/**
	 * wps_state - Current WPS state
	 */
	enum wps_state wps_state;

	/**
	 * ap_setup_locked - Whether AP setup is locked (only used at AP)
	 */
	int ap_setup_locked;

	/**
	 * uuid - Own UUID
	 */
	u8 uuid[16];

	/**
	 * ssid - SSID
	 *
	 * This SSID is used by the Registrar to fill in information for
	 * Credentials. In addition, AP uses it when acting as an Enrollee to
	 * notify Registrar of the current configuration.
	 */
	u8 ssid[32];

	/**
	 * ssid_len - Length of ssid in octets
	 */
	size_t ssid_len;

	/**
	 * dev - Own WPS device data
	 */
	struct wps_device_data dev;

	/**
	 * dh_ctx - Context data for Diffie-Hellman operation
	 */
	void *dh_ctx;

	/**
	 * dh_privkey - Diffie-Hellman private key
	 */
	struct wpabuf *dh_privkey;

	/**
	 * dh_pubkey_oob - Diffie-Hellman public key
	 */
	struct wpabuf *dh_pubkey;

	/**
	 * config_methods - Enabled configuration methods
	 *
	 * Bit field of WPS_CONFIG_*
	 */
	u16 config_methods;

	/**
	 * encr_types - Enabled encryption types (bit field of WPS_ENCR_*)
	 */
	u16 encr_types;

	/**
	 * auth_types - Authentication types (bit field of WPS_AUTH_*)
	 */
	u16 auth_types;

	/**
	 * network_key - The current Network Key (PSK) or %NULL to generate new
	 *
	 * If %NULL, Registrar will generate per-device PSK. In addition, AP
	 * uses this when acting as an Enrollee to notify Registrar of the
	 * current configuration.
	 *
	 * When using WPA/WPA2-Person, this key can be either the ASCII
	 * passphrase (8..63 characters) or the 32-octet PSK (64 hex
	 * characters). When this is set to the ASCII passphrase, the PSK can
	 * be provided in the psk buffer and used per-Enrollee to control which
	 * key type is included in the Credential (e.g., to reduce calculation
	 * need on low-powered devices by provisioning PSK while still allowing
	 * other devices to get the passphrase).
	 */
	u8 *network_key;

	/**
	 * network_key_len - Length of network_key in octets
	 */
	size_t network_key_len;

	/**
	 * psk - The current network PSK
	 *
	 * This optional value can be used to provide the current PSK if
	 * network_key is set to the ASCII passphrase.
	 */
	u8 psk[32];

	/**
	 * psk_set - Whether psk value is set
	 */
	int psk_set;

	/**
	 * ap_settings - AP Settings override for M7 (only used at AP)
	 *
	 * If %NULL, AP Settings attributes will be generated based on the
	 * current network configuration.
	 */
	u8 *ap_settings;

	/**
	 * ap_settings_len - Length of ap_settings in octets
	 */
	size_t ap_settings_len;

	/**
	 * friendly_name - Friendly Name (required for UPnP)
	 */
	char *friendly_name;

	/**
	 * manufacturer_url - Manufacturer URL (optional for UPnP)
	 */
	char *manufacturer_url;

	/**
	 * model_description - Model Description (recommended for UPnP)
	 */
	char *model_description;

	/**
	 * model_url - Model URL (optional for UPnP)
	 */
	char *model_url;

	/**
	 * upc - Universal Product Code (optional for UPnP)
	 */
	char *upc;

	/**
	 * cred_cb - Callback to notify that new Credentials were received
	 * @ctx: Higher layer context data (cb_ctx)
	 * @cred: The received Credential
	 * Return: 0 on success, -1 on failure
	 */
	int (*cred_cb)(void *ctx, const struct wps_credential *cred);

	/**
	 * event_cb - Event callback (state information about progress)
	 * @ctx: Higher layer context data (cb_ctx)
	 * @event: Event type
	 * @data: Event data
	 */
	void (*event_cb)(void *ctx, enum wps_event event,
			 union wps_event_data *data);

	/**
	 * cb_ctx: Higher layer context data for callbacks
	 */
	void *cb_ctx;

	//struct upnp_wps_device_sm *wps_upnp;

	/* Pending messages from UPnP PutWLANResponse */
	//struct upnp_pending_message *upnp_msgs;
#ifdef CONFIG_WPS_NFC

	u16 ap_nfc_dev_pw_id;
	struct wpabuf *ap_nfc_dh_pubkey;
	struct wpabuf *ap_nfc_dh_privkey;
	struct wpabuf *ap_nfc_dev_pw;
#endif
};

struct wps_registrar *
wps_registrar_init(struct wps_context *wps,
		   const struct wps_registrar_config *cfg);
void wps_registrar_deinit(struct wps_registrar *reg);
#ifdef CONFIG_WPS_PIN

int wps_registrar_add_pin(struct wps_registrar *reg, const u8 *addr,
			  const u8 *uuid, const u8 *pin, size_t pin_len,
			  int timeout);
int wps_registrar_invalidate_pin(struct wps_registrar *reg, const u8 *uuid);
int wps_registrar_unlock_pin(struct wps_registrar *reg, const u8 *uuid);
#endif
int wps_registrar_wps_cancel(struct wps_registrar *reg);

int wps_registrar_button_pushed(struct wps_registrar *reg,
				const u8 *p2p_dev_addr);
void wps_registrar_complete(struct wps_registrar *registrar, const u8 *uuid_e,
			    const u8 *dev_pw, size_t dev_pw_len);
void wps_registrar_probe_req_rx(struct wps_registrar *reg, const u8 *addr,
				const struct wpabuf *wps_data,
				int p2p_wildcard);
int wps_registrar_update_ie(struct wps_registrar *reg);
int wps_registrar_get_info(struct wps_registrar *reg, const u8 *addr,
			   char *buf, size_t buflen);
int wps_registrar_config_ap(struct wps_registrar *reg,
			    struct wps_credential *cred);
#ifdef CONFIG_WPS_NFC

int wps_registrar_add_nfc_pw_token(struct wps_registrar *reg,
				   const u8 *pubkey_hash, u16 pw_id,
				   const u8 *dev_pw, size_t dev_pw_len);
int wps_registrar_add_nfc_password_token(struct wps_registrar *reg,
					 const u8 *oob_dev_pw,
					 size_t oob_dev_pw_len);
#endif
int wps_build_credential_wrap(struct wpabuf *msg,
			      const struct wps_credential *cred);
#ifdef CONFIG_WPS_PIN

unsigned int wps_pin_checksum(unsigned int pin);
unsigned int wps_pin_valid(unsigned int pin);
int wps_pin_str_valid(const char *pin);
#endif
unsigned int wps_generate_pin(void);

#ifdef CONFIG_WPS_OOB

struct wpabuf * wps_get_oob_cred(struct wps_context *wps);
int wps_oob_use_cred(struct wps_context *wps, struct wps_parse_attr *attr);
#endif
int wps_attr_text(struct wpabuf *data, char *buf, char *end);

struct wps_er * wps_er_init(struct wps_context *wps, const char *ifname,
			    const char *filter);
void wps_er_refresh(struct wps_er *er);
void wps_er_deinit(struct wps_er *er, void (*cb)(void *ctx), void *ctx);
void wps_er_set_sel_reg(struct wps_er *er, int sel_reg, u16 dev_passwd_id,
			u16 sel_reg_config_methods);
int wps_er_pbc(struct wps_er *er, const u8 *uuid);
int wps_er_learn(struct wps_er *er, const u8 *uuid, const u8 *pin,
		 size_t pin_len);
int wps_er_set_config(struct wps_er *er, const u8 *uuid,
		      const struct wps_credential *cred);
int wps_er_config(struct wps_er *er, const u8 *uuid, const u8 *pin,
		  size_t pin_len, const struct wps_credential *cred);
#ifdef CONFIG_WPS_NFC

struct wpabuf * wps_er_nfc_config_token(struct wps_er *er, const u8 *uuid);

#endif

int wps_dev_type_str2bin(const char *str, u8 dev_type[WPS_DEV_TYPE_LEN]);
char * wps_dev_type_bin2str(const u8 dev_type[WPS_DEV_TYPE_LEN], char *buf,
			    size_t buf_len);
void uuid_gen_mac_addr(const u8 *mac_addr, u8 *uuid);
u16 wps_config_methods_str2bin(const char *str);

#ifdef CONFIG_WPS_NFC

struct wpabuf * wps_build_nfc_pw_token(u16 dev_pw_id,
				       const struct wpabuf *pubkey,
				       const struct wpabuf *dev_pw);
struct wpabuf * wps_nfc_token_gen(int ndef, int *id, struct wpabuf **pubkey,
				  struct wpabuf **privkey,
				  struct wpabuf **dev_pw);
#endif

/* ndef.c */
struct wpabuf * ndef_parse_wifi(const struct wpabuf *buf);
struct wpabuf * ndef_build_wifi(const struct wpabuf *buf);
struct wpabuf * ndef_build_wifi_hr(void);

#ifdef CONFIG_WPS_STRICT
int wps_validate_beacon(const struct wpabuf *wps_ie);
int wps_validate_beacon_probe_resp(const struct wpabuf *wps_ie, int probe,
				   const u8 *addr);
int wps_validate_probe_req(const struct wpabuf *wps_ie, const u8 *addr);
int wps_validate_assoc_req(const struct wpabuf *wps_ie);
int wps_validate_assoc_resp(const struct wpabuf *wps_ie);
int wps_validate_m1(const struct wpabuf *tlvs);
int wps_validate_m2(const struct wpabuf *tlvs);
int wps_validate_m2d(const struct wpabuf *tlvs);
int wps_validate_m3(const struct wpabuf *tlvs);
int wps_validate_m4(const struct wpabuf *tlvs);
int wps_validate_m4_encr(const struct wpabuf *tlvs, int wps2);
int wps_validate_m5(const struct wpabuf *tlvs);
int wps_validate_m5_encr(const struct wpabuf *tlvs, int wps2);
int wps_validate_m6(const struct wpabuf *tlvs);
int wps_validate_m6_encr(const struct wpabuf *tlvs, int wps2);
int wps_validate_m7(const struct wpabuf *tlvs);
int wps_validate_m7_encr(const struct wpabuf *tlvs, int ap, int wps2);
int wps_validate_m8(const struct wpabuf *tlvs);
int wps_validate_m8_encr(const struct wpabuf *tlvs, int ap, int wps2);
int wps_validate_wsc_ack(const struct wpabuf *tlvs);
int wps_validate_wsc_nack(const struct wpabuf *tlvs);
int wps_validate_wsc_done(const struct wpabuf *tlvs);
int wps_validate_upnp_set_selected_registrar(const struct wpabuf *tlvs);
#else /* CONFIG_WPS_STRICT */
static inline int wps_validate_beacon(const struct wpabuf *wps_ie){
	return 0;
}

static inline int wps_validate_beacon_probe_resp(const struct wpabuf *wps_ie,
						 int probe, const u8 *addr)
{
	return 0;
}

static inline int wps_validate_probe_req(const struct wpabuf *wps_ie,
					 const u8 *addr)
{
	return 0;
}

static inline int wps_validate_assoc_req(const struct wpabuf *wps_ie)
{
	return 0;
}

static inline int wps_validate_assoc_resp(const struct wpabuf *wps_ie)
{
	return 0;
}

static inline int wps_validate_m1(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m2(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m2d(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m3(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m4(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m4_encr(const struct wpabuf *tlvs, int wps2)
{
	return 0;
}

static inline int wps_validate_m5(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m5_encr(const struct wpabuf *tlvs, int wps2)
{
	return 0;
}

static inline int wps_validate_m6(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m6_encr(const struct wpabuf *tlvs, int wps2)
{
	return 0;
}

static inline int wps_validate_m7(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m7_encr(const struct wpabuf *tlvs, int ap,
				       int wps2)
{
	return 0;
}

static inline int wps_validate_m8(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_m8_encr(const struct wpabuf *tlvs, int ap,
				       int wps2)
{
	return 0;
}

static inline int wps_validate_wsc_ack(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_wsc_nack(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_wsc_done(const struct wpabuf *tlvs)
{
	return 0;
}

static inline int wps_validate_upnp_set_selected_registrar(
	const struct wpabuf *tlvs)
{
	return 0;
}
#endif /* CONFIG_WPS_STRICT */

enum wps_cb_status {
	WPS_CB_ST_SUCCESS = 0,
	WPS_CB_ST_FAILED,
	WPS_CB_ST_TIMEOUT,
	WPS_CB_ST_WEP,
	WPS_CB_ST_SCAN_ERR,
};

typedef void (*wps_st_cb_t)(int status);

#ifdef USE_WPS_TASK
#define SIG_WPS_START	2
#define	SIG_WPS_RX	3
#define	SIG_WPS_NUM	9
#endif

#define WPS_EAP_EXT_VENDOR_TYPE "WFA-SimpleConfig-Enrollee-1-0"
#define WPS_OUTBUF_SIZE 500
struct wps_sm {
    struct wps_config *wps_cfg;
    struct wps_context *wps_ctx;
    struct wps_data *wps;
    char identity[32];
    u8 identity_len;
    u8 ownaddr[ETH_ALEN];
    u8 bssid[ETH_ALEN];
    u8 ssid[32];
    u8 ssid_len;
    struct wps_device_data *dev;
    u8 uuid[16];
    u8 eapol_version;
    char key[64];
    u8 key_len;
    ETSTimer wps_timeout_timer;
    ETSTimer wps_msg_timeout_timer;
    ETSTimer wps_scan_timer;
    ETSTimer wps_success_cb_timer;
    ETSTimer wps_eapol_start_timer;
    wps_st_cb_t st_cb;
    u8 current_identifier;
    bool is_wps_scan;
    u8 channel;
    u8 scan_cnt;
#ifdef USE_WPS_TASK
    u8 wps_sig_cnt[SIG_WPS_NUM];
#endif
    u8 discover_ssid_cnt;
    bool ignore_sel_reg;
    struct discard_ap_list_t dis_ap_list[WPS_MAX_DIS_AP_NUM];
    u8 discard_ap_cnt;
    wifi_sta_config_t config;
};

#define    IEEE80211_CAPINFO_PRIVACY        0x0010

struct wps_sm *wps_sm_get(void);
int wps_ssid_save(u8 *ssid, u8 ssid_len);
int wps_key_save(char *key, u8 key_len);
int wps_station_wps_register_cb(wps_st_cb_t cb);
int wps_station_wps_unregister_cb(void); 
int wps_start_pending(void);
int wps_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);

int wps_dev_deinit(struct wps_device_data *dev);
#endif /* WPS_H */
