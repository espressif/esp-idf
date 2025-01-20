# Recipe format description for idf.py diag

The `idf.py diag` command processes one or more `recipe` files. Each `recipe`
file outlines a collection of related data and files that should be gathered.
For instance, the `idf.yml` recipe gathers information related to the ESP-IDF.
`Recipes` are formatted in YAML. The output from each `recipe` consists of a
set of files located in the diagnostic report directory, which may be copied or
generated as a result of executing certain commands.

A `recipe` is made up of one or more `steps`, and each `step` contains one or
more commands. A recipe can consist of just one `step` where all the commands
are carried out. The aim is to split the `recipe` into logical steps if it is
convenient. For instance, a `project.yml` `recipe` might include one `step` to
gather all log files and another to collect linker script files, although these
tasks could also be completed in a single `step`. Refer to the `recipes` in
this directory for examples.

## Overview of Recipe Structure

    description: brief recipe description
    tags:        list of tags that can be used to identify the recipe
    output:      root recipe directory for its output in the report folder
    steps:       list of steps to execute within a recipe
      - name:    brief step description
        output:  step directory for its output in the report folder
        cmds:    list of commands to execute within a step
          - cmd: command name
            arg: command argument
            ...
          ...
      ...

## Recipe variables

The `recipe` can utilize the following variables. The `idf.py diag` assigns
values to these variables and expands them in the recipe upon loading. To use a
variable, format it as `${NAME}`, such as `${IDF_PATH}`.

* PROJECT_DIR

    Project directory specified by idf.py using the `-C` or `--project-dir`
    option.

* BUILD_DIR

    Build directory specified by idf.py using the `-B` or `--build-dir` option.

* IDF_PATH

    IDF path as defined in the environment variable.

* REPORT_DIR

    The report directory is where all the recipe outputs are stored. Keep in
    mind that during the execution of `idf.py diag`, it points to a temporary
    directory, which is moved to its final destination once the report is
    successfully completed.

## Recipe

* description: string (required)

    Short `recipe` description, which is shown in `idf.py diag` progress.

* tags: list (optional)

    Strings which identify this `recipe`. Used to specify which `recipes`
    should the `idf.py diag` use. All `recipes` with a given tag are used.

* output: string (optional)

    Global output directory for the `recipe`. This directory serves as the main
    directory for all files produced by this `recipe` within the report
    directory.  For instance, if it is set to `idf` and the report directory is
    `report`, then all files collected by this `recipe` will be stored in the
    `report/idf` directory. This helps organize the collected files within the
    report directory.

* steps: list (required)

    One or more `steps` to follow for this `recipe`. This allows, but does not
    require, splitting the `recipe` into more logical sections. For example,
    one `step` could gather all log files, while another might collect
    environment information.

## Step

* name: string (required)

    Brief description of the `step`, displayed in the `idf.py diag` progress
    beneath the `recipe` description.

* output: string (optional)

    Global output directory for the `step`. This directory serves as the main
    directory for all files produced by this `step` within the recipe `output`
    directory.  For instance, if it is set to `logs` and the report directory
    is `report` and recipe `output` is `idf`, then all files collected by this
    `step` will be stored in the `report/idf/logs` directory. 

* cmds: list (required)

    Sequence of commands to be executed in this `step`.

## Commands

The following commands can be used within a `step` in the `recipe`.  Each
command consists of a list that includes the command name, such as `exec` or
`glob`, along with its arguments. Please be aware that if a command fails, it
does not terminate `idf.py diag`; all commands will still be executed.  The
command mapping key has no value, and if it is present, it is ignored.

### file

Copy the specified file from the given path to the report directory. If no
`output` argument is provided, the file is copied using its original name.

* path: string (required)

    Path to the source file.

* output: string (optional)

    The destination path for the file within the report directory. If it ends
    with a `/` character, it is treated as a directory, and the source file
    name from the `path` argument is added to it. Otherwise, it is considered a
    complete path including the file name. If not provided, the source file
    name is used as the destination file name. The complete destination path in
    the report directory is built starting with the `output` directory
    specified in the `recipe`, followed by the `output` directory specified in
    the`step`, and finally the `output` specified in the `file` command, if
    each is provided. All directories that do not exist in the output path are
    created.

Example:

    - file:
      path: '${BUILD_DIR}/compile_commands.json'
      output: 'build/json_files/commands.json'

### exec

Run the given command and save its output.

* cmd: string or list (required)

    Command to run. If it's a string, it starts via the shell; if it's a list,
    it starts without shell expansions. Using the list format can help avoid
    escaping command arguments.

* output: string (optional)

    The path in the report directory where the command's standard output should
    be stored. If not specified, the command output will not be saved. The
    complete destination path in the report directory is built starting with
    the `output` directory specified in the `recipe`, followed by the `output`
    directory specified in the `step`, and finally the `output` specified in
    the `exec` command, if each is provided.  The `/` character at the end is
    disregarded in contrast to the `file` command.

* stderr: string (optional)

    Similar to `output`, but specifically for standard error output.

* timeout: int (optional)

    The number of seconds to wait for the command to execute.

* append: boolean (optional)

    Append the command's standard output and standard error to the files
    specified by the `output` and `stderr` arguments.

Example:

    - exec:
      cmd: 'idf.py --version'
      timeout: 10
      output: esp_idf.ver

    - exec:
      cmd:
        - python
        - -c
        - |
          import platform
          print(f'system: {platform.system()}')

### env

Store details of the specified environment variables in a file using the format
variable=value.

* vars: list (required)

    A list of names for environment variables to gather.

* regex: string (optional)

    Optional regular expression to gather environment variables that match it.

* output: string (optional)

    The path in the report directory where the environment variable information
    should be stored. If not specified, nothing will be saved. The complete
    destination path in the report directory is built starting with the
    `output` directory specified in the `recipe`, followed by the `output`
    directory specified in the `step`, and finally the `output` specified in
    the `env` command, if each is provided. The `/` character at the end is
    disregarded, in contrast to the `file` command.

* append: boolean (optional)

    Append the environmental variables information to the file specified by the
    `output` argument.

Example:

    - env:
      vars:
        - IDF_PATH
        - IDF_PYTHON_ENV_PATH
        - IDF_TOOLS_PATH
        - PATH
        - OPENOCD_SCRIPTS
        - PYTHONPATH
        - MSYSTEM
      regex: '.*IDF.*|.*ESP.*'
      output: environment.var

### glob

Find pathnames that match a specific pattern within a specified directory, and
include them in the report directory.

* pattern: string (required)

    Pattern matching with wildcards.

* path: string (required)

    Directory to look for files that match the `pattern`.

* output: string (optional)

    Every file that matches the `pattern` is stored in the report directory at
    the location specified by `output`. If `output` ends with a `/`, it is
    interpreted as a directory, and the matched file name is appended to it. If
    it does not end with a `/`, it is regarded as a full path including the
    file name. If `output` is not specified, the name of the matching file is
    used as the destination file name.

    The full destination path in the report directory for each matched file
    is constructed by using the `output` directory from the `recipe`, followed
    by the `output` directory from the `step`, and finally the `output` from
    the `glob` command, if each is available. Any directories that do not exist
    in the output path are created. If the `relative` argument is set and the
    `output` argument in the `glob` command ends with `/`, the file's relative
    path to the `path` argument is appended to the `output` argument in the
    `glob` command. A `.<cnt>` suffix is added to a file path if that path
    already exists in the report directory. The `<cnt>` is a number that
    increases until a unique path is found in the report directory.

* mtime: boolean (optional)

    If multiple files are found, add only the one that was modified most recently.

* recursive: boolean (optional)

    Recursively search the `path`.

* relative: boolean (optional)

    Save the file in the `output` directory, preserving the same relative path
    it has in the `path` directory.

* regex: string (optional)

    Only add files whose content matches the specified regular expression.

Example:

    - glob:
      pattern: 'idf_py_stdout_output*'
      path: '${BUILD_DIR}/log'
      regex: '^Command: .*idf_monitor.py.*$'
      output: 'logs/monitor/'
      mtime: True
