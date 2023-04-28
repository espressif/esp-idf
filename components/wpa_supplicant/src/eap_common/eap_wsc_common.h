/*
 * EAP-WSC definitions for Wi-Fi Protected Setup
 * Copyright (c) 2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_WSC_COMMON_H
#define EAP_WSC_COMMON_H

#define EAP_VENDOR_TYPE_WSC 1

#define WSC_FLAGS_MF 0x01
#define WSC_FLAGS_LF 0x02

#define WSC_ID_REGISTRAR "WFA-SimpleConfig-Registrar-1-0"
#define WSC_ID_REGISTRAR_LEN 30
#define WSC_ID_ENROLLEE "WFA-SimpleConfig-Enrollee-1-0"
#define WSC_ID_ENROLLEE_LEN 29

#define WSC_FRAGMENT_SIZE 1400


struct wpabuf * eap_wsc_build_frag_ack(u8 id, u8 code);

#endif /* EAP_WSC_COMMON_H */
