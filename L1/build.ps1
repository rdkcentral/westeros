# Build script for L1 Westeros Components (Windows PowerShell)
# Supports flexible building of individual components or all components

param(
    [switch]$Help,
    [switch]$Clean,
    [switch]$Release,
    [switch]$NoTests,
    [switch]$Coverage,
    [switch]$All,
    [string[]]$Only = @(),
    [int]$Jobs = $env:NUMBER_OF_PROCESSORS
)

# Colors
$ColorCyan = "Cyan"
$ColorGreen = "Green"
$ColorYellow = "Yellow"
$ColorRed = "Red"
$ColorWhite = "White"

function Show-Help {
    Write-Host "`nL1 Westeros Components Build Script" -ForegroundColor $ColorCyan
    Write-Host ""
    Write-Host "Usage: .\build.ps1 [OPTIONS]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Help              Show this help message"
    Write-Host "  -Clean             Clean build directory before building"
    Write-Host "  -Release           Build in Release mode (default: Debug)"
    Write-Host "  -NoTests           Disable building tests"
    Write-Host "  -Coverage          Enable code coverage"
    Write-Host "  -All               Build all components (default)"
    Write-Host "  -Only COMPONENT    Build only specific component(s)"
    Write-Host "                     Valid: simpleshell, simplebuffer,"
    Write-Host "                            linux-dmabuf, linux-explicit-sync"
    Write-Host "  -Jobs N            Number of parallel jobs (default: CPU cores)"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build.ps1                              # Build all components"
    Write-Host "  .\build.ps1 -Release                     # Build all in Release mode"
    Write-Host "  .\build.ps1 -Only simpleshell            # Build only simpleshell"
    Write-Host "  .\build.ps1 -Only simpleshell,linux-dmabuf  # Build multiple components"
    Write-Host "  .\build.ps1 -Clean -Release -Coverage    # Clean, Release, with coverage"
    exit 0
}

if ($Help) {
    Show-Help
}

# Configuration
$BuildDir = "build"
$BuildType = if ($Release) { "Release" } else { "Debug" }
$BuildTests = -not $NoTests
$EnableCoverage = $Coverage
$BuildAll = ($Only.Count -eq 0) -or $All
$Components = $Only

# Print configuration
Write-Host "`n╔════════════════════════════════════════════════════════════════╗" -ForegroundColor $ColorCyan
Write-Host "║        L1 Westeros Components - Build Configuration            ║" -ForegroundColor $ColorCyan
Write-Host "╚════════════════════════════════════════════════════════════════╝" -ForegroundColor $ColorCyan
Write-Host "Build Directory: $BuildDir" -ForegroundColor $ColorWhite
Write-Host "Build Type:      $BuildType" -ForegroundColor $ColorWhite
Write-Host "Build Tests:     $BuildTests" -ForegroundColor $ColorWhite
Write-Host "Coverage:        $EnableCoverage" -ForegroundColor $ColorWhite
Write-Host "Parallel Jobs:   $Jobs" -ForegroundColor $ColorWhite

if ($BuildAll) {
    Write-Host "Components:      All" -ForegroundColor $ColorWhite
} else {
    Write-Host "Components:      $($Components -join ', ')" -ForegroundColor $ColorWhite
}
Write-Host ""

# Clean if requested
if ($Clean) {
    Write-Host "[1/3] Cleaning build directory..." -ForegroundColor $ColorYellow
    if (Test-Path $BuildDir) {
        Remove-Item -Path $BuildDir -Recurse -Force
    }
    Write-Host "✓ Build directory cleaned" -ForegroundColor $ColorGreen
    Write-Host ""
}

# Create build directory
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

# Configure CMake
Write-Host "[2/3] Configuring with CMake..." -ForegroundColor $ColorYellow
Set-Location $BuildDir

$CMakeArgs = @(
    "..",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DBUILD_TESTS=$BuildTests",
    "-DENABLE_COVERAGE=$EnableCoverage"
)

if ($BuildAll) {
    $CMakeArgs += "-DBUILD_ALL_COMPONENTS=ON"
} else {
    $CMakeArgs += "-DBUILD_ALL_COMPONENTS=OFF"
    
    foreach ($comp in $Components) {
        $CompUpper = $comp.ToUpper() -replace '-', '_'
        $CMakeArgs += "-DBUILD_${CompUpper}=ON"
    }
}

Write-Host "Running: cmake $($CMakeArgs -join ' ')" -ForegroundColor $ColorCyan
& cmake $CMakeArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ CMake configuration successful" -ForegroundColor $ColorGreen
} else {
    Write-Host "✗ CMake configuration failed" -ForegroundColor $ColorRed
    Set-Location ..
    exit 1
}
Write-Host ""

# Build
Write-Host "[3/3] Building..." -ForegroundColor $ColorYellow
& cmake --build . --config $BuildType -- /m:$Jobs

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Build successful" -ForegroundColor $ColorGreen
} else {
    Write-Host "✗ Build failed" -ForegroundColor $ColorRed
    Set-Location ..
    exit 1
}
Write-Host ""

# Run tests if enabled
if ($BuildTests) {
    Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
    Write-Host "Running L1 Tests..." -ForegroundColor $ColorYellow
    Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
    Write-Host ""
    
    # Run tests with verbose output to show test results
    & ctest -C $BuildType --verbose --output-on-failure
    $TestResult = $LASTEXITCODE
    
    Write-Host ""
    Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
    Write-Host "Test Results Summary:" -ForegroundColor $ColorYellow
    Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
    
    # Show test summary
    & ctest -C $BuildType --quiet
    
    Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
    if ($TestResult -eq 0) {
        Write-Host "✓ All L1 tests passed successfully!" -ForegroundColor $ColorGreen
    } else {
        Write-Host "✗ Some L1 tests failed - see details above" -ForegroundColor $ColorRed
        Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
        Set-Location ..
        exit 1
    }
    Write-Host "═══════════════════════════════════════════════════════════" -ForegroundColor $ColorCyan
    Write-Host ""
}

# Generate coverage if enabled
if ($EnableCoverage) {
    Write-Host "Generating coverage report..." -ForegroundColor $ColorYellow
    & cmake --build . --target coverage
    
    if (Test-Path "coverage_html") {
        Write-Host "✓ Coverage report generated: $BuildDir\coverage_html\index.html" -ForegroundColor $ColorGreen
    }
    Write-Host ""
}

Set-Location ..

Write-Host "╔════════════════════════════════════════════════════════════════╗" -ForegroundColor $ColorGreen
Write-Host "║                   BUILD COMPLETED SUCCESSFULLY!                 ║" -ForegroundColor $ColorGreen
Write-Host "╚════════════════════════════════════════════════════════════════╝" -ForegroundColor $ColorGreen
Write-Host ""
Write-Host "Build artifacts are in: $BuildDir" -ForegroundColor $ColorCyan
