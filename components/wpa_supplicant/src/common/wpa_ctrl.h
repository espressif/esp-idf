/*
 * wpa_supplicant/hostapd control interface library
 * Copyright (c) 2004-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_CTRL_H
#define WPA_CTRL_H

#ifdef  __cplusplus
extern "C" {
#endif

/* wpa_supplicant control interface - fixed message prefixes */

/** Interactive request for identity/password/pin */
#define WPA_CTRL_REQ "CTRL-REQ-"

/** Response to identity/password/pin request */
#define WPA_CTRL_RSP "CTRL-RSP-"

/* Event messages with fixed prefix */
/** Authentication completed successfully and data connection enabled */
#define WPA_EVENT_CONNECTED "CTRL-EVENT-CONNECTED "
/** Disconnected, data connection is not available */
#define WPA_EVENT_DISCONNECTED "CTRL-EVENT-DISCONNECTED "
/** Association rejected during connection attempt */
#define WPA_EVENT_ASSOC_REJECT "CTRL-EVENT-ASSOC-REJECT "
/** wpa_supplicant is exiting */
#define WPA_EVENT_TERMINATING "CTRL-EVENT-TERMINATING "
/** Password change was completed successfully */
#define WPA_EVENT_PASSWORD_CHANGED "CTRL-EVENT-PASSWORD-CHANGED "
/** EAP-Request/Notification received */
#define WPA_EVENT_EAP_NOTIFICATION "CTRL-EVENT-EAP-NOTIFICATION "
/** EAP authentication started (EAP-Request/Identity received) */
#define WPA_EVENT_EAP_STARTED "CTRL-EVENT-EAP-STARTED "
/** EAP method proposed by the server */
#define WPA_EVENT_EAP_PROPOSED_METHOD "CTRL-EVENT-EAP-PROPOSED-METHOD "
/** EAP method selected */
#define WPA_EVENT_EAP_METHOD "CTRL-EVENT-EAP-METHOD "
/** EAP peer certificate from TLS */
#define WPA_EVENT_EAP_PEER_CERT "CTRL-EVENT-EAP-PEER-CERT "
/** EAP TLS certificate chain validation error */
#define WPA_EVENT_EAP_TLS_CERT_ERROR "CTRL-EVENT-EAP-TLS-CERT-ERROR "
/** EAP status */
#define WPA_EVENT_EAP_STATUS "CTRL-EVENT-EAP-STATUS "
/** EAP authentication completed successfully */
#define WPA_EVENT_EAP_SUCCESS "CTRL-EVENT-EAP-SUCCESS "
/** EAP authentication failed (EAP-Failure received) */
#define WPA_EVENT_EAP_FAILURE "CTRL-EVENT-EAP-FAILURE "
/** Network block temporarily disabled (e.g., due to authentication failure) */
#define WPA_EVENT_TEMP_DISABLED "CTRL-EVENT-SSID-TEMP-DISABLED "
/** Temporarily disabled network block re-enabled */
#define WPA_EVENT_REENABLED "CTRL-EVENT-SSID-REENABLED "
/** New scan results available */
#define WPA_EVENT_SCAN_RESULTS "CTRL-EVENT-SCAN-RESULTS "
/** wpa_supplicant state change */
#define WPA_EVENT_STATE_CHANGE "CTRL-EVENT-STATE-CHANGE "
/** A new BSS entry was added (followed by BSS entry id and BSSID) */
#define WPA_EVENT_BSS_ADDED "CTRL-EVENT-BSS-ADDED "
/** A BSS entry was removed (followed by BSS entry id and BSSID) */
#define WPA_EVENT_BSS_REMOVED "CTRL-EVENT-BSS-REMOVED "
#ifdef ANDROID_P2P
/** Notify the Userspace about the freq conflict */
#define WPA_EVENT_FREQ_CONFLICT "CTRL-EVENT-FREQ-CONFLICT "
#endif

/** RSN IBSS 4-way handshakes completed with specified peer */
#define IBSS_RSN_COMPLETED "IBSS-RSN-COMPLETED "

/** WPS overlap detected in PBC mode */
#define WPS_EVENT_OVERLAP "WPS-OVERLAP-DETECTED "
/** Available WPS AP with active PBC found in scan results */
#define WPS_EVENT_AP_AVAILABLE_PBC "WPS-AP-AVAILABLE-PBC "
/** Available WPS AP with our address as authorized in scan results */
#define WPS_EVENT_AP_AVAILABLE_AUTH "WPS-AP-AVAILABLE-AUTH "
/** Available WPS AP with recently selected PIN registrar found in scan results
 */
#define WPS_EVENT_AP_AVAILABLE_PIN "WPS-AP-AVAILABLE-PIN "
/** Available WPS AP found in scan results */
#define WPS_EVENT_AP_AVAILABLE "WPS-AP-AVAILABLE "
/** A new credential received */
#define WPS_EVENT_CRED_RECEIVED "WPS-CRED-RECEIVED "
/** M2D received */
#define WPS_EVENT_M2D "WPS-M2D "
/** WPS registration failed after M2/M2D */
#define WPS_EVENT_FAIL "WPS-FAIL "
/** WPS registration completed successfully */
#define WPS_EVENT_SUCCESS "WPS-SUCCESS "
/** WPS enrollment attempt timed out and was terminated */
#define WPS_EVENT_TIMEOUT "WPS-TIMEOUT "
/* PBC mode was activated */
#define WPS_EVENT_ACTIVE "WPS-PBC-ACTIVE "
/* PBC mode was disabled */
#define WPS_EVENT_DISABLE "WPS-PBC-DISABLE "

#define WPS_EVENT_ENROLLEE_SEEN "WPS-ENROLLEE-SEEN "

#define WPS_EVENT_OPEN_NETWORK "WPS-OPEN-NETWORK "

/* WPS ER events */
#define WPS_EVENT_ER_AP_ADD "WPS-ER-AP-ADD "
#define WPS_EVENT_ER_AP_REMOVE "WPS-ER-AP-REMOVE "
#define WPS_EVENT_ER_ENROLLEE_ADD "WPS-ER-ENROLLEE-ADD "
#define WPS_EVENT_ER_ENROLLEE_REMOVE "WPS-ER-ENROLLEE-REMOVE "
#define WPS_EVENT_ER_AP_SETTINGS "WPS-ER-AP-SETTINGS "
#define WPS_EVENT_ER_SET_SEL_REG "WPS-ER-AP-SET-SEL-REG "

/** P2P device found */
#define P2P_EVENT_DEVICE_FOUND "P2P-DEVICE-FOUND "

/** P2P device lost */
#define P2P_EVENT_DEVICE_LOST "P2P-DEVICE-LOST "

/** A P2P device requested GO negotiation, but we were not ready to start the
 * negotiation */
#define P2P_EVENT_GO_NEG_REQUEST "P2P-GO-NEG-REQUEST "
#define P2P_EVENT_GO_NEG_SUCCESS "P2P-GO-NEG-SUCCESS "
#define P2P_EVENT_GO_NEG_FAILURE "P2P-GO-NEG-FAILURE "
#define P2P_EVENT_GROUP_FORMATION_SUCCESS "P2P-GROUP-FORMATION-SUCCESS "
#define P2P_EVENT_GROUP_FORMATION_FAILURE "P2P-GROUP-FORMATION-FAILURE "
#define P2P_EVENT_GROUP_STARTED "P2P-GROUP-STARTED "
#define P2P_EVENT_GROUP_REMOVED "P2P-GROUP-REMOVED "
#define P2P_EVENT_CROSS_CONNECT_ENABLE "P2P-CROSS-CONNECT-ENABLE "
#define P2P_EVENT_CROSS_CONNECT_DISABLE "P2P-CROSS-CONNECT-DISABLE "
/* parameters: <peer address> <PIN> */
#define P2P_EVENT_PROV_DISC_SHOW_PIN "P2P-PROV-DISC-SHOW-PIN "
/* parameters: <peer address> */
#define P2P_EVENT_PROV_DISC_ENTER_PIN "P2P-PROV-DISC-ENTER-PIN "
/* parameters: <peer address> */
#define P2P_EVENT_PROV_DISC_PBC_REQ "P2P-PROV-DISC-PBC-REQ "
/* parameters: <peer address> */
#define P2P_EVENT_PROV_DISC_PBC_RESP "P2P-PROV-DISC-PBC-RESP "
/* parameters: <peer address> <status> */
#define P2P_EVENT_PROV_DISC_FAILURE "P2P-PROV-DISC-FAILURE"
/* parameters: <freq> <src addr> <dialog token> <update indicator> <TLVs> */
#define P2P_EVENT_SERV_DISC_REQ "P2P-SERV-DISC-REQ "
/* parameters: <src addr> <update indicator> <TLVs> */
#define P2P_EVENT_SERV_DISC_RESP "P2P-SERV-DISC-RESP "
#define P2P_EVENT_INVITATION_RECEIVED "P2P-INVITATION-RECEIVED "
#define P2P_EVENT_INVITATION_RESULT "P2P-INVITATION-RESULT "
#define P2P_EVENT_FIND_STOPPED "P2P-FIND-STOPPED "
#define P2P_EVENT_PERSISTENT_PSK_FAIL "P2P-PERSISTENT-PSK-FAIL id="

/* parameters: <PMF enabled> <timeout in ms> <Session Information URL> */
#define ESS_DISASSOC_IMMINENT "ESS-DISASSOC-IMMINENT "

#define INTERWORKING_AP "INTERWORKING-AP "
#define INTERWORKING_NO_MATCH "INTERWORKING-NO-MATCH "

#define GAS_RESPONSE_INFO "GAS-RESPONSE-INFO "

/* hostapd control interface - fixed message prefixes */
#define WPS_EVENT_PIN_NEEDED "WPS-PIN-NEEDED "
#define WPS_EVENT_NEW_AP_SETTINGS "WPS-NEW-AP-SETTINGS "
#define WPS_EVENT_REG_SUCCESS "WPS-REG-SUCCESS "
#define WPS_EVENT_AP_SETUP_LOCKED "WPS-AP-SETUP-LOCKED "
#define WPS_EVENT_AP_SETUP_UNLOCKED "WPS-AP-SETUP-UNLOCKED "
#define WPS_EVENT_AP_PIN_ENABLED "WPS-AP-PIN-ENABLED "
#define WPS_EVENT_AP_PIN_DISABLED "WPS-AP-PIN-DISABLED "
#define AP_STA_CONNECTED "AP-STA-CONNECTED "
#define AP_STA_DISCONNECTED "AP-STA-DISCONNECTED "

#define AP_REJECTED_MAX_STA "AP-REJECTED-MAX-STA "
#define AP_REJECTED_BLOCKED_STA "AP-REJECTED-BLOCKED-STA "

/* BSS command information masks */

#define WPA_BSS_MASK_ALL		0xFFFDFFFF
#define WPA_BSS_MASK_ID			BIT(0)
#define WPA_BSS_MASK_BSSID		BIT(1)
#define WPA_BSS_MASK_FREQ		BIT(2)
#define WPA_BSS_MASK_BEACON_INT		BIT(3)
#define WPA_BSS_MASK_CAPABILITIES	BIT(4)
#define WPA_BSS_MASK_QUAL		BIT(5)
#define WPA_BSS_MASK_NOISE		BIT(6)
#define WPA_BSS_MASK_LEVEL		BIT(7)
#define WPA_BSS_MASK_TSF		BIT(8)
#define WPA_BSS_MASK_AGE		BIT(9)
#define WPA_BSS_MASK_IE			BIT(10)
#define WPA_BSS_MASK_FLAGS		BIT(11)
#define WPA_BSS_MASK_SSID		BIT(12)
#define WPA_BSS_MASK_WPS_SCAN		BIT(13)
#define WPA_BSS_MASK_P2P_SCAN		BIT(14)
#define WPA_BSS_MASK_INTERNETW		BIT(15)
#define WPA_BSS_MASK_WIFI_DISPLAY	BIT(16)
#define WPA_BSS_MASK_DELIM		BIT(17)


#ifdef  __cplusplus
}
#endif

#endif /* WPA_CTRL_H */
