#!/usr/bin/env bash

set -euo pipefail

#
# Retries a command RETRY_ATTEMPTS times in case of failure
#
# Inspired by https://stackoverflow.com/a/8351489
#

max_attempts=${RETRY_ATTEMPTS-3}
RETRY_TIMEWAIT=${RETRY_TIMEWAIT-1}
attempt=1
exitCode=0
whole_start=$(date +%s)
attempt_start=whole_start

while true; do
  if "$@" ; then
    exitCode=0
    break
  else
    exitCode=$?
  fi

  if (( $attempt >= $max_attempts )) ; then
    break
  fi

  echo "Failed! ("$@") Spent time $(( $(date '+%s') - ${attempt_start} )) sec. Retrying in ${RETRY_TIMEWAIT}..." 1>&2
  sleep $RETRY_TIMEWAIT
  attempt=$(( attempt + 1 ))
  RETRY_TIMEWAIT=$(( RETRY_TIMEWAIT * 2 ))
  attempt_start=$(date +%s)
done

if [[ $exitCode != 0 ]] ; then
  echo -n "Totally failed! ("$@")" 1>&2
else
  echo -n "Done ("$@")" 1>&2
fi

echo " Spent time $(( $(date '+%s') - ${whole_start} )) sec in total" 1>&2

exit $exitCode
