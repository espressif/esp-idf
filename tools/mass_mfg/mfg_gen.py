#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import csv
import distutils.dir_util
import os
from itertools import zip_longest

import esp_idf_nvs_partition_gen.nvs_partition_gen as nvs_partition_gen


def create_temp_files(args):
    new_filenames = []
    for filename in [args.conf, args.values]:
        name, ext = os.path.splitext(filename)
        new_filename = name + '_tmp' + ext
        strip_blank_lines(filename, new_filename)
        new_filenames.append(new_filename)
    return new_filenames


def strip_blank_lines(input_filename, output_filename):
    with open(input_filename, 'r') as read_from, open(output_filename,'w', newline='') as write_to:
        for line in read_from:
            if not line.isspace():
                write_to.write(line)


def verify_values_exist(input_values_file, keys_in_values_file):
    """ Verify all keys have corresponding values in values file
    """
    with open(input_values_file, 'r') as values_file:
        values_file_reader = csv.reader(values_file, delimiter=',')
        next(values_file_reader)

        for line_num, line in enumerate(values_file_reader, start=2):
            key_count_in_values_file = len(keys_in_values_file)
            if len(line) != key_count_in_values_file:
                raise SystemExit('\nError: Number of values is not equal to number of keys in file: %s at line No:%s\n'
                                 % (str(input_values_file), str(line_num)))


def verify_keys_exist(values_file_keys, input_config_file):
    """ Verify all keys from config file are present in values file
    """
    keys_missing = []

    with open(input_config_file,'r') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')

        for line_num, line in enumerate(config_file_reader, start=1):
            if 'namespace' not in line:
                if values_file_keys:
                    if line[0] == values_file_keys[0]:
                        del values_file_keys[0]
                    else:
                        keys_missing.append([line_num, line[0]])
                else:
                    keys_missing.append([line_num, line[0]])

        if keys_missing:
            for line_num, key in keys_missing:
                print('Key:`', str(key), '` at line no:', str(line_num),
                      ' in config file is not found in values file.')
            raise SystemExit(1)


def verify_datatype_encoding(input_config_file):
    """ Verify datatype and encodings from config file is valid
    """
    valid_encodings = {'string', 'binary', 'hex2bin','u8', 'i8', 'u16', 'u32', 'i32','base64'}
    valid_datatypes = {'file','data','namespace'}

    with open(input_config_file,'r') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')

        for line_num, line in enumerate(config_file_reader, start=1):
            if line[1] not in valid_datatypes:
                raise SystemExit('Error: config file: %s has invalid datatype at line no:%s\n`'
                                 % (str(input_config_file), str(line_num)))
            if 'namespace' not in line and line[2] not in valid_encodings:
                raise SystemExit('Error: config file: %s has invalid encoding at line no:%s\n`'
                                 % (str(input_config_file), str(line_num)))


def verify_file_data_count(input_config_file, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3
    (as format must be: <key,type and encoding>)
    """
    with open(input_config_file, 'r') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')

        for line_num, line in enumerate(config_file_reader, start=1):
            if len(line) != 3 and line[0] not in keys_repeat:
                raise SystemExit('Error: data missing in config file at line no:%s <format needed:key,type,encoding>\n'
                                 % str(line_num))


def verify_data_in_file(input_config_file, input_values_file, config_file_keys, keys_in_values_file, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3 \
    (as format must be: <key,type and encoding>)
    Verify datatype and encodings from config file is valid
    Verify all keys from config file are present in values file and \
    Verify each key has corresponding value in values file
    """
    try:
        verify_file_data_count(input_config_file, keys_repeat)

        verify_datatype_encoding(input_config_file)

        # Get keys from values file present in config files
        values_file_keys = get_keys(keys_in_values_file, config_file_keys)

        verify_keys_exist(values_file_keys, input_config_file)

        verify_values_exist(input_values_file, keys_in_values_file)

    except Exception as err:
        print(err)
        exit(1)


def get_keys(keys_in_values_file, config_file_keys):
    """ Get keys from values file present in config file
    """
    values_file_keys = [key for key in keys_in_values_file if key in config_file_keys]
    return values_file_keys


def add_config_data_per_namespace(input_config_file):
    """ Add config data per namespace to `config_data_to_write` list
    """
    config_data_to_write = []
    config_data_per_namespace = []

    with open(input_config_file,'r') as csv_config_file:
        config_file_reader = csv.reader(csv_config_file, delimiter=',')

        # `config_data_per_namespace` is added to `config_data_to_write` list after reading next namespace
        for config_data in config_file_reader:
            if 'REPEAT' in config_data:
                config_data.remove('REPEAT')
            if 'namespace' in config_data:
                if config_data_per_namespace:
                    config_data_to_write.append(config_data_per_namespace)
                    config_data_per_namespace = []
                    config_data_per_namespace.append(config_data)
                else:
                    config_data_per_namespace.append(config_data)
            else:
                config_data_per_namespace.append(config_data)

        # `config_data_per_namespace` is added to `config_data_to_write` list as EOF is reached
        if (not config_data_to_write) or (config_data_to_write and config_data_per_namespace):
            config_data_to_write.append(config_data_per_namespace)

    return config_data_to_write


def get_fileid_val(file_identifier, key_value_data, fileid_value):
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

    with open(output_csv_file, 'w', newline='') as target_csv_file:
        output_file_writer = csv.writer(target_csv_file, delimiter=',')
        output_file_writer.writerow(header)

        for namespace_config_data in config_data_to_write:
            for data in namespace_config_data:
                data_to_write = data[:]
                if 'namespace' in data:
                    data_to_write.append('')
                    output_file_writer.writerow(data_to_write)
                else:
                    key = data[0]
                    while key not in key_value_pair[0]:
                        del key_value_pair[0]
                    if key in key_value_pair[0]:
                        value = key_value_pair[0][1]
                        data_to_write.append(value)
                        del key_value_pair[0]
                        output_file_writer.writerow(data_to_write)


def create_dir(filetype, output_dir_path):
    """ Create new directory(if doesn't exist) to store file generated
    """
    output_target_dir = os.path.join(output_dir_path,filetype,'')
    if not os.path.isdir(output_target_dir):
        distutils.dir_util.mkpath(output_target_dir)

    return output_target_dir


def set_repeat_value(total_keys_repeat, keys, csv_file, target_filename):
    with open(csv_file, 'r') as read_from, open(target_filename,'w', newline='') as write_to:
        csv_file_reader = csv.reader(read_from, delimiter=',')
        headers = next(csv_file_reader)
        values = next(csv_file_reader)
        csv_file_writer = csv.writer(write_to, delimiter=',')
        csv_file_writer.writerow(headers)
        csv_file_writer.writerow(values)

        # read new data, add value if key has repeat tag, write to new file
        for row in csv_file_reader:
            key_val_new = list(zip_longest(keys, row))
            key_val_pair = list(zip_longest(keys, values))
            key_repeated = total_keys_repeat[:]
            index = 0
            while key_val_new and key_repeated:
                #  if key has repeat tag, get its corresponding value, write to file
                if key_val_new[0][0] == key_repeated[0]:
                    row[index] = key_val_pair[0][1]
                    del key_repeated[0]
                del key_val_new[0]
                del key_val_pair[0]
                index += 1
            csv_file_writer.writerow(row)

    return target_filename


def create_intermediate_csv(args, keys_in_values_file, keys_repeat, is_encr=False):
    file_identifier_value = '0'
    # Add config data per namespace to `config_data_to_write` list
    config_data_to_write = add_config_data_per_namespace(args.conf)

    try:
        with open(args.values, 'r') as csv_values_file:
            values_file_reader = csv.reader(csv_values_file, delimiter=',')
            keys = next(values_file_reader)

        filename, file_ext = os.path.splitext(args.values)
        target_filename = filename + '_created' + file_ext
        if keys_repeat:
            target_values_file = set_repeat_value(keys_repeat, keys, args.values, target_filename)
        else:
            target_values_file = args.values

        with open(target_values_file, 'r') as csv_values_file:
            values_file_reader = csv.reader(csv_values_file, delimiter=',')
            next(values_file_reader)

            # Create new directory(if doesn't exist) to store csv file generated
            output_csv_target_dir = create_dir('csv', args.outdir)
            # Create new directory(if doesn't exist) to store bin file generated
            output_bin_target_dir = create_dir('bin', args.outdir)

            for values_data_line in values_file_reader:
                key_value_data = list(zip_longest(keys_in_values_file, values_data_line))

                # Get file identifier value from values file
                file_identifier_value = get_fileid_val(args.fileid, key_value_data, file_identifier_value)

                key_value_pair = key_value_data[:]

                # Verify if output csv file does not exist
                csv_filename = args.prefix + '-' + file_identifier_value + '.' + 'csv'
                output_csv_file = output_csv_target_dir + csv_filename
                if os.path.isfile(output_csv_file):
                    raise SystemExit('Target csv file: %s already exists.`' % output_csv_file)

                # Add values corresponding to each key to csv intermediate file
                add_data_to_file(config_data_to_write, key_value_pair, output_csv_file)
                print('\nCreated CSV file: ===>', output_csv_file)

                # Verify if output bin file does not exist
                bin_filename = args.prefix + '-' + file_identifier_value + '.' + 'bin'
                output_bin_file = output_bin_target_dir + bin_filename
                if os.path.isfile(output_bin_file):
                    raise SystemExit('Target binary file: %s already exists.`' % output_bin_file)

                args.input = output_csv_file
                args.output = os.path.join('bin', bin_filename)
                if args.keygen:
                    args.keyfile = 'keys-' + args.prefix + '-' + file_identifier_value

                if is_encr:
                    nvs_partition_gen.encrypt(args)
                else:
                    nvs_partition_gen.generate(args)

            print('\nFiles generated in %s ...' % args.outdir)

    except Exception as e:
        print(e)
        exit(1)


def verify_file_format(args):
    keys_in_config_file = []
    keys_in_values_file = []
    keys_repeat = []

    # Verify files have csv extension
    conf_name, conf_extension = os.path.splitext(args.conf)
    if conf_extension != '.csv':
        raise SystemExit('Error: config file: %s does not have the .csv extension.' % args.conf)
    values_name, values_extension = os.path.splitext(args.values)
    if values_extension != '.csv':
        raise SystemExit('Error: values file: %s does not have the .csv extension.' % args.values)

    # Verify files are not empty
    if os.stat(args.conf).st_size == 0:
        raise SystemExit('Error: config file: %s is empty.' % args.conf)
    if os.stat(args.values).st_size == 0:
        raise SystemExit('Error: values file: %s is empty.' % args.values)

    # Extract keys from config file
    with open(args.conf, 'r') as config_file:
        config_file_reader = csv.reader(config_file, delimiter=',')
        for config_data in config_file_reader:
            if 'namespace' not in config_data:
                keys_in_config_file.append(config_data[0])
            if 'REPEAT' in config_data:
                keys_repeat.append(config_data[0])

    # Extract keys from values file
    with open(args.values, 'r') as values_file:
        values_file_reader = csv.reader(values_file, delimiter=',')
        keys_in_values_file = next(values_file_reader)

    # Verify file identifier exists in values file
    if args.fileid:
        if args.fileid not in keys_in_values_file:
            raise SystemExit('Error: target_file_identifier: %s does not exist in values file.\n' % args.fileid)
    else:
        args.fileid = 1

    return keys_in_config_file, keys_in_values_file, keys_repeat


def generate(args):
    # Create work files with no blank lines
    args.conf, args.values = create_temp_files(args)

    # Verify input config and values file format
    keys_in_config_file, keys_in_values_file, keys_repeat = verify_file_format(args)

    # Verify data in the input_config_file and input_values_file
    verify_data_in_file(args.conf, args.values, keys_in_config_file,
                        keys_in_values_file, keys_repeat)

    encryption_enabled = False
    if (args.keygen or args.inputkey):
        encryption_enabled = True
        print('\nGenerating encrypted NVS binary images...')
    # Create intermediate csv file
    create_intermediate_csv(args, keys_in_values_file, keys_repeat, is_encr=encryption_enabled)


def generate_key(args):
    nvs_partition_gen.generate_key(args)


def main():
    try:
        parser = argparse.ArgumentParser(description='\nESP Manufacturing Utility', formatter_class=argparse.RawTextHelpFormatter)
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
                                action='store_true',
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
        parser_gen.add_argument('--key_protect_hmac',
                                action='store_true',
                                help='''If set, the NVS encryption key protection scheme based on HMAC\
                                \nperipheral is used; else the default scheme based on Flash Encryption\
                                \nis used''')
        parser_gen.add_argument('--kp_hmac_keygen',
                                action='store_true',
                                help='Generate the HMAC key for HMAC-based encryption scheme')
        parser_gen.add_argument('--kp_hmac_keyfile',
                                default=None,
                                help='Path to output HMAC key file')
        parser_gen.add_argument('--kp_hmac_inputkey',
                                default=None,
                                help='File having the HMAC key for generating the NVS encryption keys')
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
        parser_gen_key.add_argument('--key_protect_hmac',
                                    action='store_true',
                                    help='''If set, the NVS encryption key protection scheme based on HMAC\
                                    \nperipheral is used; else the default scheme based on Flash Encryption\
                                    \nis used''')
        parser_gen_key.add_argument('--kp_hmac_keygen',
                                    action='store_true',
                                    help='Generate the HMAC key for HMAC-based encryption scheme')
        parser_gen_key.add_argument('--kp_hmac_keyfile',
                                    default=None,
                                    help='Path to output HMAC key file')
        parser_gen_key.add_argument('--kp_hmac_inputkey',
                                    default=None,
                                    help='File having the HMAC key for generating the NVS encryption keys')

        args = parser.parse_args()
        args.func(args)

    except ValueError as err:
        print(err)
    except Exception as e:
        print(e)


if __name__ == '__main__':
    main()
