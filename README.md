# Archura Game Engine

![License](https://img.shields.io/badge/license-MIT-blue.svg) ![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg) ![Status](https://img.shields.io/badge/status-Active%20Development-green.svg) ![C++](https://img.shields.io/badge/C++-17-blue.svg)

**Archura**, modern C++17 standartları ile geliştirilmiş, performans odaklı ve modüler bir FPS oyun motorudur. Düşük seviyeli sistem erişimi, veri odaklı tasarım (Data-Oriented Design) ve çok çekirdekli işlemci mimarilerinden tam yararlanmayı hedefleyen bir yapı üzerine kurulmuştur.

##  Temel Özellikler

### Çekirdek Sistemler
*   **Veri Odaklı ECS (Entity Component System)**: Cache-friendly (önbellek dostu) ve yüksek performanslı entity yönetimi.
*   **Job System (Multithreading)**: [JobSystem](cci:2://file:///c:/Users/4RCHURA/Desktop/Archura_game_engine/src/core/threading/JobSystem.h:12:0-41:1) mimarisi ile fizik, animasyon ve render hazırlık aşamalarında tam paralel işlem gücü.
*   **Özel Bellek Yönetimi**: Yığın parçalanmasını (fragmentation) önleyen [Stack](cci:1://file:///c:/Users/4RCHURA/Desktop/Archura_game_engine/src/core/memory/StackAllocator.h:9:4-9:37) ve [Pool](cci:1://file:///c:/Users/4RCHURA/Desktop/Archura_game_engine/src/core/memory/PoolAllocator.cpp:8:0-23:1) tahsisçileri (allocators).

### Grafik & Render
*   **OpenGL 3.3+**: Modern render pipeline.
*   **Aydınlatma**: Dinamik ışıklandırma ve gölge haritalama (Shadow Mapping).
*   **Varlık Yönetimi**: `ufbx` entegrasyonu ile OBJ ve FBX formatında model desteği.

### Oyun Sistemleri
*   **Fizik**: AABB (Axis-Aligned Bounding Box) tabanlı hızlı çarpışma tespiti.
*   **Ağ (Networking)**: TCP tabanlı entegre Host/Join multiplayer mimarisi.
*   **UI**: ImGui destekli, oyun içi ayarlanabilir Geliştirici Konsolu ve Editör araçları.
*   **Ses**: MCI tabanlı, genişletilebilir ses sistemi.

##  Kurulum ve Derleme

### Oyuncular İçin (Release)
En son kararlı sürümü oynamak için:
1.  **[StartGame_Release.bat](cci:7://file:///c:/Users/4RCHURA/Desktop/Archura_game_engine/StartGame_Release.bat:0:0-0:0)** dosyasını çalıştırın.
2.  Script, oyunun en güncel sürümünü otomatik olarak indirip başlatacaktır.

### Geliştiriciler İçin (Build)
Kaynak koddan derlemek için:

**Gereksinimler:**
*   Visual Studio 2019 veya üzeri (C++17 desteği ile)
*   CMake 3.15+
*   Git

**Adımlar:**
```bash
# Projeyi klonlayın
git clone [https://github.com/aybarsbarut/archura_game_engine.git](https://github.com/aybarsbarut/archura_game_engine.git)
cd archura_game_engine

# Derleme ve Başlatma
StartGame_Dev.bat
