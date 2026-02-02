[CmdletBinding()]
Param(
    [string]$Destination = ".",
    [string]$Configuration = "Release"
)

if (-Not (Test-Path $Destination)) {
    New-Item -ItemType Directory -Path $Destination | Out-Null
}

Set-Variable -Name "SolutionRoot" -Value (Resolve-Path $PSScriptRoot\..).Path -Option Constant
Set-Variable -Name "CalmsRoot" -Value (Resolve-Path "$SolutionRoot\app").Path -Option Constant
Set-Variable -Name "EngineRoot" -Value (Resolve-Path "$SolutionRoot\modules\nima_engine\src").Path -Option Constant
Set-Variable -Name "BinPath" -Value (Resolve-Path "$SolutionRoot\bin").Path -Option Constant

Get-ChildItem -Path "$BinPath\$Configuration" | 
Where-Object {$_.Extension -in '.exe', '.dll'} | 
ForEach-Object {
    Write-Host "Copying $($_.Name) to $Destination" -BackgroundColor DarkBlue -ForegroundColor White
    Copy-Item -Path $_.FullName -Destination $Destination -Force
}

Set-Variable -Name "Resources_Calms" -Value "resources" -Option Constant
Set-Variable -Name "Resources_Engine" -Value "EngineResources" -Option Constant

if (-Not (Test-Path "$Destination\$Resources_Calms"))
{
    Write-Host "Creating directory $Resources_Calms in $Destination" -BackgroundColor DarkBlue -ForegroundColor White
    New-Item -ItemType Directory -Path "$Destination\$Resources_Calms" | Out-Null
}
if (-Not (Test-Path "$Destination\$Resources_Engine"))
{
    Write-Host "Creating directory $Resources_Engine in $Destination" -BackgroundColor DarkBlue -ForegroundColor White
    New-Item -ItemType Directory -Path "$Destination\$Resources_Engine" | Out-Null
}

Write-Host "Copying resources to $Destination" -BackgroundColor DarkBlue -ForegroundColor White
Write-Host " - Copying $Resources_Calms" -BackgroundColor DarkBlue -ForegroundColor White
Write-Host " - Copying $Resources_Engine" -BackgroundColor DarkBlue -ForegroundColor White

Copy-Item -Path "$CalmsRoot\$Resources_Calms\*" -Destination "$Destination\$Resources_Calms" -Recurse -Force
Copy-Item -Path "$EngineRoot\$Resources_Engine\*" -Destination "$Destination\$Resources_Engine" -Recurse -Force