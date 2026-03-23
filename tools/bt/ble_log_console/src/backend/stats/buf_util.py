# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

from src.backend.models import BufUtilEntry


class BufUtilTracker:
    def __init__(self) -> None:
        self._entries: dict[int, BufUtilEntry] = {}

    def record(self, lbm_id: int, trans_cnt: int, inflight_peak: int) -> None:
        pool = (lbm_id >> 4) & 0x0F
        index = lbm_id & 0x0F
        self._entries[lbm_id] = BufUtilEntry(
            lbm_id=lbm_id,
            pool=pool,
            index=index,
            trans_cnt=trans_cnt,
            inflight_peak=inflight_peak,
        )

    def reset(self) -> None:
        self._entries.clear()

    def snapshot(self) -> list[BufUtilEntry]:
        return sorted(self._entries.values(), key=lambda e: (e.pool, e.index))
