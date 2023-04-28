/*
 * hostapd / WPS integration
 * Copyright (c) 2008-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPS_HOSTAPD_H
#define WPS_HOSTAPD_H

#ifdef CONFIG_WPS

int hostapd_init_wps(struct hostapd_data *hapd, struct wps_data *wps_data, struct wps_context *wps);
int hostapd_init_wps_complete(struct hostapd_data *hapd);
void hostapd_deinit_wps(struct hostapd_data *hapd);
void hostapd_update_wps(struct hostapd_data *hapd);
void hostapd_wps_eap_completed(struct hostapd_data *hapd);
int hostapd_wps_add_pin(struct hostapd_data *hapd, const u8 *pin);
int hostapd_wps_button_pushed(struct hostapd_data *hapd,
			      const u8 *p2p_dev_addr);
int hostapd_wps_cancel(struct hostapd_data *hapd);
int hostapd_wps_get_mib_sta(struct hostapd_data *hapd, const u8 *addr,
			    char *buf, size_t buflen);
void hostapd_wps_ap_pin_disable(struct hostapd_data *hapd);
const char * hostapd_wps_ap_pin_random(struct hostapd_data *hapd, int timeout);
const char * hostapd_wps_ap_pin_get(struct hostapd_data *hapd);
int hostapd_wps_ap_pin_set(struct hostapd_data *hapd, const char *pin,
			   int timeout);
void hostapd_wps_update_ie(struct hostapd_data *hapd);
int hostapd_wps_config_ap(struct hostapd_data *hapd, struct wps_data *wps_data);
int hostapd_wps_nfc_tag_read(struct hostapd_data *hapd,
			     const struct wpabuf *data);
#else /* CONFIG_WPS */

static inline int hostapd_init_wps(struct hostapd_data *hapd, struct wps_data *wps_data, struct wps_context *wps)
{
	return 0;
}

static inline void hostapd_deinit_wps(struct hostapd_data *hapd)
{
}

static inline int hostapd_init_wps_complete(struct hostapd_data *hapd)
{
    return 0;
}

static inline void hostapd_update_wps(struct hostapd_data *hapd)
{
}

static inline void hostapd_wps_eap_completed(struct hostapd_data *hapd)
{
}

static inline int hostapd_wps_get_mib_sta(struct hostapd_data *hapd,
					  const u8 *addr,
					  char *buf, size_t buflen)
{
	return 0;
}

static inline int hostapd_wps_button_pushed(struct hostapd_data *hapd,
					    const u8 *p2p_dev_addr)
{
	return 0;
}

static inline int hostapd_wps_cancel(struct hostapd_data *hapd)
{
	return 0;
}

#endif /* CONFIG_WPS */

#endif /* WPS_HOSTAPD_H */
