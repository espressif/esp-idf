#! /bin/bash

# Regexp for matching job names which are incompatible with Python 3
# - assign_test, nvs_compatible_test, IT - auto_test_script causes the incompatibility
# - UT_009_ - multi-device tests are not compatible
# - UT_014_ - multi-device tests are not compatible
# - UT_017_ - multi-device tests are not compatible
py3_incomp='assign_test|nvs_compatible_test|IT|UT_009_|UT_013_|UT_014_|UT_017_'

if [ -z ${PYTHON_VER+x} ] || [[ $CI_JOB_NAME =~ $py3_incomp ]]; then
    # Use this version of the Python interpreter if it was not defined before or
    # the given job is not compatible with Python 3
    PYTHON_VER=2.7.15
fi

if [ -f /opt/pyenv/activate ];
then
    source /opt/pyenv/activate
    pyenv global $PYTHON_VER || {
                                    echo 'Python' $PYTHON_VER 'is not installed.'
                                    INSTALLED_PY_VERS=$(pyenv versions --bare)

                                    while [ ${#PYTHON_VER} -gt 0 ]
                                    do
                                        echo 'Tring to locate a match for' $PYTHON_VER

                                        for ver in ${INSTALLED_PY_VERS[@]}
                                        do
                                            if [[ $ver == $PYTHON_VER* ]];
                                            then
                                                pyenv global $ver
                                                break 2
                                            fi
                                        done

                                        # Removing last character and trying to find some match.
                                        # For example, if 3.4.8 was selected but isn't installed then it will try to
                                        # find some other installed 3.4.X version, and then some 3.X.X version.
                                        PYTHON_VER=${PYTHON_VER: : -1}
                                    done
                                }
    python --version || {
                            echo 'No matching Python interpreter is found!'
                            exit 1
                        }
elif command -v python -V 1>/dev/null 2>&1;
then
    python --version
    echo 'No /opt/pyenv/activate exists and Python from path is used.'
else
    echo 'No /opt/pyenv/activate exists and no Python interpreter is found!'
    exit 1
fi
