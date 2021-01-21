# This script is called from a Windows shortcut, with
# the working directory set to an ESP-IDF directory.
# Its purpose is to support using the "IDF Tools Directory" method of
# installation for ESP-IDF versions older than IDF v4.1.
# It does the same thing as "export.ps1" in IDF v4.1.

Param
(
    [String]$IdfGitDir,
    [String]$IdfPythonDir
)

$IDF_PATH = "."
$isEspIdfRoot = (Test-Path "$IDF_PATH/tools/idf.py")
if (-not $isEspIdfRoot) {
    Write-Output "Usage: idf_cmd_init.ps1 ^<Python directory^> ^<Git directory^>"
    Write-Output "This script must be invoked from ESP-IDF directory."
}

# Strip quotes
$IdfGitDir = $IdfGitDir.Trim("`"")
$IdfPythonDir = $IdfPythonDir.Trim("`"")

# Add Python and Git paths to PATH
$env:PATH = "$IdfGitDir;$IdfPythonDir;$env:PATH"
Write-Output "Using Python in $IdfPythonDir"
python.exe --version
Write-Output "Using Git in $IdfGitDir"
git.exe --version

# Check if this is a recent enough copy of ESP-IDF.
# If so, use export.ps1 provided there.
$isExport = (Test-Path "$IDF_PATH/export.ps1")
if ($isExport){
    . $IDF_PATH/export.ps1
}
else {
    Write-Output "IDF version does not include export.ps1. Using the fallback version."

    if ((Test-Path "$IDF_PATH/tools/tools.json")){
        $IDF_TOOLS_JSON_PATH = "$IDF_PATH/tools/tools.json"
    }
    else{
        Write-Output "IDF version does not include tools/tools.json. Using the fallback version."
        $IDF_TOOLS_JSON_PATH = "$PSScriptRoot/tools_fallback.json"
    }

    if ((Test-Path "$IDF_PATH/tools/idf_tools.py")){
        $IDF_TOOLS_PY_PATH = "$IDF_PATH/tools/idf_tools.py"
    }
    else{
        Write-Output "IDF version does not include tools/idf_tools.py. Using the fallback version."
        $IDF_TOOLS_PY_PATH = "$PSScriptRoot/idf_tools_fallback.py"
    }

    Write-Output "Setting IDF_PATH: $IDF_PATH"
    $env:IDF_PATH = $IDF_PATH

    Write-Output "Adding ESP-IDF tools to PATH..."
    $OLD_PATH = $env:Path.split(";") | Select-Object -Unique # array without duplicates
    # using idf_tools.py to get $envars_array to set
    $envars_raw = (python.exe "$IDF_TOOLS_PY_PATH" --tools-json "$IDF_TOOLS_JSON_PATH" export --format key-value)
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

    $envars_array # will be filled like:
    #               [
    #                    [vname1, vval1], [vname2, vval2], ...
    #               ]
    foreach ($line  in $envars_raw) {
        $pair = $line.split("=") # split in name, val
        $var_name = $pair[0].Trim() # trim spaces on the ends of the name
        $var_val = $pair[1].Trim() # trim spaces on the ends of the val
        $var_val = $var_val -replace "%(.+)%", "`$env:`$1" # convert var syntax to PS using RegEx
        $var_val = $ExecutionContext.InvokeCommand.ExpandString($var_val) # expand variables to values
        $envars_array += (, ($var_name, $var_val))
    }

    foreach ($pair  in $envars_array) {
        # setting the values
        $var_name = $pair[0].Trim() # trim spaces on the ends of the name
        $var_val = $pair[1].Trim() # trim spaces on the ends of the val
        Set-Item -Path "Env:$var_name" -Value "$var_val"
    }

    #Compare Path's OLD vs. NEW
    $NEW_PATH = $env:Path.split(";") | Select-Object -Unique # array without duplicates
    $dif_Path = Compare-Object -ReferenceObject $OLD_PATH -DifferenceObject $NEW_PATH -PassThru
    if ($dif_Path -ne $null) {
        Write-Output $dif_Path
    }
    else {
        Write-Output "No directories added to PATH:"
        Write-Output $OLD_PATH
    }


    Write-Output "Checking if Python packages are up to date..."

    Start-Process -Wait -NoNewWindow -FilePath "python" -Args "`"$IDF_PATH/tools/check_python_dependencies.py`""
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

    Write-Output "
Done! You can now compile ESP-IDF projects.
Go to the project directory and run:
    idf.py build
"

}
