[CmdletBinding()]
param (
    [string]$Configuration = "Release",
    [switch]$Rebuild = $false
)

Set-Variable -Name "SolutionRoot" -Value (Resolve-Path $PSScriptRoot\..).Path -Option Constant
Set-Variable -Name "MSBuildPath" -Value (Resolve-Path "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe").Path -Option Constant

if (-Not (Test-Path $MSBuildPath)) {
    Write-Error "MSBuild not found at $MSBuildPath. Please verify the path."
    exit 1
}

Write-Host "Building solution in $Configuration mode..." -BackgroundColor DarkGreen -ForegroundColor White
if ($Rebuild) {
    & $MSBuildPath "$SolutionRoot\Calms.sln" /t:Rebuild /p:Configuration=$Configuration
} else {
    & $MSBuildPath "$SolutionRoot\Calms.sln" /t:Build /p:Configuration=$Configuration
}

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
} else {
    Write-Host "Build succeeded." -BackgroundColor DarkGreen -ForegroundColor White
}