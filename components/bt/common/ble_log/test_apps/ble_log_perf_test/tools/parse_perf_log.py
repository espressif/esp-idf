#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
"""Turn captured BLE Log perf monitor output into tables / CSV.

Capture with:  idf.py -p PORT monitor | tee capture.log
Then:          python3 parse_perf_log.py capture.log            # markdown tables
               python3 parse_perf_log.py capture.log --csv out.csv
"""

import csv
import re
import sys
from typing import TypedDict

KV = re.compile(r'(\w+)=(\S+)')

WRITER_COLS = ('frames', 'failed', 'avg', 'avg_failed', 'p50', 'p95', 'p99', 'max')
RT_COLS = ('mode', 'samples', 'batch', 'payload', 'min', 'avg', 'p50', 'p95', 'max')


class Run(TypedDict):
    head: dict[str, str]
    writers: list[dict[str, str]]
    other: list[str]


def fields(line: str) -> dict[str, str]:
    return dict(KV.findall(line))


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 1
    path = sys.argv[1]
    csv_path = None
    if '--csv' in sys.argv:
        csv_path = sys.argv[sys.argv.index('--csv') + 1]

    with open(path, encoding='utf-8', errors='replace') as f:
        lines = f.read().splitlines()

    runs: list[Run] = []
    rt_rows: list[dict[str, str]] = []
    cur: Run | None = None
    for line in lines:
        if line.startswith('BLE_LOG_RT_PERF '):
            rt_rows.append(fields(line))
            continue
        if not line.startswith('BLE_LOG_PERF '):
            continue
        kv = fields(line)
        if line.startswith('BLE_LOG_PERF ===='):
            if ' RUN [' in line:
                cur = Run(head=kv, writers=[], other=[])
                runs.append(cur)
            # END separator: keep context, nothing to record
            continue
        if cur is None:
            continue
        if 'writer' in kv:
            cur['writers'].append(kv)
        elif 'mode' in kv:
            cur['head'].update(kv)  # duration etc.
        else:
            cur['other'].append(line[len('BLE_LOG_PERF ') :])

    if not runs and not rt_rows:
        print(f'no BLE_LOG_PERF or BLE_LOG_RT_PERF lines found in {path}')
        return 1

    for i, run in enumerate(runs, 1):
        h = run['head']
        title = (
            f'## Run {i}: mode={h.get("mode", "?")} '
            f'profile={h.get("profile", "?")} '
            f'link={h.get("link", "?")} isolate={h.get("isolate", "?")}'
        )
        print(title)
        if run['writers']:
            header = '| writer | ' + ' | '.join(WRITER_COLS) + ' |'
            print(header)
            print('|---|' + '---|' * len(WRITER_COLS))
            for w in run['writers']:
                row = ' | '.join(w.get(c, '-') for c in WRITER_COLS)
                print(f'| {w.get("writer", "?")} | {row} |')
        for o in run['other']:
            print(f'- `{o}`')
        print()

    if rt_rows:
        print('## Runtime dispatch')
        print('| ' + ' | '.join(RT_COLS) + ' |')
        print('|' + '---|' * len(RT_COLS))
        for rt_row in rt_rows:
            print('| ' + ' | '.join(rt_row.get(c, '-') for c in RT_COLS) + ' |')
        print()

    if csv_path:
        with open(csv_path, 'w', newline='', encoding='utf-8') as f:
            wcsv = csv.writer(f)
            wcsv.writerow(
                [
                    'kind',
                    'run',
                    'mode',
                    'profile',
                    'link',
                    'isolate',
                    'writer',
                    *WRITER_COLS,
                    'samples',
                    'batch',
                    'payload',
                    'min',
                ]
            )
            for i, run in enumerate(runs, 1):
                h = run['head']
                for w in run['writers']:
                    wcsv.writerow(
                        [
                            'perf',
                            i,
                            h.get('mode', ''),
                            h.get('profile', ''),
                            h.get('link', ''),
                            h.get('isolate', ''),
                            w.get('writer', ''),
                            *(w.get(c, '') for c in WRITER_COLS),
                            '',
                            '',
                            '',
                            '',
                        ]
                    )
            for rt_row in rt_rows:
                wcsv.writerow(
                    [
                        'rt_perf',
                        '',
                        rt_row.get('mode', ''),
                        '',
                        '',
                        '',
                        'runtime',
                        '',
                        '',
                        rt_row.get('avg', ''),
                        '',
                        rt_row.get('p50', ''),
                        rt_row.get('p95', ''),
                        '',
                        rt_row.get('max', ''),
                        rt_row.get('samples', ''),
                        rt_row.get('batch', ''),
                        rt_row.get('payload', ''),
                        rt_row.get('min', ''),
                    ]
                )
        print(f'CSV written to {csv_path}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
