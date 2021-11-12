#!/usr/bin/env bash
#
# Tool for running scripts with several versions of Python by the use of pyenv (versions must be installed before in
# the docker image)
#
# Examples:
# ./multirun_with_pyenv.sh ./exec.sh                 # Run ./exec.h with ALL installed versions of Python
# ./multirun_with_pyenv.sh ./exec.sh arg1 arg2       # Run ./exec.h with arguments (and ALL installed versions of Python)
# ./multirun_with_pyenv.sh -p 2.7.15 ./exec.sh       # Run ./exec.h with Python 2.7.15 (-p must be the first argument)
# ./multirun_with_pyenv.sh -p 3.4.8,2.7.15 ./exec.sh # Run ./exec.h with Python 3.4.8 and 2.7.15 (versions must be
#                                               # separated by coma and be without a space)

PY_VERSIONS=""

{ source /opt/pyenv/activate; } || { echo 'Pyenv activation has failed!' ; exit 1; }

if [ "$1" = "-p" ]; then
    if [ "$#" -ge 2 ]; then
        IFS=',' read -a PY_VERSIONS <<< "$2"
        shift #remove -p
        shift #remove argument after -p
    else
        echo 'No value (Python version) is given for argument -p!'
        exit 1
    fi
else
    PY_VERSIONS=$(pyenv versions --bare)
fi

if [ "$#" -lt 1 ]; then
    echo 'No executable was passed to the runner!'
    exit 1
fi

for ver in ${PY_VERSIONS[@]}
do
    echo 'Switching to Python' $ver
    $(pyenv global $ver) || exit 1
    echo 'Running' $@
    $@ || {
               echo 'Run failed! Switching back to the system version of the Python interpreter.';
               pyenv global system;
               exit 1;
          }
done

echo 'Switching back to the system version of Python'
{ pyenv global system; } || { echo 'Restoring the system version of the Python interpreter has failed!' ; exit 1; }
