#!/usr/bin/env python
# Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import argparse
import os
import sys
import hashlib
import hmac
import struct
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.utils import int_to_bytes

try:
    import nvs_partition_gen as nvs_gen
except ImportError:
    idf_path = os.getenv("IDF_PATH")
    if not idf_path or not os.path.exists(idf_path):
        raise Exception("IDF_PATH not found")
    sys.path.insert(0, os.path.join(idf_path, "components", "nvs_flash", "nvs_partition_generator"))
    import nvs_partition_gen as nvs_gen


esp_ds_data_dir = 'esp_ds_data'
# hmac_key_file is generated when HMAC_KEY is calculated, it is used when burning HMAC_KEY to efuse
hmac_key_file = esp_ds_data_dir + '/hmac_key.bin'
# csv and bin filenames are default filenames for nvs partition files created with this script
csv_filename = esp_ds_data_dir + '/pre_prov.csv'
bin_filename = esp_ds_data_dir + '/pre_prov.bin'


def load_privatekey(key_file_path, password=None):
    key_file = open(key_file_path, 'rb')
    key = key_file.read()
    key_file.close()
    return serialization.load_pem_private_key(key, password=password, backend=default_backend())


def number_as_bytes(number, pad_bits=None):
    """
    Given a number, format as a little endian array of bytes
    """
    result = int_to_bytes(number)[::-1]
    while pad_bits is not None and len(result) < (pad_bits // 8):
        result += b'\x00'
    return result


def calculate_ds_parameters(privkey, priv_key_pass):
    private_key = load_privatekey(privkey, priv_key_pass)
    if not isinstance(private_key, rsa.RSAPrivateKey):
        print("Only RSA private keys are supported")
        sys.exit(-1)

    priv_numbers = private_key.private_numbers()
    pub_numbers = private_key.public_key().public_numbers()
    Y = priv_numbers.d
    M = pub_numbers.n
    key_size = private_key.key_size
    supported_key_size = [1024, 2048, 3072, 4096]
    if key_size not in supported_key_size:
        print("Key size not supported, supported sizes are" + str(supported_key_size))
        sys.exit(-1)

    hmac_key = os.urandom(32)
    with open(hmac_key_file, 'wb') as key_file:
        key_file.write(hmac_key)

    iv = os.urandom(16)

    rr = 1 << (key_size * 2)
    rinv = rr % pub_numbers.n
    mprime = - rsa._modinv(M, 1 << 32)
    mprime &= 0xFFFFFFFF
    length = key_size // 32 - 1

    aes_key = hmac.HMAC(hmac_key, b"\xFF" * 32, hashlib.sha256).digest()

    md_in = number_as_bytes(Y, 4096) + \
        number_as_bytes(M, 4096) + \
        number_as_bytes(rinv, 4096) + \
        struct.pack("<II", mprime, length) + \
        iv
    assert len(md_in) == 12480 / 8
    md = hashlib.sha256(md_in).digest()

    # Y4096 || M4096 || Rb4096 || M_prime32 || LENGTH32 || MD256 || 0x08*8
    p = number_as_bytes(Y, 4096) + \
        number_as_bytes(M, 4096) + \
        number_as_bytes(rinv, 4096) + \
        md + \
        struct.pack("<II", mprime, length) + \
        b'\x08' * 8

    assert len(p) == 12672 / 8

    cipher = Cipher(algorithms.AES(aes_key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    c = encryptor.update(p) + encryptor.finalize()
    return c, iv, key_size


def efuse_summary(args):
    os.system("python $IDF_PATH/components/esptool_py/esptool/espefuse.py --chip esp32s2 -p %s summary" % (args.port))


def efuse_burn_key(args):
    os.system("python $IDF_PATH/components/esptool_py/esptool/espefuse.py --chip esp32s2 -p %s burn_key "
              "%s %s HMAC_DOWN_DIGITAL_SIGNATURE  --no-read-protect"
              % ((args.port), ("BLOCK_KEY" + str(args.efuse_key_id)), (hmac_key_file)))


def generate_csv_file(c, iv, hmac_key_id, key_size, csv_file):

    with open(csv_file, 'wt', encoding='utf8') as f:
        f.write("# This is a generated csv file containing required parameters for the Digital Signature operaiton\n")
        f.write("key,type,encoding,value\nesp_ds_ns,namespace,,\n")
        f.write("esp_ds_c,data,hex2bin,%s\n" % (c.hex()))
        f.write("esp_ds_iv,data,hex2bin,%s\n" % (iv.hex()))
        f.write("esp_ds_key_id,data,u8,%d\n" % (hmac_key_id))
        f.write("esp_ds_rsa_len,data,u16,%d\n" % (key_size))


class DefineArgs(object):
    def __init__(self, attributes):
        for key, value in attributes.items():
            self.__setattr__(key, value)


def generate_nvs_partition(input_filename, output_filename):

    nvs_args = DefineArgs({
        'input': input_filename,
        'outdir': os.getcwd(),
        'output': output_filename,
        'size': hex(0x3000),
        'version': 2,
        'keyfile':None,
    })

    nvs_gen.generate(nvs_args, is_encr_enabled=False, encr_key=None)


def main():
    parser = argparse.ArgumentParser(description='''Genereate an nvs partition containing the DS private key parameters from the client private key,
            Generate an HMAC key and burn it in the desired efuse key block (required for Digital Signature)''')

    parser.add_argument(
        '--private-key',
        dest='privkey',
        default='main/client.key',
        metavar='relative/path/to/client-priv-key',
        help='relative path to client private key')

    parser.add_argument(
        "--pwd", '--password',
        dest='priv_key_pass',
        metavar='[password]',
        help='the password associated with the private key')

    parser.add_argument(
        '--summary',
        dest='summary',action='store_true',
        help='Provide this option to print efuse summary the chip')

    parser.add_argument(
        '--efuse_key_id',
        dest='efuse_key_id', type=int, choices=range(1,6),
        metavar='[key_id] ',
        default=1,
        help='Provide the efuse key_id which contains/will contain HMAC_KEY, default is 1')

    parser.add_argument(
        "--port", '-p',
        dest='port',
        metavar='[port]',
        required=True,
        help='UART com port to which ESP device is connected')

    parser.add_argument(
        '--overwrite',
        dest='overwrite', action='store_true',
        help='Overwrite previously generated keys')

    args = parser.parse_args()

    if args.summary is not False:
        efuse_summary(args)
        sys.exit(0)

    if (os.path.exists(esp_ds_data_dir) is False):
        os.makedirs(esp_ds_data_dir)
    else:
        if (args.overwrite is False):
            print("WARNING: previous ecrypted private key data exists.\nIf you want to overwrite,"
                  " please execute your command with providing \"--overwrite\" option")
            sys.exit(0)
        else:
            print("overwriting previous encrypted private key data, as you have provided \"--overwrite\" option")

    c, iv, key_size = calculate_ds_parameters(args.privkey, args.priv_key_pass)
    efuse_burn_key(args)

    generate_csv_file(c, iv, args.efuse_key_id, key_size, csv_filename)
    generate_nvs_partition(csv_filename, bin_filename)


if __name__ == "__main__":
    main()
