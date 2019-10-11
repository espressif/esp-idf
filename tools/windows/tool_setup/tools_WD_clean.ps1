################################################################################
#
# Microsoft WindowsDefender exclusions cleaner
# Espressif Systems, 2019
#
################################################################################
#
# - cleans all Windows Defender process exclusions containing given path (both Process and Path)
# - run as Administrator, eg: PowerShell -ExecutionPolicy ByPass -File tools_WD_clean.ps1 -RmExclPath "C:\Program Files\Espressif\ESP-IDF Tools". If not running with admin privileges, the script tries to elevate itself (new process, output grabbed on exit)
# minimum requirements: Windows XP SP3, PowerShell 2.0, Windows Defender with relevant PS cmdlets
# - Returns 0 on success or -1 on failure
#
################################################################################


Param
(
	[String]$RmExclPath,
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

	Log-Msg -msg "Getting Windows Defender process exclusions..." -logF $logFile
	
	$Preferences = Get-MpPreference

	#ExclusionProcess	
	$cnt = $Preferences.ExclusionProcess.Count
	$cntRemoved = 0
	$cntRemovedTotal = 0
	$cntMissed = 0
	$cntMissedTotal = 0

	$bRmPath = ![string]::IsNullOrEmpty($RmExclPath)
	if( $bRmPath ) { Log-Msg -msg "Exclusion path: $RmExclPath" -logF $logFile }

	Log-Msg -msg " Found total $cnt of ExclusionProcess items" -logF $logFile

	foreach( $pref in $Preferences.ExclusionProcess ) {	

		if( $bRmPath ) { $bGoAhead = $pref.Contains($RmExclPath) }
		else { $bGoAhead = $true }
		
		if( $bGoAhead ) {
			Log-Msg -msg "  removing $pref" -logF $logFile
			Try 
			{ 
				Remove-MpPreference -ExclusionProcess $pref
				$cntRemoved++
			} 
			Catch 
			{
				if( ![string]::IsNullOrEmpty($logFile) ) { Write-Error -Exception $_.Exception *>> $logFile }
				Write-Error -Exception $_.Exception
				$cntMissed++
			}
		}
	}
	
	if( $cntMissed -eq 0 ) { Log-Msg -msg " $cntRemoved relevant items removed from ExclusionProcess list" -logF $logFile }
	else { Log-Msg -msg " WARNING: Only $cntRemoved out of $(cntRemoved+cntMissed) relevant items removed from ExclusionProcess list" -logF $logFile }

	#ExclusionPath
	$cnt = $Preferences.ExclusionPath.Count
	$cntRemovedTotal = $cntRemoved
	$cntRemoved = 0	
	$cntMissedTotal = $cntMissed
	$cntMissed = 0

	Log-Msg -msg " Found total $cnt of ExclusionPath items" -logF $logFile

	foreach( $pref in $Preferences.ExclusionPath ) {

		if( $bRmPath ) { $bGoAhead = $pref.Contains($RmExclPath) }
		else { $bGoAhead = $true }
		
		if( $bGoAhead ) {
			Log-Msg -msg "  removing $pref" -logF $logFile
			Try 
			{ 
				Remove-MpPreference -ExclusionPath $pref
				$cntRemoved++
			} 
			Catch 
			{
				if( ![string]::IsNullOrEmpty($logFile) ) { Write-Error -Exception $_.Exception *>> $logFile }
				Write-Error -Exception $_.Exception
				$cntMissed++
			}				
		}
	}

	if( $cntMissed -eq 0 ) { Log-Msg -msg " $cntRemoved relevant items removed from ExclusionPath list" -logF $logFile }
	else { Log-Msg -msg " WARNING: Only $cntRemoved out of $(cntRemoved+cntMissed) relevant items removed from ExclusionPath list" -logF $logFile }

	#TOTAL
	$cntRemovedTotal += $cntRemoved
	$cntMissedTotal += $cntMissed
	
	Log-Msg -msg "============================" -logF $logFile
	if( $cntMissedTotal -eq 0 ) { Log-Msg -msg "OK: Processed all $cntRemovedTotal items" -logF $logFile }
	else { Log-Msg -msg "WARNING: Processed only $cntRemovedTotal out of $(cntRemovedTotal+cntMissedTotal) relevat items" -logF $logFile }

	Log-Msg -msg  "`nDone" -logF $logFile
}
Catch
{
	if( ![string]::IsNullOrEmpty($logFile) ) { Write-Error -Exception $_.Exception *>> $logFile }
	Write-Error -Exception $_.Exception
	exit -1
}

