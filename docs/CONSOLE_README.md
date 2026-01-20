 # Archura FPS Engine - Developer Console Sistem Kurulumu

##  İçerik Özeti

Bu paketin içeriğinde AAA kalitesinde FPS oyunları için eksiksiz bir geliştirici konsolu sistemi bulunmaktadır.

### Paket İçeriği

```
Core System
├── ConsoleCommand.h/cpp       - Komut ve CVAR tanımı
├── DeveloperConsole.h/cpp     - Ana konsol yönetim sistemi
├── ConsoleInterface.h/cpp     - Interaktif konsol arayüzü
└── ConsoleVariable.cpp        - Değişken tipi dönüşümleri

FPS-Specific Commands
├── FPSConsoleCommands.h/cpp   - FPS oyunlarına özel komutlar
└── Komut Kategorileri:
    ├── Rendering (r_)         - 20+ komut
    ├── Networking (net_)      - 15+ komut
    ├── Gameplay (m_, cl_)     - 20+ komut
    ├── Audio (snd_)           - 12+ komut
    ├── Cheats (sv_)           - 15+ komut [ADMIN]
    ├── System (sys_)          - 15+ komut
    └── Bots (bot_)            - 10+ komut

Documentation
├── CONSOLE_COMMANDS_REFERENCE.md      - Detaylı komut referansı
├── QUICK_CONSOLE_REFERENCE.md         - Hızlı rehber
├── ADMIN_GUIDE.md                     - Yönetici kılavuzu
├── CONSOLE_INTEGRATION_EXAMPLE.cpp    - Entegrasyon örneği
└── README.md                          - Bu dosya
```

---

## Özellikler

###  dev Mimarisı

- **Singleton Pattern** - Tüm oyundan konsole erişim
- **Event-driven** - Komut callback sistemi
- **Type-safe CVARs** - Float, Int, String, Bool desteği
- **Komut Geçmişi** - Terminal benzeri yönetim
- **Config Save/Load** - Ayarları dosyaya kaydet/yükle
- **Categorized Commands** - Prefix-based organizasyon

###  130+ Komut ve Değişken

- **23 Rendering Komutu** - Grafik, gölge, AA, post-processing
- **18 Network Komutu** - Ping, latency, bant genişliği
- **25 Gameplay Komutu** - Input, crosshair, HUD
- **12 Audio Komutu** - Ses seviyesi, 3D audio
- **15 Cheat Komutu** - GOD mode, noclip, bot yönetimi
- **15 System Komutu** - Profiling, logging, benchmark
- **10 Bot Komutu** - AI yönetimi

###  Güvenlik

- **sv_cheats Kontrolü** - Hile komutları admin-only
- **Permission System** - Komut izin kontrolleri
- **Ban Sistemi** - Oyuncu yönetimi
- **Logging** - Tüm komutlar loglanabilir

###  Profesyonel Araçlar

- **Profiling** - Performance analizi
- **Memory Monitoring** - RAM/VRAM kullanımı
- **Network Debug** - Lag simulation, packet loss
- **Hitbox Debugging** - Mesh visualization
- **Bot Management** - Yapay zeka kontrolleri

---

##  Hızlı Başlangıç

### 1. Temel Entegrasyon

```cpp
#include "core/DeveloperConsole.h"
#include "core/ConsoleInterface.h"
#include "game/FPSConsoleCommands.h"

// Ana oyun döngüsünde
int main() {
    auto& console = DeveloperConsole::GetInstance();
    
    // Tüm FPS komutlarını kaydet
    FPSConsoleCommands::RegisterAllCommands();
    
    // Konsolu ayrı thread'de çalıştır
    std::thread consoleThread([]() {
        ConsoleInterface::Run();
    });
    
    // Oyun döngüsü...
    MainGameLoop();
    
    consoleThread.join();
    return 0;
}
```

### 2. Değişken Okuması

```cpp
// Oyun kodunda
auto& console = DeveloperConsole::GetInstance();

// Float değişkeni oku
float fov = console.GetVariable("r_fov")->GetAsFloat();
camera.SetFOV(fov);

// Bool değişkeni oku
bool godMode = console.GetVariable("sv_cheats")->GetAsBool();
player.SetGodMode(godMode);

// String değişkeni oku
std::string crosshairColor = console.GetVariableValue("cl_crosshair_color");
```

### 3. Custom Komut Ekleme

```cpp
auto& console = DeveloperConsole::GetInstance();

// Yeni komut ekle
console.RegisterCommand(
    std::make_shared<ConsoleCommand>(
        "my_command",
        "Bu komutun açıklaması",
        [](const std::vector<std::string>& args) {
            std::cout << "Komut çalıştırıldı!\n";
            for (const auto& arg : args) {
                std::cout << "Arg: " << arg << "\n";
            }
        }
    )
);
```

### 4. Custom Değişken Ekleme

```cpp
console.RegisterVariable(
    std::make_shared<ConsoleVariable>(
        "my_var",
        "10",  // Default değer
        ConsoleVariable::Type::Float,
        "Bu değişkenin açıklaması"
    )
);

// Daha sonra...
float value = console.GetVariable("my_var")->GetAsFloat();
```

---

##  Dokümantasyon

###  Referans Dosyaları

1. **[CONSOLE_COMMANDS_REFERENCE.md](CONSOLE_COMMANDS_REFERENCE.md)**
   - 130+ komutun detaylı açıklaması
   - Kategorilere göre organize edilmiş
   - Varsayılan değerler ve örnekler

2. **[QUICK_CONSOLE_REFERENCE.md](QUICK_CONSOLE_REFERENCE.md)**
   - Hızlı komut referansı
   - Kullanım senaryoları
   - Sık kullanılan kombinasyonlar

3. **[ADMIN_GUIDE.md](ADMIN_GUIDE.md)**
   - Sunucu yönetimi komutları
   - Anti-cheat yönetimi
   - Performans optimizasyonu

4. **[CONSOLE_INTEGRATION_EXAMPLE.cpp](CONSOLE_INTEGRATION_EXAMPLE.cpp)**
   - Entegrasyon kodu örneği
   - Oyun motoru entegrasyon
   - Mimari diyagramı

---

##  Örnek Kullanımlar

### Senaryo 1: FPS Düşüşü Hata Ayıklama

```bash
# Konsolu aç ve sırasıyla çalıştır:

# 1. Geçerli durumu izle
r_fps_display 1
sys_cpu_usage 1
sys_gpu_usage 1

# 2. Grafikleri düşür
r_shadow_quality 1
r_texture_quality 2

# 3. Profilleme yap
profile_start rendering
(5 saniye bekle)
profile_stop
profile_results

# 4. Ağ sorunlarını test et
net_ping
net_latency 100

# 5. Ayarları kaydet
save_config debug_session.cfg
```

### Senaryo 2: Multiplayer Test

```bash
# Sunucu yöneticisi olarak:
sv_cheats 1
bot_add ct easy Bot1
bot_add t hard Bot2
bot_list

# Bağlantı testi
net_packetloss 5.0
net_latency 50

# Test sonrası
bot_removeall
sv_cheats 0
```

### Senaryo 3: Girdi Konfigürasyonu

```bash
# Profesyonel oyuncu setup'ı
m_sensitivity 1.2
m_rawmouse 1
m_accel 0

cl_crosshair_style 1
cl_crosshair_size 18
cl_crosshair_color 0 255 0

bind w +forward
bind mouse1 +attack
bind e +use

save_config my_config.cfg
```

---

##  Mimari Detaylar

### Sınıf Diyagramı

```
ConsoleCommand
  └─ name: string
  └─ description: string
  └─ callback: function
  └─ requiresCheats: bool
  └─ Execute(args)

ConsoleVariable
  └─ name: string
  └─ value: string
  └─ type: enum (Float, Int, String, Bool)
  └─ description: string
  └─ GetAsFloat() / GetAsInt() / GetAsBool()

DeveloperConsole (Singleton)
  ├─ m_commands: map<string, ConsoleCommand>
  ├─ m_variables: map<string, ConsoleVariable>
  ├─ m_commandHistory: vector<string>
  ├─ m_cheatsEnabled: bool
  └─ Public Methods:
      ├─ ExecuteCommand(line)
      ├─ RegisterCommand/Variable()
      ├─ GetVariable() / SetVariableValue()
      ├─ SaveConfig() / LoadConfig()
      └─ SetCheatsEnabled()

ConsoleInterface
  └─ Run() : void
  └─ ProcessInput() : bool
  └─ PrintWelcomeMessage() : void

FPSConsoleCommands (Static Registry)
  └─ RegisterAllCommands()
      ├─ RegisterRenderingCommands()
      ├─ RegisterNetworkingCommands()
      ├─ RegisterGameplayCommands()
      ├─ RegisterAudioCommands()
      ├─ RegisterCheatCommands()
      ├─ RegisterSystemCommands()
      └─ RegisterBotCommands()
```

### Komut Akışı

```
Input: "r_fov 100"
  ↓
ConsoleInterface::ProcessInput()
  ↓
DeveloperConsole::ExecuteCommand("r_fov 100")
  ↓
Tokenization: "r_fov", ["100"]
  ↓
CVAR lookup: r_fov found
  ↓
ConsoleVariable::SetValue("100")
  ↓
Game Code: console.GetVariable("r_fov")->GetAsFloat()
  ↓
Camera.SetFOV(100.0f)
  ↓
Output: r_fov = "100"
```

---

##  Yapılandırma

### Varsayılan Config Dosyası Format

```cfg
// Archura FPS Engine Configuration File

// === Rendering ===
r_fps_max "144"
r_vsync "1"
r_resolution "1920 1080"
r_fov "90"

// === Input ===
m_sensitivity "1.0"
m_rawmouse "1"

// === Audio ===
snd_master_volume "0.8"
snd_effects_volume "0.9"

// === Network ===
net_rate "128000"
net_cmdrate "60"

// === Gameplay ===
cl_crosshair_enabled "1"
hud_enabled "1"
```

### Config Dosyası Yükleme

```bash
# Konsol içinde
load_config myconfig.cfg

# Veya command line
game.exe -config myconfig.cfg

# Varsayılan ayarları sıfırla
reset_config
```

---

## ! Önemli Notlar

1. **sv_cheats Kontrolü**
   - Hile komutları `sv_cheats 1` olmadan çalışmaz
   - Canlı sunucularda asla açılmamalı
   - Test ortamları için tasarlanmış

2. **Komut Güvenliği**
   - `kick`, `ban`, `rcon_password` gibi komutlar admin-only olmalı
   - Tüm admin komutları loglanmalı
   - Anti-cheat ile entegre edilmeli

3. **Performans**
   - Profiling sırasında overlay'ler kapalı tutun
   - `sys_cpu_usage`, `sys_gpu_usage` küçük bir overhead ekler
   - Production'da logging seviyesi düşük tutun (log_level 1)

4. **Çok-Thread Entegrasyon**
   - DeveloperConsole singleton'dur ve thread-safe
   - Konsol input ayrı thread'de çalışabilir
   - Game loop'da variable değişiklikleri safe

---

##  Komut İstatistikleri

| Kategori | Komut Sayısı | Değişken Sayısı | Toplam |
|----------|--------------|-----------------|--------|
| Rendering | 6 | 17 | 23 |
| Networking | 6 | 12 | 18 |
| Gameplay | 7 | 18 | 25 |
| Audio | 4 | 8 | 12 |
| Cheats | 11 | 4 | 15 |
| System | 7 | 8 | 15 |
| Bots | 5 | 4 | 9 |
| General | 8 | - | 8 |
| **TOPLAM** | **54** | **71** | **125+** |

---

##  Güncelleme Geçmişi

### v1.0 (Ocak 2026)
-  Ana konsol sistemi
-  7 komut kategorisi
-  125+ komut ve değişken
-  Profiling ve debugging araçları
-  Tam dokümantasyon

### Planlanan Özellikler (v2.0)
- [ ] WebSocket konsol erişimi
- [ ] Steam Rich Presence entegrasyonu
- [ ] Performance telemetry
- [ ] Cloud config sync
- [ ] Mobile companion app

---

##  Destek ve Haberler

### Hata Raporlama

Console sisteminde bir hata bulursanız:
1. Konsol çıktısını kaydedin
2. Hangi komutu çalıştırdığınızı not edin
3. Tekrar üretme adımlarını yazın

### Yardım Alınması

- `help [command]` - Komut hakkında yardım
- `cmdlist` - Tüm komutları listele
- `cvarlist` - Tüm değişkenleri listele
- Dokümantasyon dosyalarını kontrol edin

---

##  Lisans

Archura FPS Engine - Developer Console System
Copyright 2026 - Tüm hakları saklıdır.

---

## Geliştirici Notları

```
Konsol Sistemi Tasarlanırken Göz Önüne Alınan Noktalar:

1. Kullanıcı Deneyimi
   - Basit ve intuitif arayüz
   - Anlaşılır komut adlandırması
   - Kapsamlı yardım sistemi

2. Performans
   - Minimal overhead
   - Efficient string parsing
   - Lazy loading konfigürasyonları

3. Genişletilebilirlik
   - Easy custom command ekleme
   - Plugin sistemi hazır
   - Third-party integration

4. Profesyonellik
   - AAA game engine standardları
   - Comprehensive error handling
   - Production-ready code
```

---

**Console System Version:** 1.0  
**Archura FPS Engine**  
**Release Date:** Ocak 2026  
**Documentation Version:** 1.0  

Herhangi bir soru için geliştirici dokümantasyonuna başvurun.
