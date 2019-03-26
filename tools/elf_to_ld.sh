#!/bin/bash

echo '/*'
echo 'ESP32 ROM address table'
echo 'Generated for ROM with MD5sum:'
md5sum $1
echo '*/'
xtensa-esp108-elf-nm $1 | grep '[0-9a-f] [TBRD]' | while read adr ttp nm; do 
	if ! echo "$nm" | grep -q -e '^_bss' -e '_heap'; then
		echo "PROVIDE ( $nm = 0x$adr );"; 
	fi
done