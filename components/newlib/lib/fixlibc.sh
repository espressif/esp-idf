#!/bin/bash

ROMELF=~/esp8266/esp32/rom_image/eagle.pro.rom.out

rm -rf extracted
mkdir extracted
cd extracted
mkdir inrom
ar x ../libc.a
cp ../libc.a ../libc-minusrom.a

nm $ROMELF | grep -e ' [TB] ' | cut -d ' ' -f 3- > romsyms

for x in *.o; do
	nm $x | grep -e ' [TB] ' | cut -d ' ' -f 3- | while read a; do
		if [ -e "$x" ] && cat romsyms | grep -q '^'$a'$'; then
			echo "$a found in rom"
			mv $x inrom
			ar -d ../libc-minusrom.a $x
		fi
	done
done

