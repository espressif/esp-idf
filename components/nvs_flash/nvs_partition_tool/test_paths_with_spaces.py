#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Generators must accept spaces in input and output paths."""

import os
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace

import esp_idf_nvs_partition_gen.nvs_partition_gen as nvs_partition_gen

IDF_PATH = Path(os.environ['IDF_PATH'])
MFG_GEN = IDF_PATH / 'tools' / 'mass_mfg' / 'mfg_gen.py'
PARTITION_SIZE = '0x3000'


def test_nvs_partition_gen_paths_with_spaces(tmp_path: Path) -> None:
    src_dir = tmp_path / 'input files'
    out_dir = tmp_path / 'output files'
    src_dir.mkdir()
    csv_path = src_dir / 'nvs data.csv'
    bin_path = out_dir / 'nvs image.bin'
    csv_path.write_text(
        'key,type,encoding,value\nstorage,namespace,,\ncounter,data,u8,3\n',
        encoding='utf-8',
    )

    nvs_partition_gen.generate(
        SimpleNamespace(
            input=str(csv_path),
            output=str(bin_path),
            size=PARTITION_SIZE,
            version=2,
            outdir=str(out_dir),
        )
    )

    assert bin_path.is_file()
    assert bin_path.stat().st_size == int(PARTITION_SIZE, 16)


def test_mfg_gen_paths_with_spaces(tmp_path: Path) -> None:
    src_dir = tmp_path / 'input files'
    out_dir = tmp_path / 'output files'
    src_dir.mkdir()
    conf_path = src_dir / 'config file.csv'
    values_path = src_dir / 'values file.csv'
    conf_path.write_text('storage,namespace,\ncounter,data,u8\n', encoding='utf-8')
    values_path.write_text('id,counter\n1,3\n', encoding='utf-8')
    prefix = 'dev board'

    subprocess.run(
        [
            sys.executable,
            str(MFG_GEN),
            'generate',
            str(conf_path),
            str(values_path),
            prefix,
            PARTITION_SIZE,
            '--outdir',
            str(out_dir),
        ],
        check=True,
    )

    bin_path = out_dir / 'bin' / f'{prefix}-1.bin'
    csv_path = out_dir / 'csv' / f'{prefix}-1.csv'
    assert bin_path.is_file()
    assert bin_path.stat().st_size == int(PARTITION_SIZE, 16)
    assert csv_path.is_file()
