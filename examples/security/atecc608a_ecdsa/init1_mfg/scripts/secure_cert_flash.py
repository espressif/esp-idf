import os
import subprocess
artifacts_path=os.path.abspath(os.path.join(os.getcwd()))

def _exec_shell_cmd(command):
    result = subprocess.Popen((command).split(), stdout=subprocess.PIPE)
    out, err = result.communicate()
    return out
    
def _validate_bootloader_bin(bootloader_bin_value):
    out = _exec_shell_cmd('xtensa-esp32-elf-strings ' + bootloader_bin_value)
    if 'flash encrypt' not in out:
        print('Flash Encryption not enabled in the bootloader')
    if 'secure boot' not in out:
        print('Secure Boot not enabled in the bootloader')

def validate_artifacts(bin_path):
    global artifacts_path
    artifacts_path += '/' + bin_path
    print('Verifying esp_bootloader')
    if os.path.exists(artifacts_path + '/esp_bootloader.bin') == False:
        print("Could not find" + artifacts_path + "/esp_bootloader.bin")
        exit(0)

    print('Verifying cust_bootloader')
    if os.path.exists(artifacts_path + '/cust_bootloader.bin') == False:
        print("Could not find" + artifacts_path +  "/cust_bootloader.bin")   
        exit(0)
    _validate_bootloader_bin(artifacts_path + '/cust_bootloader.bin')

    print('Verifying app & partition')
    if os.path.exists(artifacts_path + '/init1_mfg.bin') == False or os.path.exists(artifacts_path + '/esp_partitions.bin') == False:
        print("Could not find init1_mfg.bin/esp_partitions.bin")        
        exit(0)
    
def flash_artifacts():
    global artifacts_path
    ops_file = 'esptool.py'
    write_operation = 'write_flash'
    erase_operation = 'erase_flash'
    esptool_command = 'python ' + os.getenv('IDF_PATH') + '/components/esptool_py/esptool/' + ops_file + ' --port ' + os.getenv('ESPPORT') + ' --baud 2000000 '
    
    print('Erasing flash')
    out = _exec_shell_cmd(esptool_command + erase_operation + ' ')
    print(out)

    esp_boot = '0x1000 ' + artifacts_path + '/esp_bootloader.bin '
    init1_fw = '0x20000 ' + artifacts_path + '/init1_mfg.bin '
    init1_part = '0xc000 ' + artifacts_path + '/esp_partitions.bin '
    cust_boot = '0x11000 ' + artifacts_path + '/cust_bootloader.bin '

    print(esptool_command + write_operation + ' ' + esp_boot+init1_fw + init1_part + cust_boot)
    print('Flashing artifacts')
    out = _exec_shell_cmd(esptool_command + write_operation + ' '+ esp_boot + init1_fw + init1_part + cust_boot)
    print(out)

if __name__ == '__main__':
    main()