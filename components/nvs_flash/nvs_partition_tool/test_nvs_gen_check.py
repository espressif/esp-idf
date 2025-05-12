#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import base64
import json
from importlib.metadata import version
from io import BufferedRandom
from io import BytesIO
from pathlib import Path
from typing import Any
from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
from typing import Union
from zlib import crc32

import esp_idf_nvs_partition_gen.nvs_partition_gen as nvs_partition_gen
import nvs_check as nvs_check
import pytest
from esp_idf_nvs_partition_gen.nvs_partition_gen import NVS
from nvs_logger import nvs_log
from nvs_logger import NVS_Logger
from nvs_logger import print_minimal_json
from nvs_parser import nvs_const
from nvs_parser import NVS_Entry
from nvs_parser import NVS_Partition
from packaging.version import Version

NVS_PART_GEN_VERSION_SKIP = '0.1.9'


# Temporary workaround for pytest skipping tests based on the version of the esp-idf-nvs-partition-gen package
@pytest.fixture(scope='session', autouse=True)
def before() -> None:
    ver = version('esp-idf-nvs-partition-gen')
    if Version(ver) < Version(NVS_PART_GEN_VERSION_SKIP):
        pytest.skip('pass')


class SilentLogger(NVS_Logger):
    def __init__(self) -> None:
        super().__init__()
        self.color = False

    def info(self, *args, **kwargs) -> None:  # type: ignore
        pass


logger = nvs_log  # SilentLogger()

LOREM_STRING = """Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Nullam eget orci fringilla, cursus nisi sit amet, hendrerit tortor.
Vivamus lectus dolor, rhoncus eget metus id, convallis placerat quam.
Nulla facilisi.
In at aliquam nunc, in dictum augue.
Nullam dapibus ligula nec enim commodo lobortis.
Praesent facilisis ante nec magna various lobortis.
Phasellus sodales sed nisi vitae pulvinar.
Aliquam tempor quis sem et tempor.
Etiam interdum nunc quis justo pharetra, sed finibus arcu lacinia.
Suspendisse potenti.
Praesent et turpis ut justo accumsan pellentesque sed at leo.
Aenean consequat ligula ac mattis porta.
Nullam id justo a arcu tincidunt sodales.
Nunc rhoncus pretium nibh ut convallis.
Maecenas orci enim, tincidunt eget vestibulum eu, placerat non ante.
Proin sit amet felis tempor, ullamcorper sem sed, scelerisque nibh.
Aliquam sit amet semper leo, in fringilla nulla.
Vestibulum sit amet tortor tincidunt, laoreet risus eget, ullamcorper sapien.
Fusce non finibus nisl. Cras vitae dui nibh.
Sed fermentum ullamcorper various.
Integer sit amet elit sed nunc fringilla molestie nec nec diam.
Etiam et ornare tellus.
Donec tristique auctor urna, ac aliquam tellus sodales id.
Duis nec magna eget mi consequat gravida.
Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae;
Name imperdiet ante neque, nec viverra sem pellentesque vel.
Sed nec arcu non nisl tempor pretium.
Quisque facilisis auctor lobortis.
Pellentesque sed finibus sem, eu lacinia tellus.
Vivamus imperdiet non augue in tincidunt.
Sed aliquet tincidunt dignissim.
Name vehicula leo eu dolor pellentesque, ultrices tempus ex hendrerit.
"""


def get_entry_type_bin(entry_type_str: str) -> Optional[int]:
    # Reverse `item_type` dict lookup
    entry_type_bin: Optional[int] = next(key for key, value in nvs_const.item_type.items() if value == entry_type_str)
    if entry_type_bin is None:
        logger.info(logger.yellow(f'Unknown entry type {entry_type_str}'))
    return entry_type_bin


def create_entry_data_bytearray(
    namespace_index: int, entry_type: int, span: int, chunk_index: int, key: str, data: Any
) -> bytearray:
    key_bytearray = bytearray(key, 'ascii')
    key_encoded = (key_bytearray + bytearray({0x00}) * (16 - len(key_bytearray)))[:16]  # Pad key with null bytes
    key_encoded[15] = 0x00  # Null-terminate the key
    is_signed = entry_type >= 0x11 and entry_type <= 0x18

    entry_data: bytearray = bytearray({0xFF}) * nvs_const.entry_size  # Empty entry
    entry_data[0] = namespace_index
    entry_data[1] = entry_type
    entry_data[2] = span
    entry_data[3] = chunk_index
    # entry_data[4:8]  # CRC32
    entry_data[8:24] = key_encoded
    entry_data[24:32] = data.to_bytes(8, byteorder='little', signed=is_signed)

    raw_without_crc = entry_data[:4] + entry_data[8:32]
    entry_data[4:8] = crc32(raw_without_crc, 0xFFFFFFFF).to_bytes(4, byteorder='little', signed=False)

    return entry_data


@pytest.fixture
def generate_nvs() -> Callable:
    def _execute_nvs_setup(nvs_setup_func: Callable, output: Optional[Path] = None) -> NVS_Partition:
        nvs_file: Optional[Union[BytesIO, BufferedRandom]] = None
        if output is None:
            nvs_file = BytesIO()
        else:
            try:
                nvs_file = open(output, 'wb+')
            except OSError as e:
                raise RuntimeError(f'Cannot open file {output}, error: {e}')
        nvs_obj = nvs_setup_func(nvs_file)
        nvs_partition_gen.nvs_close(nvs_obj)
        nvs_file.seek(0)
        nvs_parsed = NVS_Partition('test', bytearray(nvs_file.read()))
        nvs_file.close()
        return nvs_parsed

    return _execute_nvs_setup


# Setup functions
def setup_ok_primitive(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x4000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'int32_test', 'data', 'i32', str(42))
    nvs_partition_gen.write_entry(nvs_obj, 'uint32_test', 'data', 'u32', str(42))
    nvs_partition_gen.write_entry(nvs_obj, 'int8_test', 'data', 'i8', str(100))

    return nvs_obj


def setup_ok_variable_len(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x5000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'short_string_key', 'data', 'string', 'Hello world!')
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_blob.bin'
    )
    nvs_partition_gen.write_entry(nvs_obj, 'lorem_string_key', 'data', 'string', LOREM_STRING * 2)
    nvs_partition_gen.write_entry(nvs_obj, 'uniq_string_key', 'data', 'string', 'I am unique!')
    nvs_partition_gen.write_entry(
        nvs_obj, 'multi_blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )

    return nvs_obj


def setup_ok_mixed(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x6000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    prim_types = ['i8', 'u8', 'i16', 'u16', 'i32', 'u32']

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    for i in range(20):
        nvs_partition_gen.write_entry(nvs_obj, f'test_{i}', 'data', prim_types[i % len(prim_types)], str(i))
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_singlepage_blob.bin'
    )

    nvs_partition_gen.write_entry(nvs_obj, 'etc', 'namespace', '', '')
    for i in range(20):
        nvs_partition_gen.write_entry(nvs_obj, f'test_{i}', 'data', prim_types[i % len(prim_types)], str(i))

    nvs_partition_gen.write_entry(nvs_obj, 'lorem_string_key', 'data', 'string', LOREM_STRING * 2)

    nvs_partition_gen.write_entry(nvs_obj, 'abcd', 'namespace', '', '')
    for i in range(20):
        nvs_partition_gen.write_entry(nvs_obj, f'test_{i}', 'data', prim_types[i % len(prim_types)], str(i))

    nvs_partition_gen.write_entry(nvs_obj, 'uniq_string_key', 'data', 'string', 'I am unique!')
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )

    return nvs_obj


def setup_bad_mixed_same_key_different_page(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x6000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    prim_types = ['i8', 'u8', 'i16', 'u16', 'i32', 'u32']

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    for i in range(20):
        nvs_partition_gen.write_entry(nvs_obj, f'test_{i}', 'data', prim_types[i % len(prim_types)], str(i))
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_singlepage_blob.bin'
    )

    nvs_partition_gen.write_entry(nvs_obj, 'etc', 'namespace', '', '')
    for i in range(20):
        nvs_partition_gen.write_entry(nvs_obj, f'test_{i}', 'data', prim_types[i % len(prim_types)], str(i))

    nvs_partition_gen.write_entry(nvs_obj, 'lorem_string_key', 'data', 'string', LOREM_STRING * 2)

    nvs_partition_gen.write_entry(nvs_obj, 'uniq_string_key', 'data', 'string', 'I am unique!')
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )

    # Should be on a different page already - start creating duplicates

    for i in range(6):
        data_type = prim_types[i % len(prim_types)]
        nvs_partition_gen.write_entry(
            nvs_obj, f'test_{i}', 'data', data_type, str(i)
        )  # Conflicting keys under "abcd" namespace - 6 duplicates
    nvs_partition_gen.write_entry(
        nvs_obj, 'lorem_string_key', 'data', 'string', 'abc'
    )  # Conflicting key for string - 7th duplicate

    # Create new duplicates of storage namespace with an unsafe version of write_namespace function
    nvs_obj.write_namespace_unsafe(
        'storage'
    )  # Conflicting namespace - 8th duplicate (the function is only for testing)

    nvs_partition_gen.write_entry(nvs_obj, 'storage2', 'namespace', '', '')  # New namespace, ignored
    nvs_partition_gen.write_entry(
        nvs_obj, 'lorem_string_key', 'data', 'string', 'abc'
    )  # Should be ignored as is under different "storage2" namespace
    nvs_partition_gen.write_entry(
        nvs_obj, 'lorem_string', 'data', 'string', 'abc'
    )  # 3 conflicting keys under "storage2" namespace - 9th duplicate
    nvs_partition_gen.write_entry(nvs_obj, 'lorem_string', 'data', 'string', 'def')
    nvs_partition_gen.write_entry(nvs_obj, 'lorem_string', 'data', 'string', '123')

    # This no longer (nvs generator version >= 0.1.5) creates a duplicate namespace entry, only changes
    # the current used namespace index
    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')

    return nvs_obj


def setup_bad_same_key_primitive(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x4000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'unique_key', 'data', 'i16', str(1234))
    nvs_partition_gen.write_entry(nvs_obj, 'same_key', 'data', 'i32', str(42))
    nvs_partition_gen.write_entry(nvs_obj, 'same_key', 'data', 'u32', str(24))
    nvs_partition_gen.write_entry(nvs_obj, 'same_key', 'data', 'i8', str(-5))
    nvs_partition_gen.write_entry(nvs_obj, 'another_same_key', 'data', 'u16', str(321))
    nvs_partition_gen.write_entry(nvs_obj, 'another_same_key', 'data', 'u16', str(456))

    return nvs_obj


def setup_bad_same_key_variable_len(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x4000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'same_string_key', 'data', 'string', 'Hello')
    nvs_partition_gen.write_entry(nvs_obj, 'same_string_key', 'data', 'string', 'world!')
    nvs_partition_gen.write_entry(nvs_obj, 'unique_string_key', 'data', 'string', 'I am unique!')

    return nvs_obj


def setup_bad_same_key_blob_index(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x6000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key_2', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )  # Duplicate key

    return nvs_obj


def setup_read_only(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x1000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'int32_test', 'data', 'i32', str(42))
    nvs_partition_gen.write_entry(nvs_obj, 'uint32_test', 'data', 'u32', str(42))
    nvs_partition_gen.write_entry(nvs_obj, 'int8_test', 'data', 'i8', str(100))
    nvs_partition_gen.write_entry(nvs_obj, 'short_string_key', 'data', 'string', 'Hello world!')

    return nvs_obj


def setup_minimal_json(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x4000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'int32_test', 'data', 'i32', str(-42))
    nvs_partition_gen.write_entry(nvs_obj, 'uint32_test', 'data', 'u32', str(96))
    nvs_partition_gen.write_entry(nvs_obj, 'int8_test', 'data', 'i8', str(100))
    nvs_partition_gen.write_entry(
        nvs_obj, 'blob_key', 'file', 'binary', '../nvs_partition_generator/testdata/sample_multipage_blob.bin'
    )
    nvs_partition_gen.write_entry(nvs_obj, 'short_str_key', 'data', 'string', 'Another string data')
    nvs_partition_gen.write_entry(nvs_obj, 'long_str_key', 'data', 'string', LOREM_STRING)

    return nvs_obj


def setup_ok_non_ascii_string(nvs_file: Optional[Union[BytesIO, BufferedRandom]]) -> NVS:
    size_fixed, read_only = nvs_partition_gen.check_size(str(0x4000))
    nvs_obj = nvs_partition_gen.nvs_open(
        result_obj=nvs_file,
        input_size=size_fixed,
        version=nvs_partition_gen.Page.VERSION2,
        is_encrypt=False,
        key=None,
        read_only=read_only,
    )

    nvs_partition_gen.write_entry(nvs_obj, 'storage', 'namespace', '', '')
    nvs_partition_gen.write_entry(nvs_obj, 'string_key', 'data', 'string', 'ÄÄÄÄ')

    return nvs_obj


# Helper functions
def prepare_duplicate_list(nvs: NVS_Partition) -> Dict[str, List[NVS_Entry]]:
    seen_written_entires_all: Dict[str, List[NVS_Entry]] = {}
    for page in nvs.pages:
        # page: NVS_Page
        for entry in page.entries:
            # entry: NVS_Entry
            # Duplicate entry check (1) - same key, different index - find duplicates
            seen_written_entires_all = nvs_check.identify_entry_duplicates(entry, seen_written_entires_all)
    # Duplicate entry check (2) - same key, different index
    duplicates: Dict[str, List[NVS_Entry]] = nvs_check.filter_entry_duplicates(seen_written_entires_all)
    return duplicates


# Tests
@pytest.mark.parametrize('setup_func', [setup_ok_primitive, setup_ok_variable_len, setup_ok_mixed])
def test_check_partition_size(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    assert nvs_check.check_partition_size(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_ok_primitive, setup_ok_variable_len, setup_ok_mixed])
def test_check_empty_page_present(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    assert nvs_check.check_empty_page_present(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_ok_primitive, setup_ok_variable_len, setup_ok_mixed])
def test_check_empty_page_content__check_page_crc(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    for page in nvs.pages:
        if page.header['status'] == 'Empty':
            assert page.is_empty
            assert nvs_check.check_empty_page_content(page, logger)
        else:
            assert not page.is_empty
            assert nvs_check.check_page_crc(page, logger)


@pytest.mark.parametrize('setup_func', [setup_ok_primitive, setup_ok_variable_len, setup_ok_mixed])
def test_check_duplicates_ok(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    duplicates = prepare_duplicate_list(nvs)
    assert len(duplicates) == 0  # No duplicates in any page


@pytest.mark.parametrize('setup_func', [setup_bad_same_key_primitive])
def test_check_duplicates_bad_same_key_primitive_type(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    duplicates = prepare_duplicate_list(nvs)
    assert len(duplicates) == 2  # 2 different lists of duplicate keys
    assert len(list(duplicates.values())[0]) == 3  # 3 entries with the same_key
    assert len(list(duplicates.values())[1]) == 2  # 2 entries with the another_same_key
    nvs_check.integrity_check(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_bad_same_key_variable_len])
def test_check_duplicates_bad_same_key_variable_len_type(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    duplicates = prepare_duplicate_list(nvs)
    assert len(duplicates) == 1  # Only one duplicate key list
    assert len(list(duplicates.values())[0]) == 2  # 2 entries with the same_string_key
    nvs_check.integrity_check(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_bad_mixed_same_key_different_page])
def test_check_duplicates_bad_same_key_different_pages(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    duplicates = prepare_duplicate_list(nvs)
    assert len(duplicates) == 9  # 9 duplicate keys in total (8 pairs of 2 duplicates + 1 triplet)
    for i, value in enumerate(list(duplicates.values())):
        if i < 8:
            assert len(value) == 2  # i in range 0-7 -- pairs of 2 entries with the same key
        else:
            assert len(value) == 3  # i == 8 -- 3 entries with the lorem_string key
    nvs_check.integrity_check(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_bad_same_key_blob_index])
def test_check_duplicates_bad_same_key_blob_index(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    duplicates = prepare_duplicate_list(nvs)
    assert (
        len(duplicates) == 1
    )  # Only one duplicate key list - blob_index and blob_data share the same key (which is OK),
    # however there are 2 duplicates of each blob_index and blob_data
    assert len(list(duplicates.values())[0]) == 6  # 6 entries with the blob_key (2x blob_index, 4x blob_data)
    nvs_check.integrity_check(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_read_only])
def test_check_read_only_partition(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    assert nvs.raw_data is not None
    assert len(nvs.raw_data) == 0x1000
    assert nvs_check.check_partition_size(nvs, logger, read_only=True)
    assert not nvs_check.check_empty_page_present(nvs, logger)


@pytest.mark.parametrize('setup_func', [setup_minimal_json])
def test_print_minimal_json(generate_nvs: Callable, setup_func: Callable, capsys: pytest.CaptureFixture) -> None:
    nvs = generate_nvs(setup_func)
    logger.set_format('json')
    print_minimal_json(nvs)
    captured = capsys.readouterr()
    assert captured.out.startswith('[')
    assert captured.out.endswith(']\n')
    assert '"namespace"' in captured.out
    assert '"key"' in captured.out
    assert '"encoding"' in captured.out
    assert '"data"' in captured.out
    assert '"state"' in captured.out
    assert '"is_empty"' in captured.out

    # Check if the data is correct
    assert '100' in captured.out and '-42' in captured.out and '96' in captured.out
    assert 'Another string data' in captured.out
    # Check if the LOREM_STRING is present and properly formatted
    lorem_string_escaped = LOREM_STRING.replace('\n', '\\n')
    assert lorem_string_escaped in captured.out
    # Check if the blob key data is present and correct
    assert captured.out.count('"key": "blob_key"') == 2

    # Load the captured output as JSON
    output_json = json.loads(captured.out)

    # Gather all entries with the key 'blob_key' and decode them from base64
    blob_key_data_binary = b''.join(
        base64.b64decode(entry['data']) for entry in output_json if entry['key'] == 'blob_key'
    )

    # Read the sample multipage blob data from the binary file
    with open('../nvs_partition_generator/testdata/sample_multipage_blob.bin', 'rb') as f:
        sample_blob_data = f.read()

    # Check if the gathered blob_key data matches the sample multipage blob data
    assert sample_blob_data == blob_key_data_binary
    # Check if all keys are present
    assert captured.out.count('blob_key') == 2
    assert captured.out.count('short_str_key') == 1
    assert captured.out.count('long_str_key') == 1
    assert captured.out.count('int32_test') == 2  # 2 entries for int32_test
    assert captured.out.count('uint32_test') == 1
    assert captured.out.count('int8_test') == 1


@pytest.mark.parametrize('setup_func', [setup_ok_non_ascii_string])
def test_check_non_ascii_string(generate_nvs: Callable, setup_func: Callable) -> None:
    nvs = generate_nvs(setup_func)
    assert nvs.raw_data is not None
    assert nvs_check.check_partition_size(nvs, logger, read_only=True)
    nvs_check.integrity_check(nvs, logger)

    non_ascii_string_entry = nvs.pages[0].entries[1]  # entries[0] is the namespace entry
    assert non_ascii_string_entry.key == 'string_key'

    assert non_ascii_string_entry.metadata['crc']['original'] == non_ascii_string_entry.metadata['crc']['computed']
    assert (
        non_ascii_string_entry.metadata['crc']['data_original']
        == non_ascii_string_entry.metadata['crc']['data_computed']
    )
