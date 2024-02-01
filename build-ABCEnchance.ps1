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
function TestOrBuild($test, $build, $msg){
	if(!(Test-Path("../../thirdparty/install/" + $test))){
        Write-Warning ("Can not find " + $msg + ", building...")
        $cmd = ("../../scripts/" + $build)
        &$cmd
    }
}
if($BuildTarget -eq "Debug"){
    TestOrBuild "glew/x86/Debug/lib/glewd.lib" "build-glew-x86-Debug.bat" "glew"
    TestOrBuild "capstone/x86/Debug/lib/capstone.lib" "build-capstone-x86-Debug.bat" "capstone"
	TestOrBuild "FreeImage/x86/Debug/lib/FreeImaged.lib" "build-FreeImage-x86-Debug.bat" "FreeImage"
}
if($BuildTarget -eq "Release"){
    TestOrBuild "glew/x86/Release/lib/glew.lib" "build-glew-x86-Release.bat" "glew"
    TestOrBuild "capstone/x86/Release/lib/capstone.lib" "build-capstone-x86-Release.bat" "capstone"
	TestOrBuild "FreeImage/x86/Release/lib/FreeImage.lib" "build-FreeImage-x86-Release.bat" "FreeImage"
}
if($BuildTarget -eq "Release_AVX2"){
    TestOrBuild "glew/x86/Release_AVX2/lib/glew.lib" "build-glew-x86-Release_AVX2" "glew"
    TestOrBuild "capstone/x86/Release/lib/capstone.lib" "build-capstone-x86-Release.bat" "capstone"
	TestOrBuild "FreeImage/x86/Release/lib/FreeImage.lib" "build-FreeImage-x86-Release.bat" "FreeImage"
}

Write-Warning "Starting plugin building..."
$vsLocation=[string](../../tools/vswhere.exe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath)
if(Test-Path("$($vsLocation)\Common7\Tools\vsdevcmd.bat")){
    &"$($vsLocation)\Common7\Tools\vsdevcmd.bat" "-arch=x86"
    &"$($vsLocation)\Msbuild\Current\Bin\MSBuild.exe" "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)/ABCEnchance.vcxproj" /p:Configuration=$($BuildTarget) /p:Platform="Win32"
}
else {
    Write-Error "`n`
        -------------------------------------------------`n`
        (╯‵□′)╯︵┻━┻ Can not find Visual Studio`n`
        Is the plugin folder in the correct location?`n`
        Eg: ./Github/MetaHookSv/Plugins/ABCEnchance`n`
        -------------------------------------------------`n"
}
Read-Host "Press any key to exit." | Out-Null
Exit