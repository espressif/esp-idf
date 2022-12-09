#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
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
#

from __future__ import print_function
from future.moves.itertools import zip_longest
from io import open
import sys
import os
import argparse
import distutils.dir_util

try:
    sys.path.insert(0, os.getenv('IDF_PATH') + "/components/nvs_flash/nvs_partition_generator/")
    import nvs_partition_gen
except Exception as e:
    print(e)
    sys.exit("Please check IDF_PATH")


def verify_values_exist(input_values_file, values_file_data, key_count_in_values_file, line_no=1):
    """ Verify all keys have corresponding values in values file
    """
    if len(values_file_data) != key_count_in_values_file:
        raise SystemExit("\nError: Number of values is not equal to number of keys in file: %s at line No:%s\n"
                         % (str(input_values_file), str(line_no)))


def verify_keys_exist(values_file_keys, config_file_data):
    """ Verify all keys from config file are present in values file
    """
    keys_missing = []

    for line_no, config_data in enumerate(config_file_data,1):
        if not isinstance(config_data, str):
            config_data = config_data.encode('utf-8')
        config_data_line = config_data.strip().split(',')
        if 'namespace' not in config_data_line:
            if values_file_keys:
                if config_data_line[0] == values_file_keys[0]:
                    del values_file_keys[0]
                else:
                    keys_missing.append([config_data_line[0], line_no])
            else:
                keys_missing.append([config_data_line[0], line_no])

    if keys_missing:
        for key, line_no in keys_missing:
            print("Key:`", str(key), "` at line no:", str(line_no),
                  " in config file is not found in values file.")
        raise SystemExit(1)


def verify_datatype_encoding(input_config_file, config_file_data):
    """ Verify datatype and encodings from config file is valid
    """
    valid_encodings = ["string", "binary", "hex2bin","u8", "i8", "u16", "u32", "i32","base64"]
    valid_datatypes = ["file","data","namespace"]
    line_no = 0

    for data in config_file_data:
        line_no += 1
        if not isinstance(data, str):
            data = data.encode('utf-8')
        line = data.strip().split(',')
        if line[1] not in valid_datatypes:
            raise SystemExit("Error: config file: %s has invalid datatype at line no:%s\n"
                             % (str(input_config_file), str(line_no)))
        if 'namespace' not in line:
            if line[2] not in valid_encodings:
                raise SystemExit("Error: config file: %s has invalid encoding at line no:%s\n"
                                 % (str(input_config_file), str(line_no)))


def verify_file_data_count(cfg_file_data, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3
    (as format must be: <key,type and encoding>)
    """
    line_no = 0

    for data in cfg_file_data:
        line_no += 1
        if not isinstance(data, str):
            data = data.encode('utf-8')
        line = data.strip().split(',')
        if len(line) != 3 and line[0] not in keys_repeat:
            raise SystemExit("Error: data missing in config file at line no:%s <format needed:key,type,encoding>\n"
                             % str(line_no))


def verify_data_in_file(input_config_file, input_values_file, config_file_keys, keys_in_values_file, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3 \
    (as format must be: <key,type and encoding>)
    Verify datatype and encodings from config file is valid
    Verify all keys from config file are present in values file and \
    Verify each key has corresponding value in values file
    """
    try:
        values_file_keys = []
        values_file_line = None

        # Get keys from values file present in config files
        values_file_keys = get_keys(keys_in_values_file, config_file_keys)

        with open(input_config_file, 'r', newline='\n') as cfg_file:
            cfg_file_data = cfg_file.readlines()
            verify_file_data_count(cfg_file_data, keys_repeat)
            verify_datatype_encoding(input_config_file, cfg_file_data)
            verify_keys_exist(values_file_keys, cfg_file_data)

        with open(input_values_file, 'r', newline='\n') as values_file:
            key_count_in_values_file = len(keys_in_values_file)
            lineno = 0
            # Read first keys(header) line
            values_file_data = values_file.readline()
            lineno += 1
            while values_file_data:
                # Read values line
                values_file_line = values_file.readline()
                if not isinstance(values_file_line, str):
                    values_file_line = values_file_line.encode('utf-8')

                values_file_data = values_file_line.strip().split(',')

                lineno += 1
                if len(values_file_data) == 1 and '' in values_file_data:
                    break
                verify_values_exist(input_values_file, values_file_data, key_count_in_values_file, line_no=lineno)

    except Exception as err:
        print(err)
        exit(1)


def get_keys(keys_in_values_file, config_file_keys):
    """ Get keys from values file present in config file
    """
    values_file_keys = []
    for key in keys_in_values_file:
        if key in config_file_keys:
            values_file_keys.append(key)

    return values_file_keys


def add_config_data_per_namespace(input_config_file):
    """ Add config data per namespace to `config_data_to_write` list
    """
    config_data_to_write = []
    config_data_per_namespace = []

    with open(input_config_file, 'r', newline='\n') as cfg_file:
        config_data = cfg_file.readlines()

    # `config_data_per_namespace` is added to `config_data_to_write` list after reading next namespace
    for data in config_data:
        if not isinstance(data, str):
            data = data.encode('utf-8')
        cfg_data = data.strip().split(',')
        if 'REPEAT' in cfg_data:
            cfg_data.remove('REPEAT')
        if 'namespace' in cfg_data:
            if config_data_per_namespace:
                config_data_to_write.append(config_data_per_namespace)
                config_data_per_namespace = []
                config_data_per_namespace.append(cfg_data)
            else:
                config_data_per_namespace.append(cfg_data)
        else:
            config_data_per_namespace.append(cfg_data)

    # `config_data_per_namespace` is added to `config_data_to_write` list as EOF is reached
    if (not config_data_to_write) or (config_data_to_write and config_data_per_namespace):
        config_data_to_write.append(config_data_per_namespace)

    return config_data_to_write


def get_fileid_val(file_identifier, keys_in_config_file, keys_in_values_file,
                   values_data_line, key_value_data, fileid_value):
    """ Get file identifier value
    """
    file_id_found = False

    for key in key_value_data:
        if file_identifier and not file_id_found and file_identifier in key:
            fileid_value = key[1]
            file_id_found = True

    if not file_id_found:
        fileid_value = str(int(fileid_value) + 1)

    return fileid_value


def add_data_to_file(config_data_to_write, key_value_pair, output_csv_file):
    """ Add data to csv target file
    """
    header = ['key', 'type', 'encoding', 'value']
    data_to_write = []
    newline = u"\n"

    target_csv_file = open(output_csv_file, 'w', newline=None)

    line_to_write = u",".join(header)
    target_csv_file.write(line_to_write)
    target_csv_file.write(newline)
    for namespace_config_data in config_data_to_write:
        for data in namespace_config_data:
            data_to_write = data[:]
            if 'namespace' in data:
                data_to_write.append('')
                line_to_write = u",".join(data_to_write)
                target_csv_file.write(line_to_write)
                target_csv_file.write(newline)
            else:
                key = data[0]
                while key not in key_value_pair[0]:
                    del key_value_pair[0]
                if key in key_value_pair[0]:
                    value = key_value_pair[0][1]
                    data_to_write.append(value)
                    del key_value_pair[0]
                    line_to_write = u",".join(data_to_write)
                    target_csv_file.write(line_to_write)
                    target_csv_file.write(newline)

    # Set index to start of file
    target_csv_file.seek(0)
    target_csv_file.close()


def create_dir(filetype, output_dir_path):
    """ Create new directory(if doesn't exist) to store file generated
    """
    output_target_dir = os.path.join(output_dir_path,filetype,'')
    if not os.path.isdir(output_target_dir):
        distutils.dir_util.mkpath(output_target_dir)

    return output_target_dir


def set_repeat_value(total_keys_repeat, keys, csv_file, target_filename):
    key_val_pair = []
    key_repeated = []
    line = None
    newline = u"\n"
    with open(csv_file, 'r', newline=None) as read_from, open(target_filename,'w', newline=None) as write_to:
        headers = read_from.readline()
        values = read_from.readline()
        write_to.write(headers)
        write_to.write(values)
        if not isinstance(values, str):
            values = values.encode('utf-8')
        values = values.strip().split(',')
        total_keys_values = list(zip_longest(keys, values))

        # read new data, add value if key has repeat tag, write to new file
        line = read_from.readline()
        if not isinstance(line, str):
            line = line.encode('utf-8')
        row = line.strip().split(',')
        while row:
            index = -1
            key_val_new = list(zip_longest(keys, row))
            key_val_pair = total_keys_values[:]
            key_repeated = total_keys_repeat[:]
            while key_val_new and key_repeated:
                index = index + 1
                #  if key has repeat tag, get its corresponding value, write to file
                if key_val_new[0][0] == key_repeated[0]:
                    val = key_val_pair[0][1]
                    row[index] = val
                    del key_repeated[0]
                del key_val_new[0]
                del key_val_pair[0]

            line_to_write = u",".join(row)
            write_to.write(line_to_write)
            write_to.write(newline)

            # Read next line
            line = read_from.readline()
            if not isinstance(line, str):
                line = line.encode('utf-8')
            row = line.strip().split(',')
            if len(row) == 1 and '' in row:
                break

    return target_filename


def create_intermediate_csv(args, keys_in_config_file, keys_in_values_file, keys_repeat, is_encr=False):
    file_identifier_value = '0'
    csv_str = 'csv'
    bin_str = 'bin'
    line = None
    set_output_keyfile = False

    # Add config data per namespace to `config_data_to_write` list
    config_data_to_write = add_config_data_per_namespace(args.conf)

    try:
        with open(args.values, 'r', newline=None) as csv_values_file:
            # first line must be keys in file
            line = csv_values_file.readline()
            if not isinstance(line, str):
                line = line.encode('utf-8')
            keys = line.strip().split(',')

        filename, file_ext = os.path.splitext(args.values)
        target_filename = filename + "_created" + file_ext
        if keys_repeat:
            target_values_file = set_repeat_value(keys_repeat, keys, args.values, target_filename)
        else:
            target_values_file = args.values

        csv_values_file = open(target_values_file, 'r', newline=None)

        # Read header line
        csv_values_file.readline()

        # Create new directory(if doesn't exist) to store csv file generated
        output_csv_target_dir = create_dir(csv_str, args.outdir)
        # Create new directory(if doesn't exist) to store bin file generated
        output_bin_target_dir = create_dir(bin_str, args.outdir)
        if args.keygen:
            set_output_keyfile = True

        line = csv_values_file.readline()
        if not isinstance(line, str):
            line = line.encode('utf-8')
        values_data_line = line.strip().split(',')

        while values_data_line:
            key_value_data = list(zip_longest(keys_in_values_file, values_data_line))

            # Get file identifier value from values file
            file_identifier_value = get_fileid_val(args.fileid, keys_in_config_file,
                                                   keys_in_values_file, values_data_line, key_value_data,
                                                   file_identifier_value)

            key_value_pair = key_value_data[:]

            # Verify if output csv file does not exist
            csv_filename = args.prefix + "-" + file_identifier_value + "." + csv_str
            output_csv_file = output_csv_target_dir + csv_filename
            if os.path.isfile(output_csv_file):
                raise SystemExit("Target csv file: %s already exists.`" % output_csv_file)

            # Add values corresponding to each key to csv intermediate file
            add_data_to_file(config_data_to_write, key_value_pair, output_csv_file)
            print("\nCreated CSV file: ===>", output_csv_file)

            # Verify if output bin file does not exist
            bin_filename = args.prefix + "-" + file_identifier_value + "." + bin_str
            output_bin_file = output_bin_target_dir + bin_filename
            if os.path.isfile(output_bin_file):
                raise SystemExit("Target binary file: %s already exists.`" % output_bin_file)

            args.input = output_csv_file
            args.output = os.path.join(bin_str, bin_filename)
            if set_output_keyfile:
                args.keyfile = "keys-" + args.prefix + "-" + file_identifier_value

            if is_encr:
                nvs_partition_gen.encrypt(args)
            else:
                nvs_partition_gen.generate(args)

            # Read next line
            line = csv_values_file.readline()
            if not isinstance(line, str):
                line = line.encode('utf-8')
            values_data_line = line.strip().split(',')
            if len(values_data_line) == 1 and '' in values_data_line:
                break

        print("\nFiles generated in %s ..." % args.outdir)

    except Exception as e:
        print(e)
        exit(1)
    finally:
        csv_values_file.close()


def verify_empty_lines_exist(file_name, input_file_data):
    for data in input_file_data:
        if not isinstance(data, str):
            data = data.encode('utf-8')
        cfg_data = data.strip().split(',')

        if len(cfg_data) == 1 and '' in cfg_data:
            raise SystemExit("Error: file: %s cannot have empty lines. " % file_name)


def verify_file_format(args):
    keys_in_config_file = []
    keys_in_values_file = []
    keys_repeat = []
    file_data_keys = None

    # Verify config file is not empty
    if os.stat(args.conf).st_size == 0:
        raise SystemExit("Error: config file: %s is empty." % args.conf)

    # Verify values file is not empty
    if os.stat(args.values).st_size == 0:
        raise SystemExit("Error: values file: %s is empty." % args.values)

    # Verify config file does not have empty lines
    with open(args.conf, 'r', newline='\n') as csv_config_file:
        try:
            file_data = csv_config_file.readlines()
            verify_empty_lines_exist(args.conf, file_data)

            csv_config_file.seek(0)
            # Extract keys from config file
            for data in file_data:
                if not isinstance(data, str):
                    data = data.encode('utf-8')
                line_data = data.strip().split(',')
                if 'namespace' not in line_data:
                    keys_in_config_file.append(line_data[0])
                if 'REPEAT' in line_data:
                    keys_repeat.append(line_data[0])
        except Exception as e:
            print(e)

    # Verify values file does not have empty lines
    with open(args.values, 'r', newline='\n') as csv_values_file:
        try:
            # Extract keys from values file (first line of file)
            file_data = [csv_values_file.readline()]

            file_data_keys = file_data[0]
            if not isinstance(file_data_keys, str):
                file_data_keys = file_data_keys.encode('utf-8')

            keys_in_values_file = file_data_keys.strip().split(',')

            while file_data:
                verify_empty_lines_exist(args.values, file_data)
                file_data = [csv_values_file.readline()]
                if '' in file_data:
                    break

        except Exception as e:
            print(e)

    # Verify file identifier exists in values file
    if args.fileid:
        if args.fileid not in keys_in_values_file:
            raise SystemExit('Error: target_file_identifier: %s does not exist in values file.\n' % args.fileid)
    else:
        args.fileid = 1

    return keys_in_config_file, keys_in_values_file, keys_repeat


def generate(args):
    keys_in_config_file = []
    keys_in_values_file = []
    keys_repeat = []
    encryption_enabled = False

    args.outdir = os.path.join(args.outdir, '')
    # Verify input config and values file format
    keys_in_config_file, keys_in_values_file, keys_repeat = verify_file_format(args)

    # Verify data in the input_config_file and input_values_file
    verify_data_in_file(args.conf, args.values, keys_in_config_file,
                        keys_in_values_file, keys_repeat)

    if (args.keygen or args.inputkey):
        encryption_enabled = True
        print("\nGenerating encrypted NVS binary images...")

    # Create intermediate csv file
    create_intermediate_csv(args, keys_in_config_file, keys_in_values_file,
                            keys_repeat, is_encr=encryption_enabled)


def generate_key(args):
    nvs_partition_gen.generate_key(args)


def main():
    try:
        parser = argparse.ArgumentParser(description="\nESP Manufacturing Utility", formatter_class=argparse.RawTextHelpFormatter)
        subparser = parser.add_subparsers(title='Commands',
                                          dest='command',
                                          help='\nRun mfg_gen.py {command} -h for additional help\n\n')

        parser_gen = subparser.add_parser('generate',
                                          help='Generate NVS partition',
                                          formatter_class=argparse.RawTextHelpFormatter)
        parser_gen.set_defaults(func=generate)
        parser_gen.add_argument('conf',
                                default=None,
                                help='Path to configuration csv file to parse')
        parser_gen.add_argument('values',
                                default=None,
                                help='Path to values csv file to parse')
        parser_gen.add_argument('prefix',
                                default=None,
                                help='Unique name for each output filename prefix')
        parser_gen.add_argument('size',
                                default=None,
                                help='Size of NVS partition in bytes\
                                    \n(must be multiple of 4096)')
        parser_gen.add_argument('--fileid',
                                default=None,
                                help='''Unique file identifier(any key in values file) \
                                    \nfor each filename suffix (Default: numeric value(1,2,3...)''')
        parser_gen.add_argument('--version',
                                choices=[1, 2],
                                default=2,
                                type=int,
                                help='''Set multipage blob version.\
                                    \nVersion 1 - Multipage blob support disabled.\
                                    \nVersion 2 - Multipage blob support enabled.\
                                    \nDefault: Version 2 ''')
        parser_gen.add_argument('--keygen',
                                action="store_true",
                                default=False,
                                help='Generates key for encrypting NVS partition')
        parser_gen.add_argument('--keyfile',
                                default=None,
                                help=argparse.SUPPRESS)
        parser_gen.add_argument('--inputkey',
                                default=None,
                                help='File having key for encrypting NVS partition')
        parser_gen.add_argument('--outdir',
                                default=os.getcwd(),
                                help='Output directory to store files created\
                                    \n(Default: current directory)')
        parser_gen.add_argument('--input',
                                default=None,
                                help=argparse.SUPPRESS)
        parser_gen.add_argument('--output',
                                default=None,
                                help=argparse.SUPPRESS)
        parser_gen_key = subparser.add_parser('generate-key',
                                              help='Generate keys for encryption',
                                              formatter_class=argparse.RawTextHelpFormatter)
        parser_gen_key.set_defaults(func=generate_key)
        parser_gen_key.add_argument('--keyfile',
                                    default=None,
                                    help='Path to output encryption keys file')
        parser_gen_key.add_argument('--outdir',
                                    default=os.getcwd(),
                                    help='Output directory to store files created.\
                                        \n(Default: current directory)')

        args = parser.parse_args()
        args.func(args)

    except ValueError as err:
        print(err)
    except Exception as e:
        print(e)


if __name__ == "__main__":
    main()
