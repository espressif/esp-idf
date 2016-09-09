#!/bin/bash
#
# Test the build system for basic consistency
#
# Just a bash script that tests some likely make failure scenarios in a row
# Creates its own test build directory under TMP and cleans it up when done.
#
# Environment variables:
# IDF_PATH - must be set
# TMP - can override /tmp location for build directory
# ESP_IDF_TEMPLATE_GIT - Can override git clone source for template app. Otherwise github.
# NOCLEANUP - Set to '1' if you want the script to leave its temporary directory when done, for post-mortem.
#

# Set up some variables
#
[ -z ${TMP} ] && TMP="/tmp"
# override ESP_IDF_TEMPLATE_GIT to point to a local dir if you're testing and want fast iterations
[ -z ${ESP_IDF_TEMPLATE_GIT} ] && ESP_IDF_TEMPLATE_GIT=https://github.com/espressif/esp-idf-template.git
export V=1

function run_tests()
{
	FAILURES=
	STATUS="Starting"
	print_status "Checking prerequisites"
	[ -z ${IDF_PATH} ] && echo "IDF_PATH is not set. Need path to esp-idf installation." && exit 2

	print_status "Cloning template from ${ESP_IDF_TEMPLATE_GIT}..."
	git clone ${ESP_IDF_TEMPLATE_GIT} template
	cd template
	git checkout ${CI_BUILD_REF_NAME} || echo "Using esp-idf-template default branch..."

	print_status "Updating template config..."
	make defconfig || exit $?

	BOOTLOADER_BINS="bootloader/bootloader.elf bootloader/bootloader.bin"
	APP_BINS="app-template.elf app-template.bin"

	print_status "Initial clean build"
	# if make fails here, everything fails
	make || exit $?
	# check all the expected build artifacts from the clean build
	assert_built ${APP_BINS} ${BOOTLOADER_BINS} partitions_singleapp.bin
	[ -f ${BUILD}/partition*.bin ] || failure "A partition table should have been built"

	print_status "Updating component source file rebuilds component"
	# touch a file & do a build
	take_build_snapshot
	touch ${IDF_PATH}/components/esp32/syscalls.c
	make || failure "Failed to partial build"
	assert_rebuilt ${APP_BINS} esp32/libesp32.a esp32/syscalls.o
	assert_not_rebuilt lwip/liblwip.a freertos/libfreertos.a ${BOOTLOADER_BINS} partitions_singleapp.bin

	print_status "Bootloader source file rebuilds bootloader"
	take_build_snapshot
	touch ${IDF_PATH}/components/bootloader/src/main/bootloader_start.c
	make bootloader || failure "Failed to partial build bootloader"
	assert_rebuilt ${BOOTLOADER_BINS} bootloader/main/bootloader_start.o
	assert_not_rebuilt ${APP_BINS} partitions_singleapp.bin

	print_status "Partition CSV file rebuilds partitions"
	take_build_snapshot
	touch ${IDF_PATH}/components/partition_table/partitions_singleapp.csv
	make partition_table
	assert_rebuilt partitions_singleapp.bin
	assert_not_rebuilt app-template.bin app-template.elf ${BOOTLOADER_BINS}

	print_status "Partial build doesn't compile anything by default"
	take_build_snapshot
	# verify no build files are refreshed by a partial make
	ALL_BUILD_FILES=$(find ${BUILD} -type f | sed "s@${BUILD}/@@")
	make
	assert_not_rebuilt ${ALL_BUILD_FILES}

	print_status "Cleaning should remove all files from build"
	make clean
	ALL_BUILD_FILES=$(find ${BUILD} -type f)
	if [ -n "${ALL_BUILD_FILES}" ]; then
		 failure "Files weren't cleaned: ${ALL_BUILD_FILES}"
	fi

	print_status "All tests completed"
	if [ -n "${FAILURES}" ]; then
		echo "Some failures were detected:"
		echo -e "${FAILURES}"
		exit 1
	else
		echo "Build tests passed."
	fi
}

function print_status()
{
	echo "******** $1"
	STATUS="$1"
}

function failure()
{
	echo "!!!!!!!!!!!!!!!!!!!"
	echo "FAILURE: $1"
	echo "!!!!!!!!!!!!!!!!!!!"
	FAILURES="${FAILURES}${STATUS} :: $1\n"
}

TESTDIR=${TMP}/build_system_tests_$$
mkdir -p ${TESTDIR}
# set NOCLEANUP=1 if you want to keep the test directory around
# for post-mortem debugging
[ -z ${NOCLEANUP} ] && trap "rm -rf ${TESTDIR}" EXIT KILL

SNAPSHOT=${TESTDIR}/snapshot
BUILD=${TESTDIR}/template/build


# copy all the build output to a snapshot directory
function take_build_snapshot()
{
	rm -rf ${SNAPSHOT}
	cp -ap ${TESTDIR}/template/build ${SNAPSHOT}
}

# verify that all the arguments are present in the build output directory
function assert_built()
{
	until [ -z "$1" ]; do
		if [ ! -f "${BUILD}/$1" ]; then
			failure "File $1 should be in the build output directory"
		fi
		shift
	done
}

# Test if a file has been rebuilt.
function file_was_rebuilt()
{
	# can't use [ a -ot b ] here as -ot only gives second resolution
	# but stat -c %y seems to be microsecond at least for tmpfs, ext4..
	if [ "$(stat -c %y ${SNAPSHOT}/$1)" != "$(stat -c %y ${BUILD}/$1)" ]; then
		return 0
	else
		return 1
	fi
}

# verify all the arguments passed in were rebuilt relative to the snapshot
function assert_rebuilt()
{
	until [ -z "$1" ]; do
		assert_built "$1"
		if [ ! -f "${SNAPSHOT}/$1" ]; then
			failure "File $1 should have been original build snapshot"
		fi
		if ! file_was_rebuilt "$1"; then
			failure "File $1 should have been rebuilt"
		fi
		shift
	done
}

# verify all the arguments are in the build directory & snapshot,
# but were not rebuilt
function assert_not_rebuilt()
{
	until [ -z "$1" ]; do
		assert_built "$1"
		if [ ! -f "${SNAPSHOT}/$1" ]; then
			failure "File $1 should be in snapshot build directory"
		fi
		if file_was_rebuilt "$1"; then
			failure "File $1 should not have been rebuilt"
		fi
		shift
	done
}

cd ${TESTDIR}
run_tests
