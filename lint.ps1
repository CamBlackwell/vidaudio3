# lint.ps1 - Format and lint C++ files
# Usage: ./lint.ps1 [check|fix]

param(
    [string]$Action = "check"  # Default action is check
)

$ErrorActionPreference = "Stop"
$ClangFormatPath = "C:\Program Files (x86)\LLVM\bin\clang-format.exe"
$CppCheckPath = "cppcheck"  # Assuming cppcheck is in PATH

# Only include these specific files in the project root (not in build or vcpkg)
$SourceFiles = @(
    "main.cpp",
    "sawtooth.cpp", 
    "sawtooth.h",
    "infotoaudio.cpp", 
    "infotoaudio.h",
    "webcam_info.cpp", 
    "webcam_info.h",
    "scales.h"
)

$FormatConfigFile = ".clang-format"

function Test-CommandExists {
    param($Command)
    return (Get-Command $Command -ErrorAction SilentlyContinue)
}

# Print header
Write-Host "========================================="
Write-Host "          C++ Lint & Format Tool         "
Write-Host "========================================="
Write-Host

# Check if tools exist
$clangFormatExists = Test-CommandExists $ClangFormatPath
$cppCheckExists = Test-CommandExists $CppCheckPath

if (-not $clangFormatExists) {
    Write-Host "❌ clang-format not found at $ClangFormatPath" -ForegroundColor Red
}

if (-not $cppCheckExists) {
    Write-Host "❌ cppcheck not found. Install it or add to PATH" -ForegroundColor Red
}

if (-not $clangFormatExists -and -not $cppCheckExists) {
    Write-Host "No linting tools available. Please install at least one." -ForegroundColor Red
    exit 1
}

# Create format config if it doesn't exist
if (-not (Test-Path $FormatConfigFile)) {
    Write-Host "Creating default .clang-format file..." -ForegroundColor Yellow
    @"
---
Language: Cpp
BasedOnStyle: Google
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AlignConsecutiveAssignments: false
AlignTrailingComments: true
AllowShortFunctionsOnASingleLine: Inline
AllowShortIfStatementsOnASingleLine: WithoutElse
AllowShortLoopsOnASingleLine: true
BreakBeforeBraces: Attach
IndentCaseLabels: true
SpaceAfterCStyleCast: false
SpaceBeforeParens: ControlStatements
"@ | Out-File -FilePath $FormatConfigFile -Encoding utf8
}

# Run linting based on action
if ($Action -eq "check") {
    # Format checking mode
    Write-Host "Checking format on $($SourceFiles.Count) files..." -ForegroundColor Cyan
    $formatIssues = 0
    
    if ($clangFormatExists) {
        foreach ($file in $SourceFiles) {
            if (Test-Path $file) {
                $output = & $ClangFormatPath --style=file --dry-run --Werror $file 2>&1
                if ($LASTEXITCODE -ne 0) {
                    Write-Host "❌ Format issues in $file" -ForegroundColor Red
                    $formatIssues++
                } else {
                    Write-Host "✅ $file is properly formatted" -ForegroundColor Green
                }
            } else {
                Write-Host "⚠️ File not found: $file" -ForegroundColor Yellow
            }
        }
        
        if ($formatIssues -gt 0) {
            Write-Host "`n❌ Found format issues in $formatIssues files. Run './lint.ps1 fix' to fix them." -ForegroundColor Red
        } else {
            Write-Host "`n✅ All files are properly formatted!" -ForegroundColor Green
        }
    }
    
    # Static analysis
    if ($cppCheckExists) {
        Write-Host "`nRunning static analysis with cppcheck..." -ForegroundColor Cyan
        # Only check specific files, not recursively
        & $CppCheckPath --enable=all --std=c++14 --suppress=missingIncludeSystem $SourceFiles 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ No issues found by cppcheck!" -ForegroundColor Green
        } else {
            Write-Host "❌ cppcheck found issues. See above for details." -ForegroundColor Red
        }
    }
} 
elseif ($Action -eq "fix") {
    # Format fixing mode
    if ($clangFormatExists) {
        Write-Host "Fixing format on $($SourceFiles.Count) files..." -ForegroundColor Cyan
        foreach ($file in $SourceFiles) {
            if (Test-Path $file) {
                & $ClangFormatPath -i --style=file $file
                Write-Host "✅ Formatted $file" -ForegroundColor Green
            } else {
                Write-Host "⚠️ File not found: $file" -ForegroundColor Yellow
            }
        }
        Write-Host "`n✅ All files have been formatted!" -ForegroundColor Green
    }
    
    # Static analysis (just report, can't auto-fix)
    if ($cppCheckExists) {
        Write-Host "`nRunning static analysis with cppcheck..." -ForegroundColor Cyan
        & $CppCheckPath --enable=all --std=c++14 --suppress=missingIncludeSystem $SourceFiles 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ No issues found by cppcheck!" -ForegroundColor Green
        } else {
            Write-Host "⚠️ cppcheck found issues. See above for details." -ForegroundColor Yellow
            Write-Host "   These need to be fixed manually." -ForegroundColor Yellow
        }
    }
} 
else {
    Write-Host "Unknown action: $Action" -ForegroundColor Red
    Write-Host "Usage: ./lint.ps1 [check|fix]" -ForegroundColor Yellow
}

Write-Host "`nLinting complete!" -ForegroundColor Cyan