#!/usr/bin/env pwsh
$S = [IO.Path]::PathSeparator # path separator. WIN:';', UNIX:":"

$IDF_PATH = $PSScriptRoot

Write-Output "Setting IDF_PATH: $IDF_PATH"
$env:IDF_PATH = $IDF_PATH

Write-Output "Adding ESP-IDF tools to PATH..."
$OLD_PATH = $env:PATH.split($S) | Select-Object -Unique # array without duplicates
# using idf_tools.py to get $envars_array to set
$envars_raw = python $IDF_PATH/tools/idf_tools.py export --format key-value
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

$envars_array # will be filled like:
#               [
#                    [vname1, vval1], [vname2, vval2], ...
#               ]
foreach ($line  in $envars_raw) {
    $pair = $line.split("=") # split in name, val
    $var_name = $pair[0].Trim() # trim spaces on the ends of the name
    $var_val = $pair[1].Trim() # trim spaces on the ends of the val
    $envars_array += (, ($var_name, $var_val))
}

foreach ($pair  in $envars_array) {
    # setting the values
    $var_name = $pair[0].Trim() # trim spaces on the ends of the name
    $var_val = $pair[1].Trim() # trim spaces on the ends of the val
    if ($var_name -eq "PATH") {
        # trim "%PATH%" or "`$PATH"
        if ($IsWindows) {
            $var_val = $var_val.Trim($S + "%PATH%")
        } else {
            $var_val = $var_val.Trim($S + "`$PATH")
        }
        # apply
        $env:PATH = $var_val + $S + $env:PATH
    } else {
        New-Item -Path "env:$var_name" -Value "$var_val" -Force
    }
}

# Allow calling some IDF python tools without specifying the full path
# ${IDF_PATH}/tools is already added by 'idf_tools.py export'
$IDF_ADD_PATHS_EXTRAS = [IO.Path]::Combine(${IDF_PATH}, "components", "esptool_py", "esptool")
$IDF_ADD_PATHS_EXTRAS += ${S} + [IO.Path]::Combine(${IDF_PATH}, "components", "app_update")
$IDF_ADD_PATHS_EXTRAS += ${S} + [IO.Path]::Combine(${IDF_PATH}, "components", "espcoredump")
$IDF_ADD_PATHS_EXTRAS += ${S} + [IO.Path]::Combine(${IDF_PATH}, "components", "partition_table")
$env:PATH = $IDF_ADD_PATHS_EXTRAS + $S + $env:PATH

#Compare Path's OLD vs. NEW
$NEW_PATH = $env:PATH.split($S) | Select-Object -Unique # array without duplicates
$dif_Path = Compare-Object -ReferenceObject $OLD_PATH -DifferenceObject $NEW_PATH -PassThru
if ($dif_Path -ne $null) {
    Write-Output "`nAdded to PATH`n-------------"
    Write-Output $dif_Path
} else {
    Write-Output "No directories added to PATH:"
    Write-Output $OLD_PATH
}


Write-Output "Checking if Python packages are up to date..."

Start-Process -Wait -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/check_python_dependencies.py"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

Write-Output "
Done! You can now compile ESP-IDF projects.
Go to the project directory and run:
    idf.py build

"
