#!/usr/bin/env bash
#
# Short script to verify behaviour of configure_ci_environment.sh
#
#
cd $(dirname $0)  # make dir

(
    . ./configure_ci_environment.sh
    [[ $PEDANTIC_CFLAGS ]] || { echo "PEDANTIC_CFLAGS is not defined"; exit 1; }
) || { exit 1; }
