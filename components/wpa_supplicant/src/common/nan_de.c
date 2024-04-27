/*
 * NAN Discovery Engine
 * Copyright (c) 2024, Qualcomm Innovation Center, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "crypto/crypto.h"
#include "crypto/sha256.h"
#include "ieee802_11_defs.h"
#include "nan.h"
#include "nan_de.h"

static const u8 nan_network_id[ETH_ALEN] =
{ 0x51, 0x6f, 0x9a, 0x01, 0x00, 0x00 };
static const u8 wildcard_bssid[ETH_ALEN] =
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

enum nan_de_service_type {
	NAN_DE_PUBLISH,
	NAN_DE_SUBSCRIBE,
};

struct nan_de_service {
	int id;
	enum nan_de_service_type type;
	char *service_name;
	u8 service_id[NAN_SERVICE_ID_LEN];
	struct nan_publish_params publish;
	struct nan_subscribe_params subscribe;
	enum nan_service_protocol_type srv_proto_type;
	struct wpabuf *ssi;
	struct wpabuf *elems;
	struct os_reltime time_started;
	struct os_reltime end_time;
	struct os_reltime last_multicast;
	struct os_reltime first_discovered;
	struct os_reltime last_followup;
	bool needs_fsd;
	unsigned int freq;
	unsigned int default_freq;
	int *freq_list;

	/* pauseState information for Publish function */
	struct os_reltime pause_state_end;
	u8 sel_peer_id;
	u8 sel_peer_addr[ETH_ALEN];

	/* Publish state - channel iteration */
	bool in_multi_chan;
	bool first_multi_chan;
	int multi_chan_idx; /* index to freq_list[] */
	struct os_reltime next_publish_state;
	struct os_reltime next_publish_chan;
	unsigned int next_publish_duration;
};

struct nan_de {
	u8 nmi[ETH_ALEN];
	bool ap;
	struct nan_callbacks cb;

	struct nan_de_service *service[NAN_DE_MAX_SERVICE];
	unsigned int num_service;

	int next_handle;

	unsigned int ext_listen_freq;
	unsigned int listen_freq;
	unsigned int tx_wait_status_freq;
	unsigned int tx_wait_end_freq;
};


struct nan_de * nan_de_init(const u8 *nmi, bool ap,
			    const struct nan_callbacks *cb)
{
	struct nan_de *de;

	de = os_zalloc(sizeof(*de));
	if (!de)
		return NULL;

	os_memcpy(de->nmi, nmi, ETH_ALEN);
	de->ap = ap;
	os_memcpy(&de->cb, cb, sizeof(*cb));

	return de;
}


static void nan_de_service_free(struct nan_de_service *srv)
{
	os_free(srv->service_name);
	wpabuf_free(srv->ssi);
	wpabuf_free(srv->elems);
	os_free(srv->freq_list);
	os_free(srv);
}


static void nan_de_service_deinit(struct nan_de *de, struct nan_de_service *srv,
				  enum nan_de_reason reason)
{
	if (!srv)
		return;
	if (srv->type == NAN_DE_PUBLISH && de->cb.publish_terminated)
		de->cb.publish_terminated(de->cb.ctx, srv->id, reason);
	if (srv->type == NAN_DE_SUBSCRIBE && de->cb.subscribe_terminated)
		de->cb.subscribe_terminated(de->cb.ctx, srv->id, reason);
	nan_de_service_free(srv);
}


static void nan_de_clear_pending(struct nan_de *de)
{
	de->listen_freq = 0;
	de->tx_wait_status_freq = 0;
	de->tx_wait_end_freq = 0;
}


void nan_de_flush(struct nan_de *de)
{
	unsigned int i;

	if (!de)
		return;

	for (i = 0; i < NAN_DE_MAX_SERVICE; i++) {
		nan_de_service_deinit(de, de->service[i],
				      NAN_DE_REASON_USER_REQUEST);
		de->service[i] = NULL;
	}

	de->num_service = 0;
	nan_de_clear_pending(de);
}


static void nan_de_pause_state(struct nan_de_service *srv, const u8 *peer_addr,
			       u8 peer_id)
{
	wpa_printf(MSG_DEBUG, "NAN: Start pauseState");
	os_get_reltime(&srv->pause_state_end);
	srv->pause_state_end.sec += 60;
	os_memcpy(srv->sel_peer_addr, peer_addr, ETH_ALEN);
	srv->sel_peer_id = peer_id;
}


static void nan_de_unpause_state(struct nan_de_service *srv)
{
	wpa_printf(MSG_DEBUG, "NAN: Stop pauseState");
	srv->pause_state_end.sec = 0;
	srv->pause_state_end.usec = 0;
	os_memset(srv->sel_peer_addr, 0, ETH_ALEN);
	srv->sel_peer_id = 0;
}


static struct wpabuf * nan_de_alloc_sdf(size_t len)
{
	struct wpabuf *buf;

	buf = wpabuf_alloc(2 + 4 + len);
	if (buf) {
		wpabuf_put_u8(buf, WLAN_ACTION_PUBLIC);
		wpabuf_put_u8(buf, WLAN_PA_VENDOR_SPECIFIC);
		wpabuf_put_be32(buf, NAN_SDF_VENDOR_TYPE);
	}

	return buf;
}


static int nan_de_tx(struct nan_de *de, unsigned int freq,
		     unsigned int wait_time,
		     const u8 *dst, const u8 *src, const u8 *bssid,
		     const struct wpabuf *buf)
{
	int res;

	if (!de->cb.tx)
		return -1;

	res = de->cb.tx(de->cb.ctx, freq, wait_time, dst, src, bssid, buf);
	if (res < 0)
		return res;

	de->tx_wait_status_freq = freq;
	de->tx_wait_end_freq = wait_time ? freq : 0;

	return res;
}


static void nan_de_tx_sdf(struct nan_de *de, struct nan_de_service *srv,
			  unsigned int wait_time,
			  enum nan_service_control_type type,
			  const u8 *dst, u8 req_instance_id,
			  const struct wpabuf *ssi)
{
	struct wpabuf *buf;
	size_t len = 0, sda_len, sdea_len;
	u8 ctrl = type;
	u16 sdea_ctrl = 0;

	/* Service Descriptor attribute */
	sda_len = NAN_SERVICE_ID_LEN + 1 + 1 + 1;
	len += NAN_ATTR_HDR_LEN + sda_len;

	/* Service Descriptor Extension attribute */
	sdea_len = 1 + 2;
	if (ssi)
		sdea_len += 2 + 4 + wpabuf_len(ssi);
	len += NAN_ATTR_HDR_LEN + sdea_len;

	/* Element Container attribute */
	if (srv->elems)
		len += NAN_ATTR_HDR_LEN + 1 + wpabuf_len(srv->elems);

	buf = nan_de_alloc_sdf(len);
	if (!buf)
		return;

	/* Service Descriptor attribute */
	wpabuf_put_u8(buf, NAN_ATTR_SDA);
	wpabuf_put_le16(buf, sda_len);
	wpabuf_put_data(buf, srv->service_id, NAN_SERVICE_ID_LEN);
	wpabuf_put_u8(buf, srv->id); /* Instance ID */
	wpabuf_put_u8(buf, req_instance_id); /* Requestor Instance ID */
	wpabuf_put_u8(buf, ctrl);

	/* Service Descriptor Extension attribute */
	if (srv->type == NAN_DE_PUBLISH || ssi) {
		wpabuf_put_u8(buf, NAN_ATTR_SDEA);
		wpabuf_put_le16(buf, sdea_len);
		wpabuf_put_u8(buf, srv->id); /* Instance ID */
		if (srv->type == NAN_DE_PUBLISH) {
			if (srv->publish.fsd)
				sdea_ctrl |= NAN_SDEA_CTRL_FSD_REQ;
			if (srv->publish.fsd_gas)
				sdea_ctrl |= NAN_SDEA_CTRL_FSD_GAS;
		}
		wpabuf_put_le16(buf, sdea_ctrl);
		if (ssi) {
			wpabuf_put_le16(buf, 4 + wpabuf_len(ssi));
			wpabuf_put_be24(buf, OUI_WFA);
			wpabuf_put_u8(buf, srv->srv_proto_type);
			wpabuf_put_buf(buf, ssi);
		}
	}

	/* Element Container attribute */
	if (srv->elems) {
		wpabuf_put_u8(buf, NAN_ATTR_ELEM_CONTAINER);
		wpabuf_put_le16(buf, 1 + wpabuf_len(srv->elems));
		wpabuf_put_u8(buf, 0); /* Map ID */
		wpabuf_put_buf(buf, srv->elems);
	}

	/* Wi-Fi Aware specification v4.0 uses NAN Cluster ID as A3 for USD,
	 * but there is no synchronization in USD as as such, no NAN Cluster
	 * either. Use Wildcard BSSID instead. */
	nan_de_tx(de, srv->freq, wait_time, dst, de->nmi, wildcard_bssid, buf);
	wpabuf_free(buf);
}


static int nan_de_time_to_next_chan_change(struct nan_de_service *srv)
{
	struct os_reltime tmp, diff, now;

	if (os_reltime_before(&srv->next_publish_state,
			      &srv->next_publish_chan))
		tmp = srv->next_publish_state;
	else if (srv->in_multi_chan)
		tmp = srv->next_publish_chan;
	else
		tmp = srv->next_publish_state;

	os_get_reltime(&now);
	os_reltime_sub(&tmp, &now, &diff);
	return os_reltime_in_ms(&diff);
}


static void nan_de_set_publish_times(struct nan_de_service *srv)
{
	os_get_reltime(&srv->next_publish_state);
	srv->next_publish_chan = srv->next_publish_state;
	/* Swap single/multi channel state in N * 100 TU */
	os_reltime_add_ms(&srv->next_publish_state,
			  srv->next_publish_duration * 1024 / 1000);

	/* Swap channel in multi channel state after 150 ms */
	os_reltime_add_ms(&srv->next_publish_chan, 150);
}


static void nan_de_check_chan_change(struct nan_de_service *srv)
{
	if (srv->next_publish_duration) {
		/* Update end times for the first operation of the publish
		 * iteration */
		nan_de_set_publish_times(srv);
		srv->next_publish_duration = 0;
	} else if (srv->in_multi_chan) {
		if (!os_reltime_initialized(&srv->pause_state_end)) {
			srv->multi_chan_idx++;
			if (srv->freq_list[srv->multi_chan_idx] == 0)
				srv->multi_chan_idx = 0;
			srv->freq = srv->freq_list[srv->multi_chan_idx];
			wpa_printf(MSG_DEBUG,
				   "NAN: Publish multi-channel change to %u MHz",
				   srv->freq);
		}
		os_get_reltime(&srv->next_publish_chan);
		os_reltime_add_ms(&srv->next_publish_chan, 150);
	}
}


static void nan_de_tx_multicast(struct nan_de *de, struct nan_de_service *srv,
				u8 req_instance_id)
{
	enum nan_service_control_type type;
	unsigned int wait_time = 100;

	if (srv->type == NAN_DE_PUBLISH) {
		int ms;

		type = NAN_SRV_CTRL_PUBLISH;

		nan_de_check_chan_change(srv);
		ms = nan_de_time_to_next_chan_change(srv);
		if (ms < 100)
			ms = 100;
		wait_time = ms;
	} else if (srv->type == NAN_DE_SUBSCRIBE) {
		type = NAN_SRV_CTRL_SUBSCRIBE;
	} else {
		return;
	}

	nan_de_tx_sdf(de, srv, wait_time, type, nan_network_id,
		      req_instance_id, srv->ssi);
	os_get_reltime(&srv->last_multicast);
}


static void nan_de_add_srv(struct nan_de *de, struct nan_de_service *srv)
{
	int ttl;

	os_get_reltime(&srv->time_started);
	ttl = srv->type == NAN_DE_PUBLISH ? srv->publish.ttl :
		srv->subscribe.ttl;
	if (ttl) {
		srv->end_time = srv->time_started;
		srv->end_time.sec += ttl;
	}

	de->service[srv->id - 1] = srv;
	de->num_service++;
}


static void nan_de_del_srv(struct nan_de *de, struct nan_de_service *srv,
			   enum nan_de_reason reason)
{
	de->service[srv->id - 1] = NULL;
	nan_de_service_deinit(de, srv, reason);
	de->num_service--;
	if (de->num_service == 0)
		nan_de_clear_pending(de);
}


static bool nan_de_srv_expired(struct nan_de_service *srv,
			       struct os_reltime *now)
{
	if (os_reltime_initialized(&srv->end_time))
		return os_reltime_before(&srv->end_time, now);

	if (srv->type == NAN_DE_PUBLISH) {
		/* Time out after one transmission (and wait for FSD) */
		if (!os_reltime_initialized(&srv->last_multicast))
			return false;
		if (!srv->publish.fsd)
			return true;
		if (os_reltime_initialized(&srv->last_followup) &&
		    !os_reltime_expired(now, &srv->last_followup, 1))
			return false;
		if (os_reltime_expired(now, &srv->last_multicast, 1))
			return true;
	}

	if (srv->type == NAN_DE_SUBSCRIBE) {
		/* Time out after first DiscoveryResult event (and wait for
		 * FSD) */
		if (!os_reltime_initialized(&srv->first_discovered))
			return false;
		if (!srv->needs_fsd)
			return true;
		if (os_reltime_initialized(&srv->last_followup) &&
		    !os_reltime_expired(now, &srv->last_followup, 1))
			return false;
		if (os_reltime_expired(now, &srv->first_discovered, 1))
			return true;
	}

	return false;
}


static int nan_de_next_multicast(struct nan_de *de, struct nan_de_service *srv,
				 struct os_reltime *now)
{
	unsigned int period;
	struct os_reltime next, diff;

	if (srv->type == NAN_DE_PUBLISH && !srv->publish.unsolicited)
		return -1;
	if (srv->type == NAN_DE_SUBSCRIBE && !srv->subscribe.active)
		return -1;

	if (!os_reltime_initialized(&srv->last_multicast))
		return 0;

	if (srv->type == NAN_DE_PUBLISH && srv->publish.ttl == 0)
		return -1;

	if (srv->type == NAN_DE_PUBLISH &&
	    os_reltime_initialized(&srv->pause_state_end))
		return -1;

	period = srv->type == NAN_DE_PUBLISH ?
		srv->publish.announcement_period :
		srv->subscribe.query_period;
	if (period == 0)
		period = 100;
	next = srv->last_multicast;
	os_reltime_add_ms(&next, period);

	if (srv->type == NAN_DE_PUBLISH) {
		if (!de->tx_wait_end_freq && srv->publish.unsolicited &&
		    os_reltime_before(&next, now))
			return 0;
		next = srv->next_publish_state;
	}

	if (os_reltime_before(&next, now))
		return 0;

	os_reltime_sub(&next, now, &diff);
	return os_reltime_in_ms(&diff);
}


static int nan_de_srv_time_to_next(struct nan_de *de,
				   struct nan_de_service *srv,
				   struct os_reltime *now)
{
	struct os_reltime diff;
	int next = -1, tmp;

	if (os_reltime_initialized(&srv->end_time)) {
		os_reltime_sub(&srv->end_time, now, &diff);
		tmp = os_reltime_in_ms(&diff);
		if (next == -1 || tmp < next)
			next = tmp;
	}

	tmp = nan_de_next_multicast(de, srv, now);
	if (tmp >= 0 && (next == -1 || tmp < next))
		next = tmp;

	if (srv->type == NAN_DE_PUBLISH &&
	    os_reltime_initialized(&srv->last_multicast)) {
		/* Time out after one transmission (and wait for FSD) */
		tmp = srv->publish.fsd ? 1000 : 100;
		if (next == -1 || tmp < next)
			next = tmp;
	}

	if (srv->type == NAN_DE_SUBSCRIBE &&
	    os_reltime_initialized(&srv->first_discovered)) {
		/* Time out after first DiscoveryResult event (and wait for
		 * FSD) */
		tmp = srv->needs_fsd ? 1000 : 100;
		if (next == -1 || tmp < next)
			next = tmp;
	}

	if (os_reltime_initialized(&srv->next_publish_state)) {
		os_reltime_sub(&srv->next_publish_state, now, &diff);
		if (diff.sec < 0 || (diff.sec == 0 && diff.usec < 0))
			tmp = 0;
		else
			tmp = os_reltime_in_ms(&diff);
		if (next == -1 || tmp < next)
			next = tmp;
	}

	return next;
}


static void nan_de_start_new_publish_state(struct nan_de_service *srv,
					   bool force_single)
{
	unsigned int n;

	if (force_single || !srv->freq_list || srv->freq_list[0] == 0)
		srv->in_multi_chan = false;
	else
		srv->in_multi_chan = !srv->in_multi_chan;

	/* Use hardcoded Nmin=5 and Nmax=10 and pick a random N from that range.
	 * Use same values for M. */
	n = 5 + os_random() % 5;
	srv->next_publish_duration = n * 100;

	nan_de_set_publish_times(srv);

	if (os_reltime_initialized(&srv->pause_state_end))
		return;

	if (srv->in_multi_chan && srv->freq_list && srv->freq_list[0]) {
		if (!srv->first_multi_chan)
			srv->multi_chan_idx++;
		if (srv->freq_list[srv->multi_chan_idx] == 0)
			srv->multi_chan_idx = 0;
		srv->first_multi_chan = false;
		srv->freq = srv->freq_list[srv->multi_chan_idx];
	} else {
		srv->freq = srv->default_freq;
	}

	wpa_printf(MSG_DEBUG,
		   "NAN: Publish in %s channel state for %u TU; starting with %u MHz",
		   srv->in_multi_chan ? "multi" : "single", n * 100, srv->freq);
}


static void nan_de_timer(void *eloop_ctx, void *timeout_ctx)
{
	struct nan_de *de = eloop_ctx;
	unsigned int i;
	int next = -1;
	bool started = false;
	struct os_reltime now;

	os_get_reltime(&now);

	for (i = 0; i < NAN_DE_MAX_SERVICE; i++) {
		struct nan_de_service *srv = de->service[i];
		int srv_next;

		if (!srv)
			continue;

		if (nan_de_srv_expired(srv, &now)) {
			wpa_printf(MSG_DEBUG, "NAN: Service id %d expired",
				   srv->id);
			nan_de_del_srv(de, srv, NAN_DE_REASON_TIMEOUT);
			continue;
		}

		if (os_reltime_initialized(&srv->next_publish_state) &&
		    os_reltime_before(&srv->next_publish_state, &now))
			nan_de_start_new_publish_state(srv, false);

		if (srv->type == NAN_DE_PUBLISH &&
		    os_reltime_initialized(&srv->pause_state_end) &&
		    (os_reltime_before(&srv->pause_state_end, &now) ||
		     (srv->publish.fsd &&
		      os_reltime_initialized(&srv->last_followup) &&
		      os_reltime_expired(&now, &srv->last_followup, 1))))
			nan_de_unpause_state(srv);

		srv_next = nan_de_srv_time_to_next(de, srv, &now);
		if (srv_next >= 0 && (next == -1 || srv_next < next))
			next = srv_next;

		if (srv_next == 0 && !started &&
		    de->listen_freq == 0 && de->ext_listen_freq == 0 &&
		    de->tx_wait_end_freq == 0 &&
		    nan_de_next_multicast(de, srv, &now) == 0) {
			started = true;
			nan_de_tx_multicast(de, srv, 0);
		}

		if (!started && de->cb.listen &&
		    de->listen_freq == 0 && de->ext_listen_freq == 0 &&
		    de->tx_wait_end_freq == 0 &&
		    ((srv->type == NAN_DE_PUBLISH &&
		      !srv->publish.unsolicited && srv->publish.solicited) ||
		     (srv->type == NAN_DE_SUBSCRIBE &&
		      !srv->subscribe.active))) {
			int duration = 1000;

			if (srv->type == NAN_DE_PUBLISH) {
				nan_de_check_chan_change(srv);
				duration = nan_de_time_to_next_chan_change(srv);
				if (duration < 150)
					duration = 150;
			}

			started = true;
			if (de->cb.listen(de->cb.ctx, srv->freq, duration) == 0)
				de->listen_freq = srv->freq;
		}

	}

	if (next < 0)
		return;

	if (next == 0)
		next = 1;
	wpa_printf(MSG_DEBUG, "NAN: Next timer in %u ms", next);
	eloop_register_timeout(next / 1000, (next % 1000) * 1000, nan_de_timer,
			       de, NULL);
}


static void nan_de_run_timer(struct nan_de *de)
{
	eloop_cancel_timeout(nan_de_timer, de, NULL);
	eloop_register_timeout(0, 0, nan_de_timer, de, NULL);
}


void nan_de_deinit(struct nan_de *de)
{
	eloop_cancel_timeout(nan_de_timer, de, NULL);
	nan_de_flush(de);
	os_free(de);
}


void nan_de_listen_started(struct nan_de *de, unsigned int freq,
			   unsigned int duration)
{
	if (freq != de->listen_freq)
		de->ext_listen_freq = freq;
}


void nan_de_listen_ended(struct nan_de *de, unsigned int freq)
{
	if (freq == de->ext_listen_freq)
		de->ext_listen_freq = 0;

	if (freq == de->listen_freq) {
		de->listen_freq = 0;
		nan_de_run_timer(de);
	}
}


void nan_de_tx_status(struct nan_de *de, unsigned int freq, const u8 *dst)
{
	if (freq == de->tx_wait_status_freq)
		de->tx_wait_status_freq = 0;
}


void nan_de_tx_wait_ended(struct nan_de *de)
{
	de->tx_wait_end_freq = 0;
	nan_de_run_timer(de);
}


static const u8 *
nan_de_get_attr(const u8 *buf, size_t len, enum nan_attr_id id,
		unsigned int skip)
{
	const u8 *pos = buf, *end = buf + len;

	while (end - pos >= NAN_ATTR_HDR_LEN) {
		const u8 *attr = pos;
		u8 attr_id;
		u16 attr_len;

		attr_id = *pos++;
		attr_len = WPA_GET_LE16(pos);
		pos += 2;
		if (attr_len > end - pos) {
			wpa_printf(MSG_DEBUG,
				   "NAN: Truncated attribute %u (len %u; left %zu)",
				   attr_id, attr_len, end - pos);
			break;
		}

		if (attr_id == id) {
			if (skip == 0)
				return attr;
			skip--;
		}

		pos += attr_len;
	}

	return NULL;
}


static void nan_de_get_sdea(const u8 *buf, size_t len, u8 instance_id,
			    u16 *sdea_control,
			    enum nan_service_protocol_type *srv_proto_type,
			    const u8 **ssi, size_t *ssi_len)
{
	unsigned int skip;
	const u8 *sdea, *end;
	u16 sdea_len;

	for (skip = 0; ; skip++) {
		sdea = nan_de_get_attr(buf, len, NAN_ATTR_SDEA, skip);
		if (!sdea)
			break;

		sdea++;
		sdea_len = WPA_GET_LE16(sdea);
		sdea += 2;
		if (sdea_len < 1 + 2)
			continue;
		end = sdea + sdea_len;

		if (instance_id != *sdea++)
			continue; /* Mismatching Instance ID */

		*sdea_control = WPA_GET_LE16(sdea);
		sdea += 2;

		if (*sdea_control & NAN_SDEA_CTRL_RANGE_LIMIT) {
			if (end - sdea < 4)
				continue;
			sdea += 4;
		}

		if (*sdea_control & NAN_SDEA_CTRL_SRV_UPD_INDIC) {
			if (end - sdea < 1)
				continue;
			sdea++;
		}

		if (end - sdea >= 2) {
			u16 srv_info_len;

			srv_info_len = WPA_GET_LE16(sdea);
			sdea += 2;

			if (srv_info_len > end - sdea)
				continue;

			if (srv_info_len >= 4 &&
			    WPA_GET_BE24(sdea) == OUI_WFA) {
				*srv_proto_type = sdea[3];
				*ssi = sdea + 4;
				*ssi_len = srv_info_len - 4;
			}
		}
	}
}


static void nan_de_rx_publish(struct nan_de *de, struct nan_de_service *srv,
			      const u8 *peer_addr, u8 instance_id,
			      u8 req_instance_id, u16 sdea_control,
			      enum nan_service_protocol_type srv_proto_type,
			      const u8 *ssi, size_t ssi_len)
{
	/* Subscribe function processing of a receive Publish message */
	if (!os_reltime_initialized(&srv->first_discovered)) {
		os_get_reltime(&srv->first_discovered);
		srv->needs_fsd = sdea_control & NAN_SDEA_CTRL_FSD_REQ;
		nan_de_run_timer(de);
	}

	if (srv->subscribe.active && req_instance_id == 0) {
		/* Active subscriber replies with a Subscribe message if it
		 * received a matching unsolicited Publish message. */
		nan_de_tx_multicast(de, srv, instance_id);
	}

	if (!srv->subscribe.active && req_instance_id == 0) {
		/* Passive subscriber replies with a Follow-up message without
		 * Service Specific Info field if it received a matching
		 * unsolicited Publish message. */
		nan_de_transmit(de, srv->id, NULL, NULL, peer_addr,
				instance_id);
	}

	if (de->cb.discovery_result)
		de->cb.discovery_result(
			de->cb.ctx, srv->id, srv_proto_type,
			ssi, ssi_len, instance_id,
			peer_addr,
			sdea_control & NAN_SDEA_CTRL_FSD_REQ,
			sdea_control & NAN_SDEA_CTRL_FSD_GAS);
}


static bool nan_de_filter_match(struct nan_de_service *srv,
				const u8 *matching_filter,
				size_t matching_filter_len)
{
	const u8 *pos, *end;

	/* Since we do not currently support matching_filter_rx values for the
	 * local Publish function, any matching filter with at least one
	 * <length,value> pair with length larger than zero implies a mismatch.
	 */

	if (!matching_filter)
		return true;

	pos = matching_filter;
	end = matching_filter + matching_filter_len;

	while (pos < end) {
		u8 len;

		len = *pos++;
		if (len > end - pos)
			break;
		if (len) {
			/* A non-empty Matching Filter entry: no match since
			 * there is no local matching_filter_rx. */
			return false;
		}
	}

	return true;
}


static void nan_de_rx_subscribe(struct nan_de *de, struct nan_de_service *srv,
				const u8 *peer_addr, u8 instance_id,
				const u8 *matching_filter,
				size_t matching_filter_len,
				enum nan_service_protocol_type srv_proto_type,
				const u8 *ssi, size_t ssi_len)
{
	struct wpabuf *buf;
	size_t len = 0, sda_len, sdea_len;
	u8 ctrl = 0;
	u16 sdea_ctrl = 0;

	/* Publish function processing of a receive Subscribe message */

	if (!nan_de_filter_match(srv, matching_filter, matching_filter_len))
		return;

	if (!srv->publish.solicited)
		return;

	if (os_reltime_initialized(&srv->pause_state_end) &&
	    (!ether_addr_equal(peer_addr, srv->sel_peer_addr) ||
	     instance_id != srv->sel_peer_id)) {
		wpa_printf(MSG_DEBUG,
			   "NAN: In pauseState - ignore Subscribe message from another subscriber");
		return;
	}

	/* Reply with a solicited Publish message */
	/* Service Descriptor attribute */
	sda_len = NAN_SERVICE_ID_LEN + 1 + 1 + 1;
	len += NAN_ATTR_HDR_LEN + sda_len;

	/* Service Descriptor Extension attribute */
	sdea_len = 1 + 2;
	if (srv->ssi)
		sdea_len += 2 + 4 + wpabuf_len(srv->ssi);
	len += NAN_ATTR_HDR_LEN + sdea_len;

	/* Element Container attribute */
	if (srv->elems)
		len += NAN_ATTR_HDR_LEN + 1 + wpabuf_len(srv->elems);

	buf = nan_de_alloc_sdf(len);
	if (!buf)
		return;

	/* Service Descriptor attribute */
	wpabuf_put_u8(buf, NAN_ATTR_SDA);
	wpabuf_put_le16(buf, sda_len);
	wpabuf_put_data(buf, srv->service_id, NAN_SERVICE_ID_LEN);
	wpabuf_put_u8(buf, srv->id); /* Instance ID */
	wpabuf_put_u8(buf, instance_id); /* Requestor Instance ID */
	ctrl |= NAN_SRV_CTRL_PUBLISH;
	wpabuf_put_u8(buf, ctrl);

	/* Service Descriptor Extension attribute */
	if (srv->type == NAN_DE_PUBLISH || srv->ssi) {
		wpabuf_put_u8(buf, NAN_ATTR_SDEA);
		wpabuf_put_le16(buf, sdea_len);
		wpabuf_put_u8(buf, srv->id); /* Instance ID */
		if (srv->type == NAN_DE_PUBLISH) {
			if (srv->publish.fsd)
				sdea_ctrl |= NAN_SDEA_CTRL_FSD_REQ;
			if (srv->publish.fsd_gas)
				sdea_ctrl |= NAN_SDEA_CTRL_FSD_GAS;
		}
		wpabuf_put_le16(buf, sdea_ctrl);
		if (srv->ssi) {
			wpabuf_put_le16(buf, 4 + wpabuf_len(srv->ssi));
			wpabuf_put_be24(buf, OUI_WFA);
			wpabuf_put_u8(buf, srv->srv_proto_type);
			wpabuf_put_buf(buf, srv->ssi);
		}
	}

	/* Element Container attribute */
	if (srv->elems) {
		wpabuf_put_u8(buf, NAN_ATTR_ELEM_CONTAINER);
		wpabuf_put_le16(buf, 1 + wpabuf_len(srv->elems));
		wpabuf_put_u8(buf, 0); /* Map ID */
		wpabuf_put_buf(buf, srv->elems);
	}

	/* Wi-Fi Aware specification v4.0 uses NAN Cluster ID as A3 for USD,
	 * but there is no synchronization in USD as as such, no NAN Cluster
	 * either. Use Wildcard BSSID instead. */
	nan_de_tx(de, srv->freq, 100,
		  srv->publish.solicited_multicast ? nan_network_id : peer_addr,
		  de->nmi, wildcard_bssid, buf);
	wpabuf_free(buf);

	nan_de_pause_state(srv, peer_addr, instance_id);

	if (!srv->publish.disable_events && de->cb.replied)
		de->cb.replied(de->cb.ctx, srv->id, peer_addr, instance_id,
			       srv_proto_type, ssi, ssi_len);
}


static void nan_de_rx_follow_up(struct nan_de *de, struct nan_de_service *srv,
				const u8 *peer_addr, u8 instance_id,
				const u8 *ssi, size_t ssi_len)
{
	/* Follow-up function processing of a receive Follow-up message for a
	 * Subscribe or Publish instance */

	if (srv->type == NAN_DE_PUBLISH &&
	    os_reltime_initialized(&srv->pause_state_end) &&
	    (!ether_addr_equal(peer_addr, srv->sel_peer_addr) ||
	     instance_id != srv->sel_peer_id ||
	     !ssi)) {
		wpa_printf(MSG_DEBUG,
			   "NAN: In pauseState - ignore Follow-up message from another subscriber or without ssi");
		return;
	}

	os_get_reltime(&srv->last_followup);

	if (srv->type == NAN_DE_PUBLISH && !ssi)
		nan_de_pause_state(srv, peer_addr, instance_id);

	if (de->cb.receive)
		de->cb.receive(de->cb.ctx, srv->id, instance_id, ssi, ssi_len,
			       peer_addr);
}


static void nan_de_rx_sda(struct nan_de *de, const u8 *peer_addr,
			  unsigned int freq, const u8 *buf, size_t len,
			  const u8 *sda, size_t sda_len)
{
	const u8 *service_id;
	u8 instance_id, req_instance_id, ctrl;
	u16 sdea_control = 0;
	unsigned int i;
	enum nan_service_control_type type = 0;
	enum nan_service_protocol_type srv_proto_type = 0;
	const u8 *ssi = NULL;
	size_t ssi_len = 0;
	bool first = true;
	const u8 *end;
	const u8 *matching_filter = NULL;
	size_t matching_filter_len = 0;

	if (sda_len < NAN_SERVICE_ID_LEN + 1 + 1 + 1)
		return;
	end = sda + sda_len;

	service_id = sda;
	sda += NAN_SERVICE_ID_LEN;
	instance_id = *sda++;
	req_instance_id = *sda++;
	ctrl = *sda;
	type = ctrl & NAN_SRV_CTRL_TYPE_MASK;
	wpa_printf(MSG_DEBUG,
		   "NAN: SDA - Service ID %02x%02x%02x%02x%02x%02x Instance ID %u Requestor Instance ID %u Service Control 0x%x (Service Control Type %u)",
		   MAC2STR(service_id), instance_id, req_instance_id,
		   ctrl, type);
	if (type != NAN_SRV_CTRL_PUBLISH &&
	    type != NAN_SRV_CTRL_SUBSCRIBE &&
	    type != NAN_SRV_CTRL_FOLLOW_UP) {
		wpa_printf(MSG_DEBUG,
			   "NAN: Discard SDF with unknown Service Control Type %u",
			   type);
		return;
	}

	if (ctrl & NAN_SRV_CTRL_BINDING_BITMAP) {
		if (end - sda < 2)
			return;
		sda += 2;
	}

	if (ctrl & NAN_SRV_CTRL_MATCHING_FILTER) {
		u8 flen;

		if (end - sda < 1)
			return;
		flen = *sda++;
		if (end - sda < flen)
			return;
		matching_filter = sda;
		matching_filter_len = flen;
		sda += flen;
	}

	if (ctrl & NAN_SRV_CTRL_RESP_FILTER) {
		u8 flen;

		if (end - sda < 1)
			return;
		flen = *sda++;
		if (end - sda < flen)
			return;
		sda += flen;
	}

	if (ctrl & NAN_SRV_CTRL_SRV_INFO) {
		u8 flen;

		if (end - sda < 1)
			return;
		flen = *sda++;
		if (end - sda < flen)
			return;
		if (flen >= 4 && WPA_GET_BE24(sda) == OUI_WFA) {
			srv_proto_type = sda[3];
			ssi = sda + 4;
			ssi_len = flen - 4;
			wpa_printf(MSG_DEBUG, "NAN: Service Protocol Type %d",
				   srv_proto_type);
			wpa_hexdump(MSG_MSGDUMP, "NAN: ssi", ssi, ssi_len);
		}
		sda += flen;
	}

	for (i = 0; i < NAN_DE_MAX_SERVICE; i++) {
		struct nan_de_service *srv = de->service[i];

		if (!srv)
			continue;
		if (os_memcmp(srv->service_id, service_id,
			      NAN_SERVICE_ID_LEN) != 0)
			continue;
		if (type == NAN_SRV_CTRL_PUBLISH) {
			if (srv->type == NAN_DE_PUBLISH)
				continue;
			if (req_instance_id && srv->id != req_instance_id)
				continue;
		}
		if (type == NAN_SRV_CTRL_SUBSCRIBE &&
		    srv->type == NAN_DE_SUBSCRIBE)
			continue;
		wpa_printf(MSG_DEBUG, "NAN: Received SDF matches service ID %u",
			   i + 1);

		if (first) {
			first = false;
			nan_de_get_sdea(buf, len, instance_id, &sdea_control,
					&srv_proto_type, &ssi, &ssi_len);

			if (ssi) {
				wpa_printf(MSG_DEBUG,
					   "NAN: Service Protocol Type %d",
					   srv_proto_type);
				wpa_hexdump(MSG_MSGDUMP, "NAN: ssi",
					    ssi, ssi_len);
			}
		}

		switch (type) {
		case NAN_SRV_CTRL_PUBLISH:
			nan_de_rx_publish(de, srv, peer_addr, instance_id,
					  req_instance_id,
					  sdea_control, srv_proto_type,
					  ssi, ssi_len);
			break;
		case NAN_SRV_CTRL_SUBSCRIBE:
			nan_de_rx_subscribe(de, srv, peer_addr, instance_id,
					    matching_filter,
					    matching_filter_len,
					    srv_proto_type,
					    ssi, ssi_len);
			break;
		case NAN_SRV_CTRL_FOLLOW_UP:
			nan_de_rx_follow_up(de, srv, peer_addr, instance_id,
					    ssi, ssi_len);
			break;
		}
	}
}


void nan_de_rx_sdf(struct nan_de *de, const u8 *peer_addr, unsigned int freq,
		   const u8 *buf, size_t len)
{
	const u8 *sda;
	u16 sda_len;
	unsigned int skip;

	if (!de->num_service)
		return;

	wpa_printf(MSG_DEBUG, "NAN: RX SDF from " MACSTR " freq=%u len=%zu",
		   MAC2STR(peer_addr), freq, len);

	wpa_hexdump(MSG_MSGDUMP, "NAN: SDF payload", buf, len);

	for (skip = 0; ; skip++) {
		sda = nan_de_get_attr(buf, len, NAN_ATTR_SDA, skip);
		if (!sda)
			break;

		sda++;
		sda_len = WPA_GET_LE16(sda);
		sda += 2;
		nan_de_rx_sda(de, peer_addr, freq, buf, len, sda, sda_len);
	}
}


static int nan_de_get_handle(struct nan_de *de)
{
	int i = de->next_handle;

	if (de->num_service >= NAN_DE_MAX_SERVICE)
		goto fail;

	do {
		if (!de->service[i]) {
			de->next_handle = (i + 1) % NAN_DE_MAX_SERVICE;
			return i + 1;
		}
		i = (i + 1) % NAN_DE_MAX_SERVICE;
	} while (i != de->next_handle);

fail:
	wpa_printf(MSG_DEBUG, "NAN: No more room for a new service");
	return -1;
}


static int nan_de_derive_service_id(struct nan_de_service *srv)
{
	u8 hash[SHA256_MAC_LEN];
	char *name, *pos;
	int ret;
	const u8 *addr[1];
	size_t len[1];

	name = os_strdup(srv->service_name);
	if (!name)
		return -1;
	pos = name;
	while (*pos) {
		*pos = tolower(*pos);
		pos++;
	}

	addr[0] = (u8 *) name;
	len[0] = os_strlen(name);
	ret = sha256_vector(1, addr, len, hash);
	os_free(name);
	if (ret == 0)
		os_memcpy(srv->service_id, hash, NAN_SERVICE_ID_LEN);

	return ret;
}


int nan_de_publish(struct nan_de *de, const char *service_name,
		   enum nan_service_protocol_type srv_proto_type,
		   const struct wpabuf *ssi, const struct wpabuf *elems,
		   struct nan_publish_params *params)
{
	int publish_id;
	struct nan_de_service *srv;

	if (!service_name) {
		wpa_printf(MSG_DEBUG, "NAN: Publish() - no service_name");
		return -1;
	}

	if (!params->unsolicited && !params->solicited) {
		wpa_printf(MSG_INFO,
			   "NAN: Publish() - both unsolicited and solicited disabled is invalid");
		return -1;
	}

	publish_id = nan_de_get_handle(de);
	if (publish_id < 1)
		return -1;

	srv = os_zalloc(sizeof(*srv));
	if (!srv)
		return -1;
	srv->type = NAN_DE_PUBLISH;
	srv->freq = srv->default_freq = params->freq;
	srv->service_name = os_strdup(service_name);
	if (!srv->service_name)
		goto fail;
	if (nan_de_derive_service_id(srv) < 0)
		goto fail;
	os_memcpy(&srv->publish, params, sizeof(*params));

	if (params->freq_list) {
		size_t len;

		len = (int_array_len(params->freq_list) + 1) * sizeof(int);
		srv->freq_list = os_memdup(params->freq_list, len);
		if (!srv->freq_list)
			goto fail;
	}
	srv->publish.freq_list = NULL;

	srv->srv_proto_type = srv_proto_type;
	if (ssi) {
		srv->ssi = wpabuf_dup(ssi);
		if (!srv->ssi)
			goto fail;
	}
	if (elems) {
		srv->elems = wpabuf_dup(elems);
		if (!srv->elems)
			goto fail;
	}

	/* Prepare for single and multi-channel states; starting with
	 * single channel */
	srv->first_multi_chan = true;
	nan_de_start_new_publish_state(srv, true);

	wpa_printf(MSG_DEBUG, "NAN: Assigned new publish handle %d for %s",
		   publish_id, service_name);
	srv->id = publish_id;
	nan_de_add_srv(de, srv);
	nan_de_run_timer(de);
	return publish_id;
fail:
	nan_de_service_free(srv);
	return -1;
}


void nan_de_cancel_publish(struct nan_de *de, int publish_id)
{
	struct nan_de_service *srv;

	wpa_printf(MSG_DEBUG, "NAN: CancelPublish(publish_id=%d)", publish_id);

	if (publish_id < 1 || publish_id > NAN_DE_MAX_SERVICE)
		return;
	srv = de->service[publish_id - 1];
	if (!srv || srv->type != NAN_DE_PUBLISH)
		return;
	nan_de_del_srv(de, srv, NAN_DE_REASON_USER_REQUEST);
}


int nan_de_update_publish(struct nan_de *de, int publish_id,
			  const struct wpabuf *ssi)
{
	struct nan_de_service *srv;

	wpa_printf(MSG_DEBUG, "NAN: UpdatePublish(publish_id=%d)", publish_id);

	if (publish_id < 1 || publish_id > NAN_DE_MAX_SERVICE)
		return -1;
	srv = de->service[publish_id - 1];
	if (!srv || srv->type != NAN_DE_PUBLISH)
		return -1;

	wpabuf_free(srv->ssi);
	srv->ssi = NULL;
	if (!ssi)
		return 0;
	srv->ssi = wpabuf_dup(ssi);
	if (!srv->ssi)
		return -1;
	return 0;
}


int nan_de_subscribe(struct nan_de *de, const char *service_name,
		     enum nan_service_protocol_type srv_proto_type,
		     const struct wpabuf *ssi, const struct wpabuf *elems,
		     struct nan_subscribe_params *params)
{
	int subscribe_id;
	struct nan_de_service *srv;

	if (!service_name) {
		wpa_printf(MSG_DEBUG, "NAN: Subscribe() - no service_name");
		return -1;
	}

	subscribe_id = nan_de_get_handle(de);
	if (subscribe_id < 1)
		return -1;

	srv = os_zalloc(sizeof(*srv));
	if (!srv)
		return -1;
	srv->type = NAN_DE_SUBSCRIBE;
	srv->freq = params->freq;
	srv->service_name = os_strdup(service_name);
	if (!srv->service_name)
		goto fail;
	if (nan_de_derive_service_id(srv) < 0)
		goto fail;
	os_memcpy(&srv->subscribe, params, sizeof(*params));
	srv->srv_proto_type = srv_proto_type;
	if (ssi) {
		srv->ssi = wpabuf_dup(ssi);
		if (!srv->ssi)
			goto fail;
	}
	if (elems) {
		srv->elems = wpabuf_dup(elems);
		if (!srv->elems)
			goto fail;
	}

	wpa_printf(MSG_DEBUG, "NAN: Assigned new subscribe handle %d for %s",
		   subscribe_id, service_name);
	srv->id = subscribe_id;
	nan_de_add_srv(de, srv);
	nan_de_run_timer(de);
	return subscribe_id;
fail:
	nan_de_service_free(srv);
	return -1;
}


void nan_de_cancel_subscribe(struct nan_de *de, int subscribe_id)
{
	struct nan_de_service *srv;

	if (subscribe_id < 1 || subscribe_id > NAN_DE_MAX_SERVICE)
		return;
	srv = de->service[subscribe_id - 1];
	if (!srv || srv->type != NAN_DE_SUBSCRIBE)
		return;
	nan_de_del_srv(de, srv, NAN_DE_REASON_USER_REQUEST);
}


int nan_de_transmit(struct nan_de *de, int handle,
		    const struct wpabuf *ssi, const struct wpabuf *elems,
		    const u8 *peer_addr, u8 req_instance_id)
{
	struct nan_de_service *srv;

	if (handle < 1 || handle > NAN_DE_MAX_SERVICE)
		return -1;

	srv = de->service[handle - 1];
	if (!srv)
		return -1;

	nan_de_tx_sdf(de, srv, 100, NAN_SRV_CTRL_FOLLOW_UP,
		      peer_addr, req_instance_id, ssi);

	os_get_reltime(&srv->last_followup);
	return 0;
}
