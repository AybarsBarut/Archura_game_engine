# Archura FPS Engine - Admin/Developer Kılavuzu

##  Admin Komutları (Server Tarafı)

Bu komutlar **sadece sunucu yöneticileri** tarafından çalıştırılabilir.

---

## Oyun Yönetimi Komutları

### Oyuncu Kontrolü

```bash
# Oyuncu atmak (kick)
kick [player_name]
# Örnek: kick Hacker123

# Oyuncuyu kalıcı olarak banlamak
ban [player_name] [minutes]
# Örnek: ban Cheater 0 (sonsuza kadar)
# Örnek: ban Troll 60 (1 saat)

# Ban listesini göster
banlist

# Oyuncuyu unban etmek
unban [player_name]
```

### Sunucu Ayarları

```bash
# Sunucu adını ayarla
hostname [name]
# Örnek: hostname "Archura Competitive Server #1"

# Maksimum oyuncu sayısı
maxplayers [number]
# Örnek: maxplayers 32

# Sunucu şifresi ayarla
sv_password [password]
# Örnek: sv_password myserverpass123

# RCON şifresi (uzak konsol)
rcon_password [password]
# Örnek: rcon_password admin12345

# Sunucu mottosu
motd "Archura FPS Server"
```

---

## Hile Kontrolü (sv_cheats)

### Hile Modunu Yönetme

```bash
# Hile komutlarını etkinleştir (SADECE TEST SERVERLERİ İÇİN!)
sv_cheats 1

# Hile komutlarını devre dışı bırak (CANLI SUNUCULAR)
sv_cheats 0
```

### Hile Modunda Yapılabilecekler

```bash
# Ölümsüzlük modu
god 1

# Duvarlardan geçme (harita tasarımı test etme)
noclip 1

# Yapay zeka (AI) testi
sv_cheats 1
bot_add ct easy TestBot
bot_add t hard TestBot2

# Hitbox debugging (client-side hit detection testi)
sv_hitbox_debug 1

# Düşman görüşü test etme
notarget 1

# Fizik motoru test etme
sv_gravity 0.5  # Ay gibi
sv_gravity 2.0  # Ağır gravite
sv_gravity 1.0  # Normal
```

---

## Tur ve Maç Yönetimi

```bash
# Sunucu süresi limiti (dakika)
server_time_limit 45
# Örnek: 45 dakikalık maç süresi

# Tur sayısı limiti
server_round_limit 30
# Örnek: Tur bazlı oyunda 30 tur

# Skoru sıfırla
reset_scores

# Oyunu durdur (pause)
sv_pause 1

# Oyunu devam ettir
sv_pause 0

# Harita başa dön
mp_restartgame 1
```

---

## Harita ve Mod Yönetimi

```bash
# Harita yükle
map [map_name]
# Örnek: map de_mirage

# Sunucu rotasyon haritalarını ayarla
mp_maplist de_mirage de_inferno de_cache de_dust2

# Sonraki harita ayarla
nextmap de_train

# Harita seçim modu
mp_random_mapcycle 1  # Rastgele sırada
mp_random_mapcycle 0  # Sırayla

# Oyun modu ayarla
game_mode competitive  # Rekabetçi (5v5)
game_mode deathmatch   # Deathmatch
game_mode casual       # Casual (rahat oyun)
```

---

## Kayıt (Logging) ve Veri Tabanı

```bash
# Log dosyasını etkinleştir
log_enable 1

# Log seviyesi (detay)
# 0 = Sadece kritik hatalar
# 1 = Hatalar
# 2 = Uyarılar (default)
# 3 = Bilgiler
# 4 = Debug (çok detaylı)
log_level 3

# Server log'u
log_server_events 1

# Oyuncu aktivitesini logla
log_player_activity 1

# Cheat/Admin aktivitesini logla
log_admin_actions 1

# Anti-cheat raporlarını logla
log_anticheat_reports 1

# Log dosyası adını değiştir
log_file "server_logs/myserver_2026_01.log"
```

---

## Anti-Cheat Yönetimi

```bash
# Anti-cheat sistemini etkinleştir
sv_anticheat_enabled 1

# Şüpheli oyuncuları otomatik olarak kick et
sv_anticheat_autoban 1

# Anti-cheat hassasiyeti (0-100)
# 0 = Çok toleranslı (yanlış pozitif az)
# 50 = Orta (default)
# 100 = Sıkı (yanlış pozitif çok)
sv_anticheat_sensitivity 65

# Şüpheli oyuncu raporu
report_player [player_name] [reason]
# Örnek: report_player Cheater "wallhack"

# Oyuncuyu 24 saat geçici olarak banla
temp_ban [player_name]

# Permanent ban (UID-based)
permanent_ban [player_uid]

# Ban geçmişi
show_ban_history [player_name]
```

---

## Performans Yönetimi

```bash
# Sunucu FPS sınırı
sv_fps_max 128

# İstemci güncelleme hızı
sv_updaterate 64

# İstemci komut hızı
sv_cmdrate 128

# Network rate
sv_rate 128000

# Bant genişliği limitesi
sv_bandwidth_limit 10000000  # 10 Mbps

# Interpolation ayarları
sv_interpolate_enabled 1
sv_interpolate_time 0.5

# Lag compensation
sv_lag_compensate 1
sv_lag_compensate_enabled 1

# CPU kullanımı monitörü
sys_cpu_monitor 1

# GPU kullanımı monitörü
sys_gpu_monitor 1
```

---

## Kalite Kontrol (QA Testing)

```bash
# Crash test (sunucuyu intentionally crash et - TEST SADECE)
# UYARI: Sadece offline test sunucularında!
crash_test 1

# Memory leak testi
memory_leak_test 1

# Stress test (maksimum yük)
stress_test_players 128

# Network stress
network_stress_test 1
network_packet_loss 10  # %10 paket kaybı simüle et

# Hitbox validation
hitbox_validation_mode 1

# Prediction error detection
prediction_error_detection 1
```

---

## Güvenlik Yönetimi

```bash
# Admin hesabı oluştur
create_admin [username] [password] [permissions]
# Örnek: create_admin AdminUser password123 full

# Admin hesabını sil
delete_admin [username]

# Admin listesini göster
admin_list

# Rcon (Remote Console) etkinleştir
rcon_enabled 1

# Rcon adresini kısıtla (sadece belirli IP'lerden bağlantı)
rcon_allow_from [IP_address]
# Örnek: rcon_allow_from 192.168.1.100

# Firewall kuralları
sv_firewall_enabled 1

# DDoS koruması
sv_ddos_protection 1

# Rate limiting
sv_rate_limit_enabled 1
sv_rate_limit_connections_per_second 100
```

---

## Ağ Ayarları

```bash
# Server IP ve port
server_ip 0.0.0.0
server_port 27015

# NAT/UPnP desteği
sv_upnp_enabled 1

# Sunucu bağlantı noktası aralığı (demo oyunlar için)
demo_port_start 27020
demo_port_end 27030

# Netcode versiyonu
netcode_version 2

# Network iyileştirmesi
sv_network_optimization 1
```

---

## İstatistik ve Raporlama

```bash
# Sunucu istatistiklerini göster
server_stats

# Oyuncu istatistiklerini kaydet
save_player_stats [filename]

# Maç raporunu oluştur
generate_match_report

# Sunucu performans raporu
generate_performance_report

# Yönetim panosu
admin_dashboard

# Raporları CSV'ye aktar
export_stats_csv [filename]
```

---

## Yönetim Komutları Tablosu

| Komut | Açıklama | Örnek |
|-------|----------|-------|
| `sv_cheats` | Hile modu kontrol | `sv_cheats 1` |
| `god` | Ölümsüzlük | `god 1` |
| `noclip` | Duvardan geç | `noclip 1` |
| `bot_add` | Bot ekle | `bot_add ct easy` |
| `bot_removeall` | Tüm botları sil | `bot_removeall` |
| `kick` | Oyuncu at | `kick PlayerName` |
| `ban` | Oyuncu banla | `ban PlayerName 60` |
| `map` | Harita yükle | `map spawn` |
| `hostname` | Sunucu adı | `hostname "MyServer"` |
| `maxplayers` | Oyuncu limiti | `maxplayers 32` |
| `sv_password` | Sunucu şifresi | `sv_password pass123` |
| `server_time_limit` | Maç süresi | `server_time_limit 45` |
| `reset_scores` | Skorları sıfırla | `reset_scores` |
| `log_enable` | Loglama | `log_enable 1` |
| `log_level` | Log detayı | `log_level 3` |
| `sv_anticheat_enabled` | Anti-cheat | `sv_anticheat_enabled 1` |
| `sv_fps_max` | Sunucu FPS | `sv_fps_max 128` |
| `sv_rate` | Network hızı | `sv_rate 128000` |

---

##  Sunucu Konfigürasyonu Örneği

### İdeal Competitive Setup

```cfg
// === Network ===
sv_fps_max 128
sv_updaterate 64
sv_cmdrate 128
sv_rate 128000

// === Gameplay ===
game_mode competitive
maxplayers 10
server_time_limit 45
server_round_limit 30

// === Security ===
sv_password ""
rcon_password "admin_secret_123"
sv_anticheat_enabled 1
sv_anticheat_sensitivity 65

// === Logging ===
log_enable 1
log_level 2
log_admin_actions 1
log_anticheat_reports 1

// === Performance ===
sv_network_optimization 1
sv_lag_compensate 1

// === Server Info ===
hostname "Archura Competitive Server"
motd "Welcome to competitive FPS gaming"
```

### Casual Server Setup

```cfg
// === Network ===
sv_fps_max 64
sv_updaterate 32
sv_cmdrate 64
sv_rate 80000

// === Gameplay ===
game_mode casual
maxplayers 32
server_time_limit 30

// === Security ===
sv_password ""
sv_anticheat_enabled 1
sv_anticheat_sensitivity 45

// === Logging ===
log_enable 1
log_level 2

// === Server Info ===
hostname "Archura Casual Server"
motd "Fun games for everyone"
```

---

##  Önemli Notlar

1. **Hile Modunu Açma Riski:** `sv_cheats 1` canlı sunucularda asla açılmamalı. Test sunucuları için kullanın.

2. **Bant Genişliği:** Fazla oyuncuyla sunucuyu çalıştırmak bant genişliğini tüketir. İyi dimensiyon seçin.

3. **Loglar:** Loglama sunucunun disk alanını tüketebilir. Dosya boyutunu kontrol edin.

4. **Anti-Cheat Hassasiyeti:** Çok yüksek ayar = yanlış pozitif, Çok düşük = hileciler kaçar. 65 optimal başlangıç noktasıdır.

5. **Sunucu Maintenance:** Periyodik yedekleme ve güncellemeler yapın.

---

##  Hızlı Admin Komutları Referansı

```bash
# En çok kullanılan komutlar
sv_cheats 1                    # Hile modu (TEST ONLY!)
god 1                          # Ölümsüzlük
noclip 1                       # Duvardan geç
bot_add ct easy MyBot          # Bot ekle
bot_removeall                  # Botları sil
kick Hacker123                 # Oyuncu at
ban Cheater 0                  # Oyuncu banla
map de_mirage                  # Harita yükle
save_config default.cfg        # Ayarları kaydet
log_level 3                    # Debug loglama
sv_anticheat_enabled 1         # Anti-cheat aç
server_stats                   # İstatistik
```

---

**Admin Kılavuzu Versiyonu:** 1.0  
**Archura FPS Engine**  
**Son Güncelleme:** Ocak 2026
