# Build script for L1 Test Suite for Westeros (PowerShell)
# Run this on Windows to build with all components and coverage

param(
    [switch]$Clean,
    [switch]$Coverage,
    [switch]$Release,
    [int]$Jobs = $env:NUMBER_OF_PROCESSORS
)

$ErrorActionPreference = "Stop"

# Colors
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

Write-ColorOutput Cyan "=========================================="
Write-ColorOutput Cyan "L1 Test Suite for Westeros - Build"
Write-ColorOutput Cyan "=========================================="
Write-Output ""

# Determine build type
$BuildType = if ($Release) { "Release" } else { "Debug" }
$EnableCoverage = if ($Coverage) { "ON" } else { "OFF" }

Write-ColorOutput Yellow "Build Type: $BuildType"
Write-ColorOutput Yellow "Coverage: $EnableCoverage"
Write-ColorOutput Yellow "Jobs: $Jobs"
Write-Output ""

# Get absolute paths
$RootDir = Get-Location
$WesterosMainSrc = $RootDir
$SimpleshellSrc = Join-Path $RootDir "simpleshell"
$SimplebufferSrc = Join-Path $RootDir "simplebuffer"
$LinuxDmabufSrc = Join-Path $RootDir "linux-dmabuf"
$LinuxExpsyncSrc = Join-Path $RootDir "linux-expsync"

# Verify source files exist
Write-ColorOutput Cyan "Verifying source files..."
$requiredFiles = @(
    "westeros-compositor.cpp",
    "simpleshell\westeros-simpleshell.cpp",
    "simplebuffer\westeros-simplebuffer.cpp",
    "linux-dmabuf\westeros-linux-dmabuf.cpp",
    "linux-expsync\westeros-linux-expsync.cpp"
)

foreach ($file in $requiredFiles) {
    if (!(Test-Path $file)) {
        Write-ColorOutput Red "ERROR: Required file not found: $file"
        exit 1
    }
}
Write-ColorOutput Green "✓ All source files found"
Write-Output ""

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-ColorOutput Yellow "Cleaning build directory..."
    Remove-Item -Recurse -Force "build"
}

# Create build directory
if (!(Test-Path "build")) {
    Write-ColorOutput Green "Creating build directory..."
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Configure CMake
Write-ColorOutput Cyan "Configuring CMake..."
Set-Location "build"

$cmakeArgs = @(
    "..",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DENABLE_COVERAGE=$EnableCoverage",
    "-DWESTEROS_MAIN_SRC_ROOT=$WesterosMainSrc",
    "-DSIMPLESHELL_SRC_ROOT=$SimpleshellSrc",
    "-DSIMPLEBUFFER_SRC_ROOT=$SimplebufferSrc",
    "-DLINUX_DMABUF_SRC_ROOT=$LinuxDmabufSrc",
    "-DLINUX_EXPSYNC_SRC_ROOT=$LinuxExpsyncSrc",
    "-DBUILD_SIMPLESHELL_TESTS=ON",
    "-DBUILD_SIMPLEBUFFER_TESTS=ON",
    "-DBUILD_LINUX_DMABUF_TESTS=ON",
    "-DBUILD_LINUX_EXPSYNC_TESTS=ON"
)

cmake $cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-ColorOutput Red "CMake configuration failed!"
    Set-Location ..
    exit 1
}

# Build
Write-ColorOutput Cyan "Building..."
cmake --build . --config $BuildType --parallel $Jobs

if ($LASTEXITCODE -ne 0) {
    Write-ColorOutput Red "Build failed!"
    Set-Location ..
    exit 1
}

Set-Location ..

Write-Output ""
Write-ColorOutput Green "=========================================="
Write-ColorOutput Green "Build Successful!"
Write-ColorOutput Green "=========================================="
Write-Output ""

# Generate coverage if enabled
if ($Coverage) {
    Write-ColorOutput Cyan "=========================================="
    Write-ColorOutput Cyan "Generating Coverage Report..."
    Write-ColorOutput Cyan "=========================================="
    Write-Output ""
    
    Set-Location "build"
    cmake --build . --target coverage
    Set-Location ..
    
    Write-Output ""
    Write-ColorOutput Green "=========================================="
    Write-ColorOutput Green "Coverage Report Generated!"
    Write-ColorOutput Green "=========================================="
    Write-Output ""
    Write-ColorOutput Yellow "Coverage report available at:"
    Write-ColorOutput Cyan "  build\coverage_html\index.html"
    Write-Output ""
} else {
    Write-ColorOutput Yellow "Run tests with:"
    Write-Output "  cd build"
    Write-Output "  ctest --output-on-failure"
    Write-Output ""
}

Write-ColorOutput Green "Done!"
