# GLFW Build Workaround

## Problem
GLFW kaynak kodundan derlenirken Windows SDK header'ları ile APIENTRY çakışması oluyor.

## Çözümler

### Çözüm 1: Mevcut Executable Kullan (ÖNERİLEN) ⭐
Zaten çalışan bir executable var: `bin/Release_Dist/ArchuraEngine.exe`

**Kullanım:**
```batch
QuickRun.bat
```

### Çözüm 2: Pre-built GLFW Kullan 🔧

1. **GLFW Pre-built Binary İndir:**
   - https://www.glfw.org/download.html
   - Windows pre-compiled binaries (64-bit) indir

2. **Klasör Yapısı Oluştur:**
   ```
   external/
   └── glfw-prebuilt/
       ├── include/
       │   └── GLFW/
       │       └── glfw3.h
       └── lib/
           └── glfw3.lib
   ```

3. **CMake ile Derle:**
   ```batch
   cmake -S . -B build -DUSE_PREBUILT_GLFW=ON
   cmake --build build --config Release
   ```

### Çözüm 3: Kaynak Koddan Derle (Mevcut)
GLFW'yi kaynak koddan derler ama Windows header çakışmaları olabilir.

```batch
cmake -S . -B build
cmake --build build --config Release
```

## Önerilen Geliştirme Akışı

1. **Kod değişikliği yap**
2. **QuickRun.bat ile test et** (mevcut executable)
3. **Önemli değişiklikler için yeniden derle** (pre-built GLFW ile)

## Not
Engine çalışıyor durumda. GLFW sorunu sadece yeniden derleme sırasında ortaya çıkıyor.
