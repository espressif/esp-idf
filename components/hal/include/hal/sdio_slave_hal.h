/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SDIO slave (common part)

// SDIO slave HAL usages:

/*
Architecture:

The whole SDIO slave peripheral consists of three parts: the registers (including the interrupt
control and shared registers), a send FIFO, and a receive FIFO. The document
``esp_slave_protocol.rst`` describes the functionality of the peripheral in detail. An SDIO host
will only ever access one of the three parts at any one time, thus the hardware functionality of
the SDIO slave peripheral are completely independent. Likewise, this HAL is organized in such a
fashion as to correspond to the three independent parts.

The shared registers are quite simple: the slave can directly access them from the internal data
bus, while the host can access them by CMD52/53 with the correct address. As for the interrupts:
when an SDIO host interrupts the SDIO slave peripheral (by writing a command), the corresponding
bit in the interrupt register will be set; when the SDIO slave peripheral needs to interrupt the
host, it write some register to cause the host interrupt bit being set, and the slave hardware
will output the interrupt signal on the DAT1 line.

For the FIFOs, the peripheral provides counters as registers so that the host can always know whether the slave
is ready to send/receive data. The HAL resets the counters during initialization, and the host should somehow
inform the slave to reset the counters again if it should reboot (or lose the counter value for some reasons).
Then the host can read/write the FIFOs by CMD53 commands according to the counters.

In order to avoid copying data to/from the FIFOs or memory buffers each time, the HAL layer
contains a descriptor queue (implemented as linked-list) that allows descriptors of memory
buffers to be queued for transmission/reception. Once a buffer is queued, the HAL takes ownership
of the buffer until some "finish" functions successfully return, indicating the
transmission/reception of that buffer is complete. The ISR is invoked multiple times to iterate
through the queued descriptors, and also to signal to the upper layer if a buffer has been
freed.

The HAL is used as below:

- Receiving part:

    1.  Call `sdio_slave_hal_recv_start` to start the receiving DMA.

        If there are already buffers loaded, the receiving will start from those buffers first.

    2.  Call `sdio_slave_hal_recv_init_desc` with a `sdio_slave_hal_recv_desc_t` and the buffer address to
        associate the descriptor with the buffer.

        The HAL initialize this descriptors with the determined length and maybe some extra data.

    3.  Call `sdio_slave_hal_load_buf` with the initialized descriptor of the buffer to load a
        receiving buffer to the HAL.

        When the DMA is started, the descriptors is loaded onto the DMA linked-list, and the
        counter of receiving buffers is increased so that the host will know this by the
        receiving interrupt. The hardware will automatically go through the linked list and write
        data into the buffers loaded on the list.

    4.  (Optional, mandatory only when interrupt enabled) Call `sdio_slave_hal_recv_done` to check
        and clear the receiving interrupt bits.

    5.  Call `sdio_slave_hal_recv_has_next_item` to check whether there are finished buffers.

    6.  Call `sdio_slave_hal_recv_unload_desc` for the same times as
        `sdio_slave_hal_recv_has_next_item` successfully returns.

    7.  (Optional) Call `sdio_slave_hal_recv_reset_counter` to reset the counter to current loaded
        but not used buffers if you want to reset the counter only. This is available only when
        the DMA is stopped.

    8.  (Optional) Call `sdio_slave_hal_recv_flush_one_buffer` (recursively) if you want to
        discard data of one (or more) buffers and load them again. This is available only when
        the DMA is stopped.

    9.  (Optional when deinitialization) Call `sdio_slave_hal_recv_unload_desc` recursively to get
        all the buffers loaded to the HAL, no matter they are used or not. Don't do this when the
        DMA is not stopped.

- Sending part:

    The sending driver is slightly different, since we are not using the re-start feature.
    (TODO: re-write this part if the stitch mode is released)

    1.  Call `sdio_slave_hal_send_start` to start the sending DMA.

        If there is already any data queued, it will ne ready to be sent to host now.

    2.  Call `sdio_slave_hal_send_queue` to queue the data to send.

        If the interrupt is enabled, the ISR will be invoked.

    3.  (Required if interrupt enabled) Call `` to clear the interrupt bits used by the SW
        invoking logic.

    4.  Call `sdio_slave_hal_send_new_packet_if_exist` to check and send new packet (if there is
        data queued).

    5.  Call `sdio_slave_hal_send_eof_happened` to check whether the previous packet is done.

        It will also clear the interrupt status bit for this event.

    6.  Call `sdio_slave_hal_send_get_next_finished_arg` recursively to get the arguments for the
        finished buffers.

    7.  (Optional when deinitialization) Call `sdio_slave_hal_send_flush_next_buffer` recursively
        to get all buffers queued, regardless sent or not. Don't do this when the DMA is not stopped.

    8.  (Optional) Call `sdio_slave_hal_send_reset_counter` to reset the counter to current loaded
        but not sent buffers if you want to reset the counter only. Don't do this when the DMA is not
        stopped.

    Note a counter should be used when performing step 2 and 6, to make sure that the queue size
    is enough.

- Host part:

    1.  Call `sdio_slave_hal_hostint_set_ena` and `sdio_slave_hal_hostint_get_ena` to
        enable/disable the interrupt sent to master. Note that the host can also modify the same
        registers at the same time. Try to avoid using them outside the initialization process.

    2.  Call `sdio_slave_hal_hostint_send` and `sdio_slave_hal_hostint_clear` to trigger general
        purpose interrupts or cancel all kinds of interrupts send to the host. These interrupts are
        set/cleared in a concurrent-safe way, so the slave can call these functions safely.

    3.  Call `sdio_slave_hal_slvint_fetch_clear` to fetch the general purpose interrupts sent by
        the host to the slave. These interrupts will also be cleared after the calls.

    4.  Call `sdio_slave_hal_host_get_reg` and `sdio_slave_hal_host_set_reg` to read/write the
        general purpose shared between the host and slave. Note that these registers are also not
        concurrent-safe. Try not to write to the same register from two directions at the same time.
*/

#pragma once
#include <esp_err.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#if SOC_SDIO_SLAVE_SUPPORTED
#include "hal/sdio_slave_types.h"
#include "hal/sdio_slave_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SDIO_SLAVE_SUPPORTED
/// Space used for each sending descriptor. Should initialize the sendbuf accoring to this size.
#define SDIO_SLAVE_SEND_DESC_SIZE   sizeof(sdio_slave_hal_send_desc_t)


/// Status of the sending part
typedef enum {
    STATE_IDLE = 1,
    STATE_WAIT_FOR_START = 2,
    STATE_SENDING = 3,
    STATE_GETTING_RESULT = 4,
    STATE_GETTING_UNSENT_DESC = 5,
} send_state_t;

typedef struct {
    uint8_t* data;      ///< Address of the buffer
    size_t  size;       ///< Size of the buffer, but can only queue (size/SDIO_SLAVE_SEND_DESC_SIZE)-1 descriptors
    uint8_t* write_ptr;
    uint8_t* read_ptr;
    uint8_t* free_ptr;
} sdio_ringbuf_t;

// Append two extra words to be used by the HAL.
// Should Initialize the member `data` of `send_desc_queue` of the HAL context
// with size of this desc * N.

/// DMA descriptor with extra fields
typedef struct sdio_slave_hal_send_desc_s {
    sdio_slave_ll_desc_t dma_desc;    ///< Used by Hardware, has pointer linking to next desc
    uint32_t pkt_len;     ///< Accumulated length till this descriptor
    void*   arg;          ///< Holding arguments indicating this buffer */
} sdio_slave_hal_send_desc_t;

/// Descriptor used by the receiving part, call `sdio_slave_hal_recv_init_desc`
/// to initialize it before use.
typedef sdio_slave_ll_desc_t sdio_slave_hal_recv_desc_t;
#define sdio_slave_hal_recv_desc_s sdio_slave_ll_desc_s
typedef STAILQ_HEAD(recv_stailq_head_s, sdio_slave_hal_recv_desc_s) sdio_slave_hal_recv_stailq_t;


/** HAL context structure. Call `sdio_slave_hal_init` to initialize it and
 * configure required members before actually use the HAL.
 */
typedef struct {
    /// Hardware registers for this SDIO slave peripheral, configured by
    /// `sdio_slave_hal_init`
    struct {
        slc_dev_t*      slc;
        host_dev_t*     host;
        hinf_dev_t*     hinf;
    };
    sdio_slave_sending_mode_t sending_mode; /**< Sending mode, should be manually configured before using the HAL.
                                             * see `sdio_slave_sending_mode_t`.
                                             */
    sdio_slave_timing_t timing;             /**< Timing mode (launch edge and latch edge settings). Should be manually
                                             * configured before using the HAL. `SDIO_SLAVE_TIMING_PSEND_PSAMPLE` is
                                             * recommended by default.
                                             */
    //some boolean flags
    struct {
        uint32_t no_highspeed: 1;           /**< Disable the highspeed support */
    };
    int                 send_queue_size;    /**< Max buffers that can be queued before sending. Should be manually
                                             * configured before using the HAL.
                                             */
    size_t              recv_buffer_size;   /**< The size of each buffer. The host and slave should share a
                                             * pre-negotiated value. Should be manually configured before using
                                             * the HAL.
                                             */
    sdio_ringbuf_t      send_desc_queue;            /**< The ring buffer used to hold queued descriptors. Should be manually
                                             * initialized before using the HAL.
                                             */

    //Internal status, no need to touch.
    send_state_t        send_state;         // Current state of sending part.
    uint32_t            tail_pkt_len;       // The accumulated send length of the tail packet.
    sdio_slave_hal_send_desc_t*         in_flight_head; // The head of linked list in-flight.
    sdio_slave_hal_send_desc_t*         in_flight_end;  // The end of linked list in-flight.
    sdio_slave_hal_send_desc_t*         in_flight_next; // The header of linked list to be sent next time.
    sdio_slave_hal_send_desc_t*         returned_desc;  // The last returned descriptor

    sdio_slave_hal_recv_stailq_t            recv_link_list; // Linked list of buffers ready to hold data and the buffers already hold data.
    volatile sdio_slave_hal_recv_desc_t*    recv_cur_ret;   // Next desc to return, NULL if all loaded descriptors are returned.
} sdio_slave_context_t ;

/**
 * Initialize the HAL, should provide buffers to the context and configure the
 * members before this funciton is called.
 *
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_init(sdio_slave_context_t *hal);

/**
 * Initialize the SDIO slave peripheral hardware.
 *
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_hw_init(sdio_slave_context_t *hal);

/**
 * Set the IO ready for host to read.
 *
 * @param hal Context of the HAL layer.
 * @param ready true to tell the host the slave is ready, otherwise false.
 */
void sdio_slave_hal_set_ioready(sdio_slave_context_t *hal, bool ready);

/*---------------------------------------------------------------------------
 *                  Send
 *--------------------------------------------------------------------------*/

/**
 * The hardware sending DMA starts. If there is existing data, send them.
 *
 * @param hal Context of the HAL layer.
 */
esp_err_t sdio_slave_hal_send_start(sdio_slave_context_t *hal);

/**
 * Stops hardware sending DMA.
 *
 * @note The data in the queue, as well as the counter are not touched.
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_send_stop(sdio_slave_context_t *hal);

/**
 * Put some data into the sending queue.
 *
 * @note The caller should keeps the buffer, until the `arg` is returned by
 *       `sdio_slave_hal_send_get_next_finished_arg`.
 * @note The caller should count to ensure there is enough space in the queue.
 *       The initial queue size is sizeof(sendbuf.data)/sizeof(sdio_slave_hal_send_desc_t)-1,
 *       Will decrease by one when this function successfully returns.
 *       Released only by `sdio_slave_hal_send_get_next_finished_arg` or
 *       `sdio_slave_hal_send_flush_next_buffer`.
 *
 * @note The HAL is not thread-safe. The caller should use a spinlock to ensure
 *       the `sdio_slave_hal_send_queue` and ... are not called at the same time.
 *
 * @param hal Context of the HAL layer.
 * @param addr Address of data in the memory to send.
 * @param len Length of data to send.
 * @param arg Argument indicating this sending.
 * @return Always ESP_OK.
 */
esp_err_t sdio_slave_hal_send_queue(sdio_slave_context_t *hal, uint8_t *addr, size_t len, void *arg);

/**
 * The ISR should call this, to handle the SW invoking event.
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_send_handle_isr_invoke(sdio_slave_context_t *hal);

/**
 * Check whether there is no in-flight transactions, and send new packet if there
 * is new packets queued.
 *
 * @param hal Context of the HAL layer.
 * @return
 *  - ESP_OK: The DMA starts to send a new packet.
 *  - ESP_ERR_NOT_FOUND: No packet waiting to be sent.
 *  - ESP_ERR_INVALID_STATE: There is packet in-flight.
 */
esp_err_t sdio_slave_hal_send_new_packet_if_exist(sdio_slave_context_t *hal);

/**
 * Check whether the sending EOF has happened and clear the interrupt.
 *
 * Call `sdio_slave_hal_send_get_next_finished_arg` recursively to retrieve arguments of finished
 * buffers.
 *
 * @param hal Context of the HAL layer.
 * @return true if happened, otherwise false.
 */
bool sdio_slave_hal_send_eof_happened(sdio_slave_context_t *hal);

/**
 * Get the arguments of finished packets. Call recursively until all finished
 * arguments are all retrieved.
 *
 * @param hal Context of the HAL layer.
 * @param out_arg Output argument of the finished buffer.
 * @param out_returned_cnt Released queue size to be queued again.
 * @return
 *  - ESP_OK: if one argument retrieved.
 *  - ESP_ERR_NOT_FOUND: All the arguments of the finished buffers are retrieved.
 */
esp_err_t sdio_slave_hal_send_get_next_finished_arg(sdio_slave_context_t *hal, void **out_arg, uint32_t* out_returned_cnt);

/**
 * Flush one buffer in the queue, no matter sent, canceled or not sent yet.
 *
 * Call recursively to clear the whole queue before deinitialization.
 *
 * @note Only call when the DMA is stopped!
 * @param hal Context of the HAL layer.
 * @param out_arg Argument indiciating the buffer to send
 * @param out_return_cnt Space in the queue released after this descriptor is flushed.
 * @return
 *  - ESP_ERR_INVALID_STATE: This function call be called only when the DMA is stopped.
 *  - ESP_ERR_NOT_FOUND: if no buffer in the queue
 *  - ESP_OK: if a buffer is successfully flushed and returned.
 */
esp_err_t sdio_slave_hal_send_flush_next_buffer(sdio_slave_context_t *hal, void **out_arg, uint32_t *out_return_cnt);

/**
 * Walk through all the unsent buffers and reset the counter to the accumulated length of them. The data will be kept.
 *
 * @note Only call when the DMA is stopped!
 * @param hal Context of the HAL layer.
 * @return
 *  - ESP_ERR_INVALID_STATE: this function call be called only when the DMA is stopped
 *  - ESP_OK: if success
 */
esp_err_t sdio_slave_hal_send_reset_counter(sdio_slave_context_t *hal);


/*---------------------------------------------------------------------------
 *                  Receive
 *--------------------------------------------------------------------------*/
/**
 * Start the receiving DMA.
 *
 * @note If there are already some buffers loaded, will receive from them first.
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_recv_start(sdio_slave_context_t *hal);

/**
 * Stop the receiving DMA.
 *
 * @note Data and the counter will not be touched. You can still call
 *       `sdio_slave_hal_recv_has_next_item` to get the received buffer.
 *       And unused buffers loaded to the HAL will still be in the `loaded`
 *       state in the HAL, until returned by `sdio_slave_hal_recv_unload_desc`.
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_recv_stop(sdio_slave_context_t* hal);

/**
 * Associate the buffer to the descriptor given. The descriptor may also be initialized with some
 * other data.
 *
 * @param hal Context of the HAL layer.
 * @param desc Descriptor to associate with the buffer
 * @param start Start address of the buffer
 */
void sdio_slave_hal_recv_init_desc(sdio_slave_context_t *hal, sdio_slave_hal_recv_desc_t *desc, uint8_t *start);

/**
 * Load the buffer to the HAL to be used to receive data.
 *
 * @note Loaded buffers will be returned to the upper layer only when:
 *       1. Returned by `sdio_slave_hal_recv_has_next_item` when receiving to that buffer successfully
 *       done.
 *       2. Returned by `sdio_slave_hal_recv_unload_desc` unconditionally.
 * @param hal Context of the HAL layer.
 * @param desc Descriptor to load to the HAL to receive.
 */
void sdio_slave_hal_load_buf(sdio_slave_context_t *hal, sdio_slave_hal_recv_desc_t *desc);

/**
 * Check and clear the interrupt indicating a buffer has finished receiving.
 *
 * @param hal Context of the HAL layer.
 * @return true if interrupt triggered, otherwise false.
 */
bool sdio_slave_hal_recv_done(sdio_slave_context_t* hal);

/**
 * Call this function recursively to check whether there is any buffer that has
 * finished receiving.
 *
 * Will walk through the linked list to find a newer finished buffer. For each successful return,
 * it means there is one finished buffer. You can one by `sdio_slave_hal_recv_unload_desc`. You can
 * also call `sdio_slave_hal_recv_has_next_item` several times continuously before you call the
 * `sdio_slave_hal_recv_unload_desc` for the same times.
 *
 * @param hal Context of the HAL layer.
 * @return true if there is
 */
bool sdio_slave_hal_recv_has_next_item(sdio_slave_context_t* hal);

/**
 * Unconditionally remove and return the first descriptor loaded to the HAL.
 *
 * Unless during de-initialization, `sdio_slave_hal_recv_has_next_item` should have succeed for the
 * same times as this function is called, to ensure the returned descriptor has finished its
 * receiving job.
 *
 * @param hal Context of the HAL layer.
 * @return The removed descriptor, NULL means the linked-list is empty.
 */
sdio_slave_hal_recv_desc_t *sdio_slave_hal_recv_unload_desc(sdio_slave_context_t *hal);

/**
 * Walk through all the unused buffers and reset the counter to the number of
 * them.
 *
 * @note Only call when the DMA is stopped!
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_recv_reset_counter(sdio_slave_context_t *hal);

/**
 * Walk through all the used buffers, clear the finished flag and appended them
 * back to the end of the unused list, waiting to receive then.
 *
 * @note You will lose all the received data in the buffer.
 * @note Only call when the DMA is stopped!
 * @param hal Context of the HAL layer.
 */
void sdio_slave_hal_recv_flush_one_buffer(sdio_slave_context_t *hal);


/*---------------------------------------------------------------------------
 *                  Host
 *--------------------------------------------------------------------------*/

/**
 * Enable some of the interrupts for the host.
 *
 * @note May have concurrency issue wit the host or other tasks, suggest only use it during
 *       initialization.
 * @param hal Context of the HAL layer.
 * @param mask Bitwise mask for the interrupts to enable.
 */
void sdio_slave_hal_hostint_set_ena(sdio_slave_context_t *hal, const sdio_slave_hostint_t *mask);

/**
 * Get the enabled interrupts.
 *
 * @param hal Context of the HAL layer.
 * @param out_int_mask Output of the enabled interrupts
 */
void sdio_slave_hal_hostint_get_ena(sdio_slave_context_t *hal, sdio_slave_hostint_t *out_int_mask);

/**
 * Send general purpose interrupt (slave send to host).
 * @param hal Context of the HAL layer.
 * @param mask Interrupts to send, only `SDIO_SLAVE_HOSTINT_BIT*` are allowed.
 */
void sdio_slave_hal_hostint_send(sdio_slave_context_t *hal, const sdio_slave_hostint_t *mask);

/**
 * Cleared the specified interrupts for the host.
 *
 * @param hal Context of the HAL layer.
 * @param mask Interrupts to clear.
 */
void sdio_slave_hal_hostint_clear(sdio_slave_context_t *hal, const sdio_slave_hostint_t *mask);


/**
 * Fetch the interrupt (host send to slave) status bits and clear all of them.
 * @param hal Context of the HAL layer.
 * @param out_int_mask Output interrupt status
 */
void sdio_slave_hal_slvint_fetch_clear(sdio_slave_context_t *hal, sdio_slave_ll_slvint_t *out_int_mask);

/**
 * Get the value of a shared general purpose register.
 *
 * @param hal Context of the HAL layer.
 * @param pos Position of the register, 4 bytes share a word. 0-63 except 24-27.
 * @return The register value.
 */
uint8_t sdio_slave_hal_host_get_reg(sdio_slave_context_t *hal, int pos);

/**
 * Set the value of shared general purpose register.
 *
 * @param hal Context of the HAL layer.
 * @param pos Position of the register, 4 bytes share a word. 0-63 except 24-27.
 * @param reg Value to set.
 */
void sdio_slave_hal_host_set_reg(sdio_slave_context_t *hal, int pos, uint8_t reg);

#endif  // SOC_SDIO_SLAVE_SUPPORTED

#ifdef __cplusplus
}
#endif
