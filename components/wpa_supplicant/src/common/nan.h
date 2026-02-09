/*
 * NAN (Wi-Fi Aware) definitions
 * Copyright (c) 2024, Qualcomm Innovation Center, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef NAN_H
#define NAN_H

enum nan_attr_id {
	NAN_ATTR_MASTER_INDICATION = 0x00,
	NAN_ATTR_CLUSTER = 0x01,
	NAN_ATTR_NAN_ATTR_SERVICE_ID_LIST = 0x02,
	NAN_ATTR_SDA = 0x03, /* Service Descriptor attribute */
	NAN_ATTR_CONN_CAPA = 0x04,
	NAN_ATTR_WLAN_INFRA = 0x05,
	NAN_ATTR_P2P_OPER = 0x06,
	NAN_ATTR_IBSS = 0x07,
	NAN_ATTR_MESH = 0x08,
	NAN_ATTR_FURTHER_NAN_SD = 0x09,
	NAN_ATTR_FURTHER_AVAIL_MAP = 0x0A,
	NAN_ATTR_COUNTRY_CODE = 0x0B,
	NAN_ATTR_RANGIN = 0x0C,
	NAN_ATTR_CLUSTER_DISCOVERY = 0x0D,
	NAN_ATTR_SDEA = 0x0E, /* Service Descriptor Extension attribute */
	NAN_ATTR_DEVICE_CAPABILITY = 0x0F,
	NAN_ATTR_NDP = 0x10,
	NAN_ATTR_NAN_AVAILABILITY = 0x12,
	NAN_ATTR_NDC = 0x13,
	NAN_ATTR_NDL = 0x14,
	NAN_ATTR_NDL_QOS = 0x15,
	NAN_ATTR_UNALIGNED_SCHEDULE = 0x17,
	NAN_ATTR_RANGING_INFO = 0x1A,
	NAN_ATTR_RANGING_SETUP = 0x1B,
	NAN_ATTR_FTM_RANGING_REPORT = 0x1C,
	NAN_ATTR_ELEM_CONTAINER = 0x1D,
	NAN_ATTR_EXT_WLAN_INFRA = 0x1E,
	NAN_ATTR_EXT_P2P_OPER = 0x1FE,
	NAN_ATTR_EXT_IBSS = 0x20,
	NAN_ATTR_EXT_MESH = 0x21,
	NAN_ATTR_CSIA = 0x22, /* Cipher Suite Info attribute */
	NAN_ATTR_SCIA = 0x23, /* Security Context Info attribute */
	NAN_ATTR_SHARED_KEY_DESCR = 0x24,
	NAN_ATTR_PUBLIC_AVAILABILITY = 0x27,
	NAN_ATTR_SUBSC_SERVICE_ID_LIST = 0x28,
	NAN_ATTR_NDP_EXT = 0x29,
	NAN_ATTR_DCEA = 0x2A, /* Device Capability Extension attribute */
	NAN_ATTR_NIRA = 0x2B, /* NAN Identity Resolution attribute */
	NAN_ATTR_BPBA = 0x2C, /* NAN Pairing Bootstrapping attribute */
	NAN_ATTR_S3 = 0x2D,
	NAN_ATTR_TPEA = 0x2E, /* Transmit Power Envelope attribute */
	NAN_ATTR_VENDOR_SPECIFIC = 0xDD,
};

/* Service Descriptor attribute (SDA) */

/* Service Control field */
#define NAN_SRV_CTRL_TYPE_MASK (BIT(0) | BIT(1))
#define NAN_SRV_CTRL_MATCHING_FILTER BIT(2)
#define NAN_SRV_CTRL_RESP_FILTER BIT(3)
#define NAN_SRV_CTRL_SRV_INFO BIT(4)
#define NAN_SRV_CTRL_DISCOVERY_RANGE_LIMITED BIT(5)
#define NAN_SRV_CTRL_BINDING_BITMAP BIT(6)

enum nan_service_control_type {
	NAN_SRV_CTRL_PUBLISH = 0,
	NAN_SRV_CTRL_SUBSCRIBE = 1,
	NAN_SRV_CTRL_FOLLOW_UP = 2,
};

/* Service Descriptor Extension attribute (SDEA) */

/* SDEA Control field */
#define NAN_SDEA_CTRL_FSD_REQ BIT(0)
#define NAN_SDEA_CTRL_FSD_GAS BIT(1)
#define NAN_SDEA_CTRL_DATA_PATH_REQ BIT(2)
#define NAN_SDEA_CTRL_DATA_PATH_TYPE BIT(3)
#define NAN_SDEA_CTRL_QOS_REQ BIT(5)
#define NAN_SDEA_CTRL_SECURITY_REQ BIT(6)
#define NAN_SDEA_CTRL_RANGING_REQ BIT(7)
#define NAN_SDEA_CTRL_RANGE_LIMIT BIT(8)
#define NAN_SDEA_CTRL_SRV_UPD_INDIC BIT(9)
#define NAN_SDEA_CTRL_GTK_REQ BIT(10)

enum nan_service_protocol_type {
	NAN_SRV_PROTO_BONJOUR = 1,
	NAN_SRV_PROTO_GENERIC = 2,
	NAN_SRV_PROTO_CSA_MATTER = 3,
};

#define NAN_ATTR_HDR_LEN 3
#define NAN_SERVICE_ID_LEN 6

#define NAN_USD_DEFAULT_FREQ 2437

#endif /* NAN_H */
