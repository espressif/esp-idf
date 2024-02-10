/*
 * NAN Discovery Engine
 * Copyright (c) 2024, Qualcomm Innovation Center, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef NAN_DE_H
#define NAN_DE_H

#include "nan.h"

/* Maximum number of active local publish and subscribe instances */
#ifndef NAN_DE_MAX_SERVICE
#define NAN_DE_MAX_SERVICE 20
#endif /* NAN_DE_MAX_SERVICE */

struct nan_de;

enum nan_de_reason {
	NAN_DE_REASON_TIMEOUT,
	NAN_DE_REASON_USER_REQUEST,
	NAN_DE_REASON_FAILURE,
};

struct nan_callbacks {
	void *ctx;

	int (*tx)(void *ctx, unsigned int freq, unsigned int wait_time,
		  const u8 *dst, const u8 *src, const u8 *bssid,
		  const struct wpabuf *buf);
	int (*listen)(void *ctx, unsigned int freq, unsigned int duration);

	/* NAN DE Events */
	void (*discovery_result)(void *ctx, int subscribe_id,
				 enum nan_service_protocol_type srv_proto_type,
				 const u8 *ssi, size_t ssi_len,
				 int peer_publish_id,
				 const u8 *peer_addr, bool fsd, bool fsd_gas);

	void (*replied)(void *ctx, int publish_id, const u8 *peer_addr,
			int peer_subscribe_id,
			enum nan_service_protocol_type srv_proto_type,
			const u8 *ssi, size_t ssi_len);

	void (*publish_terminated)(void *ctx, int publish_id,
				    enum nan_de_reason reason);

	void (*subscribe_terminated)(void *ctx, int subscribe_id,
				     enum nan_de_reason reason);

	void (*receive)(void *ctx, int id, int peer_instance_id,
			const u8 *ssi, size_t ssi_len,
			const u8 *peer_addr);
};

struct nan_de * nan_de_init(const u8 *nmi, bool ap,
			    const struct nan_callbacks *cb);
void nan_de_flush(struct nan_de *de);
void nan_de_deinit(struct nan_de *de);

void nan_de_listen_started(struct nan_de *de, unsigned int freq,
			   unsigned int duration);
void nan_de_listen_ended(struct nan_de *de, unsigned int freq);
void nan_de_tx_status(struct nan_de *de, unsigned int freq, const u8 *dst);
void nan_de_tx_wait_ended(struct nan_de *de);

void nan_de_rx_sdf(struct nan_de *de, const u8 *peer_addr, unsigned int freq,
		   const u8 *buf, size_t len);

struct nan_publish_params {
	/* configuration_parameters */

	/* Publish type */
	bool unsolicited;
	bool solicited;

	/* Solicited transmission type */
	bool solicited_multicast;

	/* Time to live (in seconds); 0 = one TX only */
	unsigned int ttl;

	/* Event conditions */
	bool disable_events;

	/* Further Service Discovery flag */
	bool fsd;

	/* Further Service Discovery function */
	bool fsd_gas;

	/* Default frequency (defaultPublishChannel) */
	unsigned int freq;

	/* Multi-channel frequencies (publishChannelList) */
	const int *freq_list;

	/* Announcement period in ms; 0 = use default */
	unsigned int announcement_period;
};

/* Returns -1 on failure or >0 publish_id */
int nan_de_publish(struct nan_de *de, const char *service_name,
		   enum nan_service_protocol_type srv_proto_type,
		   const struct wpabuf *ssi, const struct wpabuf *elems,
		   struct nan_publish_params *params);

void nan_de_cancel_publish(struct nan_de *de, int publish_id);

int nan_de_update_publish(struct nan_de *de, int publish_id,
			  const struct wpabuf *ssi);

struct nan_subscribe_params {
	/* configuration_parameters */

	/* Subscribe type */
	bool active;

	/* Time to live (in seconds); 0 = until first result */
	unsigned int ttl;

	/* Selected frequency */
	unsigned int freq;

	/* Query period in ms; 0 = use default */
	unsigned int query_period;
};

/* Returns -1 on failure or >0 subscribe_id */
int nan_de_subscribe(struct nan_de *de, const char *service_name,
		     enum nan_service_protocol_type srv_proto_type,
		     const struct wpabuf *ssi, const struct wpabuf *elems,
		     struct nan_subscribe_params *params);

void nan_de_cancel_subscribe(struct nan_de *de, int subscribe_id);

/* handle = publish_id or subscribe_id
 * req_instance_id = peer publish_id or subscribe_id */
int nan_de_transmit(struct nan_de *de, int handle,
		    const struct wpabuf *ssi, const struct wpabuf *elems,
		    const u8 *peer_addr, u8 req_instance_id);

#endif /* NAN_DE_H */
