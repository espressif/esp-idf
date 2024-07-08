# Tests whether a path is an IDF path
def is_idf [] {
    ($in | path join tools/idf.py | path exists) or ($in | path join tools/idf_tools.py | path exists)
}

# Returns a value that's to be assigned to `$env.IDF_PATH`:
# - Uses an existing value if it's valid;
# - Sets it if the script is inside an IDF directory;
# - Raises an error otherwise.
def detect_idf_path [] {
    let path = $env.IDF_PATH? | default $env.FILE_PWD;
    $path | is_idf | if $in {
        # Success
        $path
    } else {
        # Failure
        if $env.IDF_PATH? == null {
            error make -u {
                msg: "IDF_PATH is not set and this script is not inside an IDF directory.",
                help: "set IDF_PATH manually using $env.IDF_PATH = \"/your/path/here\"",
            }
        } else {
            error make -u {
                msg: "IDF_PATH points to something that is not an IDF directory.",
                help: "set IDF_PATH manually using $env.IDF_PATH = \"/your/path/here\"",
            }
        }
    }
}

# Returns a value that's to be assigned to `$env.ESP_PYTHON`:
# the path to a Python interpreter whose version falls in the specified range.
def detect_py [major: int, min_minor: int, max_minor: int] {
    let suitable_pythons = $min_minor..$max_minor
                         | each { |i| $"python($major).($i)" }
                         | append [python, $"python($major)"]
                         | each { |executable|
        try {
            let ver = run-external $executable "--version" # "Python 3.9.13"
                    | split row " " | get 1                # "3.9.13"
                    | split column "." major minor         # [[major, minor]; ["3", "9"]]
                    | into record                          # {major: "3", minor: "9"}
                    | update major { into int }            # {major: 3, minor: "9"}
                    | update minor { into int }            # {major: 3, minor: 9}
            let suitable = $ver.major == $major and $ver.minor >= $min_minor and $ver.minor <= $max_minor
            { executable: $executable, version: $ver, suitable: $suitable }
        } catch {
            # in case we try to run an executable that doesn't exist
            null
        }
    } | where suitable | sort-by -r version
    let result = $suitable_pythons.0.executable?
    if $result == null {
        error make -u {
            msg: $"Could not find a Python interpreter in the version range ($major).($min_minor) .. ($major).($max_minor)",
            help: "install Python per instructions in the documentation"
        }
    } else {
        $result
    }
}

# Returns a record with the required environment variables set to their corresponding values
def add_exports [] {
    let extras = [
        ($env.IDF_PATH | path join components/espcoredump)
        ($env.IDF_PATH | path join components/partition_table)
        ($env.IDF_PATH | path join components/app_update)
    ]
    let tools = $env.IDF_PATH | path join tools/idf_tools.py
    let args = [export, "--format", key-value, $"--add_paths_extras=($extras | str join ":")"]
    let vars = run-external $env.ESP_PYTHON $tools ...$args               # "K1=V1\nK2=V2"
             | split row "\n"                                             # ["K1=V1", "K2=V2"]
             | each { split row "=" }                                     # [["K1", "V1"], ["K2", "V2"]]
             | each {|i| if $i.0 == PATH {                                #   PATH requires special treatment:
                [$i.0 ($i.1 | split row ":"                               #   parse text into list
                | drop 1 | append $env.PATH                               #   replace last "$PATH" with its actual value
                | prepend $extras)]                                       #   prepend extras
             } else $i}                                                   #   non-PATH
             | reduce {|item, acc| $acc | insert $item.0 $item.1} -f {}   # {"K1": "V1", "K2": "V2"}
    $vars
}

$env.IDF_PATH = (detect_idf_path)
print $"IDF_PATH: ($env.IDF_PATH)"

$env.ESP_PYTHON = (detect_py 3 8 12)
print $"ESP_PYTHON: ($env.ESP_PYTHON)"

add_exports | load-env $in
print $"PATH: ($env.PATH)"

print ""
print "Done! You can now compile ESP-IDF projects."
print "Go to the project directory and run:"
print ""
print "  idf.py build"
