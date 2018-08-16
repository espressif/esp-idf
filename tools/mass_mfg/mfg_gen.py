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

import sys
import os
import csv
import argparse
import shutil
import distutils.dir_util
sys.path.insert(0, os.getenv('IDF_PATH') + "/components/nvs_flash/nvs_partition_generator/")
import nvs_partition_gen


def verify_values_exist(input_values_file, keys_in_values_file):
    """ Verify all keys have corresponding values in values file
    """
    line_no = 1
    key_count_in_values_file = len(keys_in_values_file)

    values_file = open(input_values_file,'rb')
    values_file_reader = csv.reader(values_file, delimiter=',')
    keys = values_file_reader.next()

    for values_data in values_file_reader:
        line_no +=1
        if len(values_data) != key_count_in_values_file:
            raise SystemExit("\nOops...Number of values is not equal to number of keys in file: '" + \
            str(input_values_file) + "' at line No:" + str(line_no) )


def verify_keys_exist(values_file_keys, input_config_file):
    """ Verify all keys from config file are present in values file
    """
    keys_missing = []

    config_file = open(input_config_file,'r')
    config_file_reader = csv.reader(config_file, delimiter=',')
    for line_no, config_data in enumerate(config_file_reader,1):
        if 'namespace' not in config_data:
            if values_file_keys:
                if config_data[0] == values_file_keys[0]:
                    del values_file_keys[0]
                else:
                    keys_missing.append([config_data[0], line_no])
            else:
                keys_missing.append([config_data[0], line_no])

    
    if keys_missing:
        print "Oops..."
        for key, line_no in keys_missing:
            print "Key:`" + str(key) + "` at line no:" + str(line_no) + \
            " in config file is not found in values file..."
        config_file.close()
        raise SystemExit(1)

    config_file.close()



def verify_datatype_encoding(input_config_file):
    """ Verify datatype and encodings from config file is valid
    """
    valid_encodings = ["string", "binary", "hex2bin","u8", "i8", "u16", "u32", "i32","base64"]
    valid_datatypes = ["file","data","namespace"]
    line_no = 0

    config_file = open(input_config_file,'r')
    config_file_reader = csv.reader(config_file, delimiter=',')
    for config_data in config_file_reader:
        line_no+=1
        if config_data[1] not in valid_datatypes:
            raise SystemExit("Oops...config file: `" + str(input_config_file) + \
            "` has invalid datatype at line no:" + str(line_no))
        if 'namespace' not in config_data:
            if config_data[2] not in valid_encodings:
                raise SystemExit("Oops...config file: `" + str(input_config_file) + \
                "` has invalid encoding at line no:" + str(line_no))



def verify_file_data_count(input_config_file, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3
    (as format must be: <key,type and encoding>)
    """
    line_no = 0
    config_file = open(input_config_file, 'r')
    config_file_reader = csv.reader(config_file, delimiter=',')
    for line in config_file_reader:
        line_no += 1
        if len(line) != 3 and line[0] not in keys_repeat:
            raise SystemExit("Oops...data missing in config file at line no: " + str(line_no) + \
            " <format needed:key,type,encoding>")

    config_file.close()


def verify_data_in_file(input_config_file, input_values_file, config_file_keys, keys_in_values_file, keys_repeat):
    """ Verify count of data on each line in config file is equal to 3 \
    (as format must be: <key,type and encoding>)
    Verify datatype and encodings from config file is valid
    Verify all keys from config file are present in values file and \
    Verify each key has corresponding value in values file
    """
    try:
        values_file_keys = []

        verify_file_data_count(input_config_file, keys_repeat)

        verify_datatype_encoding(input_config_file)

        # Get keys from values file present in config files
        values_file_keys = get_keys(keys_in_values_file, config_file_keys)

        verify_keys_exist(values_file_keys, input_config_file)

        verify_values_exist(input_values_file, keys_in_values_file)

    except StandardError as std_err:
        print  std_err
    except:
        raise


def get_keys(keys_in_values_file, config_file_keys):
    """ Get keys from values file present in config file
    """
    values_file_keys = []
    for key in range(len(keys_in_values_file)):
        if keys_in_values_file[key] in config_file_keys:
            values_file_keys.append(keys_in_values_file[key])

    return values_file_keys


def add_config_data_per_namespace(input_config_file):
    """ Add config data per namespace to `config_data_to_write` list
    """
    config_data_to_write = []
    config_data_per_namespace = []

    csv_config_file = open(input_config_file,'r')
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

    csv_config_file.close()

    return config_data_to_write


def get_fileid_val(file_identifier, keys_in_config_file, keys_in_values_file,\
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

    target_csv_file = open(output_csv_file, 'w')
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


    # Set index to start of file
    target_csv_file.seek(0)

    target_csv_file.close()


def create_dir(filetype, output_dir_path):
    """ Create new directory(if doesn't exist) to store file generated
    """
    output_target_dir = output_dir_path + filetype
    if not os.path.isdir(output_target_dir):
        distutils.dir_util.mkpath(output_target_dir)

    return output_target_dir


def set_repeat_value(total_keys_repeat, keys, csv_file):
    key_val_pair = []
    key_repeated = []
    filename, file_ext = os.path.splitext(csv_file)
    target_filename = filename + "_created" + file_ext
    with open(csv_file, 'r') as read_from, open(target_filename,'w') as write_to:
        csv_file_reader = csv.reader(read_from, delimiter=',')
        headers = csv_file_reader.next()
        values = csv_file_reader.next()
        total_keys_values = map(None, keys, values)

        csv_file_writer = csv.writer(write_to, delimiter=',')
        csv_file_writer.writerow(headers)
        csv_file_writer.writerow(values)

        # read new data, add value if key has repeat tag, write to new file
        for row in csv_file_reader:
            index = -1
            key_val_new = map(None, keys, row)
            key_val_pair = total_keys_values[:]
            key_repeated = total_keys_repeat[:]
            while key_val_new and key_repeated:
                index = index + 1
                #  if key has repeat tag, get its corresponding value, write to file
                if key_val_new[0][0] == key_repeated[0]:
                    val = key_val_pair[0][1]
                    row[index] = val
                    csv_file_writer.writerow(row)
                    del key_repeated[0]
                del key_val_new[0]
                del key_val_pair[0]
           

    return target_filename


def main(input_config_file=None,input_values_file=None,target_file_name_prefix=None,\
file_identifier=None,output_dir_path=None):
    try:
        if all(arg is None for arg in [input_config_file,input_values_file,target_file_name_prefix,\
            file_identifier,output_dir_path]):
            parser = argparse.ArgumentParser(prog='./mfg_gen.py',
                                             description="Create binary files from input config and values file",
                                             formatter_class=argparse.RawDescriptionHelpFormatter)

            parser.add_argument('--conf',
                                dest='config_file',
                                required=True,
                                help='the input configuration csv file')

            parser.add_argument('--values',
                                dest='values_file',
                                required=True,
                                help='the input values csv file')

            parser.add_argument('--prefix',
                                dest='prefix',
                                required=True,
                                help='the unique name as each filename prefix')

            parser.add_argument('--fileid',
                                dest='fileid',
                                help='the unique file identifier(any key in values file) \
                                as each filename suffix (Default: numeric value(1,2,3...)')

            parser.add_argument('--outdir',
                                dest='outdir',
                                default='./',
                                help='the output directory to store the files created\
                                (Default: current directory)')

            args = parser.parse_args()

            # Verify if output_dir_path argument is given then output directory exists
            if not os.path.isdir(args.outdir):
                parser.error('--outdir ' + args.outdir + ' does not exist...')

            # Add '/' to outdir if it is not present
            if not args.outdir.endswith('/'):
                args.outdir = args.outdir + '/'

            input_config_file = args.config_file
            input_values_file = args.values_file
            target_file_name_prefix = args.prefix
            output_dir_path = args.outdir
            file_identifier = ''

            if args.fileid:
                file_identifier = args.fileid


        keys_in_values_file = []
        keys_in_config_file = []
        config_data_to_write = []
        key_value_data = []
        csv_file_list = []
        keys_repeat = []
        is_keys_missing = True
        file_id_found = False
        is_empty_line = False
        files_created = False
        file_identifier_value = '0'
        output_target_dir = ''

        # Verify config file is not empty
        if os.stat(input_config_file).st_size == 0:
            raise SystemExit("Oops...config file: " + input_config_file + " is empty...")

        # Verify values file is not empty
        if os.stat(input_values_file).st_size == 0:
            raise SystemExit("Oops...values file: " + input_values_file + " is empty...")

        # Verify config file does not have empty lines
        csv_config_file = open(input_config_file,'r')
        try:
            config_file_reader = csv.reader(csv_config_file, delimiter=',')
            for config_data in config_file_reader:
                for data in config_data:
                    empty_line = data.strip()
                    if empty_line is '':
                        is_empty_line = True
                    else:
                        is_empty_line = False
                        break
                if is_empty_line:
                    raise SystemExit("Oops...config file: " + input_config_file + " cannot have empty lines...")
                if not config_data:
                    raise SystemExit("Oops...config file: " + input_config_file + " cannot have empty lines...")

            csv_config_file.seek(0)

            # Extract keys from config file
            for config_data in config_file_reader:
                if 'namespace' in config_data:
                    namespace = config_data[0]
                else:
                    keys_in_config_file.append(config_data[0])
                if 'REPEAT' in config_data:
                    keys_repeat.append(config_data[0])

            csv_config_file.close()
        except Exception as e:
            print e
        finally:
            csv_config_file.close()

        is_empty_line = False


        # Verify values file does not have empty lines
        csv_values_file = open(input_values_file,'rb')
        try:
            values_file_reader = csv.reader(csv_values_file, delimiter=',')
            for values_data in values_file_reader:
                for data in values_data:
                    empty_line = data.strip()
                    if empty_line is '':
                        is_empty_line = True
                    else:
                        is_empty_line = False
                        break
                if is_empty_line:
                    raise SystemExit("Oops...values file: " + input_values_file + " cannot have empty lines...")
                if not values_data:
                    raise SystemExit("Oops...values file: " + input_values_file + " cannot have empty lines...")

            csv_values_file.seek(0)

            # Extract keys from values file
            keys_in_values_file = values_file_reader.next()

            csv_values_file.close()
        except Exception as e:
            print e
            exit(1)
        finally:
            csv_values_file.close()

        # Verify file identifier exists in values file
        if file_identifier:
            if file_identifier not in keys_in_values_file:
                raise SystemExit('Oops...target_file_identifier: ' + file_identifier + \
                ' does not exist in values file...\n')

        # Verify data in the input_config_file and input_values_file
        verify_data_in_file(input_config_file, input_values_file, keys_in_config_file,\
        keys_in_values_file, keys_repeat)

        # Add config data per namespace to `config_data_to_write` list
        config_data_to_write = add_config_data_per_namespace(input_config_file)

        try:
            with open(input_values_file,'rb') as csv_values_file:
                values_file_reader = csv.reader(csv_values_file, delimiter=',')
                keys = values_file_reader.next()
            target_values_file = set_repeat_value(keys_repeat, keys, input_values_file)
            csv_values_file = open(target_values_file, 'rb')
            values_file_reader = csv.reader(csv_values_file, delimiter=',')
            values_file_reader.next()

            for values_data_line in values_file_reader:
                key_value_data = map(None,keys_in_values_file,values_data_line)

                # Get file identifier value from values file
                file_identifier_value = get_fileid_val(file_identifier, keys_in_config_file, \
                keys_in_values_file, values_data_line, key_value_data, file_identifier_value)

                key_value_pair = key_value_data[:]

                # Create new directory(if doesn't exist) to store csv file generated
                output_target_dir = create_dir("csv/", output_dir_path)

                # Verify if output csv file does not exist
                csv_filename = target_file_name_prefix + "-" + file_identifier_value + ".csv"
                csv_file_list.append(csv_filename)
                output_csv_file = output_target_dir + csv_filename
                if os.path.isfile(output_csv_file):
                    raise SystemExit("Target csv file: `" + output_csv_file + "` already exists...")

                # Add values corresponding to each key to csv target file
                add_data_to_file(config_data_to_write, key_value_pair, output_csv_file)

                # Create new directory(if doesn't exist) to store bin file generated
                output_target_dir = create_dir("bin/", output_dir_path)

                # Verify if output bin file does not exist
                output_bin_file = output_target_dir + target_file_name_prefix + "-" +\
                file_identifier_value + ".bin"
                if os.path.isfile(output_bin_file):
                    raise SystemExit("Target csv file: `" + output_bin_file + "` already exists...")

                # Create output csv and bin file
                print "CSV Generated: " + str(output_csv_file)
                nvs_partition_gen.nvs_part_gen(input_filename = output_csv_file, output_filename = output_bin_file)
                print "NVS Flash Binary Generated: " + str(output_bin_file)

                files_created = True

            csv_values_file.close()
        except Exception as e:
            print e
            exit(1)
        finally:
            csv_values_file.close()


        return csv_file_list, files_created

    except StandardError as std_err:
        print std_err
    except:
        raise

if __name__ == "__main__":
    main()
