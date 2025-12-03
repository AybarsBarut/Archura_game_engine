# Archura Game Launcher
# Bu script:
# 1. GitHub'dan guncelleme var mi kontrol eder.
# 2. Varsa indirir ve oyunu yeniden derler.
# 3. Oyunu baslatir.

$ErrorActionPreference = "Stop"

Write-Host "=== Archura Engine Launcher ===" -ForegroundColor Cyan

# 1. Guncelleme Kontrolu
Write-Host "Guncellemeler kontrol ediliyor..."
try {
    git fetch origin
    $status = git status -uno
    
    if ($status -match "Your branch is behind") {
        Write-Host "Yeni guncelleme bulundu! Indiriliyor..." -ForegroundColor Yellow
        git pull
        $needsBuild = $true
    }
    else {
        Write-Host "Sistem guncel." -ForegroundColor Green
        $needsBuild = $false
    }
}
catch {
    Write-Host "Guncelleme kontrolu sirasinda hata (Internet yok mu?)" -ForegroundColor Red
    $needsBuild = $false
}

# 2. Derleme (Gerekirse)
if ($needsBuild) {
    Write-Host "Yeni kodlar derleniyor..." -ForegroundColor Yellow
    if (!(Test-Path "build")) { mkdir build }
    Set-Location build
    cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Derleme Hatasi!" -ForegroundColor Red
        Pause
        exit
    }
    Set-Location ..
}

# 3. Baslatma
$exePath = "build\bin\Release\ArchuraEngine.exe"
if (Test-Path $exePath) {
    Write-Host "Oyun baslatiliyor..." -ForegroundColor Green
    Start-Process $exePath
}
else {
    Write-Host "HATA: Oyun dosyasi ($exePath) bulunamadi!" -ForegroundColor Red
    Write-Host "Lutfen projeyi manuel derleyin."
    Pause
}
