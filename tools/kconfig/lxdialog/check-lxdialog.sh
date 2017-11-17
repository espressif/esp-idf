#!/bin/bash
# Check ncurses compatibility

# What library to link
ldflags()
{
	if [ $(uname -s) == "Darwin" ]; then 
		#OSX seems to need ncurses too
		echo -n "-lncurses "
	fi
	pkg-config --libs ncursesw 2>/dev/null && exit
	pkg-config --libs ncurses 2>/dev/null && exit
	for ext in so a dll.a dylib ; do
		for lib in ncursesw ncurses curses ; do
			$cc -print-file-name=lib${lib}.${ext} $extra_libs | grep -q /
			if [ $? -eq 0 ]; then
				echo "-l${lib}"
				exit
			fi
		done
	done
	exit 1
}

# Where is ncurses.h?
ccflags()
{
	if pkg-config --cflags ncursesw 2>/dev/null; then
		echo '-DCURSES_LOC="<ncurses.h>" -DNCURSES_WIDECHAR=1'
	elif pkg-config --cflags ncurses 2>/dev/null; then
		echo '-DCURSES_LOC="<ncurses.h>"'
	elif [ -f /usr/include/ncursesw/curses.h ]; then
		echo '-I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"'
		echo ' -DNCURSES_WIDECHAR=1'
	elif [ -f /usr/include/ncurses/ncurses.h ]; then
		echo '-I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"'
	elif [ -f /usr/include/ncurses/curses.h ]; then
		echo '-I/usr/include/ncurses -DCURSES_LOC="<curses.h>"'
	elif [ -f /usr/include/ncurses.h ]; then
		echo '-DCURSES_LOC="<ncurses.h>"'
	else
		echo '-DCURSES_LOC="<curses.h>"'
	fi
	if [ $(uname -s) == "Darwin" ]; then
		#OSX doesn't have libintl
		echo -n "-DKBUILD_NO_NLS -Wno-format-security "
	fi
}

# Temp file, try to clean up after us
tmp=.lxdialog.tmp
trap "rm -f $tmp ${tmp%.tmp}.d" 0 1 2 3 15

# Check if we can link to ncurses
check() {
        $cc -x c - -o $tmp 2>/dev/null <<'EOF'
#include CURSES_LOC
main() {}
EOF
	if [ $? != 0 ]; then
	    echo " *** Unable to find the ncurses libraries or the"       1>&2
	    echo " *** required header files."                            1>&2
	    echo " *** 'make menuconfig' requires the ncurses libraries." 1>&2
	    echo " *** "                                                  1>&2
	    echo " *** Install ncurses (ncurses-devel) and try again."    1>&2
	    echo " *** "                                                  1>&2
	    exit 1
	fi
}

usage() {
	printf "Usage: $0 [-check compiler options|-ccflags|-ldflags compiler options]\n"
}

if [ $# -eq 0 ]; then
	usage
	exit 1
fi

cc=""
case "$1" in
	"-check")
		shift
		cc="$@"
		check
		;;
	"-ccflags")
		ccflags
		;;
	"-ldflags")
		shift
		cc="$@"
		ldflags
		;;
	"*")
		usage
		exit 1
		;;
esac
