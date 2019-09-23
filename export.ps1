if ($env:MSYSTEM -ne $null) {
    Write-Output "This .ps1 file is for Windows Powershell only. When using MSYS, run:`n. ./export.sh."
    exit 1
}


$IDF_PATH = $PSScriptRoot

Write-Output "Setting IDF_PATH: $IDF_PATH"
$env:IDF_PATH=$IDF_PATH

Write-Output "Adding ESP-IDF tools to PATH..."
$OLD_PATH=$env:Path.split(";") | Select-Object -Unique # array without duplicates
# using idf_tools.py to get $envars_array to set
$envars_raw = python.exe $IDF_PATH\tools\idf_tools.py export --format key-value
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } # if error

$envars_array # will be filled like: 
#               [
#                    [vname1, vval1], [vname2, vval2], ...
#               ]
foreach ($line  in $envars_raw)
{
    $pair = $line.split("=") # split in name, val
    $var_name = $pair[0].Trim() # trim spaces on the ends of the name
    $var_val = $pair[1].Trim() # trim spaces on the ends of the val
    $var_val = $var_val -replace "%(.+)%", "`$env:`$1" # convert var syntax to PS using RegEx
    $var_val = $ExecutionContext.InvokeCommand.ExpandString($var_val) # expand variables to values
    $envars_array+=(,($var_name, $var_val))
}

foreach ($pair  in $envars_array) # setting the values
{
    $var_name = $pair[0].Trim() # trim spaces on the ends of the name
    $var_val = $pair[1].Trim() # trim spaces on the ends of the val
    Set-Item -Path "Env:$var_name" -Value "$var_val" 
}

#Compare Path's OLD vs. NEW
$NEW_PATH = $env:Path.split(";") | Select-Object -Unique # array without duplicates
$dif_Path = Compare-Object -ReferenceObject $OLD_PATH -DifferenceObject $NEW_PATH -PassThru
if ($dif_Path -ne $null)
{
    Write-Output $dif_Path
}
else {
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
