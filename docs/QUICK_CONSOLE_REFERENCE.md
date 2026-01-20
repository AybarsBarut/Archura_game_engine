# Archura FPS Engine - Konsol Komutları Hızlı Referans

##  Hızlı Başlangıç

```bash
# Konsolu aç
--console

# Temel yardım
help
help r_resolution
help m_sensitivity

# Komutu çalıştır
r_resolution 2560 1440
m_sensitivity 1.5
god 1 (sv_cheats 1 gerekli)
```

---

## 🎮 KATEGORI 1: RENDERING KOMUTLARI (r_ Prefix)

### Kritik Rendering Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `r_stats` | Render istatistiklerini göster | `r_stats` |
| `r_reload_shaders` | Shader'ları yeniden yükle | `r_reload_shaders` |
| `r_texture_reload` | Dokular'ı yeniden yükle | `r_texture_reload` |
| `r_dump_statistics` | İstatistikleri dosyaya kaydet | `r_dump_statistics render_stats.txt` |
| `r_clear_cache` | Cache'i temizle | `r_clear_cache` |
| `r_list_textures` | Yüklü dokuları listele | `r_list_textures` |

### Rendering Değişkenleri (CVARs)

```
r_fps_max [value]              # Maksimum FPS (default: 144)
r_fps_display [0/1]            # FPS göster (default: 0)
r_frametime_display [0/1]      # Frame zamanını göster (default: 0)
r_vsync [0/1]                  # V-Sync (default: 1)
r_resolution [w h]             # Çözünürlük (default: 1920 1080)
r_fullscreen [0/1]             # Tam ekran (default: 1)
r_fov [degrees]                # Alan görüşü (default: 90)
r_texture_quality [0-4]        # Doku kalitesi (default: 3)
r_shadow_quality [0-4]         # Gölge kalitesi (default: 2)
r_shadow_distance [meters]     # Gölge mesafe (default: 1000)
r_draw_distance [meters]       # Render mesafe (default: 3000)
r_aa_mode [0-3]                # Anti-aliasing: 0=Off, 1=FXAA, 2=SMAA, 3=TAA (default: 1)
r_bloom_enabled [0/1]          # Bloom efekti (default: 1)
r_bloom_intensity [0-2]        # Bloom şiddeti (default: 1.0)
r_gamma [0.5-2.5]              # Parlaklık (default: 1.0)
r_motion_blur [0/1]            # Motion blur (default: 0)
r_ao_enabled [0/1]             # Ambient Occlusion (default: 1)
r_model_quality [0-4]          # Model detayı (default: 3)
r_lod_bias [-2-2]              # LOD ayarı (default: 0.0)
```

### Örnek Render Optimizasyon

```
# Düşük Kalite (Notebook)
r_texture_quality 1
r_shadow_quality 0
r_draw_distance 1500
r_aa_mode 1
r_bloom_enabled 0

# Yüksek Kalite (High-End PC)
r_texture_quality 4
r_shadow_quality 4
r_shadow_distance 2000
r_draw_distance 5000
r_aa_mode 3
r_bloom_enabled 1
r_bloom_intensity 1.5
```

---

##  KATEGORI 2: NETWORK KOMUTLARI (net_, cl_ Prefix)

### Network Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `net_ping` | Ping göster | `net_ping` |
| `net_stats` | Ağ istatistiklerini göster | `net_stats` |
| `connect [IP:Port]` | Sunucuya bağlan | `connect 192.168.1.1:27015` |
| `disconnect` | Bağlantıyı kes | `disconnect` |
| `status` | Bağlantı durumunu göster | `status` |
| `net_graph` | Ağ grafiği göster/gizle | `net_graph` |

### Network Değişkenleri (CVARs)

```
net_rate [bytes/sec]           # Veri hızı (default: 128000)
net_cmdrate [packets/sec]      # Komut hızı (default: 60)
net_updaterate [packets/sec]   # Güncelleme hızı (default: 32)
net_interpolate [0/1]          # İnterpolasyon (default: 1)
net_lerp [seconds]             # İnterpolasyon miktarı (default: 0.015)
net_latency [ms]               # Test latency (default: 0) [DEBUG]
net_packetloss [0-100]         # Test packet loss (default: 0) [DEBUG]
net_lag_compensate [0/1]       # Lag compensation (default: 1)
cl_predict [0/1]               # Client prediction (default: 1)
cl_predict_correct [0/1]       # Prediction düzeltme (default: 1)
net_stats_display [0/1]        # Stats overlay (default: 0)
```

### Örnek Network Test

```
# Test: 100ms Latency ile Ping
net_latency 100
net_ping

# Test: 5% Paket Kaybı Simülasyonu
net_packetloss 5.0
net_stats

# İnterpolasyonu Açıp Kapatıp Fark Gözle
net_interpolate 1
(Pürüzsüz - interpolation açık)

net_interpolate 0
(Sıçramalı - interpolation kapalı)
```

---

##  KATEGORI 3: GAMEPLAY KOMUTLARI (m_, cl_ Prefix)

### Oyun Kontrol Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `bind [key] [command]` | Tuşa komut ata | `bind w +forward` |
| `unbind [key]` | Tuş atamayı sil | `unbind w` |
| `unbindall` | Tüm tuş atamalarını sil | `unbindall` |
| `bindlist` | Tuş atamalarını listele | `bindlist` |
| `map [map_name]` | Harita yükle | `map de_mirage` |
| `maplist` | Harita listesi | `maplist` |
| `game_mode [mode]` | Oyun modu ayarla | `game_mode competitive` |

### Input Değişkenleri (CVARs)

```
m_sensitivity [0.1-5.0]        # Fare duyarlılığı (default: 1.0)
m_sensitivity_ads [0.1-5.0]    # ADS duyarlılığı (default: 0.75)
m_accel [0-1]                  # Fare hızlanması (default: 0.0)
m_rawmouse [0/1]               # Ham mouse input (default: 1)
m_filter [0/1]                 # Fare filtreleme (default: 0)
m_invert_y [0/1]               # Y eksenini ters çevir (default: 0)
m_pitch [0.022-0.5]            # Dikey hassasiyet (default: 0.022)
m_yaw [0.022-0.5]              # Yatay hassasiyet (default: 0.022)

cl_crosshair_enabled [0/1]     # Crosshair göster (default: 1)
cl_crosshair_style [0-5]       # Crosshair stili (default: 0)
cl_crosshair_size [5-50]       # Crosshair boyutu (default: 20)
cl_crosshair_color [R G B]     # Crosshair rengi (default: 0 255 0)
cl_crosshair_alpha [0-1]       # Crosshair saydamlığı (default: 1.0)
cl_crosshair_outline [0/1]     # Crosshair kontur (default: 1)

cl_jump_height [0.5-2.0]       # Zıplama yüksekliği (default: 1.0)
cl_move_speed [0.5-2.0]        # Hareket hızı (default: 1.0)
cl_crouch_speed [0-1]          # Eğilme hızı (default: 0.5)

hud_enabled [0/1]              # HUD göster (default: 1)
hud_scale [0.5-2.0]            # HUD boyutu (default: 1.0)
hud_opacity [0-1]              # HUD saydamlığı (default: 1.0)
hud_health_display [0/1]       # Can göster (default: 1)
hud_ammo_display [0/1]         # Mermi göster (default: 1)
hud_minimap [0/1]              # Mini harita (default: 1)
hud_scoreboard [0/1]           # Skor tablosu (default: 1)
```

### Örnek Input Konfigürasyonu

```
# Profesyonel FPS Ayarları
m_sensitivity 1.2
m_rawmouse 1
m_accel 0.0
m_invert_y 0

# Crosshair Setup
cl_crosshair_enabled 1
cl_crosshair_style 1
cl_crosshair_size 18
cl_crosshair_color 0 255 0
cl_crosshair_alpha 0.9

# Tuş Atamaları
bind w +forward
bind a +moveleft
bind s +back
bind d +moveright
bind space +jump
bind ctrl +duck
bind e +use
bind r +reload
bind mouse1 +attack
bind mouse2 +attack2
```

---

##  KATEGORI 4: AUDIO KOMUTLARI (snd_ Prefix)

### Ses Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `snd_test [sound]` | Ses test et | `snd_test default_beep` |
| `snd_stats` | Ses istatistiklerini göster | `snd_stats` |
| `snd_list_sounds` | Yüklü sesleri listele | `snd_list_sounds` |
| `snd_reload` | Ses sistemini yeniden yükle | `snd_reload` |

### Audio Değişkenleri (CVARs)

```
snd_master_volume [0-1]        # Ana ses (default: 0.8)
snd_music_volume [0-1]         # Müzik (default: 0.5)
snd_effects_volume [0-1]       # Efektler (default: 0.9)
snd_ui_volume [0-1]            # UI sesleri (default: 0.7)
snd_voice_volume [0-1]         # Ses chat (default: 0.85)

snd_3d_enabled [0/1]           # 3D ses (default: 1)
snd_doppler_effect [0/1]       # Doppler efekti (default: 1)
snd_reverb_enabled [0/1]       # Reverb (default: 1)
snd_spatial_quality [0-3]      # Spatial audio: 0=Mono, 1=Stereo, 2=5.1, 3=7.1 (default: 2)

snd_channels [16-512]          # Ses kanalı sayısı (default: 128)
snd_quality [0-2]              # Kalite: 0=11kHz, 1=22kHz, 2=44kHz+ (default: 2)
snd_hrtf [0/1]                 # HRTF 3D ses (default: 1)
snd_stats [0/1]                # Stats göster (default: 0)
```

### Örnek Ses Ayarı

```
# Oyun Ses Seviyesi Ayarı
snd_master_volume 0.8
snd_effects_volume 0.9
snd_music_volume 0.5
snd_voice_volume 0.85

# 3D Ses Optimizasyonu
snd_3d_enabled 1
snd_spatial_quality 3
snd_hrtf 1
snd_doppler_effect 1
snd_reverb_enabled 1
```

---

##  KATEGORI 5: HILE KOMUTLARI (sv_ Prefix) - **sv_cheats 1 GEREKLI**
###  ÖNEMLİ: Hile Komutlarını Kullanmak İçin

```
sv_cheats 1
```

### Hile Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `god` | Ölümsüzlük | `god` |
| `buddha` | Buddha modu | `buddha` |
| `noclip` | Duvardan geç | `noclip` |
| `notarget` | Düşmanlar görmez | `notarget` |
| `give [item] [amt]` | Eşya ver | `give rifle_m4`, `give grenade 5` |
| `impulse [code]` | Komut çalıştır | `impulse 101` (tüm silah) |
| `sv_hitbox_debug` | Hitboxları göster | `sv_hitbox_debug` |
| `sv_wireframe` | Wireframe modu | `sv_wireframe` |
| `sv_skeleton_debug` | İskelet göster | `sv_skeleton_debug` |
| `sv_bullet_trails` | Mermi izlerini göster | `sv_bullet_trails` |
| `sv_trace_rays` | Raycast göster | `sv_trace_rays` |

### Hile Değişkenleri (CVARs)

```
sv_cheats [0/1]                # Hile modunu aç/kapat (default: 0)
sv_gravity [0-2]               # Yer çekimi çarpanı (default: 1.0)
sv_infinite_ammo [0/1]         # Sınırsız mermi (default: 0)
sv_blood_enabled [0/1]         # Kan efektleri (default: 1)
sv_impact_particles [0/1]      # Çarpma parçacıkları (default: 1)
```

### Hile Kullanım Örneği

```
# Hile modunu aç
sv_cheats 1

# Ölümsüz mode
god 1

# Tüm silah ve mermiler
impulse 101

# Duvarlardan geç
noclip 1

# Hitboxları göster (çıkış: sv_hitbox_debug)
sv_hitbox_debug 1

# Sonsuz mermi
sv_infinite_ammo 1

# Normal moda dön
sv_cheats 0
```

---

##  KATEGORI 6: BOT KOMUTLARI (bot_ Prefix) - **sv_cheats 1 GEREKLI**

### Bot Yönetim Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `bot_add [team] [diff] [name]` | Bot ekle | `bot_add ct easy MyBot` |
| `bot_remove [name]` | Bot sil | `bot_remove MyBot` |
| `bot_removeall` | Tüm botları sil | `bot_removeall` |
| `bot_list` | Botları listele | `bot_list` |
| `bot_kick` | Tüm botları at | `bot_kick` |

### Bot Değişkenleri (CVARs)

```
bot_difficulty [0-3]           # Zorluk: 0=Çok Kolay, 3=Zor (default: 1)
bot_aggressiveness [0-1]       # Saldırganlık (default: 0.5)
bot_accuracy [0-1]             # Atış doğruluğu (default: 0.7)
bot_reaction_time [ms]         # Tepki süresi (default: 100)
```

### Bot Kullanım Örneği

```
# Hile modunu aç
sv_cheats 1

# 3 orta zorluk botu ekle
bot_add ct normal Bot_CT1
bot_add ct normal Bot_CT2
bot_add t normal Bot_T1

# Botları listele
bot_list

# Bot öğretimini ayarla
bot_difficulty 1
bot_accuracy 0.5
bot_aggressiveness 0.7

# Tüm botları sil
bot_removeall
```

---

##  KATEGORI 7: SISTEM KOMUTLARI (sys_ Prefix)

### Sistem Komutları

| Komut | Açıklama | Örnek Kullanım |
|-------|----------|----------------|
| `sys_info` | Sistem bilgisi | `sys_info` |
| `sys_benchmark` | Performans testi | `sys_benchmark` |
| `profile_start [name]` | Profilleme başlat | `profile_start rendering` |
| `profile_stop` | Profillemeyi durdur | `profile_stop` |
| `profile_results` | Sonuçları göster | `profile_results` |
| `log_enable` | Loglama aç | `log_enable 1` |
| `log_clear` | Log temizle | `log_clear` |

### Sistem Değişkenleri (CVARs)

```
sys_cpu_usage [0/1]            # CPU % göster (default: 0)
sys_gpu_usage [0/1]            # GPU % göster (default: 0)
sys_memory_usage [0/1]         # RAM MB göster (default: 0)
sys_frame_analysis [0/1]       # Frame analizi (default: 0)

log_enable [0/1]               # Loglama aç/kapat (default: 1)
log_file [filename]            # Log dosyası (default: game.log)
log_level [0-4]                # Log seviyesi (default: 2)
log_show_timestamp [0/1]       # Zaman ekle (default: 1)
```

### Sistem Profilleme Örneği

```
# Profillemeyi başlat
profile_start game_logic
(30 saniye oyun oyna)
profile_stop

# Sonuçları gözle
profile_results

# İstatistikleri göster
sys_info
sys_cpu_usage 1
sys_gpu_usage 1
sys_memory_usage 1
```

---

##  KATEGORI 8: GENEL KOMUTLAR

### Yardım ve Bilgi

| Komut | Açıklama | Örnek |
|-------|----------|-------|
| `help` | Genel yardım | `help` |
| `help [cmd]` | Komut yardımı | `help r_fov` |
| `cmdlist` | Tüm komutları listele | `cmdlist` |
| `cvarlist` | Tüm değişkenleri listele | `cvarlist` |
| `history` | Komut geçmişi | `history` |

### Konfigürasyon

| Komut | Açıklama | Örnek |
|-------|----------|-------|
| `save_config [file]` | Ayarları kaydet | `save_config my_settings.cfg` |
| `load_config [file]` | Ayarları yükle | `load_config my_settings.cfg` |
| `reset_config` | Varsayılan ayarlar | `reset_config` |

### Diğer

| Komut | Açıklama | Örnek |
|-------|----------|-------|
| `clear` | Konsolu temizle | `clear` |
| `echo [text]` | Metin yazdır | `echo Hello World` |
| `quit` / `exit` | Oyundan çık | `quit` |

---

##  KULLANIM SENARYOLARI
### Senaryo 1: FPS Düşüşü Analizi

```bash
# 1. Geçerli durumu ölç
r_fps_display 1
sys_cpu_usage 1
sys_gpu_usage 1
r_frametime_display 1

# 2. Grafikleri düşür
r_shadow_quality 1
r_texture_quality 2
r_aa_mode 1

# 3. Profilleme yap
profile_start rendering
(5 saniye bekle)
profile_stop
profile_results

# 4. Ağ sorunlarını test et
net_ping
net_stats
net_latency 100
net_packetloss 5.0

# 5. Ayarları kaydet
save_config fps_debug.cfg
```

### Senaryo 2: Girdi Konfigürasyonu

```bash
# Profesyonel FPS Setup
m_sensitivity 1.2
m_rawmouse 1
m_accel 0
m_invert_y 0

# Crosshair
cl_crosshair_style 1
cl_crosshair_size 18
cl_crosshair_color 0 255 0
cl_crosshair_alpha 0.9

# Tuş Atamaları
bind w +forward
bind a +moveleft
bind s +back
bind d +moveright
bind space +jump
bind lctrl +duck
bind e +use
bind r +reload
bind mouse1 +attack
bind mouse2 +attack2

# Kaydet
save_config my_config.cfg
```

### Senaryo 3: Multiplayer Test

```bash
# Hile modunu aç
sv_cheats 1

# Botlar ekle
bot_add ct easy Bot1
bot_add ct easy Bot2
bot_add t hard Bot3
bot_list

# Ağ delay test et
net_latency 50
(oyun oyna)
net_latency 0

# Paket kaybı test et
net_packetloss 3.0
(oyun oyna)
net_packetloss 0

# Botları sil
bot_removeall
sv_cheats 0
```

---

##  Komut Hızlı Referans

```
# Sık Kullanılan Komutlar
r_fps_max 240              # 240 FPS limit
r_vsync 0                  # VSync kapat
m_sensitivity 1.5          # Fare hassaslığı
god 1                      # Ölümsüz (cheats gerekli)
noclip 1                   # Duvardan geç (cheats gerekli)
map de_mirage              # Harita yükle
help [komut]               # Yardım
save_config default.cfg    # Ayarları kaydet
```

---

## Notlar

1. **Hile Komutları:** `sv_cheats 1` olmadan çoğu hile komutu çalışmaz
2. **Geliştiriciler:** Debug komutları oyun performansını etkileyebilir
3. **Konfigürasyon:** `save_config` ile ayarlarınızı kaydedin
4. **Veri Tabanı:** Tüm komutlar `cmdlist` ve `cvarlist` ile listelenebilir

---

**Son Güncelleme:** Ocak 2026  
**Versiyon:** 1.0  
**Motor:** Archura FPS Engine
