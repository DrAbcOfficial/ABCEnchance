Write-Host "[Chose your building target]"
Write-Host "#1 Release"
Write-Host "#2 Release_AVX2"
Write-Host "#3 Release_AVX512"
Write-Host "#4 Debug"
$Chosen = Read-Host
switch ($Chosen) {
    "1" { $BuildTarget = "Release" }
    "2" { $BuildTarget = "Release_AVX2" }
    "3" { $BuildTarget = "Release_AVX512" }
    "4" { $BuildTarget = "Debug" }
    Default {
        Read-Host "Ivalid Input, exiting.." | Out-Null
        Exit
    }
}
if(!(Test-Path("../../global.props"))){
    Write-Host "Init build enviroment..."
    &"../../build-initdeps.bat"
}
if($BuildTarget -eq "Debug"){
    if(!(Test-Path("../../glew/build/lib/Debug/libglew32.lib"))){
        Write-Host "Building glew..."
        &"../../build-glew-debug.bat"
    }
}
else{
    if(!(Test-Path("../../glew/build/lib/Release/libglew32.lib"))){
        Write-Host "Building glew..."
        &"../../build-glew.bat"
    }
}
$vsLocation=[string](../../vswhere.exe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath)
if(Test-Path("$($vsLocation)\Common7\Tools\vsdevcmd.bat")){
    &"$($vsLocation)\Common7\Tools\vsdevcmd.bat" "-arch=x86"
    &"$($vsLocation)\Msbuild\Current\Bin\MSBuild.exe" "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)/ABCEnchance.vcxproj" /p:Configuration=$($BuildTarget) /p:Platform="Win32"
}
else {
    Write-Host "Can not found VS..."
}
Write-Host
Read-Host "Press any key to exit." | Out-Null
Exit