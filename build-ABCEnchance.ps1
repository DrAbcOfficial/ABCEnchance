Write-Output "[Chose your building target]"
Write-Output "#1 Release"
Write-Output "#2 Release_AVX2"
Write-Output "#3 Release_AVX512"
 "#4 Debug"
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
    Write-Output "Init build enviroment..."
    &"../../build-initdeps.bat"
}
if($BuildTarget -eq "Debug"){
    if(!(Test-Path("../../glew/build/lib/Debug/libglew32.lib"))){
        Write-Output "Building glew..."
        &"../../build-glew-debug.bat"
    }
}
else{
    if(!(Test-Path("../../glew/build/lib/Release/libglew32.lib"))){
        Write-Output "Building glew..."
        &"../../build-glew.bat"
    }
}
$vsLocation=[string](../../vswhere.exe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath)
if(Test-Path("$($vsLocation)\Common7\Tools\vsdevcmd.bat")){
    &"$($vsLocation)\Common7\Tools\vsdevcmd.bat" "-arch=x86"
    &"$($vsLocation)\Msbuild\Current\Bin\MSBuild.exe" "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)/ABCEnchance.vcxproj" /p:Configuration=$($BuildTarget) /p:Platform="Win32"
}
else {
    Write-Output "Can not found VS..."
}
Read-Host "Press any key to exit." | Out-Null
Exit