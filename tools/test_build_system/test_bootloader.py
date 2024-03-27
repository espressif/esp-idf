# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import shutil
from pathlib import Path
from typing import Union

from test_build_system_helpers import EnvDict, IdfPyFunc, bin_file_contains, file_contains, replace_in_file


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
                         (test_app_copy / 'components' / 'bootloader' / 'subproject' / 'main' / 'bootloader_start.c'))


def test_bootloader_custom_ignores_extra_component(test_app_copy: Path, idf_py: IdfPyFunc, default_idf_env: EnvDict) -> None:
    logging.info('Custom bootloader can ignore extra components')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    # Import the main bootloader component that overrides the default one
    shutil.copytree(idf_path / 'examples' / 'custom_bootloader' / 'bootloader_override' / 'bootloader_components',
                    test_app_copy / 'bootloader_components')
    # Compile it normally and make sure the bootloader is overridden for now
    idf_py('bootloader')
    # Search for 'Custom bootloader message defined in the KConfig file.' in the resulting binary
    # which is the default value for EXAMPLE_BOOTLOADER_WELCOME_MESSAGE Kconfig option.
    default_message = bytes('Custom bootloader message defined in the KConfig file.', 'ascii')
    assert bin_file_contains(test_app_copy / 'build' / 'bootloader' / 'bootloader.bin', default_message)
    # Clean the project
    idf_py('fullclean')
    # Ignore bootloader component and rebuild
    replace_in_file(test_app_copy / 'CMakeLists.txt',
                    '# placeholder_after_include_project_cmake',
                    'set(BOOTLOADER_IGNORE_EXTRA_COMPONENT "main")')
    idf_py('bootloader')
    # The overridden message must not be present anymore
    assert not bin_file_contains(test_app_copy / 'build' / 'bootloader' / 'bootloader.bin', default_message)


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
