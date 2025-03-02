param([int]$netease)

$MetaHookBaseDir = "thirdparty/MetaHookSv/"

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
if(!(Test-Path("$($MetaHookBaseDir)MetaHook.sln"))){
    Write-Warning "Init submodule..."
    &"git" "submodule" "update" "--init" "--recursive"
}
Write-Warning "Starting plugin building..."

&"msbuild" "/m" "/p:Configuration=$($BuildTarget)" "/t:ABCEnchance"

Read-Host "Press any key to exit." | Out-Null
Exit