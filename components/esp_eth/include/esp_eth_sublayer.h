/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_ETH_SUBLAYER_SUPPORT

#include <stddef.h>
#include <stdint.h>
#include "esp_eth_driver.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_eth_sublayer_s *esp_eth_sublayer_handle_t;
typedef struct esp_eth_sublayer_vlan_s *esp_eth_sublayer_vlan_handle_t;
#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
typedef struct esp_eth_sublayer_switch_s *esp_eth_sublayer_switch_handle_t;
#endif

#define ESP_ETH_SUBLAYER_UNTAGGED_VID         UINT16_MAX
#define ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY CONFIG_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY

/**
 * @brief Mutable TX buffer descriptor set passed through the sublayer TX path.
 */
typedef struct {
    esp_eth_buf_desc_t *bufs; /*!< Descriptor array */
    size_t *buf_count;        /*!< In/out: number of valid entries in @c bufs */
    const size_t buf_capacity; /*!< Maximum number of entries in @c bufs */
} esp_eth_sublayer_tx_bufs_t;

/**
 * @brief Event info structure for sublayer optional events.
 */
typedef struct
{
    esp_event_base_t base;
    int32_t event_id;
    void *event_data;
} esp_eth_sublayer_event_t;

/**
 * @brief Extra information for RX hook callbacks.
 */
typedef struct {
    void *l2_buffer;  /*!< Starting address of the original Ethernet driverRX allocation */
    void *driver_info; /*!< Opaque per-frame metadata from MAC / stack_input_info, may be NULL */
} esp_eth_sublayer_rx_info_t;

/**
 * @brief Optional TX hook on frames passed to the Ethernet driver (after VLAN tag insert for VLAN netifs).
 *
 * When a VLAN tag is inserted the sublayer splits the frame into two
 * esp_eth_buf_desc_t entries (tagged header + payload) so that no payload
 * memcpy is needed. For untagged frames a single descriptor is passed.
 *
 * The hook may modify the descriptor array in place:
 *  - Change @c buf pointers, @c len values, and descriptor order (e.g. insert a
 *    prefix in @c bufs[0] and move the original frame to @c bufs[1], encrypt into
 *    a new buffer, or merge scattered segments into one contiguous @c bufs[0]).
 *    Use @c post_tx_hook to free any buffers allocated for these edits after the
 *    driver finishes transmitting.
 *  - Adjust *@a tx_bufs->buf_count up or down, but never exceed @a tx_bufs->buf_capacity.
 *  - Must NOT free the input buffers.
 *  - May set *@a tx_bufs->buf_count to 0 to skip driver transmit and @c post_tx_hook
 *    (e.g. the hook handled the frame elsewhere or chose to drop it). The @a tx_bufs
 *    descriptor set remains caller-owned; only transmission is suppressed.
 *
 * @param eth     Underlying Ethernet driver handle
 * @param tx_bufs Mutable buffer descriptor set comprising the frame
 * @param ctx     User context from esp_eth_sublayer_config_t::hook_ctx
 * @return ESP_OK to transmit; other values abort transmit for this packet
 */
typedef esp_err_t (*esp_eth_sublayer_tx_hook_t)(esp_eth_handle_t eth,
                                                      esp_eth_sublayer_tx_bufs_t *tx_bufs,
                                                      void *ctx);

/**
 * @brief Optional post-TX hook called after a successful TX hook, to release resources it allocated.
 *
 * @warning The Tx buffer descriptors may have different order than on the output of the TX hook.
 *
 * Intended for freeing buffers that were allocated by the TX hook.
 * Called whenever the TX hook completed with @a *tx_bufs->buf_count > 0, regardless of the transmit result
 * (or of any failure that occurs after the TX hook and before the actual transmit). This guarantees that
 * buffers allocated by the TX hook are always released.
 *
 * Not called when the TX hook sets @a *tx_bufs->buf_count to 0 (transmit suppressed) or returns non-ESP_OK.
 *
 * @param eth     Underlying Ethernet driver handle
 * @param tx_bufs Buffer descriptor set that was passed to (or prepared for) the driver
 * @param ctx     User context from esp_eth_sublayer_config_t::hook_ctx
 */
typedef void (*esp_eth_sublayer_post_tx_hook_t)(esp_eth_handle_t eth,
                                                const esp_eth_sublayer_tx_bufs_t *tx_bufs,
                                                void *ctx);

/**
 * @brief Optional RX hook on frames from the Ethernet driver (before VLAN demux / input path).
 *
 * The hook may strip a leading protocol header (e.g. MACsec SecTAG) by advancing @p buf forward and
 * decrementing @p len, or remove a trailing trailer by decrementing @p len only.
 * @p buf must remain within the original allocation.
 *
 * Set @c *len to 0 to take ownership of the original allocation and stop further sublayer processing.
 * The hook must then release or forward @c info->alloc_base itself.
 *
 * @param eth        Underlying Ethernet driver handle (when integrated switch is used, this is per port handle)
 * @param[in,out] buf in: frame start; out: frame start with hook-specific header/trailer removed
 * @param[in,out] len frame length; set to 0 to stop further processing
 * @param info       Per-frame RX metadata; never NULL
 * @param ctx        User context from esp_eth_sublayer_config_t::hook_ctx
 * @return ESP_OK to continue processing; other values cause the sublayer to free the allocation and stop
 */
typedef esp_err_t (*esp_eth_sublayer_rx_hook_t)(esp_eth_handle_t eth, uint8_t **buf, uint32_t *len, esp_eth_sublayer_rx_info_t *info, void *ctx);

/**
 * @brief Sublayer configuration structure.
 *
 */
typedef struct
{
    esp_eth_handle_t eth_handle; /*!< Underlying Ethernet driver handle the sublayer is bound to (required) */

    /**
     * Optional event that must be observed (in addition to the physical link being up) before the sublayer
     * reports child netifs as connected. Leave @c base NULL (the default) to bring netifs up as soon as
     * ETHERNET_EVENT_CONNECTED fires, with no extra confirmation needed.
     *
     * The order in which this event and ETHERNET_EVENT_CONNECTED are received is not significant, the child
     * netifs are brought up once both conditions are met. Any subsequent disconnect (ETHERNET_EVENT_DISCONNECTED,
     * ETHERNET_EVENT_STOP or @c disconnect_trigger_event) invalidates the confirmation, i.e. the event needs to
     * be posted again to bring the child netifs up.
     */
    esp_eth_sublayer_event_t connect_confirm_event;

    /**
     * Optional event that, in addition to ETHERNET_EVENT_DISCONNECTED, forces all child netifs down
     * (e.g. an upper-layer authentication/link-partner event). Leave @c base NULL (the default) to only
     * react to ETHERNET_EVENT_DISCONNECTED.
     */
    esp_eth_sublayer_event_t disconnect_trigger_event;

    esp_eth_sublayer_tx_hook_t tx_hook;           /*!< Optional TX hook, see esp_eth_sublayer_tx_hook_t (NULL to disable) */
    esp_eth_sublayer_post_tx_hook_t post_tx_hook; /*!< Optional post-TX hook, see esp_eth_sublayer_post_tx_hook_t (NULL to disable) */
    esp_eth_sublayer_rx_hook_t rx_hook;           /*!< Optional RX hook, see esp_eth_sublayer_rx_hook_t (NULL to disable) */
    void *hook_ctx;                              /*!< User context passed to tx_hook, post_tx_hook, and rx_hook */

    /**
     * Timeout (ms) to acquire the CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX before failing with ESP_ERR_TIMEOUT.
     * 0 uses the built-in default. Ignored when CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX is disabled.
     */
    uint32_t transmit_mutex_timeout_ms;
} esp_eth_sublayer_config_t;

#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
/**
 * @brief Integrated switch "tag process init" callback.
 *
 * Invoked once by the sublayer when it is created. The driver allocates and returns (through @p ctx) all the
 * state it needs to mux/demux Tail Tagged traffic between the single Host Ethernet interface and the per-port
 * Ethernet interfaces. The sublayer owns the returned @p ctx and passes it back to the demux and deinit callbacks.
 *
 * @param[in] host_eth_handle Host Ethernet driver handle (the one the sublayer is bound to)
 * @param[in] port_eth_handles array of per-port Ethernet driver handles
 * @param[in] ports_count number of entries in @p port_eth_handles
 * @param[out] ctx location to store the driver's integrated switch context
 * @return ESP_OK on success, error code otherwise
 */
 typedef esp_err_t (*esp_eth_sublayer_switch_tag_init_t)(esp_eth_handle_t host_eth_handle,
    esp_eth_handle_t *port_eth_handles,
    uint32_t ports_count,
    void **ctx);

/**
* @brief Integrated switch "tag process deinit" callback. Releases the context allocated by the init callback.
*
* @param[in] ctx driver integrated switch context returned by the init callback
* @return ESP_OK on success, error code otherwise
*/
typedef esp_err_t (*esp_eth_sublayer_switch_tag_deinit_t)(void *ctx);

/**
* @brief Integrated switch RX demux callback.
*
* Called by the sublayer at the very beginning of the RX path (before RX hook / L2 TAP / VLAN demux). The driver
* strips its switch tag and reports the ingress port number through @p src_port. The frame is NOT re-routed by
* this callback; the sublayer continues its normal RX pipeline on the now tag-free frame.
*
* Tag stripping strategies:
*  - Trailing tag (e.g. KSZ8863 Tail Tag): decrement @p length.
*  - Leading tag after the MAC header (e.g. DSA, 802.1Q port-VLAN): advance @p buffer forward and decrement
*    @p length (typically after memmove of the 12-byte MAC header over the tag).
*
* The callback must NOT free or realloc the buffer; on error the sublayer frees the original allocation.
* @p buffer must remain within the original allocation.
*
* @param[in] ctx driver integrated switch context returned by the init callback
* @param[in,out] buffer in: frame start including the switch tag; out: frame start with tag removed
* @param[in,out] length frame length including the tag on input, with the tag removed on output
* @param[out] src_port ingress port number (callback may ignore if NULL)
* @return ESP_OK on success, error code otherwise (frame will be dropped by the sublayer)
*/
typedef esp_err_t (*esp_eth_sublayer_switch_demux_t)(void *ctx, uint8_t **buffer, uint32_t *length, int32_t *src_port);

/**
* @brief Integrated switch TX mux callback.
*
* Called by the sublayer during transmit to append switch-specific tagging (e.g. KSZ8863 Tail Tag) to
* @a tx_bufs before the frame is sent on the Host Ethernet interface.
*
* @param[in] ctx driver integrated switch context returned by the init callback
* @param[in,out] tx_bufs buffer descriptor set comprising the frame
* @param port destination port number, or negative for default switch lookup behaviour
* @return ESP_OK on success, error code otherwise
*/
typedef esp_err_t (*esp_eth_sublayer_switch_mux_t)(void *ctx, esp_eth_sublayer_tx_bufs_t *tx_bufs, int32_t port);

/**
 * @brief Integrated switch configuration.
 *
 */
typedef struct
{
    esp_eth_sublayer_switch_tag_init_t tag_process_init;     /*!< Called when the switch is added; returns @c ctx */
    esp_eth_sublayer_switch_tag_deinit_t tag_process_deinit; /*!< Called when the switch is deleted */
    esp_eth_sublayer_switch_demux_t demux;                   /*!< Called per RX frame to strip the tag / resolve the ingress port */
    esp_eth_sublayer_switch_mux_t mux;                       /*!< Called per TX frame to append the switch tag */
    esp_eth_handle_t host_eth_handle;
    esp_eth_handle_t *port_eth_handles;
    uint32_t ports_count;
} esp_eth_sublayer_switch_config_t;
#endif // CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT

/**
 * @brief Default sublayer configuration.
 *
 */
#define ESP_ETH_SUBLAYER_CONFIG_DEFAULT()               \
    {                                                   \
        .eth_handle = NULL,                             \
        .connect_confirm_event = { NULL, 0, NULL },     \
        .disconnect_trigger_event = { NULL, 0, NULL },  \
        .tx_hook = NULL,                                \
        .post_tx_hook = NULL,                           \
        .rx_hook = NULL,                                \
        .hook_ctx = NULL,                               \
        .transmit_mutex_timeout_ms = 0,                 \
    }

/**
 * @brief Create a Ethernet sublayer for an Ethernet driver.
 *
 * @note The interface base sublayer is required to be created. Derived child
 *       interfaces can be used and attached to netifs (e.g. VLAN interfaces).
 *
 * @warning Management of the Ethernet-Netif sublayer is not thread-safe for performance reasons.
 *          Configure it only from a single task/context (for example during initialization).
 *
 * @param config Sublayer configuration
 * @param[out] sublayer Location to store the created sublayer handle
 * @return ESP_OK on success
 */
esp_err_t esp_eth_sublayer_new(const esp_eth_sublayer_config_t *config, esp_eth_sublayer_handle_t *sublayer);

/**
 * @brief Delete Ethernet sublayer and all child entries (VLANs).
 *
 * Caller must detach and destroy esp_netif instances before calling this.
 *
 * @param sublayer Sublayer handle
 * @return ESP_OK on success
 */
esp_err_t esp_eth_sublayer_del(esp_eth_sublayer_handle_t sublayer);

/**
 * @brief Add a VLAN entry to the sublayer (802.1Q VID 0..4095 excluding duplicates).
 *
 * Attach the VLAN netif with esp_netif_attach(esp_netif, returned handle).
 *
 * @warning Management of the Ethernet-Netif sublayer is not thread-safe for performance reasons.
 *          Configure it only from a single task/context (for example during initialization).
 *
 * @param sublayer Sublayer handle
 * @param vlan_id  VLAN ID (12-bit), or ESP_ETH_SUBLAYER_UNTAGGED_VID for the untagged child
 * @param[out] vlan Location to store the created VLAN handle
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if the VLAN ID is already present;
 *         ESP_ERR_NOT_SUPPORTED if @p vlan_id is not ESP_ETH_SUBLAYER_UNTAGGED_VID and
 *         CONFIG_ETH_SUBLAYER_VLAN_SUPPORT is disabled
 */
esp_err_t esp_eth_sublayer_vlan_add(esp_eth_sublayer_handle_t sublayer, uint16_t vlan_id,
                                    esp_eth_sublayer_vlan_handle_t *vlan);

/**
 * @brief Remove a VLAN entry from the sublayer.
 *
 * @param sublayer Sublayer handle
 * @param vlan VLAN handle returned by esp_eth_sublayer_vlan_add
 * @return ESP_ERR_NOT_FOUND if not on this sublayer; ESP_OK otherwise
 */
esp_err_t esp_eth_sublayer_vlan_del(esp_eth_sublayer_handle_t sublayer, esp_eth_sublayer_vlan_handle_t vlan);

#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
/**
 * @brief Add the integrated switch (Tail Tag) processing to the sublayer.
 *
 * A sublayer can hold at most one switch instance. The switch is attached to the sublayer similarly to a VLAN
 * child but it is a single instance rather than a list. The provided @p config callbacks (`tag_process_init`,
 * `demux`, `tag_process_deinit`) are implemented by the integrated switch driver (e.g. KSZ8863). `tag_process_init`
 * is invoked here to create the driver's switch context.
 *
 * @warning Management of the Ethernet-Netif sublayer is not thread-safe for performance reasons.
 *          Configure it only from a single task/context (for example during initialization).
 *
 * @param sublayer Sublayer handle
 * @param config Integrated switch configuration (its own configuration, not part of the sublayer config)
 * @param[out] sw Location to store the created switch handle
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if a switch is already added
 */
esp_err_t esp_eth_sublayer_switch_add(esp_eth_sublayer_handle_t sublayer,
                                      const esp_eth_sublayer_switch_config_t *config,
                                      esp_eth_sublayer_switch_handle_t *sw);

/**
 * @brief Remove the integrated switch from the sublayer.
 *
 * @param sw Switch handle returned by esp_eth_sublayer_switch_add
 * @return ESP_OK on success
 */
esp_err_t esp_eth_sublayer_switch_del(esp_eth_sublayer_switch_handle_t sw);
#endif // CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT

/**
 * @brief Get the underlying Ethernet driver handle from the sublayer.
 *
 * @param sublayer Sublayer handle
 * @return Ethernet driver handle
 */
esp_eth_handle_t esp_eth_sublayer_get_eth_handle(esp_eth_sublayer_handle_t sublayer);

/**
 * @brief Get the underlying Ethernet driver handle from the VLAN child of the sublayer.
 *
 * @param vlan VLAN handle
 * @return Ethernet driver handle
 */
esp_eth_handle_t esp_eth_sublayer_vlan_get_eth_handle(esp_eth_sublayer_vlan_handle_t vlan);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_ETH_SUBLAYER_SUPPORT
