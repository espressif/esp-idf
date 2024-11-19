/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_CLI_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_CLI_H_

#include <sys/types.h>

#include <zephyr/bluetooth/mesh/access.h>
#include <zephyr/bluetooth/mesh/blob.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bt_mesh_blob_cli Bluetooth Mesh BLOB Transfer Client model API
 * @ingroup bt_mesh
 * @{
 */

struct bt_mesh_blob_cli;

/**
 *
 * @brief BLOB Transfer Client model Composition Data entry.
 *
 * @param _cli Pointer to a @ref bt_mesh_blob_cli instance.
 */
#define BT_MESH_MODEL_BLOB_CLI(_cli)                                           \
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_BLOB_CLI, _bt_mesh_blob_cli_op,      \
			 NULL, _cli, &_bt_mesh_blob_cli_cb)

/** Target node's Pull mode (Pull BLOB Transfer Mode) context used
 *  while sending chunks to the Target node.
 */
struct bt_mesh_blob_target_pull {
	/** Timestamp when the Block Report Timeout Timer expires for this Target node. */
	int64_t block_report_timestamp;

	/** Missing chunks reported by this Target node. */
	uint8_t missing[DIV_ROUND_UP(CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX, 8)];
};

/** BLOB Transfer Client Target node. */
struct bt_mesh_blob_target {
	/** Linked list node */
	sys_snode_t n;

	/** Target node address. */
	uint16_t addr;

    /** Target node's Pull mode context.
     *  Needs to be initialized when sending a BLOB in Pull mode.
     */
	struct bt_mesh_blob_target_pull *pull;

	/** BLOB transfer status, see @ref bt_mesh_blob_status. */
	uint8_t status;

	uint8_t procedure_complete:1, /* Procedure has been completed. */
		acked:1,              /* Message has been acknowledged. Not used when sending. */
		timedout:1,           /* Target node didn't respond after specified timeout. */
		skip:1;               /* Skip Target node from broadcast. */
};

/** BLOB transfer information.
 *
 * If @c phase is @ref BT_MESH_BLOB_XFER_PHASE_INACTIVE,
 * the fields below @c phase are not initialized.
 * If @c phase is @ref BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START,
 * the fields below @c id are not initialized.
 */
struct bt_mesh_blob_xfer_info {
	/** BLOB transfer status. */
	enum bt_mesh_blob_status status;

	/** BLOB transfer mode. */
	enum bt_mesh_blob_xfer_mode mode;

	/** BLOB transfer phase. */
	enum bt_mesh_blob_xfer_phase phase;

	/** BLOB ID. */
	uint64_t id;

	/** BLOB size in octets. */
	uint32_t size;

	/** Logarithmic representation of the block size. */
	uint8_t block_size_log;

	/** MTU size in octets. */
	uint16_t mtu_size;

	/** Bit field indicating blocks that were not received.  */
	const uint8_t *missing_blocks;
};

/** BLOB Transfer Client transfer inputs. */
struct bt_mesh_blob_cli_inputs {
	/** Linked list of Target nodes. Each node should point to @ref
	 *  bt_mesh_blob_target::n.
	 */
	sys_slist_t targets;

	/** AppKey index to send with. */
	uint16_t app_idx;

	/** Group address destination for the BLOB transfer, or @ref
	 *  BT_MESH_ADDR_UNASSIGNED to send every message to each Target
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
struct bt_mesh_blob_cli_caps {
	/** Max BLOB size. */
	size_t max_size;

	/** Logarithmic representation of the minimum block size. */
	uint8_t min_block_size_log;

	/** Logarithmic representation of the maximum block size. */
	uint8_t max_block_size_log;

	/** Max number of chunks per block. */
	uint16_t max_chunks;

	/** Max chunk size. */
	uint16_t max_chunk_size;

	/** Max MTU size. */
	uint16_t mtu_size;

	/** Supported transfer modes. */
	enum bt_mesh_blob_xfer_mode modes;
};

/** BLOB Transfer Client state. */
enum bt_mesh_blob_cli_state {
	/** No transfer is active. */
	BT_MESH_BLOB_CLI_STATE_NONE,
	/** Retrieving transfer capabilities. */
	BT_MESH_BLOB_CLI_STATE_CAPS_GET,
	/** Sending transfer start. */
	BT_MESH_BLOB_CLI_STATE_START,
	/** Sending block start. */
	BT_MESH_BLOB_CLI_STATE_BLOCK_START,
	/** Sending block chunks. */
	BT_MESH_BLOB_CLI_STATE_BLOCK_SEND,
	/** Checking block status. */
	BT_MESH_BLOB_CLI_STATE_BLOCK_CHECK,
	/** Checking transfer status. */
	BT_MESH_BLOB_CLI_STATE_XFER_CHECK,
	/** Cancelling transfer. */
	BT_MESH_BLOB_CLI_STATE_CANCEL,
	/** Transfer is suspended. */
	BT_MESH_BLOB_CLI_STATE_SUSPENDED,
	/** Checking transfer progress. */
	BT_MESH_BLOB_CLI_STATE_XFER_PROGRESS_GET,
};

/** Event handler callbacks for the BLOB Transfer Client model.
 *
 *  All handlers are optional.
 */
struct bt_mesh_blob_cli_cb {
	/** @brief Capabilities retrieval completion callback.
	 *
	 *  Called when the capabilities retrieval procedure completes, indicating that
	 *  a common set of acceptable transfer parameters have been established
	 *  for the given list of Target nodes. All compatible Target nodes have
	 *  status code @ref BT_MESH_BLOB_SUCCESS.
	 *
	 *  @param cli     BLOB Transfer Client instance.
	 *  @param caps    Safe transfer capabilities if the transfer capabilities
	 *                 of at least one Target node has satisfied the Client, or NULL otherwise.
	 */
	void (*caps)(struct bt_mesh_blob_cli *cli,
		     const struct bt_mesh_blob_cli_caps *caps);

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
	void (*lost_target)(struct bt_mesh_blob_cli *cli,
			    struct bt_mesh_blob_target *target,
			    enum bt_mesh_blob_status reason);

	/** @brief Transfer is suspended.
	 *
	 * Called when the transfer is suspended due to response timeout from all Target nodes.
	 *
	 * @param cli    BLOB Transfer Client instance.
	 */
	void (*suspended)(struct bt_mesh_blob_cli *cli);

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
	void (*end)(struct bt_mesh_blob_cli *cli,
		    const struct bt_mesh_blob_xfer *xfer, bool success);

	/** @brief Transfer progress callback
	 *
	 * The content of @c info is invalidated upon exit from the callback.
	 * Therefore it needs to be copied if it is planned to be used later.
	 *
	 *  @param cli      BLOB Transfer Client instance.
	 *  @param target   Target node that responded to the request.
	 *  @param info     BLOB transfer information.
	 */
	void (*xfer_progress)(struct bt_mesh_blob_cli *cli,
			      struct bt_mesh_blob_target *target,
			      const struct bt_mesh_blob_xfer_info *info);

	/** @brief End of Get Transfer Progress procedure.
	 *
	 * Called when all Target nodes have responded or the procedure timed-out.
	 *
	 *  @param cli     BLOB Transfer Client instance.
	 */
	void (*xfer_progress_complete)(struct bt_mesh_blob_cli *cli);
};

/** @cond INTERNAL_HIDDEN */
struct blob_cli_broadcast_ctx {
	/** Called for every Target node in unicast mode, or once in case of multicast mode. */
	void (*send)(struct bt_mesh_blob_cli *cli, uint16_t dst);
	/** Called after every @ref blob_cli_broadcast_ctx::send callback. */
	void (*send_complete)(struct bt_mesh_blob_cli *cli, uint16_t dst);
	/** If @ref blob_cli_broadcast_ctx::acked is true, called after all Target nodes
	 *  have confirmed reception by @ref blob_cli_broadcast_rsp. Otherwise, called
	 *  after transmission has been completed.
	 */
	void (*next)(struct bt_mesh_blob_cli *cli);
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
};
/** INTERNAL_HIDDEN @endcond */

/** BLOB Transfer Client model instance. */
struct bt_mesh_blob_cli {
	/** Event handler callbacks */
	const struct bt_mesh_blob_cli_cb *cb;

	/* Runtime state */
	const struct bt_mesh_model *mod;

	struct {
		struct bt_mesh_blob_target *target;
		struct blob_cli_broadcast_ctx ctx;
		struct k_work_delayable retry;
		/* Represents Client Timeout timer in a timestamp. Used in Pull mode only. */
		int64_t cli_timestamp;
		struct k_work_delayable complete;
		uint16_t pending;
		uint8_t retries;
		uint8_t sending : 1,
			cancelled : 1;
	} tx;

	const struct bt_mesh_blob_io *io;
	const struct bt_mesh_blob_cli_inputs *inputs;
	const struct bt_mesh_blob_xfer *xfer;
	uint32_t chunk_interval_ms;
	uint16_t block_count;
	uint16_t chunk_idx;
	uint16_t mtu_size;
	enum bt_mesh_blob_cli_state state;
	struct bt_mesh_blob_block block;
	struct bt_mesh_blob_cli_caps caps;
};

/** @brief Retrieve transfer capabilities for a list of Target nodes.
 *
 *  Queries the availability and capabilities of all Target nodes, producing a
 *  cumulative set of transfer capabilities for the Target nodes, and returning
 *  it through the @ref bt_mesh_blob_cli_cb::caps callback.
 *
 *  Retrieving the capabilities may take several seconds, depending on the
 *  number of Target nodes and mesh network performance. The end of the procedure
 *  is indicated through the @ref bt_mesh_blob_cli_cb::caps callback.
 *
 *  This procedure is not required, but strongly recommended as a
 *  preparation for a transfer to maximize performance and the chances of
 *  success.
 *
 *  @param cli     BLOB Transfer Client instance.
 *  @param inputs  Statically allocated BLOB Transfer Client transfer inputs.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_blob_cli_caps_get(struct bt_mesh_blob_cli *cli,
			      const struct bt_mesh_blob_cli_inputs *inputs);

/** @brief Perform a BLOB transfer.
 *
 *  Starts sending the transfer to the Target nodes. Only Target nodes with a
 *  @c status of @ref BT_MESH_BLOB_SUCCESS will be considered.
 *
 *  The transfer will keep going either until all Target nodes have been dropped, or
 *  the full BLOB has been sent.
 *
 *  The BLOB transfer may take several minutes, depending on the number of
 *  Target nodes, size of the BLOB and mesh network performance. The end of the
 *  transfer is indicated through the @ref bt_mesh_blob_cli_cb::end callback.
 *
 *  A Client only supports one transfer at the time.
 *
 *  @param cli    BLOB Transfer Client instance.
 *  @param inputs Statically allocated BLOB Transfer Client transfer inputs.
 *  @param xfer   Statically allocated transfer parameters.
 *  @param io     BLOB stream to read the transfer from.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_blob_cli_send(struct bt_mesh_blob_cli *cli,
			  const struct bt_mesh_blob_cli_inputs *inputs,
			  const struct bt_mesh_blob_xfer *xfer,
			  const struct bt_mesh_blob_io *io);

/** @brief Suspend the active transfer.
 *
 *  @param cli BLOB Transfer Client instance.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_blob_cli_suspend(struct bt_mesh_blob_cli *cli);

/** @brief Resume the suspended transfer.
 *
 *  @param cli BLOB Transfer Client instance.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_blob_cli_resume(struct bt_mesh_blob_cli *cli);

/** @brief Cancel an ongoing transfer.
 *
 *  @param cli BLOB Transfer Client instance.
 */
void bt_mesh_blob_cli_cancel(struct bt_mesh_blob_cli *cli);

/** @brief Get the progress of BLOB transfer.
 *
 *  This function can only be used if the BLOB Transfer Client is currently
 *  not performing a BLOB transfer.
 *  To get progress of the active BLOB transfer, use the
 *  @ref bt_mesh_blob_cli_xfer_progress_active_get function.
 *
 *  @param cli BLOB Transfer Client instance.
 *  @param inputs Statically allocated BLOB Transfer Client transfer inputs.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_blob_cli_xfer_progress_get(struct bt_mesh_blob_cli *cli,
				       const struct bt_mesh_blob_cli_inputs *inputs);

/** @brief Get the current progress of the active transfer in percent.
 *
 *  @param cli BLOB Transfer Client instance.
 *
 *  @return The current transfer progress, or 0 if no transfer is active.
 */
uint8_t bt_mesh_blob_cli_xfer_progress_active_get(struct bt_mesh_blob_cli *cli);

/** @brief Get the current state of the BLOB Transfer Client.
 *
 *  @param cli BLOB Transfer Client instance.
 *
 *  @return true if the BLOB Transfer Client is currently participating in a transfer or
 *          retrieving the capabilities and false otherwise.
 */
bool bt_mesh_blob_cli_is_busy(struct bt_mesh_blob_cli *cli);

/** @brief Set chunk sending interval in ms
 *
 *  This function is optional, and can be used to define how fast chunks are sent in the BLOB Client
 *  Model.
 *  Without an added delay, for example a Bluetooth Mesh DFU can cause network blockage by
 *  constantly sending the next chunks, especially if the chunks are sent to group addresses or
 *  multiple unicast addresses.
 *
 *  @note: Big intervals may cause timeouts. Increasing the @c timeout_base accordingly can
 *  circumvent this.
 *
 *  @param cli BLOB Transfer Client instance.
 *  @param interval_ms the delay before each chunk is sent out in ms.
 */
void bt_mesh_blob_cli_set_chunk_interval_ms(struct bt_mesh_blob_cli *cli, uint32_t interval_ms);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_blob_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_blob_cli_cb;
/** @endcond */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_CLI_H_ */
