/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * gs_usb wire protocol definitions.
 *
 * Names and layout follow Linux drivers/net/can/usb/gs_usb.c
 * (CAN names are kept on purpose). `struct can_bt_const` groups the
 * timing-range fields that the kernel inlines in gs_device_bt_const.
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Vendor control bRequest values used by the Linux gs_usb host driver.
 * Comments mark requests this example does not handle (still kept for protocol parity).
 */
enum gs_usb_breq {
    GS_USB_BREQ_HOST_FORMAT = 0,    /* Host writes endianness probe value */
    GS_USB_BREQ_SET_BITTIMING,      /* Classic / arbitration bit timing */
    GS_USB_BREQ_MODE,               /* Start or stop the CAN channel */
    GS_USB_BREQ_BERR,               /* Not implemented here (legacy bus-error counter) */
    GS_USB_BREQ_GET_BT_CONST,       /* Classic bit-timing limits */
    GS_USB_BREQ_DEVICE_CONFIG,      /* Channel count and versions */
    GS_USB_BREQ_TIMESTAMP,          /* Device µs timestamp (host clock sync) */
    GS_USB_BREQ_IDENTIFY,           /* Not implemented here (blink/identify LED) */
    GS_USB_BREQ_GET_USER_ID,        /* Not implemented here */
    GS_USB_BREQ_SET_USER_ID,        /* Not implemented here */
    GS_USB_BREQ_SET_DATA_BITTIMING, /* CAN FD data-phase bit timing */
    GS_USB_BREQ_GET_BT_CONST_EXT,   /* Classic + FD data-phase limits */
    GS_USB_BREQ_SET_TERMINATION,    /* Not implemented here (bus termination) */
    GS_USB_BREQ_GET_TERMINATION,    /* Not implemented here */
    GS_USB_BREQ_GET_STATE,          /* Error state and TEC/REC */
    /* Optional HW filter; Linux SocketCAN uses host-side software filters instead. */
    GS_USB_BREQ_SET_FILTER,         /* Not implemented here */
    GS_USB_BREQ_GET_FILTER,         /* Not implemented here */
};

/* Channel start/stop, sent in gs_device_mode.mode */
enum gs_can_mode {
    GS_CAN_MODE_RESET = 0,
    GS_CAN_MODE_START,
};

/* Controller error state, sent in gs_device_state.state */
enum gs_can_state {
    GS_CAN_STATE_ERROR_ACTIVE = 0,
    GS_CAN_STATE_ERROR_WARNING,
    GS_CAN_STATE_ERROR_PASSIVE,
    GS_CAN_STATE_BUS_OFF,
    GS_CAN_STATE_STOPPED,
    GS_CAN_STATE_SLEEPING,
};

/* gs_device_mode.flags: host-requested operating modes */
#define GS_CAN_MODE_NORMAL                       0
#define GS_CAN_MODE_LISTEN_ONLY                 (1U << 0)
#define GS_CAN_MODE_LOOP_BACK                   (1U << 1)
#define GS_CAN_MODE_TRIPLE_SAMPLE               (1U << 2)
#define GS_CAN_MODE_ONE_SHOT                    (1U << 3)
#define GS_CAN_MODE_HW_TIMESTAMP                (1U << 4)
#define GS_CAN_MODE_PAD_PKTS_TO_MAX_PKT_SIZE    (1U << 7)
#define GS_CAN_MODE_FD                          (1U << 8)
#define GS_CAN_MODE_BERR_REPORTING              (1U << 12)

/* gs_device_bt_const.feature: capabilities advertised to the host */
#define GS_CAN_FEATURE_LISTEN_ONLY              (1U << 0)
#define GS_CAN_FEATURE_LOOP_BACK                (1U << 1)
#define GS_CAN_FEATURE_TRIPLE_SAMPLE            (1U << 2)
#define GS_CAN_FEATURE_ONE_SHOT                 (1U << 3)
#define GS_CAN_FEATURE_HW_TIMESTAMP             (1U << 4)
#define GS_CAN_FEATURE_IDENTIFY                 (1U << 5)
#define GS_CAN_FEATURE_USER_ID                  (1U << 6)
#define GS_CAN_FEATURE_PAD_PKTS_TO_MAX_PKT_SIZE (1U << 7)
#define GS_CAN_FEATURE_FD                       (1U << 8)
#define GS_CAN_FEATURE_BT_CONST_EXT             (1U << 10)
#define GS_CAN_FEATURE_TERMINATION              (1U << 11)
#define GS_CAN_FEATURE_BERR_REPORTING           (1U << 12)
#define GS_CAN_FEATURE_GET_STATE                (1U << 13)

/* gs_host_frame.flags */
#define GS_CAN_FLAG_OVERFLOW                    (1U << 0)   /* RX overflow since last frame */
#define GS_CAN_FLAG_FD                          (1U << 1)   /* CAN FD frame */
#define GS_CAN_FLAG_BRS                         (1U << 2)   /* Bit-rate switch */
#define GS_CAN_FLAG_ESI                         (1U << 3)   /* Error state indicator */

/* SocketCAN can_id flag bits, stored in gs_host_frame.can_id */
#define CAN_EFF_FLAG 0x80000000U    /* Extended 29-bit ID */
#define CAN_RTR_FLAG 0x40000000U    /* Remote transmission request */
#define CAN_ERR_FLAG 0x20000000U    /* Error frame (not a data frame) */

#define CAN_ERR_DLC 8               /* Error frames always use DLC 8 */

/* Error-class bits in can_id when CAN_ERR_FLAG is set */
#define CAN_ERR_CRTL      0x00000004U
#define CAN_ERR_BUSOFF    0x00000040U
#define CAN_ERR_RESTARTED 0x00000100U

/* Error-frame data[1] when CAN_ERR_CRTL is set */
#define CAN_ERR_CRTL_RX_WARNING 0x04
#define CAN_ERR_CRTL_TX_WARNING 0x08
#define CAN_ERR_CRTL_RX_PASSIVE 0x10
#define CAN_ERR_CRTL_TX_PASSIVE 0x20
#define CAN_ERR_CRTL_ACTIVE     0x40

#define GS_HOST_FRAME_ECHO_ID_RX UINT32_MAX /* echo_id for frames received from the bus */

struct gs_host_config {
    uint32_t byte_order;    /* Host writes 0x0000beef so the device can detect endianness */
} __attribute__((packed));

struct gs_device_config {
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t icount;         /* Number of CAN channels minus 1 */
    uint32_t sw_version;
    uint32_t hw_version;
} __attribute__((packed));

struct gs_device_mode {
    uint32_t mode;          /* GS_CAN_MODE_RESET or GS_CAN_MODE_START */
    uint32_t flags;         /* GS_CAN_MODE_* bit mask */
} __attribute__((packed));

struct gs_device_bittiming {
    uint32_t prop_seg;      /* Propagation segment, in time quanta */
    uint32_t phase_seg1;    /* Phase segment 1, in time quanta */
    uint32_t phase_seg2;    /* Phase segment 2, in time quanta */
    uint32_t sjw;           /* Synchronization jump width, in time quanta */
    uint32_t brp;           /* Bit-rate prescaler */
} __attribute__((packed));

/* Hardware bit-timing ranges. Linux stores these fields inline in gs_device_bt_const. */
struct can_bt_const {
    uint32_t tseg1_min;     /* Minimum of (prop_seg + phase_seg1) */
    uint32_t tseg1_max;
    uint32_t tseg2_min;
    uint32_t tseg2_max;
    uint32_t sjw_max;
    uint32_t brp_min;
    uint32_t brp_max;
    uint32_t brp_inc;       /* Prescaler step (1 or 2 depending on hardware) */
} __attribute__((packed));

struct gs_device_bt_const {
    uint32_t feature;               /* GS_CAN_FEATURE_* bit mask */
    uint32_t fclk_can;              /* CAN clock in Hz, used with brp to form bit time */
    struct can_bt_const bt_const;   /* Classic / arbitration timing limits */
} __attribute__((packed));

/* Layout must begin with the same three members as gs_device_bt_const (feature,
 * fclk_can, bt_const), in the same order/size, so GET_BT_CONST can reuse the
 * leading bytes of this extended struct.
 */
struct gs_device_bt_const_extended {
    uint32_t feature;
    uint32_t fclk_can;
    struct can_bt_const bt_const;   /* Classic / arbitration timing limits */
    struct can_bt_const dbt_const;  /* CAN FD data-phase timing limits */
} __attribute__((packed));

struct gs_device_state {
    uint32_t state;         /* GS_CAN_STATE_* */
    uint32_t rxerr;         /* Receive error counter (REC) */
    uint32_t txerr;         /* Transmit error counter (TEC) */
} __attribute__((packed));

struct gs_host_frame {
    uint32_t echo_id;       /* Host TX cookie; echo the same value when TX finishes. UINT32_MAX = RX from bus */
    uint32_t can_id;        /* 11/29-bit ID plus CAN_EFF_FLAG / CAN_RTR_FLAG / CAN_ERR_FLAG */
    uint8_t can_dlc;        /* DLC field (0-8 classic, 0-15 FD), not the byte length */
    uint8_t channel;        /* CAN channel index on this USB device (gs_usb supports multi-port; this example has one channel, so always 0) */
    uint8_t flags;          /* GS_CAN_FLAG_* */
    uint8_t reserved;
    uint8_t data[64];       /* Payload; classic uses first 8 bytes, FD uses up to 64 */
    /* Appended on device->host frames when HW_TIMESTAMP is enabled (classic overlays data[8..11] instead). */
    uint32_t timestamp_us;
} __attribute__((packed));

#define GS_HOST_FRAME_HEADER_SIZE offsetof(struct gs_host_frame, data)
#define GS_HOST_FRAME_CLASSIC_SIZE (GS_HOST_FRAME_HEADER_SIZE + 8)
#define GS_HOST_FRAME_FD_SIZE (GS_HOST_FRAME_HEADER_SIZE + 64)
#define GS_HOST_FRAME_TIMESTAMP_SIZE sizeof(uint32_t)
#define GS_HOST_FRAME_CLASSIC_TS_SIZE (GS_HOST_FRAME_CLASSIC_SIZE + GS_HOST_FRAME_TIMESTAMP_SIZE)
#define GS_HOST_FRAME_FD_TS_SIZE (GS_HOST_FRAME_FD_SIZE + GS_HOST_FRAME_TIMESTAMP_SIZE)
_Static_assert(GS_HOST_FRAME_FD_TS_SIZE == sizeof(struct gs_host_frame), "FD+TS wire size must match struct");
_Static_assert(GS_HOST_FRAME_CLASSIC_TS_SIZE == GS_HOST_FRAME_HEADER_SIZE + 12, "classic+TS wire size");

/* Classic+TS stores timestamp at data[8]; FD+TS uses timestamp_us after data[64]. */
static inline void gs_host_frame_set_timestamp(struct gs_host_frame *frame, bool is_fd, uint32_t timestamp_us)
{
    if (is_fd) {
        frame->timestamp_us = timestamp_us;
    } else {
        memcpy(&frame->data[8], &timestamp_us, sizeof(timestamp_us));
    }
}
