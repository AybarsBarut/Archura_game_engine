# Archura Game Launcher
# Bu script:
# 1. GitHub'dan guncelleme var mi kontrol eder.
# 2. Varsa indirir ve oyunu yeniden derler.
# 3. Oyunu baslatir.

$ErrorActionPreference = "Stop"

Write-Host "=== Archura Engine Launcher ===" -ForegroundColor Cyan

# 1. Baslangic Kontrolu (Ilk Kurulum mu?)
$exePath = "build\bin\Release\ArchuraEngine.exe"
$needsBuild = $false

if (-not (Test-Path $exePath)) {
    Write-Host "Oyun dosyasi bulunamadi. Ilk kurulum yapiliyor..." -ForegroundColor Yellow
    $needsBuild = $true
}

# 2. Guncelleme Kontrolu
Write-Host "Guncellemeler kontrol ediliyor..."
try {
    git fetch origin
    
    # Yerel versiyonu oku
    if (Test-Path "version.txt") {
        $localVersion = Get-Content "version.txt" -Raw
        $localVersion = $localVersion.Trim()
    }
    else {
        $localVersion = "0.0.0"
    }

    # Uzak versiyonu oku (GitHub'dan)
    try {
        $remoteVersion = git show origin/main:version.txt
        if ($remoteVersion) {
            $remoteVersion = $remoteVersion.Trim()
        }
    }
    catch {
        $remoteVersion = $null
    }

    Write-Host "Yerel Versiyon: $localVersion" -ForegroundColor Gray
    Write-Host "Uzak Versiyon:  $(if ($remoteVersion) { $remoteVersion } else { 'Bilinmiyor' })" -ForegroundColor Gray

    $status = git status -uno
    $isBehind = $status -match "Your branch is behind"
    $versionMismatch = ($remoteVersion -ne $null) -and ($localVersion -ne $remoteVersion)

    if ($isBehind -or $versionMismatch) {
        Write-Host "Yeni guncelleme bulundu! Indiriliyor..." -ForegroundColor Yellow
        git pull
        $needsBuild = $true
    }
    else {
        Write-Host "Sistem guncel." -ForegroundColor Green
    }
}
catch {
    Write-Host "Guncelleme kontrolu sirasinda hata: $_" -ForegroundColor Red
    Write-Host "Internet baglantinizi kontrol edin." -ForegroundColor Red
    # Eger exe yoksa ve internet de yoksa, build denemeliyiz (belki kodlar vardir)
    if (-not (Test-Path $exePath)) {
        $needsBuild = $true
    }
}

# 3. Derleme (Gerekirse)
if ($needsBuild) {
    Write-Host "Derleme islemi baslatiliyor..." -ForegroundColor Yellow
    
    # Build klasoru yoksa olustur ve cmake configure yap
    if (!(Test-Path "build")) { 
        mkdir build 
        Set-Location build
        Write-Host "CMake konfigurasyonu yapiliyor..." -ForegroundColor Cyan
        cmake ..
        if ($LASTEXITCODE -ne 0) {
            Write-Host "CMake Konfigurasyon Hatasi!" -ForegroundColor Red
            Pause
            exit
        }
    }
    else {
        Set-Location build
    }

    Write-Host "Kodlar derleniyor (Release)..." -ForegroundColor Cyan
    cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Derleme Hatasi!" -ForegroundColor Red
        Pause
        exit
    }
    Set-Location ..
}

# 4. Baslatma
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
