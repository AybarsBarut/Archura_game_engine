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
│   ├── core/           # Motor çekirdeği
│   ├── rendering/      # Render sistemi
│   ├── ecs/            # Entity Component System
│   ├── input/          # Input yönetimi
│   ├── game/           # Oyun sistemleri
│   ├── editor/         # Editor UI
│   └── main.cpp        # Giriş noktası
├── external/           # Harici kütüphaneler
└── CMakeLists.txt      # Build konfigürasyonu
```

## Editor Kullanımı

Motor çalıştırıldığında ImGui tabanlı editor arayüzü açılır:

- **Scene Hierarchy**: Entity'leri görüntüle ve düzenle
- **Inspector**: Component özelliklerini değiştir
- **Viewport**: 3D sahneyi etkileşimli görüntüle
- **Asset Browser**: Proje dosyalarını yönet
- **Console**: Log mesajlarını takip et

### Editor Kısayolları

- `W/A/S/D`: Kamera hareketi
- `Q/E`: Yukarı/aşağı hareket
- `Mouse Sağ Tık + Hareket`: Kamera döndürme
- `Ctrl+S`: Sahneyi kaydet
- `Ctrl+O`: Sahne aç
- `F`: Seçili objeyi odakla

## Teknoloji Yığını

- **C++17**: Modern C++ özellikleri
- **OpenGL 3.3+**: Grafik API
- **GLFW**: Window ve input yönetimi
- **GLM**: Matematik kütüphanesi
- **Dear ImGui**: Editor UI
- **stb_image**: Image loading



## Lisans

MIT License - Detaylar için LICENSE dosyasına bakınız.

## Katkıda Bulunma

Pull request'ler memnuniyetle karşılanır! Büyük değişiklikler için lütfen önce bir issue açın.

## İletişim

- GitHub: [@archura](https://github.com/aybarsbarut)
- Email: barutaybarsfahri@gmail.com
