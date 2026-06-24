# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Decode BLE_LOG_SRC_INTERNAL(0) frame payloads.

Payload format on wire: [4B os_ts][1B int_src_code][variable sub-payload]
See Spec Section 9.
"""

import struct

from src.backend.models import BufUtilResult
from src.backend.models import EnhStatResult
from src.backend.models import InfoResult
from src.backend.models import InternalDecoderResult
from src.backend.models import InternalSource

# Minimum payload size: 4B os_ts + 1B int_src_code
_MIN_PAYLOAD_SIZE = 5

# ble_log_info_t: [1B int_src_code][1B version] — used by INIT_DONE, INFO, FLUSH
_INFO_STRUCT = struct.Struct('<BB')

# ble_log_enh_stat_t: [1B int_src_code][1B src_code][4B written][4B lost][4B written_bytes][4B lost_bytes]
_ENH_STAT_STRUCT = struct.Struct('<BBIIII')

# ble_log_buf_util_t: [1B int_src_code][1B lbm_id][1B trans_cnt][1B inflight_peak]
_BUF_UTIL_STRUCT = struct.Struct('<BBBB')


def decode_internal_frame(payload: bytes) -> InternalDecoderResult | None:
    """Decode an INTERNAL frame payload.

    Args:
        payload: Full frame payload including os_ts prefix.

    Returns:
        Typed dict with decoded fields, or None if the frame should be ignored (TS) or is malformed.
    """
    if len(payload) < _MIN_PAYLOAD_SIZE:
        return None

    os_ts_ms = struct.unpack_from('<I', payload, 0)[0]
    int_src_code = payload[4]
    sub_payload = payload[4:]  # starts at int_src_code

    try:
        int_src = InternalSource(int_src_code)
    except ValueError:
        return None

    if int_src == InternalSource.TS:
        return None  # Ignored per spec

    if int_src in (InternalSource.INIT_DONE, InternalSource.INFO, InternalSource.FLUSH):
        if len(sub_payload) < _INFO_STRUCT.size:
            return None
        _, version = _INFO_STRUCT.unpack_from(sub_payload, 0)
        return InfoResult(
            int_src=int_src,
            version=version,
            os_ts_ms=os_ts_ms,
        )

    if int_src == InternalSource.ENH_STAT:
        if len(sub_payload) < _ENH_STAT_STRUCT.size:
            return None
        _, src_code, written_frame_cnt, lost_frame_cnt, written_bytes_cnt, lost_bytes_cnt = (
            _ENH_STAT_STRUCT.unpack_from(sub_payload, 0)
        )
        return EnhStatResult(
            int_src=int_src,
            src_code=src_code,
            written_frame_cnt=written_frame_cnt,
            lost_frame_cnt=lost_frame_cnt,
            written_bytes_cnt=written_bytes_cnt,
            lost_bytes_cnt=lost_bytes_cnt,
            os_ts_ms=os_ts_ms,
        )

    if int_src == InternalSource.BUF_UTIL:
        if len(sub_payload) < _BUF_UTIL_STRUCT.size:
            return None
        _, lbm_id, trans_cnt, inflight_peak = _BUF_UTIL_STRUCT.unpack_from(sub_payload, 0)
        pool = (lbm_id >> 4) & 0x0F
        index = lbm_id & 0x0F
        return BufUtilResult(
            int_src=int_src,
            lbm_id=lbm_id,
            pool=pool,
            index=index,
            trans_cnt=trans_cnt,
            inflight_peak=inflight_peak,
            os_ts_ms=os_ts_ms,
        )

    return None
