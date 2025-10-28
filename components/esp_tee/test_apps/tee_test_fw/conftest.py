# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# pylint: disable=W0621  # redefined-outer-name
import base64
import csv
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any

import espsecure
import esptool
import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.serial import IdfSerial
from pytest_embedded_serial_esp.serial import EspSerial

# fmt: off
esp_tee_empty_bin = {
    'esp32c6': [
        0xE9, 0x04, 0x02, 0x10, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x0D, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x58),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x56,
        # Image SHA256
        0xF4, 0xA4, 0xCF, 0x06, 0xAE, 0x94, 0x75, 0x47, 0xBC, 0x88, 0xA2, 0xCA,
        0x52, 0x97, 0x7A, 0x5C, 0x55, 0x43, 0xD9, 0xF5, 0xD3, 0x45, 0xD1, 0x34,
        0xFC, 0x74, 0xB2, 0xB9, 0x34, 0x72, 0xC3, 0x00
    ],
    'esp32h2': [
        0xE9, 0x04, 0x02, 0x1F, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x58),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x56,
        # Image SHA256
        0xDC, 0x60, 0x86, 0x6C, 0x37, 0x76, 0xAA, 0x30, 0x1F, 0x61, 0x48, 0x23,
        0xEA, 0x34, 0xAA, 0x19, 0xE8, 0xDE, 0x04, 0x7D, 0x2A, 0x30, 0xC1, 0xDD,
        0x61, 0x38, 0x9D, 0xB5, 0xCA, 0x13, 0x5A, 0x79
    ],
    'esp32c5': [
        0xE9, 0x04, 0x02, 0x1F, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x17, 0x00, 0x00, 0x64, 0x00, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x58),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x56,
        # Image SHA256
        0xCD, 0xCC, 0xF2, 0xE3, 0x52, 0x76, 0xE5, 0x6D, 0xF6, 0x32, 0x95, 0x27,
        0x5F, 0xF3, 0xD8, 0x90, 0xD7, 0x95, 0xA0, 0x95, 0xD5, 0xDA, 0xE7, 0xA4,
        0x58, 0x08, 0x84, 0xBB, 0x8F, 0x29, 0xAB, 0xE4
    ],
}
# fmt: on


# This is a custom IdfSerial class to support custom functionality
# which is required only for this test
class TEESerial(IdfSerial):
    def __init__(self, *args, **kwargs) -> None:  # type: ignore
        super().__init__(*args, **kwargs)

    def _get_flash_size(self) -> Any:
        return self.app.sdkconfig.get('ESPTOOLPY_FLASHSIZE', '')

    @EspSerial.use_esptool()
    def bootloader_force_flash_if_req(self) -> None:
        # Forcefully flash the bootloader only if security features are enabled
        if any(
            (
                self.app.sdkconfig.get('SECURE_BOOT', True),
                self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED', True),
            )
        ):
            offs = int(self.app.sdkconfig.get('BOOTLOADER_OFFSET_IN_FLASH', 0))
            bootloader_path = os.path.join(self.app.binary_path, 'bootloader', 'bootloader.bin')
            encrypt = '--encrypt' if self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED') else ''
            flash_size = self._get_flash_size()

            esptool.main(
                f'--no-stub write-flash {offs} {bootloader_path} --force {encrypt} --flash-size {flash_size}'.split(),
                esp=self.esp,
            )

    @EspSerial.use_esptool()
    def custom_erase_partition(self, partition: str) -> None:
        if self.app.sdkconfig.get('SECURE_ENABLE_SECURE_ROM_DL_MODE'):
            with tempfile.NamedTemporaryFile(delete=True) as temp_file:
                offs = self.app.partition_table[partition]['offset']
                size = self.app.partition_table[partition]['size']
                flash_size = self._get_flash_size()

                binstr = b'\xff' * int(size)
                temp_file.write(binstr)
                temp_file.flush()

                esptool.main(
                    f'--no-stub write-flash {offs} {temp_file.name} --flash-size {flash_size}'.split(), esp=self.esp
                )
        else:
            self.erase_partition(partition)

    @EspSerial.use_esptool()
    def custom_write_partition(self, partition: str, bin_path: str, encrypt: bool = False) -> None:
        offs = self.app.partition_table[partition]['offset']
        no_stub = '--no-stub' if self.app.sdkconfig.get('SECURE_ENABLE_SECURE_ROM_DL_MODE') else ''
        flash_size = self._get_flash_size()
        flash_file = bin_path

        args = f'{no_stub} write-flash {offs} {flash_file}'.split()

        if encrypt:
            args.append('--encrypt')
        args += f'--flash-size {flash_size}'.split()

        esptool.main(args, esp=self.esp)

    @EspSerial.use_esptool()
    def copy_test_tee_img(self, partition: str, is_rollback: bool = False) -> None:
        flash_file = os.path.join(self.app.binary_path, 'esp_tee', 'esp_tee.bin')
        encrypt = self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED', False)

        if is_rollback:
            datafile = 'esp_tee_empty.bin'
            datafile_signed = 'esp_tee_empty_signed.bin'
            flash_file = datafile

            with open(datafile, 'wb') as data_file:
                bin_data = esp_tee_empty_bin.get(self.app.sdkconfig.get('IDF_TARGET'), None)
                if bin_data is not None:
                    data_file.write(bytes(bin_data))
                    data_file.flush()

            if self.app.sdkconfig.get('SECURE_BOOT'):
                keyfile = self.app.sdkconfig.get('SECURE_BOOT_SIGNING_KEY')
                espsecure.main(
                    [
                        'sign-data',
                        '--version',
                        '2',
                        '--append-signatures',
                        '--keyfile',
                        keyfile,
                        '--output',
                        datafile_signed,
                        datafile,
                    ]
                )
                flash_file = datafile_signed

        self.custom_write_partition(partition, flash_file, encrypt=encrypt)

        if is_rollback:
            for file in [datafile, datafile_signed]:
                if os.path.exists(file):
                    os.remove(file)

    @EspSerial.use_esptool()
    def custom_flash(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()

    @EspSerial.use_esptool()
    def custom_flash_w_test_tee_img_gen(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()
        self.copy_test_tee_img('ota_1', False)

    @EspSerial.use_esptool()
    def custom_flash_w_test_tee_img_rb(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()
        self.copy_test_tee_img('ota_1', True)

    @EspSerial.use_esptool()
    def custom_flash_with_empty_sec_stg(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()
        self.custom_erase_partition('secure_storage')

    KEY_DEFS: list[dict[str, Any]] = [
        {'key': 'aes256_key0', 'type': 'aes256', 'input': None, 'write_once': True},
        {
            'key': 'aes256_key1',
            'type': 'aes256',
            'input': 'aes256_key.bin',
            'write_once': False,
            'b64': 'qZxftt2T8mOpLxALIfsDqI65srqPxrJtCVnDU8wrKXbFCJekDRzXqINlU5s=',
        },
        {'key': 'p256_key0', 'type': 'ecdsa_p256', 'input': None, 'write_once': False},
        {
            'key': 'attest_key',
            'type': 'ecdsa_p256',
            'input': 'ecdsa_p256_key.pem',
            'write_once': True,
            'b64': (
                'LS0tLS1CRUdJTiBFQyBQUklWQVRFIEtFWS0tLS0tCk1IY0NBUUVFSUlNU1VpUktHaVZjSTIvbUZFekI3eXRIOVJj'
                'd0wyUThkNDhONHNFUHFYc0RvQW9HQ0NxR1NNNDkKQXdFSG9VUURRZ0FFSkYxYXRZQUxrdnB4cCt4N3c1dmVPQ1Vj'
                'RUhFRTY5azkvcFB5eFdTbEZkbW5wMnBmbVJpZwp5NnRTMDNaM2tnN2hYcitTQmNLbmRCV2RlZW81Vm9XV29nPT0K'
                'LS0tLS1FTkQgRUMgUFJJVkFURSBLRVktLS0tLQo='
            ),
        },
    ]

    NVS_KEYS_B64 = 'MzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzPMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzA=='

    TMP_DIR = Path('tmp')
    NVS_KEYS_PATH = TMP_DIR / 'nvs_keys.bin'
    NVS_CSV_PATH = TMP_DIR / 'tee_sec_stg_val.csv'
    NVS_BIN_PATH = TMP_DIR / 'tee_sec_stg_nvs.bin'

    def run_command(self, command: list[str]) -> None:
        try:
            subprocess.check_call(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
            print(f'Command failed: {" ".join(command)}')
            raise

    def write_keys_to_file(self, b64_data: str, path: Path) -> None:
        path.write_bytes(base64.b64decode(b64_data))

    def create_tee_sec_stg_csv(self, tmp_dir: Path) -> Path:
        csv_path = self.NVS_CSV_PATH
        rows: list[list[str]] = [
            ['key', 'type', 'encoding', 'value'],
            ['tee_sec_stg_ns', 'namespace', '', ''],
        ]
        rows += [[entry['key'], 'file', 'binary', str(tmp_dir / f'{entry["key"]}.bin')] for entry in self.KEY_DEFS]
        with csv_path.open('w', newline='') as f:
            csv.writer(f).writerows(rows)
        return csv_path

    def custom_flash_with_host_gen_sec_stg_img(self) -> None:
        tmp_dir = self.TMP_DIR
        tmp_dir.mkdir(parents=True, exist_ok=True)

        for entry in self.KEY_DEFS:
            if entry['input']:
                input_path = tmp_dir / entry['input']
                self.write_keys_to_file(entry['b64'], input_path)
                entry['input'] = str(input_path)
        self.write_keys_to_file(self.NVS_KEYS_B64, self.NVS_KEYS_PATH)

        idf_path = Path(os.environ['IDF_PATH'])
        ESP_TEE_SEC_STG_KEYGEN = os.path.join(
            idf_path, 'components', 'esp_tee', 'scripts', 'esp_tee_sec_stg_keygen', 'esp_tee_sec_stg_keygen.py'
        )
        NVS_PARTITION_GEN = os.path.join(
            idf_path, 'components', 'nvs_flash', 'nvs_partition_generator', 'nvs_partition_gen.py'
        )

        cmds = [
            [sys.executable, ESP_TEE_SEC_STG_KEYGEN, '-k', entry['type'], '-o', str(tmp_dir / f'{entry["key"]}.bin')]
            + (['-i', entry['input']] if entry['input'] else [])
            + (['--write-once'] if entry['write_once'] else [])
            for entry in self.KEY_DEFS
        ]

        csv_path = self.create_tee_sec_stg_csv(tmp_dir)
        nvs_bin = self.NVS_BIN_PATH
        nvs_keys = self.NVS_KEYS_PATH
        size = self.app.partition_table['secure_storage']['size']

        cmds.append(
            [
                sys.executable,
                NVS_PARTITION_GEN,
                'encrypt',
                str(csv_path),
                str(nvs_bin),
                str(size),
                '--inputkey',
                str(nvs_keys),
            ]
        )

        try:
            for cmd in cmds:
                self.run_command(cmd)

            self.bootloader_force_flash_if_req()
            self.flash()
            self.custom_erase_partition('secure_storage')
            self.custom_write_partition('secure_storage', nvs_bin)

        finally:
            shutil.rmtree(tmp_dir)


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', TEESerial)
