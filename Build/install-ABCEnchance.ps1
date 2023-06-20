$conflictPlugins = @("CommunicationDemo.dll")

function WritePluginLine($path){
    $lines = Get-Content($path)
    $new = "ABCEnchance.dll"
    $newLines = "@($new)`n"
    foreach($l in $lines){
        if(!(([string]$l).CompareTo($new))){
            continue
        }
        if($conflictPlugins.Contains($l)){
            Write-Warning "$($l) conflicts with ABCEnchance and has been removed from the load list"
            continue
        }
        $newLines += @($l)
    }
    Set-Content $path -Value $newLines
}
$svenLocation = (Get-ItemPropertyValue -Path "Registry::HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 225840" -Name "InstallLocation" -ErrorAction SilentlyContinue)
if([String]::IsNullOrEmpty($svenLocation)){
    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = "../../SteamAppsLocation/SteamAppsLocation.exe"
    $pinfo.RedirectStandardOutput = $true
    $pinfo.UseShellExecute = $false
    $pinfo.Arguments = ("225840", "InstallDir")
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null
    $p.WaitForExit()
    $svenLocation = $p.StandardOutput.ReadToEnd()
    $svenLocation = $svenLocation.Substring(0,$svenLocation.Length-2)
}
$metaLocation = "$($svenLocation)/svencoop/metahook/"
if(Test-Path("$($svenLocation)/svencoop")){
    Write-Output "Copying dlls"
    Copy-Item -Force -Path "./abcenchance" -Destination "$($svenLocation)/svencoop"
    Copy-Item -Force -Path "./ABCEnchance.dll" -Destination "$($metaLocation)/plugins"
    Copy-Item -Force -Path "./ABCEnchance_AVX2.dll" -Destination "$($metaLocation)/plugins"
    Write-Output "Done"
    Write-Output "Writing plugin load list"
    if(Test-Path("$($metaLocation)/configs/plugins.lst")){
        WritePluginLine "$($metaLocation)/configs/plugins.lst"
    }
    Write-Output "Done"
}
else{
    Write-Error "`n`
        --------------------------------------------------------`n`
        (╯‵□′)╯︵┻━┻ Can not find Sven-Coop game dir`n`
        Is the key value of Sven coop in the registry corrupted?`n`
        --------------------------------------------------------`n"
}

Write-Output "Press any key to exit..."
Read-Host | Out-Null