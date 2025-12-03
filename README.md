# Archura Game Engine


## Sistem Gereksinimleri

### Minimum
- **OS**: Windows 10 / Linux
- **GPU**: NVIDIA GTX 1050 / AMD RX 560
- **RAM**: 1GB
- **OpenGL**: 3.3+

## Kurulum

### Gereksinimler
- CMake 3.15+
- C++17 destekli compiler (MSVC 2019+, GCC 7+, Clang 5+)
- Git

### Build

```bash
# Repository'yi clone'la
git clone https://github.com/aybarsbarut/archura_game_engine.git
cd archura_game_engine

# Submodule'leri indir
git submodule update --init --recursive

# Build dizini oluştur
mkdir build
cd build

# CMake ile configure et
cmake ..

# Compile et
cmake --build . --config Release

# Çalıştır
./bin/ArchuraEngine
```

### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

## Proje Yapısı

```
archura_game_engine/
├── assets/              # Oyun asset'leri
│   ├── shaders/        # GLSL shader dosyaları
│   ├── textures/       # Texture dosyaları
│   └── models/         # 3D model dosyaları
├── src/
│   ├── core/           # Motor cekirdegi
│   ├── rendering/      # Render sistemi
│   ├── ecs/            # Entity Component System
│   ├── input/          # Input yonetimi
│   ├── game/           # Oyun sistemleri
│   ├── editor/         # Editor UI
│   └── main.cpp        # Giris noktasi
├── external/           # Harici kutuphaneler
└── CMakeLists.txt      # Build konfigurasyonu
```

## Editor Kullanimi

Motor calistirildiginda ImGui tabanli editor arayuzu acilir:

- **Scene Hierarchy**: Entity'leri goruntule ve duzenle
- **Inspector**: Component ozelliklerini degistir
- **Viewport**: 3D sahneyi etkilesimli goruntule
- **Asset Browser**: Proje dosyalarini yonet
- **Console**: Log mesajlarini takip et

### Editor Kisayollari

- `W/A/S/D`: Kamera hareketi
- `Q/E`: Yukari/asagi hareket
- `Mouse Sag Tik + Hareket`: Kamera dondurme
- `Ctrl+S`: Sahneyi kaydet
- `Ctrl+O`: Sahne ac
- `F`: Secili objeyi odakla

## Teknoloji Yigini

- **C++17**: Modern C++ ozellikleri
- **OpenGL 3.3+**: Grafik API
- **GLFW**: Window ve input yönetimi
- **GLM**: Matematik kutuphanesi
- **Dear ImGui**: Editor UI
- **stb_image**: Image loading



## Lisans

MIT License - Detaylar icin LICENSE dosyasina bakiniz.

## Katkida Bulunma

Pull request'ler memnuniyetle karsilanir! Buyuk degisiklikler icin lutfen once bir issue acin.

## Iletisim

- GitHub: [@archura](https://github.com/aybarsbarut)
- Email: barutaybarsfahri@gmail.com
