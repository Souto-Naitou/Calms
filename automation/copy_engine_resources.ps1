param(
    [switch]$Debug = $false
)

# Define paths
$sourcePath = "..\modules\nima_engine\src\EngineResources"
$destinationPath = "..\app\EngineResources"

Set-Location -Path $PSScriptRoot

Write-Host ">> $(Split-Path -Leaf $MyInvocation.MyCommand.Path) ----------------------------------------"
if ($Debug) {
    Write-Host "Debug mode is ON"
    Write-Host "Source Path: $sourcePath"
    Write-Host "Destination Path: $destinationPath"
    Write-Host "Current Directory: $(Get-Location)"
}


# Create destination directory if it doesn't exist
if (!(Test-Path -Path $destinationPath)) {
    New-Item -ItemType Directory -Path $destinationPath
}

# Copy all files from source to destination
Copy-Item -Path "$sourcePath\*" -Destination $destinationPath -Recurse -Force

Write-Host "Engine resources copied from $sourcePath to $destinationPath"
