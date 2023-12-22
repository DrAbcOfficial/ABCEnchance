Write-Output "[Chose your building target]"
Write-Output "#1 Release"
Write-Output "#2 Release_AVX2"
Write-Output "#3 Debug"
$Chosen = Read-Host
switch ($Chosen) {
    "1" { $BuildTarget = "Release" }
    "2" { $BuildTarget = "Release_AVX2" }
    "3" { $BuildTarget = "Debug" }
    Default {
        Read-Host "Ivalid Input, exiting.." | Out-Null
        Exit
    }
}
if(!(Test-Path("../../tools/global.props"))){
    Write-Warning "Init build enviroment..."
    &Rename-Item -Path "../../tools/global_template.props" -NewName "global.props"
}
if($BuildTarget -eq "Debug"){
    if(!(Test-Path("../../install/glew/x86/Debug/lib/glewd.lib"))){
        Write-Warning "Can not find glew, building..."
        &"../../scripts/build-glew-x86-Debug.bat"
    }
    if(!(Test-Path("../../thirdparty/install/capstone/x86/Debug/lib/capstone.lib"))){
        Write-Warning "Can not find capstone, building..."
        &"../../scripts/build-capstone-x86-Debug.bat"
    }
}
if($BuildTarget -eq "Release"){
    if(!(Test-Path("../../thirdparty/install/glew/x86/Release/lib/glew.lib"))){
        Write-Warning "Can not find glew, building..."
        &"../../scripts/build-glew-x86-Release.bat"
    }
    if(!(Test-Path("../../thirdparty/install/capstone/x86/Release/lib/capstone.lib"))){
        Write-Warning "Can not find capstone, building..."
        &"../../scripts/build-capstone-x86-Release.bat"
    }
}
if($BuildTarget -eq "Release_AVX2"){
    if(!(Test-Path("../../thirdparty/install/glew/x86/Release_AVX2/lib/glew.lib"))){
        Write-Warning "Can not find glew, building..."
        &"../../scripts/build-glew-x86-Release_AVX2.bat"
    }
    if(!(Test-Path("../../thirdparty/install/capstone/x86/Release/lib/capstone.lib"))){
        Write-Warning "Can not find capstone, building..."
        &"../../scripts/build-capstone-x86-Release.bat"
    }
}
$vsLocation=[string](../../tools/vswhere.exe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath)
if(Test-Path("$($vsLocation)\Common7\Tools\vsdevcmd.bat")){
    &"$($vsLocation)\Common7\Tools\vsdevcmd.bat" "-arch=x86"
    &"$($vsLocation)\Msbuild\Current\Bin\MSBuild.exe" "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)/ABCEnchance.vcxproj" /p:Configuration=$($BuildTarget) /p:Platform="Win32"
}
else {
    Write-Error "`n`
        -------------------------------------------------`n`
        (¨s¨F¡õ¡ä)¨s¦à©ß©¥©ß Can not find Visual Studio`n`
        Is the plugin folder in the correct location?`n`
        Eg: ./Github/MetaHookSv/Plugins/ABCEnchance`n`
        -------------------------------------------------`n"
}
Read-Host "Press any key to exit." | Out-Null
Exit