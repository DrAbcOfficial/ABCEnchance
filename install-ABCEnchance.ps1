$conflictPlugins = @("CommunicationDemo.dll")

function WritePluginLine($path, $avx){
    if($avx){
        $new = "ABCEnchance_AVX2.dll"
    }
    else{
        $new = "ABCEnchance.dll"
    }
    $lines = Get-Content($path)
    $newLines = @($new)
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
    Copy-Item -Force -Path "./Build/abcenchance" -Destination "$($svenLocation)/svencoop"
    Copy-Item -Force -Path "./Build/ABCEnchance.dll" -Destination "$($metaLocation)/plugins"
    Copy-Item -Force -Path "./Build/ABCEnchance_AVX2.dll" -Destination "$($metaLocation)/plugins"
    Write-Output "Done"
    Write-Output "Writing plugin load list"
    if(Test-Path("$($metaLocation)/configs/plugins.lst")){
        WritePluginLine "$($metaLocation)/configs/plugins.lst" $false
    }
    if(Test-Path("$($metaLocation)/configs/plugins_svencoop.lst")){
        WritePluginLine "$($metaLocation)/configs/plugins_svencoop.lst" $false
    }
    if(Test-Path("$($metaLocation)/configs/plugins_svencoop_avx2.lst")){
        WritePluginLine "$($metaLocation)/configs/plugins_svencoop_avx2.lst" $true
    }
    Write-Output "Done"
}
else{
    Write-Error "`n`
        --------------------------------------------------------`n`
        (¨s¨F¡õ¡ä)¨s¦à©ß©¥©ß Can not find Sven-Coop game dir`n`
        Is the key value of Sven coop in the registry corrupted?`n`
        --------------------------------------------------------`n"
}

Write-Output "Press any key to exit..."
Read-Host | Out-Null