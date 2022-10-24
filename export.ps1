#!/usr/bin/env pwsh
$S = [IO.Path]::PathSeparator # path separator. WIN:';', UNIX:":"

$IDF_PATH = "$PSScriptRoot"

Write-Output "Setting IDF_PATH: $IDF_PATH"
$env:IDF_PATH = "$IDF_PATH"

Write-Output "Checking Python compatibility"
python "$IDF_PATH/tools/python_version_checker.py"

Write-Output "Adding ESP-IDF tools to PATH..."
$OLD_PATH = $env:PATH.split($S) | Select-Object -Unique # array without duplicates
# using idf_tools.py to get $envars_array to set
$envars_raw = python "$IDF_PATH/tools/idf_tools.py" export --format key-value
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

$envars_array = @() # will be filled like:
#               [
#                    [vname1, vval1], [vname2, vval2], ...
#               ]
foreach ($line  in $envars_raw) {
    $pair = $line.split("=") # split in name, val
    $var_name = $pair[0].Trim() # trim spaces on the ends of the name
    $var_val = $pair[1].Trim() # trim spaces on the ends of the val
    $envars_array += (, ($var_name, $var_val))
}

if ($IsWindows -eq $null) {
    # $IsWindows was added in PowerShell Core 6 and PowerShell 7 together with multi-platform support. # I.E. if this
    # internal variable is not set then PowerShell 5 is used and # the platform cannot be # anything else than Windows.
    $IsWindows = $true
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
function idf.py { &python "$IDF_PATH\tools\idf.py" $args }
function espefuse.py { &python "$IDF_PATH\components\esptool_py\esptool\espefuse.py" $args }
function espsecure.py { &python "$IDF_PATH\components\esptool_py\esptool\espsecure.py" $args }
function otatool.py { &python "$IDF_PATH\components\app_update\otatool.py" $args }
function parttool.py { &python "$IDF_PATH\components\partition_table\parttool.py" $args }

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

Start-Process -Wait -NoNewWindow -FilePath "python" -Args "`"$IDF_PATH/tools/idf_tools.py`" check-python-dependencies"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

$uninstall = python "$IDF_PATH/tools/idf_tools.py" uninstall --dry-run

if (![string]::IsNullOrEmpty($uninstall)){
    Write-Output ""
    Write-Output "Detected installed tools that are not currently used by active ESP-IDF version."
    Write-Output "$uninstall"
    Write-Output "For free up even more space, remove installation packages of those tools. Use option 'python.exe $IDF_PATH\tools\idf_tools.py uninstall --remove-archives'."
    Write-Output ""
}

Write-Output "
Done! You can now compile ESP-IDF projects.
Go to the project directory and run:
    idf.py build

"
