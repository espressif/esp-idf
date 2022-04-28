#!/usr/bin/env python
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import hashlib
import hmac
import json
import os
import struct
import subprocess
import sys

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.utils import int_to_bytes

try:
    import nvs_partition_gen as nvs_gen
except ImportError:
    idf_path = os.getenv('IDF_PATH')
    if not idf_path or not os.path.exists(idf_path):
        raise Exception('IDF_PATH not found')
    sys.path.insert(0, os.path.join(idf_path, 'components', 'nvs_flash', 'nvs_partition_generator'))
    import nvs_partition_gen as nvs_gen

# Check python version is proper or not to avoid script failure
assert sys.version_info >= (3, 6, 0), 'Python version too low.'

esp_ds_data_dir = 'esp_ds_data'
# hmac_key_file is generated when HMAC_KEY is calculated, it is used when burning HMAC_KEY to efuse
hmac_key_file = esp_ds_data_dir + '/hmac_key.bin'
# csv and bin filenames are default filenames for nvs partition files created with this script
csv_filename = esp_ds_data_dir + '/pre_prov.csv'
bin_filename = esp_ds_data_dir + '/pre_prov.bin'
expected_json_path = os.path.join('build', 'config', 'sdkconfig.json')
# Targets supported by the script
supported_targets = {'esp32s2', 'esp32c3', 'esp32s3'}
supported_key_size = {'esp32s2':[1024, 2048, 3072, 4096], 'esp32c3':[1024, 2048, 3072], 'esp32s3':[1024, 2048, 3072, 4096]}


# @return
#       on success  idf_target - value of the IDF_TARGET read from build/config/sdkconfig.json
#       on failure  None
def get_idf_target():
    if os.path.exists(expected_json_path):
        sdkconfig = json.load(open(expected_json_path))
        idf_target_read = sdkconfig['IDF_TARGET']
        return idf_target_read
    else:
        print('ERROR: IDF_TARGET has not been set for the supported targets,'
              "\nplase execute command \"idf.py set-target {TARGET}\" in the example directory")
        return None


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


# @return
#       c               : ciphertext_c
#       iv              : initialization vector
#       key_size        : key size of the RSA private key in bytes.
# @input
#       privkey         : path to the RSA private key
#       priv_key_pass   : path to the RSA privaete key password
#       hmac_key        : HMAC key value ( to calculate DS params)
#       idf_target      : The target chip for the script (e.g. esp32s2, esp32c3, esp32s3)
# @info
#       The function calculates the encrypted private key parameters.
#       Consult the DS documentation (available for the ESP32-S2) in the esp-idf programming guide for more details about the variables and calculations.
def calculate_ds_parameters(privkey, priv_key_pass, hmac_key, idf_target):
    private_key = load_privatekey(privkey, priv_key_pass)
    if not isinstance(private_key, rsa.RSAPrivateKey):
        print('ERROR: Only RSA private keys are supported')
        sys.exit(-1)
    if hmac_key is None:
        print('ERROR: hmac_key cannot be None')
        sys.exit(-2)

    priv_numbers = private_key.private_numbers()
    pub_numbers = private_key.public_key().public_numbers()
    Y = priv_numbers.d
    M = pub_numbers.n
    key_size = private_key.key_size
    if key_size not in supported_key_size[idf_target]:
        print('ERROR: Private key size {0} not supported for the target {1},\nthe supported key sizes are {2}'
              .format(key_size, idf_target, str(supported_key_size[idf_target])))
        sys.exit(-1)

    iv = os.urandom(16)

    rr = 1 << (key_size * 2)
    rinv = rr % pub_numbers.n
    mprime = - rsa._modinv(M, 1 << 32)
    mprime &= 0xFFFFFFFF
    length = key_size // 32 - 1

    # get max supported key size for the respective target
    max_len = max(supported_key_size[idf_target])
    aes_key = hmac.HMAC(hmac_key, b'\xFF' * 32, hashlib.sha256).digest()

    md_in = number_as_bytes(Y, max_len) + \
        number_as_bytes(M, max_len) + \
        number_as_bytes(rinv, max_len) + \
        struct.pack('<II', mprime, length) + \
        iv

    # expected_len = max_len_Y + max_len_M + max_len_rinv + (mprime + length packed (8 bytes))+ iv (16 bytes)
    expected_len = (max_len / 8) * 3 + 8 + 16
    assert len(md_in) == expected_len
    md = hashlib.sha256(md_in).digest()
    # In case of ESP32-S2
    # Y4096 || M4096 || Rb4096 || M_prime32 || LENGTH32 || MD256 || 0x08*8
    # In case of ESP32-C3
    # Y3072 || M3072 || Rb3072 || M_prime32 || LENGTH32 || MD256 || 0x08*8
    p = number_as_bytes(Y, max_len) + \
        number_as_bytes(M, max_len) + \
        number_as_bytes(rinv, max_len) + \
        md + \
        struct.pack('<II', mprime, length) + \
        b'\x08' * 8

    # expected_len = max_len_Y + max_len_M + max_len_rinv + md (32 bytes) + (mprime + length packed (8bytes)) + padding (8 bytes)
    expected_len = (max_len / 8) * 3 + 32 + 8 + 8
    assert len(p) == expected_len

    cipher = Cipher(algorithms.AES(aes_key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    c = encryptor.update(p) + encryptor.finalize()
    return c, iv, key_size


# @info
#       The function makes use of the "espefuse.py" script to read the efuse summary
def efuse_summary(args, idf_target):
    os.system('python $IDF_PATH/components/esptool_py/esptool/espefuse.py --chip {0} -p {1} summary'.format(idf_target, (args.port)))


# @info
#       The function makes use of the "espefuse.py" script to burn the HMAC key on the efuse.
def efuse_burn_key(args, idf_target):
    # In case of a development (default) usecase we disable the read protection.
    key_block_status = '--no-read-protect'

    if args.production is True:
        # Whitespace character will have no additional effect on the command and
        # read protection will be enabled as the default behaviour of the command
        key_block_status = ' '

    os.system('python $IDF_PATH/components/esptool_py/esptool/espefuse.py --chip {0} -p {1} burn_key '
              '{2} {3} HMAC_DOWN_DIGITAL_SIGNATURE {4}'
              .format((idf_target), (args.port), ('BLOCK_KEY' + str(args.efuse_key_id)), (hmac_key_file), (key_block_status)))


# @info
#       Generate a custom csv file of encrypted private key parameters.
#       The csv file is required by the nvs_partition_generator utility to create the nvs partition.
def generate_csv_file(c, iv, hmac_key_id, key_size, csv_file):

    with open(csv_file, 'wt', encoding='utf8') as f:
        f.write('# This is a generated csv file containing required parameters for the Digital Signature operation\n')
        f.write('key,type,encoding,value\nesp_ds_ns,namespace,,\n')
        f.write('esp_ds_c,data,hex2bin,%s\n' % (c.hex()))
        f.write('esp_ds_iv,data,hex2bin,%s\n' % (iv.hex()))
        f.write('esp_ds_key_id,data,u8,%d\n' % (hmac_key_id))
        f.write('esp_ds_rsa_len,data,u16,%d\n' % (key_size))


class DefineArgs(object):
    def __init__(self, attributes):
        for key, value in attributes.items():
            self.__setattr__(key, value)


# @info
#       This function uses the nvs_partition_generater utility
#       to generate the nvs partition of the encrypted private key parameters.
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


# @return
#         The json formatted summary of the efuse.
def get_efuse_summary_json(args, idf_target):
    _efuse_summary = None
    try:
        _efuse_summary = subprocess.check_output(('python $IDF_PATH/components/esptool_py/esptool/espefuse.py '
                                                  '--chip {0} -p {1} summary --format json'.format(idf_target, (args.port))), shell=True)
    except subprocess.CalledProcessError as e:
        print((e.output).decode('UTF-8'))
        sys.exit(-1)

    _efuse_summary = _efuse_summary.decode('UTF-8')
    # Remove everything before actual json data from efuse_summary command output.
    _efuse_summary = _efuse_summary[_efuse_summary.find('{'):]
    try:
        _efuse_summary_json = json.loads(_efuse_summary)
    except json.JSONDecodeError:
        print('ERROR: failed to parse the json output')
        sys.exit(-1)
    return _efuse_summary_json


# @return
#       on success: 256 bit HMAC key present in the given key_block (args.efuse_key_id)
#       on failure: None
# @info
#       This function configures the provided efuse key_block.
#       If the provided efuse key_block is empty the function generates a new HMAC key and burns it in the efuse key_block.
#       If the key_block already contains a key the function reads the key from the efuse key_block
def configure_efuse_key_block(args, idf_target):
    efuse_summary_json = get_efuse_summary_json(args, idf_target)
    key_blk = 'BLOCK_KEY' + str(args.efuse_key_id)
    key_purpose = 'KEY_PURPOSE_' + str(args.efuse_key_id)

    kb_writeable = efuse_summary_json[key_blk]['writeable']
    kb_readable = efuse_summary_json[key_blk]['readable']
    hmac_key_read = None

    # If the efuse key block is writable (empty) then generate and write
    # the new hmac key and check again
    # If the efuse key block is not writable (already contains a key) then check if it is redable
    if kb_writeable is True:
        print('Provided key block (KEY BLOCK %1d) is writable\n Generating a new key and burning it in the efuse..\n' % (args.efuse_key_id))

        new_hmac_key = os.urandom(32)
        with open(hmac_key_file, 'wb') as key_file:
            key_file.write(new_hmac_key)
        # Burn efuse key
        efuse_burn_key(args, idf_target)
        if args.production is False:
            # Read fresh summary of the efuse to read the key value from efuse.
            # If the key read from efuse matches with the key generated
            # on host then burn_key operation was successfull
            new_efuse_summary_json = get_efuse_summary_json(args, idf_target)
            hmac_key_read = new_efuse_summary_json[key_blk]['value']
            print(hmac_key_read)
            hmac_key_read = bytes.fromhex(hmac_key_read)
            if new_hmac_key == hmac_key_read:
                print('Key was successfully written to the efuse (KEY BLOCK %1d)' % (args.efuse_key_id))
            else:
                print('ERROR: Failed to burn the hmac key to efuse (KEY BLOCK %1d),'
                      '\nPlease execute the script again using a different key id' % (args.efuse_key_id))
                return None
        else:
            new_efuse_summary_json = get_efuse_summary_json(args, idf_target)
            if new_efuse_summary_json[key_purpose]['value'] != 'HMAC_DOWN_DIGITAL_SIGNATURE':
                print('ERROR: Failed to verify the key purpose of the key block{})'.format(args.efuse_key_id))
                return None
            hmac_key_read = new_hmac_key
    else:
        # If the efuse key block is redable, then read the key from efuse block and use it for encrypting the RSA private key parameters.
        # If the efuse key block is not redable or it has key purpose set to a different
        # value than "HMAC_DOWN_DIGITAL_SIGNATURE" then we cannot use it for DS operation
        if kb_readable is True:
            if efuse_summary_json[key_purpose]['value'] == 'HMAC_DOWN_DIGITAL_SIGNATURE':
                print('Provided efuse key block (KEY BLOCK %1d) already contains a key with key_purpose=HMAC_DOWN_DIGITAL_SIGNATURE,'
                      '\nusing the same key for encrypting the private key data...\n' % (args.efuse_key_id))
                hmac_key_read = efuse_summary_json[key_blk]['value']
                hmac_key_read = bytes.fromhex(hmac_key_read)
                if args.keep_ds_data is True:
                    with open(hmac_key_file, 'wb') as key_file:
                        key_file.write(hmac_key_read)
            else:
                print('ERROR: Provided efuse key block ((KEY BLOCK %1d)) contains a key with key purpose different'
                      'than HMAC_DOWN_DIGITAL_SIGNATURE,\nplease execute the script again with a different value of the efuse key id.' % (args.efuse_key_id))
                return None
        else:
            print('ERROR: Provided efuse key block (KEY BLOCK %1d) is not readable and writeable,'
                  '\nplease execute the script again with a different value of the efuse key id.' % (args.efuse_key_id))
            return None

    # Return the hmac key burned into the efuse
    return hmac_key_read


def cleanup(args):
    if args.keep_ds_data is False:
        if os.path.exists(hmac_key_file):
            os.remove(hmac_key_file)
        if os.path.exists(csv_filename):
            os.remove(csv_filename)


def main():
    parser = argparse.ArgumentParser(description='''Generate an HMAC key and burn it in the desired efuse key block (required for Digital Signature),
    Generates an NVS partition containing the encrypted private key parameters from the client private key.
            ''')

    parser.add_argument(
        '--private-key',
        dest='privkey',
        default='client.key',
        metavar='relative/path/to/client-priv-key',
        help='relative path to client private key')

    parser.add_argument(
        '--pwd', '--password',
        dest='priv_key_pass',
        metavar='[password]',
        help='the password associated with the private key')

    parser.add_argument(
        '--summary',
        dest='summary',action='store_true',
        help='Provide this option to print efuse summary of the chip')

    parser.add_argument(
        '--efuse_key_id',
        dest='efuse_key_id', type=int, choices=range(1,6),
        metavar='[key_id] ',
        default=1,
        help='Provide the efuse key_id which contains/will contain HMAC_KEY, default is 1')

    parser.add_argument(
        '--port', '-p',
        dest='port',
        metavar='[port]',
        required=True,
        help='UART com port to which the ESP device is connected')

    parser.add_argument(
        '--keep_ds_data_on_host','-keep_ds_data',
        dest='keep_ds_data', action='store_true',
        help='Keep encrypted private key data and key on host machine for testing purpose')

    parser.add_argument(
        '--production', '-prod',
        dest='production', action='store_true',
        help='Enable production configurations. e.g.keep efuse key block read protection enabled')

    args = parser.parse_args()

    idf_target = get_idf_target()
    if idf_target not in supported_targets:
        if idf_target is not None:
            print('ERROR: The script does not support the target %s' % idf_target)
        sys.exit(-1)
    idf_target = str(idf_target)

    if args.summary is not False:
        efuse_summary(args, idf_target)
        sys.exit(0)

    if (os.path.exists(args.privkey) is False):
        print('ERROR: The provided private key file does not exist')
        sys.exit(-1)

    if (os.path.exists(esp_ds_data_dir) is False):
        os.makedirs(esp_ds_data_dir)

    # Burn hmac_key on the efuse block (if it is empty) or read it
    # from the efuse block (if the efuse block already contains a key).
    hmac_key_read = configure_efuse_key_block(args, idf_target)
    if hmac_key_read is None:
        sys.exit(-1)

    # Calculate the encrypted private key data along with all other parameters
    c, iv, key_size = calculate_ds_parameters(args.privkey, args.priv_key_pass, hmac_key_read, idf_target)

    # Generate csv file for the DS data and generate an NVS partition.
    generate_csv_file(c, iv, args.efuse_key_id, key_size, csv_filename)
    generate_nvs_partition(csv_filename, bin_filename)
    cleanup(args)


if __name__ == '__main__':
    main()
