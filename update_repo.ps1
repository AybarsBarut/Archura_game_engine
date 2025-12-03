# Git Update Script
# Bu script degisiklikleri algilar, otomatik ekler ve GitHub'a yukler.

$status = git status --porcelain
if ([string]::IsNullOrWhiteSpace($status)) {
    Write-Host "Hicbir degisiklik yok. Her sey guncel." -ForegroundColor Green
    exit
}

Write-Host "=== Degisiklikler Algilandi ===" -ForegroundColor Yellow
git status
Write-Host "===============================" -ForegroundColor Yellow

$confirmation = Read-Host "Tum degisiklikleri ekleyip yuklemek istiyor musun? (E/H)"
if ($confirmation -ne 'E' -and $confirmation -ne 'e') {
    Write-Host "Islem iptal edildi." -ForegroundColor Red
    exit
}

# Degisiklikleri ekle
Write-Host "Dosyalar ekleniyor..." -ForegroundColor Cyan
git add .

# Commit mesaji al
$commitMsg = Read-Host "Commit mesaji girin (Ornek: Bug fix, Yeni ozellik)"
if ([string]::IsNullOrWhiteSpace($commitMsg)) {
    $commitMsg = "Otomatik guncelleme: " + (Get-Date).ToString("yyyy-MM-dd HH:mm")
}

# Commit ve Push
Write-Host "Commit yapiliyor..." -ForegroundColor Cyan
git commit -m "$commitMsg"

Write-Host "GitHub'a yukleniyor..." -ForegroundColor Cyan
git push

if ($?) {
    Write-Host "Basariyla yuklendi!" -ForegroundColor Green
} else {
    Write-Host "Yukleme sirasinda bir hata olustu." -ForegroundColor Red
}

Pause
