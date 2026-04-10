/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_BLOB_MODEL_API_H_
#define _ESP_BLE_MESH_BLOB_MODEL_API_H_

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_blob_model_api.h"

#if CONFIG_BLE_MESH_BLOB_CLI || CONFIG_BLE_MESH_BLOB_SRV
#ifndef _BLE_MESH_BLOB_DEPRECATE_WARN
#define _BLE_MESH_BLOB_DEPRECATE_WARN
// #warning Please note: All APIs published in this document are in Preview version and may undergo significant changes in the future.
#endif
#endif /* CONFIG_BLE_MESH_BLOB_CLI || CONFIG_BLE_MESH_BLOB_SRV */

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_BLOB_ID_SIZE   8

/** @def    ESP_BLE_MESH_MODEL_BLOB_CLI
 *
 *  @brief  Define a new BLOB Transfer Client model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a BLOB Transfer Client model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New BLOB Transfer Client model instance.
 */
#define ESP_BLE_MESH_MODEL_BLOB_CLI(cli_pub, cli_data)              \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_BLOB_CLI,      \
                               NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_BLOB_SRV
 *
 *  @brief  Define a new BLOB Transfer Server model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a BLOB Transfer Server model.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_blob_trans_srv_t.
 *
 *  @return New BLOB Transfer Server model instance.
 */
#define ESP_BLE_MESH_MODEL_BLOB_SRV(srv_pub, srv_data)              \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_BLOB_SRV,      \
                               NULL, srv_pub, srv_data)


/** BLOB stream. */
typedef struct esp_ble_mesh_blob_io esp_ble_mesh_blob_io_t;

/** @brief BLOB Transfer Server instance. */
typedef struct esp_ble_mesh_blob_srv esp_ble_mesh_blob_srv_t;

/** @brief BLOB Transfer Server model event handlers. */
typedef struct esp_ble_mesh_blob_srv_cb esp_ble_mesh_blob_srv_cb_t;

/** Target node's Pull mode (Pull BLOB Transfer Mode) context used
 *  while sending chunks to the Target node.
 */
typedef struct esp_ble_mesh_blob_target_pull esp_ble_mesh_blob_target_pull_t;

/** BLOB Transfer Client Target node. */
typedef struct esp_ble_mesh_blob_target esp_ble_mesh_blob_target_t;

/** BLOB transfer information. */
typedef struct esp_ble_mesh_blob_xfer_info esp_ble_mesh_blob_xfer_info_t;

/** BLOB Transfer Client transfer inputs. */
typedef struct esp_ble_mesh_blob_cli_inputs esp_ble_mesh_blob_cli_inputs_t;

/** Transfer capabilities of a Target node. */
typedef struct esp_ble_mesh_blob_cli_caps esp_ble_mesh_blob_cli_caps_t;

/**
 * @brief BLOB Transfer Client model instance.
 * @note Preview version, the contents of this struct may change in the future.
 */
typedef struct esp_ble_mesh_blob_cli esp_ble_mesh_blob_cli_t;

/** Event handler callbacks for the BLOB Transfer Client model. */
typedef struct esp_ble_mesh_blob_cli_cb esp_ble_mesh_blob_cli_cb_t;

/** BLOB transfer. */
typedef struct esp_ble_mesh_blob_xfer esp_ble_mesh_blob_xfer_t;

/** BLOB transfer data block. */
typedef struct esp_ble_mesh_blob_block esp_ble_mesh_blob_block_t;

/** BLOB data chunk. */
typedef struct esp_ble_mesh_blob_chunk esp_ble_mesh_blob_chunk_t;

/** Transfer phase. */
typedef enum esp_ble_mesh_blob_xfer_phase {
    ESP_BLE_MESH_BLOB_XFER_PHASE_INACTIVE,              /*!< The BLOB Transfer Server is awaiting configuration. */
    ESP_BLE_MESH_BLOB_XFER_PHASE_WAITING_FOR_START,     /*!< The BLOB Transfer Server is ready to receive a BLOB transfer. */
    ESP_BLE_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK,     /*!< The BLOB Transfer Server is waiting for the next block of data. */
    ESP_BLE_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK,     /*!< The BLOB Transfer Server is waiting for the next chunk of data. */
    ESP_BLE_MESH_BLOB_XFER_PHASE_COMPLETE,              /*!< The BLOB was transferred successfully. */
    ESP_BLE_MESH_BLOB_XFER_PHASE_SUSPENDED,             /*!< The BLOB transfer is paused. */
} esp_ble_mesh_blob_xfer_phase_t;

/** BLOB transfer mode. */
typedef enum esp_ble_mesh_blob_xfer_mode {
    ESP_BLE_MESH_BLOB_XFER_MODE_NONE,                   /*!< No valid transfer mode. */
    ESP_BLE_MESH_BLOB_XFER_MODE_PUSH,                   /*!< Push mode (Push BLOB Transfer Mode). */
    ESP_BLE_MESH_BLOB_XFER_MODE_PULL,                   /*!< Pull mode (Pull BLOB Transfer Mode). */
    ESP_BLE_MESH_BLOB_XFER_MODE_ALL,                    /*!< Both modes are valid. */
} esp_ble_mesh_blob_xfer_mode_t;

/** BLOB model status codes. */
typedef enum esp_ble_mesh_blob_status {
    /** The message was processed successfully. */
    ESP_BLE_MESH_BLOB_SUCCESS,
    /** The Block Number field value is not within the range of blocks being
     *  transferred.
     */
    ESP_BLE_MESH_BLOB_ERR_INVALID_BLOCK_NUM,
    /** The block size is smaller than the size indicated by the Min Block
     *  Size Log state or is larger than the size indicated by the Max Block
     *  Size Log state.
     */
    ESP_BLE_MESH_BLOB_ERR_INVALID_BLOCK_SIZE,
    /** The chunk size exceeds the size indicated by the Max Chunk Size
     *  state, or the number of chunks exceeds the number specified by the
     *  Max Total Chunks state.
     */
    ESP_BLE_MESH_BLOB_ERR_INVALID_CHUNK_SIZE,
    /** The operation cannot be performed while the server is in the current
     *  phase.
     */
    ESP_BLE_MESH_BLOB_ERR_WRONG_PHASE,
    /** A parameter value in the message cannot be accepted. */
    ESP_BLE_MESH_BLOB_ERR_INVALID_PARAM,
    /** The message contains a BLOB ID value that is not expected. */
    ESP_BLE_MESH_BLOB_ERR_WRONG_BLOB_ID,
    /** There is not enough space available in memory to receive the BLOB.
     */
    ESP_BLE_MESH_BLOB_ERR_BLOB_TOO_LARGE,
    /** The transfer mode is not supported by the BLOB Transfer Server
     *  model.
     */
    ESP_BLE_MESH_BLOB_ERR_UNSUPPORTED_MODE,
    /** An internal error occurred on the node. */
    ESP_BLE_MESH_BLOB_ERR_INTERNAL,
    /** The requested information cannot be provided while the server is in
     *  the current phase.
     */
    ESP_BLE_MESH_BLOB_ERR_INFO_UNAVAILABLE,
} esp_ble_mesh_blob_status_t;

/** BLOB Transfer Client state. */
typedef enum esp_ble_mesh_blob_cli_state {
    ESP_BLE_MESH_BLOB_CLI_STATE_NONE,               /*!<  No transfer is active.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_CAPS_GET,           /*!<  Retrieving transfer capabilities.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_START,              /*!<  Sending transfer start.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_BLOCK_START,        /*!<  Sending block start.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_BLOCK_SEND,         /*!<  Sending block chunks.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_BLOCK_CHECK,        /*!<  Checking block status.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_XFER_CHECK,         /*!<  Checking transfer status.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_CANCEL,             /*!<  Cancelling transfer.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_SUSPENDED,          /*!<  Transfer is suspended.  */
    ESP_BLE_MESH_BLOB_CLI_STATE_XFER_PROGRESS_GET,  /*!<  Checking transfer progress.  */
} esp_ble_mesh_blob_cli_state_t;

/** BLOB stream interaction mode. */
typedef enum esp_ble_mesh_blob_io_mode {
    ESP_BLE_MESH_BLOB_READ,                                             /*!<  Read data from the stream.  */
    ESP_BLE_MESH_BLOB_WRITE,                                            /*!<  Write data to the stream.  */
} esp_ble_mesh_blob_io_mode_t;

/** BLOB transfer. */
struct esp_ble_mesh_blob_xfer {
    uint64_t id;                                                        /*!<  BLOB ID.  */
    size_t size;                                                        /*!<  Total BLOB size in bytes.  */
    esp_ble_mesh_blob_xfer_mode_t mode;                                 /*!<  BLOB transfer mode.  */
    uint8_t block_size_log;                                             /*!< Logarithmic representation of the block size. */
    uint16_t chunk_size;                                                /*!<  Base chunk size. May be smaller for the last chunk.  */
    /* Enhanced parameters for message context in blob chunks  */
    esp_ble_mesh_msg_enh_params_t chunk_enh_params;
};

/** BLOB transfer data block. */
struct esp_ble_mesh_blob_block {
    size_t size;                                                        /*!<  Block size in bytes  */
    off_t offset;                                                       /*!<  Offset in bytes from the start of the BLOB.  */
    uint16_t number;                                                    /*!<  Block number  */
    uint16_t chunk_count;                                               /*!<  Number of chunks in block.  */
    uint8_t missing[DIV_ROUND_UP(CONFIG_BLE_MESH_BLOB_CHUNK_COUNT_MAX,
                                 8)];                                               /*!<  Bitmap of missing chunks.  */
};

/** BLOB data chunk. */
struct esp_ble_mesh_blob_chunk {
    off_t offset;                                                       /*!<  Offset of the chunk data from the start of the block.  */
    size_t size;                                                        /*!<  Chunk data size.  */
    uint8_t *data;                                                      /*!<  Chunk data.  */
};

/** BLOB stream. */
struct esp_ble_mesh_blob_io {
    /** @brief Open callback.
     *
     *  Called when the reader is opened for reading.
     *
     *  @param io   BLOB stream.
     *  @param xfer BLOB transfer.
     *  @param mode Direction of the stream (read/write).
     *
     *  @return 0 on success, or (negative) error code otherwise.
     */
    int (*open)(const esp_ble_mesh_blob_io_t *io,
                const esp_ble_mesh_blob_xfer_t *xfer,
                esp_ble_mesh_blob_io_mode_t mode);

    /** @brief Close callback.
     *
     *  Called when the reader is closed.
     *
     *  @param io   BLOB stream.
     *  @param xfer BLOB transfer.
     */
    void (*close)(const esp_ble_mesh_blob_io_t *io,
                  const esp_ble_mesh_blob_xfer_t *xfer);

    /** @brief Block start callback.
     *
     *  Called when a new block is opened for sending. Each block is only
     *  sent once, and are always sent in increasing order. The data chunks
     *  inside a single block may be requested out of order and multiple
     *  times.
     *
     *  @param io    BLOB stream.
     *  @param xfer  BLOB transfer.
     *  @param block Block that was started.
     */
    int (*block_start)(const esp_ble_mesh_blob_io_t *io,
                       const esp_ble_mesh_blob_xfer_t *xfer,
                       const esp_ble_mesh_blob_block_t *block);

    /** @brief Block end callback.
     *
     *  Called when the current block has been transmitted in full.
     *  No data from this block will be requested again, and the application
     *  data associated with this block may be discarded.
     *
     *  @param io    BLOB stream.
     *  @param xfer  BLOB transfer.
     *  @param block Block that finished sending.
     */
    void (*block_end)(const esp_ble_mesh_blob_io_t *io,
                      const esp_ble_mesh_blob_xfer_t *xfer,
                      const esp_ble_mesh_blob_block_t *block);

    /** @brief Chunk data write callback.
     *
     *  Used by the BLOB Transfer Server on incoming data.
     *
     *  Each block is divided into chunks of data. This callback is called
     *  when a new chunk of data is received. Chunks may be received in
     *  any order within their block.
     *
     *  If the callback returns successfully, this chunk will be marked as
     *  received, and will not be received again unless the block is
     *  restarted due to a transfer suspension. If the callback returns a
     *  non-zero value, the chunk remains unreceived, and the BLOB Transfer
     *  Client will attempt to resend it later.
     *
     *  Note that the Client will only perform a limited number of attempts
     *  at delivering a chunk before dropping a Target node from the transfer.
     *  The number of retries performed by the Client is implementation
     *  specific.
     *
     *  @param io    BLOB stream.
     *  @param xfer  BLOB transfer.
     *  @param block Block the chunk is part of.
     *  @param chunk Received chunk.
     *
     *  @return 0 on success, or (negative) error code otherwise.
     */
    int (*write)(const esp_ble_mesh_blob_io_t *io,
                 const esp_ble_mesh_blob_xfer_t *xfer,
                 const esp_ble_mesh_blob_block_t *block,
                 const esp_ble_mesh_blob_chunk_t *chunk);

    /** @brief Chunk data read callback.
     *
     *  Used by the BLOB Transfer Client to fetch outgoing data.
     *
     *  The Client calls the chunk data request callback to populate a chunk
     *  message going out to the Target nodes. The data request callback
     *  may be called out of order and multiple times for each offset, and
     *  cannot be used as an indication of progress.
     *
     *  Returning a non-zero status code on the chunk data request callback
     *  results in termination of the transfer.
     *
     *  @param io    BLOB stream.
     *  @param xfer  BLOB transfer.
     *  @param block Block the chunk is part of.
     *  @param chunk Chunk to get the data of. The buffer pointer to by the
     *               @c data member should be filled by the callback.
     *
     *  @return 0 on success, or (negative) error code otherwise.
     */
    int (*read)(const esp_ble_mesh_blob_io_t *io,
                const esp_ble_mesh_blob_xfer_t *xfer,
                const esp_ble_mesh_blob_block_t *block,
                const esp_ble_mesh_blob_chunk_t *chunk);
};

/** @brief BLOB Transfer Server model event handlers.
 *
 *  All callbacks are optional.
 */
struct esp_ble_mesh_blob_srv_cb {
    /** @brief Transfer start callback.
     *
     *  Called when the transfer has started with the prepared BLOB ID.
     *
     *  @param srv  BLOB Transfer Server instance.
     *  @param ctx  Message context for the incoming start message. The
     *              entire transfer will be sent from the same source
     *              address.
     *  @param xfer Transfer parameters.
     *
     *  @return 0 on success, or (negative) error code to reject the
     *          transfer.
     */
    int (*start)(esp_ble_mesh_blob_srv_t *srv, esp_ble_mesh_msg_ctx_t *ctx,
                 esp_ble_mesh_blob_xfer_t *xfer);

    /** @brief Transfer end callback.
     *
     *  Called when the transfer ends, either because it was cancelled, or
     *  because it finished successfully. A new transfer may be prepared.
     *
     *  @note The transfer may end before it's started if the start
     *        parameters are invalid.
     *
     *  @param srv     BLOB Transfer Server instance.
     *  @param id      BLOB ID of the cancelled transfer.
     *  @param success Whether the transfer was successful.
     */
    void (*end)(esp_ble_mesh_blob_srv_t *srv, uint64_t id, bool success);

    /** @brief Transfer suspended callback.
     *
     *  Called if the Server timed out while waiting for a transfer packet.
     *  A suspended transfer may resume later from the start of the current
     *  block. Any received chunks in the current block should be discarded,
     *  they will be received again if the transfer resumes.
     *
     *  The transfer will call @c resumed again when resuming.
     *
     *  @note The BLOB Transfer Server does not run a timer in the suspended state,
     *        and it's up to the application to determine whether the
     *        transfer should be permanently cancelled. Without interaction,
     *        the transfer will be suspended indefinitely, and the BLOB Transfer
     *        Server will not accept any new transfers.
     *
     *  @param srv BLOB Transfer Server instance.
     */
    void (*suspend)(esp_ble_mesh_blob_srv_t *srv);

    /** @brief Transfer resume callback.
     *
     *  Called if the transfer is resumed after being suspended.
     *
     *  @param srv BLOB Transfer Server instance.
     */
    void (*resume)(esp_ble_mesh_blob_srv_t *srv);

    /** @brief Transfer recovery callback.
     *
     *  Called when the Bluetooth mesh subsystem is started if the device is rebooted
     *  in the middle of a transfer.
     *
     *  Transfers will not be resumed after a reboot if this callback is not
     *  defined.
     *
     *  @param srv  BLOB Transfer Server instance.
     *  @param xfer Transfer to resume.
     *  @param io   BLOB stream return parameter. Must be set to a valid
     *              BLOB stream by the callback.
     *
     *  @return 0 on success, or (negative) error code to abandon the
     *          transfer.
     */
    int (*recover)(esp_ble_mesh_blob_srv_t *srv,
                   esp_ble_mesh_blob_xfer_t *xfer,
                   esp_ble_mesh_blob_io_t **io);
};

#if defined(CONFIG_BLE_MESH_BLOB_SRV)
#define ESP_BLE_MESH_BLOB_BLOCKS_MAX                                \
    (DIV_ROUND_UP(CONFIG_BLE_MESH_BLOB_SIZE_MAX,                    \
              CONFIG_BLE_MESH_BLOB_BLOCK_SIZE_MIN))
#else
#define ESP_BLE_MESH_BLOB_BLOCKS_MAX 1
#endif

/** @brief BLOB Transfer Server instance. */
struct esp_ble_mesh_blob_srv {
    /** Event handler callbacks. */
    esp_ble_mesh_blob_srv_cb_t *cb;

    /** Runtime state: */
    esp_ble_mesh_blob_io_t *io;
    struct k_delayed_work rx_timeout;
    esp_ble_mesh_blob_block_t block;
    esp_ble_mesh_model_t *mod;
    esp_ble_mesh_blob_xfer_phase_t phase;

    /** State of blob serber. */
    struct esp_ble_mesh_blob_srv_state {
        esp_ble_mesh_blob_xfer_t xfer;
        uint16_t cli;
        uint16_t app_idx;
        uint16_t timeout_base;
        uint16_t mtu_size;
        uint8_t ttl;

        /* Bitfield of pending blocks. */
        BLE_MESH_ATOMIC_DEFINE(blocks, ESP_BLE_MESH_BLOB_BLOCKS_MAX);
    } state;

    /* Pull mode (Pull BLOB Transfer Mode) behavior. */
    struct {
        uint16_t chunk_idx;
        struct k_delayed_work report;
    } pull;
};

/** Target node's Pull mode (Pull BLOB Transfer Mode) context used
 *  while sending chunks to the Target node.
 */
struct esp_ble_mesh_blob_target_pull {
    int64_t block_report_timestamp;                                             /*!<  Timestamp when the Block Report Timeout Timer expires for this Target node. */
    uint8_t missing[DIV_ROUND_UP(CONFIG_BLE_MESH_BLOB_CHUNK_COUNT_MAX, 8)];     /*!<  Missing chunks reported by this Target node. */
};

/** BLOB Transfer Client Target node. */
struct esp_ble_mesh_blob_target {
    sys_snode_t n;                                                              /*!<  Linked list node  */

    uint16_t addr;                                                              /*!<  Target node address.  */

    /** Target node's Pull mode context.
     *  Needs to be initialized when sending a BLOB in Pull mode.
     */
    esp_ble_mesh_blob_target_pull_t *pull;

    uint8_t status;                                                             /*!<  BLOB transfer status, see esp_ble_mesh_blob_status.  */

    uint8_t procedure_complete: 1,                                              /*!< Procedure has been completed. */
            acked: 1,                                                           /*!< Message has been acknowledged. Not used when sending. */
            timedout: 1,                                                        /*!< Target node didn't respond after specified timeout. */
            skip: 1;                                                            /*!< Skip Target node from broadcast. */
};


/** BLOB transfer information. */
struct esp_ble_mesh_blob_xfer_info {
    esp_ble_mesh_blob_status_t status;           /*!<  BLOB transfer status. */
    esp_ble_mesh_blob_xfer_mode_t mode;          /*!<  BLOB transfer mode. */
    esp_ble_mesh_blob_xfer_phase_t phase;        /*!<  BLOB transfer phase. */
    uint64_t id;                                 /*!<  BLOB ID. */
    uint32_t size;                               /*!<  BLOB size in octets. */
    uint8_t block_size_log;                      /*!<  Logarithmic representation of the block size. */
    uint16_t mtu_size;                           /*!<  MTU size in octets. */
    const uint8_t *missing_blocks;               /*!<  Bit field indicating blocks that were not received. */
};


/** BLOB Transfer Client transfer inputs. */
struct esp_ble_mesh_blob_cli_inputs {
    /** Linked list of Target nodes. Each node should point to
     *  esp_ble_mesh_blob_target_t::n.
     */
    sys_slist_t targets;

    /** AppKey index to send with. */
    uint16_t app_idx;

    /** Group address destination for the BLOB transfer, or
     *  ESP_BLE_MESH_ADDR_UNASSIGNED to send every message to each Target
     *  node individually.
     */
    uint16_t group;

    /** Time to live value of BLOB transfer messages. */
    uint8_t ttl;

    /** Additional response time for the Target nodes, in 10-second increments.
     *
     *  The extra time can be used to give the Target nodes more time to respond
     *  to messages from the Client. The actual timeout will be calculated
     *  according to the following formula:
     *
     *  @verbatim
     *  timeout = 20 seconds + (10 seconds * timeout_base) + (100 ms * TTL)
     *  @endverbatim
     *
     *  If a Target node fails to respond to a message from the Client within the
     *  configured transfer timeout, the Target node is dropped.
     */
    uint16_t timeout_base;
};

/** Transfer capabilities of a Target node. */
struct esp_ble_mesh_blob_cli_caps {
    size_t max_size;                /*!<  Max BLOB size.  */
    uint8_t min_block_size_log;     /*!<  Logarithmic representation of the minimum block size.  */
    uint8_t max_block_size_log;     /*!<  Logarithmic representation of the maximum block size.  */
    uint16_t max_chunks;            /*!<  Max number of chunks per block.  */
    uint16_t max_chunk_size;        /*!<  Max chunk size.  */
    uint16_t mtu_size;              /*!<  Max MTU size.  */
    esp_ble_mesh_blob_xfer_mode_t modes;  /*!<  Supported transfer modes.  */
};

/** Event handler callbacks for the BLOB Transfer Client model.
 *
 *  All handlers are optional.
 */
struct esp_ble_mesh_blob_cli_cb {
    /** @brief Capabilities retrieval completion callback.
     *
     *  Called when the capabilities retrieval procedure completes, indicating that
     *  a common set of acceptable transfer parameters have been established
     *  for the given list of Target nodes. All compatible Target nodes have
     *  status code ESP_BLE_MESH_BLOB_SUCCESS.
     *
     *  @param cli     BLOB Transfer Client instance.
     *  @param caps    Safe transfer capabilities if the transfer capabilities
     *                 of at least one Target node has satisfied the Client, or NULL otherwise.
     */
    void (*caps)(esp_ble_mesh_blob_cli_t *cli,
                 const esp_ble_mesh_blob_cli_caps_t *caps);

    /** @brief Target node loss callback.
     *
     *  Called whenever a Target node has been lost due to some error in the
     *  transfer. Losing a Target node is not considered a fatal error for
     *  the Client until all Target nodes have been lost.
     *
     *  @param cli    BLOB Transfer Client instance.
     *  @param target Target node that was lost.
     *  @param reason Reason for the Target node loss.
     */
    void (*lost_target)(esp_ble_mesh_blob_cli_t *cli,
                        esp_ble_mesh_blob_target_t *target,
                        esp_ble_mesh_blob_status_t reason);

    /** @brief Transfer is suspended.
     *
     * Called when the transfer is suspended due to response timeout from all Target nodes.
     *
     * @param cli    BLOB Transfer Client instance.
     */
    void (*suspend)(esp_ble_mesh_blob_cli_t *cli);

    /** @brief Transfer end callback.
     *
     *  Called when the transfer ends.
     *
     *  @param cli     BLOB Transfer Client instance.
     *  @param xfer    Completed transfer.
     *  @param success Status of the transfer.
     *                 Is @c true if at least one Target
     *                 node received the whole transfer.
     */
    void (*end)(esp_ble_mesh_blob_cli_t *cli,
                const esp_ble_mesh_blob_xfer_t *xfer, bool success);

    /** @brief Transfer progress callback
     *
     * The content of @c info is invalidated upon exit from the callback.
     * Therefore it needs to be copied if it is planned to be used later.
     *
     *  @param cli      BLOB Transfer Client instance.
     *  @param target   Target node that responded to the request.
     *  @param info     BLOB transfer information.
     */
    void (*xfer_progress)(esp_ble_mesh_blob_cli_t *cli,
                          esp_ble_mesh_blob_target_t *target,
                          const esp_ble_mesh_blob_xfer_info_t *info);

    /** @brief End of Get Transfer Progress procedure.
     *
     * Called when all Target nodes have responded or the procedure timed-out.
     *
     *  @param cli     BLOB Transfer Client instance.
     */
    void (*xfer_progress_complete)(esp_ble_mesh_blob_cli_t *cli);
};

/** @cond INTERNAL_HIDDEN */
typedef struct {
    /** Called for every Target node in unicast mode, or once in case of multicast mode. */
    void (*send)(esp_ble_mesh_blob_cli_t *cli, uint16_t dst);
    /** Called after every @ref blob_cli_broadcast_ctx::send callback. */
    void (*send_complete)(esp_ble_mesh_blob_cli_t *cli, uint16_t dst);
    /** If @ref blob_cli_broadcast_ctx::acked is true, called after all Target nodes
     *  have confirmed reception by @ref blob_cli_broadcast_rsp. Otherwise, called
     *  after transmission has been completed.
     */
    void (*next)(esp_ble_mesh_blob_cli_t *cli);
    /** If true, every transmission needs to be confirmed by @ref blob_cli_broadcast_rsp before
     * @ref blob_cli_broadcast_ctx::next is called.
     */
    bool acked;
    /** If true, the message is always sent in a unicast way. */
    bool force_unicast;
    /** If true, non-responsive Target nodes won't be dropped after transfer has timed out. */
    bool optional;
    /** Set to true by the BLOB Transfer Client between blob_cli_broadcast
     *  and broadcast_complete calls.
     */
    bool is_inited;
    /* Defines a time in ms by which the broadcast API postpones sending the message to a next
     * target or completing the broadcast.
     */
    uint32_t post_send_delay_ms;
} esp_blob_cli_broadcast_ctx_t;
/** INTERNAL_HIDDEN @endcond */

/**
 * @brief BLOB Transfer Client model instance.
 * @note Preview version, the contents of this struct may change in the future.
 */
struct esp_ble_mesh_blob_cli {
    /** Event handler callbacks */
    const esp_ble_mesh_blob_cli_cb_t *cb;

    /** Runtime state */
    const esp_ble_mesh_model_t *mod;

    /** Firmware transfer information */
    struct {
        esp_ble_mesh_blob_target_t *target;
        esp_blob_cli_broadcast_ctx_t ctx;
        struct k_delayed_work retry;
        /* Represents Client Timeout timer in a timestamp. Used in Pull mode only. */
        int64_t cli_timestamp;
        struct k_work complete;
        uint16_t pending;
        uint8_t retries;
        uint8_t sending : 1,
                cancelled : 1;
    } tx;

    /** Firmware IO operation callbacks */
    const esp_ble_mesh_blob_io_t *io;

    /** BLOB Transfer Client transfer inputs. */
    const esp_ble_mesh_blob_cli_inputs_t *inputs;

    /** BLOB transfer. */
    const esp_ble_mesh_blob_xfer_t *xfer;

    /** Interval between chunk transmissions, in milliseconds. */
    uint32_t chunk_interval_ms;

    /** BLOCK count */
    uint16_t block_count;

    /** CHUNK index */
    uint16_t chunk_idx;

    /** Max Transfer Unit Size of BLOB Transfer */
    uint16_t mtu_size;

    /** State of blob client mode transfer */
    esp_ble_mesh_blob_cli_state_t state;

    /** BLOB transfer data block. */
    esp_ble_mesh_blob_block_t block;

    /** Transfer capabilities of a Target node. */
    esp_ble_mesh_blob_cli_caps_t caps;
};

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_BLOB_MODEL_API_H_ */
