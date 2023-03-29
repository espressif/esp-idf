# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import shutil
from pathlib import Path
from typing import Union

from test_build_system_helpers import EnvDict, IdfPyFunc, file_contains


def get_two_header_bytes(file_path: Union[str, Path]) -> str:
    '''
    get the bytes 3-4 of the given file
    https://docs.espressif.com/projects/esptool/en/latest/esp32/advanced-topics/firmware-image-format.html
    '''
    data = b''
    with open(file_path, 'rb') as f:
        data = f.read(4)
    extracted_bytes = data[2:4]
    return extracted_bytes.hex()


def test_bootloader_custom_overrides_original(test_app_copy: Path, idf_py: IdfPyFunc, default_idf_env: EnvDict) -> None:
    logging.info('Custom bootloader overrides original')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    shutil.copytree(idf_path / 'components' / 'bootloader', test_app_copy / 'components' / 'bootloader')
    # Because of relative include of Kconfig, also esp_bootloader_format needs to be copied.
    shutil.copytree(idf_path / 'components' / 'esp_bootloader_format', test_app_copy / 'components' / 'esp_bootloader_format')
    idf_py('bootloader')
    assert file_contains(test_app_copy / 'build' / 'bootloader' / 'compile_commands.json',
                         str(test_app_copy / 'components' / 'bootloader' / 'subproject' / 'main' / 'bootloader_start.c'))


def test_bootloader_correctly_set_image_header(test_app_copy: Path, idf_py: IdfPyFunc) -> None:
    logging.info('Flash size is correctly set in the bootloader image header')
    # Build with the default 2MB setting
    idf_py('bootloader')
    assert get_two_header_bytes(test_app_copy / 'build' / 'bootloader' / 'bootloader.bin') == '0210'

    # Change to 4MB
    (test_app_copy / 'sdkconfig').write_text('CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y')
    idf_py('reconfigure', 'bootloader')
    assert get_two_header_bytes(test_app_copy / 'build' / 'bootloader' / 'bootloader.bin') == '0220'

    # Change to QIO, bootloader should still be DIO (will change to QIO in 2nd stage bootloader)
    (test_app_copy / 'sdkconfig').write_text('CONFIG_FLASHMODE_QIO=y')
    idf_py('reconfigure', 'bootloader')
    assert get_two_header_bytes(test_app_copy / 'build' / 'bootloader' / 'bootloader.bin') == '0210'

    # Change to 80 MHz
    (test_app_copy / 'sdkconfig').write_text('CONFIG_ESPTOOLPY_FLASHFREQ_80M=y')
    idf_py('reconfigure', 'bootloader')
    assert get_two_header_bytes(test_app_copy / 'build' / 'bootloader' / 'bootloader.bin') == '021f'
