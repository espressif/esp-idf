################################################################################
#
# Microsoft WindowsDefender exclusions handler
# Espressif Systems, 2019
#
################################################################################
# 	
#	PS utility to add/remove PROCESS exceptions to/from MS WD real-time 
#	scanning. Files (referenced by 'path' or 'path\filemask') are expected
#	to be Windows process executables, for obvious reasons.
#
#	The script requires Administrator privileges to succeed -> self-elevation procedure is involved
#
# Usage:
#
# 	PowerShell -ExecutionPolicy ByPass -File tools_WD_excl.ps1 <ARGUMENTS>
#
#	ARGUMENTS:
#		-AddExclPath <path | path\*.filemask>
#			add all matching files in the path (recursive) to the WD exception list
#	
#		-AddExclFile <filepath>
#			adds file to the WD exception list exactly as specified by 'filepath'
#
#		-RmExclPath <path | path\*.filemask>
#			remove all matching files in the path (recursive) from WD exclusions
#
#		-RmExclFile <filepath>
#			adds file to the WD exception list exactly as specified by 'filepath'
#
#		-logFile <filepath>
#			stdout/stderr redirection file. Used internally for elevated process (generated in tempdir, deleted after the script finishing) 
#			use manually at your own risk
#
#	Returns 0 on success or -1 on failure
#
#
#	Example:
#		PowerShell -ExecutionPolicy ByPass -File tools_WD_excl.ps1 -AddExclPath "C:\Program Files\Espressif\ESP-IDF Tools\*.exe"
#
#	Notes:
# 		 - default scenario is set to the following 
#			-AddExclPath "$Env:ProgramFiles\Espressif\ESP-IDF Tools\*.exe"
#			(eg when called with no params)
#		 - only named parameters are supported, any other use-cases redirect to the default
#		 - multiple paths/files in 1 parameter are not supported by this version
#		 - minimum requirements: Windows XP SP3, PowerShell 2.0, Windows Defender with relevant PS cmdlets
#
################################################################################


Param
(
	[String]$AddExclPath,
	[String]$AddExclFile,
	[String]$RmExclPath,
	[String]$RmExclFile,
	[String]$logFile
)

Import-Module Defender

function Check-Command($cmdname)
{
    return [bool](Get-Command -Name $cmdname -ErrorAction SilentlyContinue)
}

function Log-Msg($msg, $logF = $null)
{
	if( ![string]::IsNullOrEmpty($logF) ) { Write-Output $msg *>> $logF } 
	else { Write-Output $msg }
	[Console]::Out.Flush()
}


Try
{	
	#self-elevation support
	$myWindowsID=[System.Security.Principal.WindowsIdentity]::GetCurrent()
	$myWindowsPrincipal=new-object System.Security.Principal.WindowsPrincipal($myWindowsID)
	$adminRole=[System.Security.Principal.WindowsBuiltInRole]::Administrator

	if( -not $myWindowsPrincipal.IsInRole($adminRole) ) { 
	   
		 $params = ""
		 foreach($key in $PSBoundParameters.keys) {
			 $params = -join( $params, "-", $key, " `"", $PSBoundParameters[$key], "`"" )
		 }
		
		#running elevated and logFile not set
		if( [string]::IsNullOrEmpty($logFile) ) {
			$tempFileName = Get-Date -UFormat "%Y%m%d%H%M%s"
			$lf = Join-Path -Path $env:TEMP -ChildPath "WDEspLog$tempFileName.log"
			#Write-Output "Logfile: $lf"
		}
		
		$newProcess = new-object System.Diagnostics.ProcessStartInfo "PowerShell"
		$newProcess.Arguments = "-ExecutionPolicy ByPass -File " + $script:MyInvocation.MyCommand.Definition + " " + $params + " -logFile $lf"
		$newProcess.Verb = "RunAs"
		$newProcess.WindowStyle = [System.Diagnostics.ProcessWindowStyle]::Hidden
		
		$proc = [System.Diagnostics.Process]::Start($newProcess)
		$proc.WaitForExit()

		if (Test-Path -Path $lf ) {
			foreach($line in Get-Content $lf) {
				Log-Msg -msg $line
			}
			Remove-Item $lf
		}
		
		#Write-Output "Process finished with code " $proc.ExitCode	
		exit $proc.ExitCode
	}
	
	#parameter sanity check
	if( $Args.Count -gt 0 ) {
		$Exception = [ArgumentException]::new("Only named parameters are supported: $Args")
		throw $Exception
	}

	#check WinDefender cmdlets are available
	if (!(Check-Command -cmdname 'Add-MpPreference')) {
		$Exception = [NotSupportedException ]::new("Windows Defender cmdlets not available")
		throw $Exception
	}

	$pathsToExclude = New-Object 'System.Collections.Generic.List[String]'
	$filesToExclude = New-Object 'System.Collections.Generic.List[String]'
	$pathsToInclude = New-Object 'System.Collections.Generic.List[String]'
	$filesToRemove = New-Object 'System.Collections.Generic.List[String]'

	if( $PSBoundParameters.Count -gt 0 ) {

		$bAddPath = ![string]::IsNullOrEmpty($AddExclPath)
		$bAddFile = ![string]::IsNullOrEmpty($AddExclFile)
		$bRmPath = ![string]::IsNullOrEmpty($RmExclPath)
		$bRmFile = ![string]::IsNullOrEmpty($RmExclFile)
		
		if( !$bAddPath -And !$bAddFile -And !$bRmPath -And !$bRmFile ) {
			$Exception = [ArgumentException]::new("Invalid parameter(s): $Args")
			throw $Exception
		}

		#ADD exclusion paths
		if( $bAddPath ) {
			#foreach ($wdPath in $AddExclPath) {
			#	$pathsToExclude.Add( $wdPath )
			#}
			$pathsToExclude.Add( $AddExclPath )
		}

		#ADD exclusion files
		if( $bAddFile ) {
			#foreach ($wdFile in $AddExclFile) {
			#	$filesToExclude.Add( $wdFile )
			#}
			$filesToExclude.Add( $AddExclFile )
		}

		#REMOVE exclusion paths
		if( $bRmPath ) {
			#foreach ($wdPath in $RmExclPath) {
			#	$pathsToInclude.Add( $wdPath )
			#}
			$pathsToInclude.Add( $RmExclPath )
		}

		#ADD exclusion file
		if( $bAddFile ) {
			#foreach ($wdFile in $RmExclFile) {
			#	$filesToRemove.Add( $wdFile )
			#}
			$filesToRemove.Add( $RmExclFile )
		}
	}
	#default: throw exception
	else {
		$Exception = [ArgumentException]::new("Mandatory parameter missing")
		throw $Exception
	}


	#to exclude all files opened by a process including the process' binary, a record must be added to both Exclusions/Paths and Exclusions/Processes configurations, see
	# https://docs.microsoft.com/en-us/windows/security/threat-protection/windows-defender-antivirus/configure-process-opened-file-exclusions-windows-defender-antivirus :
	# "When you add a process to the process exclusion list, Windows Defender Antivirus won't scan files opened by that process, no matter where the files are located. The process itself, however, will be scanned unless it has also been added to the file exclusion list.
	#The exclusions only apply to always-on real-time protection and monitoring. They don't apply to scheduled or on-demand scans."

	Log-Msg -msg "Updating Windows Defender real-time scan exclusions:" -logF $logFile

	$itemCount = 0

	#exclusions
	foreach( $exclPath in $pathsToExclude ) {
		$exclFiles = Get-ChildItem -Recurse -File -Path $exclPath | % { $_.FullName }
		foreach ($exfile in $exclFiles) {
			Log-Msg -msg " adding $exfile" -logF $logFile
			Add-MpPreference -ExclusionProcess $exfile
			Add-MpPreference -ExclusionPath $exfile
			$itemCount++
		}
	}

	### ! better run in separate, adding files to exclusion object array from above is very inefficient (forced reallocations)
	foreach ($exfile1 in $filesToExclude) {
		Log-Msg -msg " adding $exfile1" -logF $logFile
		Add-MpPreference -ExclusionProcess $exfile1
		Add-MpPreference -ExclusionPath $exfile1
		$itemCount++
	}

	#inclusions
	foreach( $inclPath in $pathsToInclude ) {
		$inclFiles = Get-ChildItem -Recurse -File -Path $inclPath | % { $_.FullName }
		foreach ($infile in $inclFiles) {
			Log-Msg -msg " removing $infile" -logF $logFile
			Remove-MpPreference -ExclusionProcess $infile
			Remove-MpPreference -ExclusionPath $infile
			$itemCount++
		}
	}

	### ! see exclusions
	foreach ($infile1 in $filesToExclude) {
		Log-Msg -msg " removing $infile1" -logF $logFile
		Remove-MpPreference -ExclusionProcess $infile1
		Remove-MpPreference -ExclusionPath $infile1
		$itemCount++
	}

	Log-Msg -msg "Done (processed $itemCount items)" -logF $logFile
}
Catch
{
	if( ![string]::IsNullOrEmpty($logFile) ) { Write-Error -Exception $_.Exception *>> $logFile }
	Write-Error -Exception $_.Exception
	exit -1
}
