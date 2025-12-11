# Archura Game Engine

Modern, C++17 tabanlı ve modüler bir oyun motoru.

## Özellikler

*   **Grafik**: OpenGL 3.3+, Dinamik Işıklandırma, Gölge ve Texture desteği.
*   **Fizik**: Özel çarpışma algılama ve AABB sistemi.
*   **Ses**: MCI tabanlı ses sistemi (OpenAL hazırlığı ile).
*   **Networking**: Entegre Host/Join multiplayer desteği (TCP).
*   **UI**: ImGui tabanlı Editör ve Oyun İçi Menüler.
    *   *Ayarlar*: Çözünürlük, Hassasiyet, Keybind Görüntüleme.
*   **Modelleme**: OBJ ve FBX format desteği (ufbx & özel parser).
*   **Scripting**: Temel komut sistemi.

## Sistem Gereksinimleri

*   **OS**: Windows 10/11 (Linux desteği geliştiriliyor)
*   **GPU**: OpenGL 3.3 destekli ekran kartı
*   **RAM**: 4GB+
*   **Disk**: 500MB+

## Kurulum ve Çalıştırma

### Oyuncular İçin (Release)
1.  **`StartGame_Release.bat`** dosyasını çalıştırın.
    *   Bu script otomatik olarak GitHub'dan en güncel kararlı sürümü indirir ve açar.

### Geliştiriciler İçin (Local Dev)
1.  Projeyi klonlayın:
    ```bash
    git clone https://github.com/aybarsbarut/archura_game_engine.git
    cd archura_game_engine
    ```
2.  **`StartGame_Dev.bat`** dosyasını çalıştırın.
    *   CMake kullanarak projeyi yerel makinenizde derler ve başlatır.
    *   *Gereksinimler*: Visual Studio 2019+, CMake 3.15+, Git.

## Kontroller

### Oyun İçi
*   **W / A / S / D**: Hareket
*   **SPACE**: Zıplama
*   **SHIFT**: Koşma
*   **Sol Tık**: Ateş Etme
*   **R**: Şarjör Değiştirme
*   **1 / 2 / 3**: Silah Değiştirme (Tüfek, Tabanca, Bıçak)
*   **ESC**: Pause Menüsü (Ayarlar, Çıkış)
*   **TAB**: Geliştirici Modu (Mouse Kilidini Açar/Kapatır)

### Editör (Geliştirici Modu)
*   **Sol Tık + Mouse**: Kamera Yönlendirme
*   **Q / E**: Yükselme / Alçalma
*   **Ctrl+S**: Sahneyi Kaydet

## Proje Yapısı

```
archura_game_engine/
├── bin/Release_Dist/    # Dağıtıma hazır oyun dosyaları
├── src/                 # Kaynak kodlar (Core, Game, Rendering, Network...)
├── assets/              # Texture, Model ve Shader dosyaları
├── external/            # 3. Parti kütüphaneler (GLFW, GLM, ImGui...)
└── StartGame_*.bat      # Başlatıcı scriptleri
```

## Lisans

MIT License - Detaylar için `LICENSE` dosyasına bakınız.

## İletişim

*   **GitHub**: [@aybarsbarut](https://github.com/aybarsbarut)
