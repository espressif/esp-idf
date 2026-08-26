/*
 * EAP peer: EAP-TLS/PEAP/TTLS/FAST common functions
 * Copyright (c) 2004-2009, 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_TLS_H
#define EAP_TLS_H

#include "eap_i.h"
#include "eap_common.h"
#include "eap.h"
#include "utils/wpabuf.h"

void * eap_tls_init(struct eap_sm *sm);
void eap_tls_deinit(struct eap_sm *sm, void *priv);
struct wpabuf * eap_tls_process(struct eap_sm *sm, void *priv,
                                       struct eap_method_ret *ret,
                                       const struct wpabuf *reqData);

u8 * eap_tls_getKey(struct eap_sm *sm, void *priv, size_t *len);

#endif /* EAP_TLS_H */
