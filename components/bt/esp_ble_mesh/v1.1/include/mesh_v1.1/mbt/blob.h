/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_H__
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_H__

#include <sys/types.h>

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bt_mesh_blob Bluetooth Mesh BLOB model API
 * @ingroup bt_mesh
 * @{
 */

#ifndef CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX
#define CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX 0
#endif

/** BLOB transfer mode. */
enum bt_mesh_blob_xfer_mode {
	/** No valid transfer mode. */
	BT_MESH_BLOB_XFER_MODE_NONE,
	/** Push mode (Push BLOB Transfer Mode). */
	BT_MESH_BLOB_XFER_MODE_PUSH,
	/** Pull mode (Pull BLOB Transfer Mode). */
	BT_MESH_BLOB_XFER_MODE_PULL,
	/** Both modes are valid. */
	BT_MESH_BLOB_XFER_MODE_ALL,
};

/** Transfer phase. */
enum bt_mesh_blob_xfer_phase {
	/** The BLOB Transfer Server is awaiting configuration. */
	BT_MESH_BLOB_XFER_PHASE_INACTIVE,
	/** The BLOB Transfer Server is ready to receive a BLOB transfer. */
	BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START,
	/** The BLOB Transfer Server is waiting for the next block of data. */
	BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK,
	/** The BLOB Transfer Server is waiting for the next chunk of data. */
	BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK,
	/** The BLOB was transferred successfully. */
	BT_MESH_BLOB_XFER_PHASE_COMPLETE,
	/** The BLOB transfer is paused. */
	BT_MESH_BLOB_XFER_PHASE_SUSPENDED,
};

/** BLOB model status codes. */
enum bt_mesh_blob_status {
	/** The message was processed successfully. */
	BT_MESH_BLOB_SUCCESS,
	/** The Block Number field value is not within the range of blocks being
	 *  transferred.
	 */
	BT_MESH_BLOB_ERR_INVALID_BLOCK_NUM,
	/** The block size is smaller than the size indicated by the Min Block
	 *  Size Log state or is larger than the size indicated by the Max Block
	 *  Size Log state.
	 */
	BT_MESH_BLOB_ERR_INVALID_BLOCK_SIZE,
	/** The chunk size exceeds the size indicated by the Max Chunk Size
	 *  state, or the number of chunks exceeds the number specified by the
	 *  Max Total Chunks state.
	 */
	BT_MESH_BLOB_ERR_INVALID_CHUNK_SIZE,
	/** The operation cannot be performed while the server is in the current
	 *  phase.
	 */
	BT_MESH_BLOB_ERR_WRONG_PHASE,
	/** A parameter value in the message cannot be accepted. */
	BT_MESH_BLOB_ERR_INVALID_PARAM,
	/** The message contains a BLOB ID value that is not expected. */
	BT_MESH_BLOB_ERR_WRONG_BLOB_ID,
	/** There is not enough space available in memory to receive the BLOB.
	 */
	BT_MESH_BLOB_ERR_BLOB_TOO_LARGE,
	/** The transfer mode is not supported by the BLOB Transfer Server
	 *  model.
	 */
	BT_MESH_BLOB_ERR_UNSUPPORTED_MODE,
	/** An internal error occurred on the node. */
	BT_MESH_BLOB_ERR_INTERNAL,
	/** The requested information cannot be provided while the server is in
	 *  the current phase.
	 */
	BT_MESH_BLOB_ERR_INFO_UNAVAILABLE,
};

/** BLOB transfer data block. */
struct bt_mesh_blob_block {
	/** Block size in bytes */
	size_t size;
	/** Offset in bytes from the start of the BLOB. */
	off_t offset;
	/** Block number */
	uint16_t number;
	/** Number of chunks in block. */
	uint16_t chunk_count;
	/** Bitmap of missing chunks. */
	uint8_t missing[DIV_ROUND_UP(CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX,
					 8)];
};

/** BLOB data chunk. */
struct bt_mesh_blob_chunk {
	/** Offset of the chunk data from the start of the block. */
	off_t offset;
	/** Chunk data size. */
	size_t size;
	/** Chunk data. */
	uint8_t *data;
};

/** BLOB transfer. */
struct bt_mesh_blob_xfer {
	/** BLOB ID. */
	uint64_t id;
	/** Total BLOB size in bytes. */
	size_t size;
	/** BLOB transfer mode. */
	enum bt_mesh_blob_xfer_mode mode;
	/* Logarithmic representation of the block size. */
	uint8_t block_size_log;
	/** Base chunk size. May be smaller for the last chunk. */
	uint16_t chunk_size;
};

/** BLOB stream interaction mode. */
enum bt_mesh_blob_io_mode {
	/** Read data from the stream. */
	BT_MESH_BLOB_READ,
	/** Write data to the stream. */
	BT_MESH_BLOB_WRITE,
};

/** BLOB stream. */
struct bt_mesh_blob_io {
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
	int (*open)(const struct bt_mesh_blob_io *io,
		    const struct bt_mesh_blob_xfer *xfer,
		    enum bt_mesh_blob_io_mode mode);

	/** @brief Close callback.
	 *
	 *  Called when the reader is closed.
	 *
	 *  @param io   BLOB stream.
	 *  @param xfer BLOB transfer.
	 */
	void (*close)(const struct bt_mesh_blob_io *io,
		      const struct bt_mesh_blob_xfer *xfer);

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
	int (*block_start)(const struct bt_mesh_blob_io *io,
			   const struct bt_mesh_blob_xfer *xfer,
			   const struct bt_mesh_blob_block *block);

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
	void (*block_end)(const struct bt_mesh_blob_io *io,
			  const struct bt_mesh_blob_xfer *xfer,
			  const struct bt_mesh_blob_block *block);

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
	int (*wr)(const struct bt_mesh_blob_io *io,
		  const struct bt_mesh_blob_xfer *xfer,
		  const struct bt_mesh_blob_block *block,
		  const struct bt_mesh_blob_chunk *chunk);

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
	int (*rd)(const struct bt_mesh_blob_io *io,
		  const struct bt_mesh_blob_xfer *xfer,
		  const struct bt_mesh_blob_block *block,
		  const struct bt_mesh_blob_chunk *chunk);
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_H__ */
