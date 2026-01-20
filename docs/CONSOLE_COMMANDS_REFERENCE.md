# Archura FPS Engine - Geliştirici Konsolu Komut Referansı

## 1. İsimlendirme Konvansiyonu (Naming Convention)

Tüm konsol komutları aşağıdaki önekleri (prefix) takip eder:

| Prefix | Kategori | Açıklama |
|--------|----------|----------|
| `r_` | Rendering | Grafik, çizim, ve görsel ayarları |
| `net_` | Ağ (İstemci) | Ağ bağlantısı ve veri aktarımı |
| `sv_` | Sunucu | Sunucu tarafı kontrolleri ve hile komutu etkinleştirme |
| `m_` | Mouse/Input | Girdi ve kontrol ayarları |
| `cl_` | İstemci | İstemci tarafı ayarları |
| `snd_` | Ses | Ses motoru ayarları |
| `bot_` | Yapay Zeka | Bot ve NPC davranışları |
| `sys_` | Sistem | Sistem bilgisi ve profilleme |

---

## 2. Komut Listesi (Kategorize Edilmiş)

### A. Grafik ve Rendering (Rendering & Graphics) - `r_` Prefix

#### A.1 Çözünürlük ve Ekran Ayarları

* **Komut:** `r_resolution [width] [height]`
  * **Varsayılan:** 1920 1080
  * **Açıklama:** Oyun çözünürlüğünü ayarlar. Örn: `r_resolution 2560 1440` (2K), `r_resolution 3840 2160` (4K)

* **Komut:** `r_fullscreen [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Tam ekran modu etkinleştir/devre dışı bırak. 1: Tam ekran, 0: Pencereli

* **Komut:** `r_windowed_width [width]`
  * **Varsayılan:** 1280
  * **Açıklama:** Pencereli modda pencere genişliği

* **Komut:** `r_windowed_height [height]`
  * **Varsayılan:** 720
  * **Açıklama:** Pencereli modda pencere yüksekliği

* **Komut:** `r_monitor [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Monitör seçimi (multi-monitor setup için)

#### A.2 FPS ve Performans

* **Komut:** `r_fps_max [0-240]`
  * **Varsayılan:** 144
  * **Açıklama:** Maksimum FPS sınırı. 0 = Sınırsız (VSync kapalı), 144 = 144 Hz, 240 = 240 Hz

* **Komut:** `r_fps_unlocked [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** FPS sınırını tamamen kaldır (termal ve güç tüketimi artar)

* **Komut:** `r_vsync [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Dikey senkronizasyon (VSync) etkinleştir/devre dışı bırak. 1: Açık (input lag artabilir)

* **Komut:** `r_fps_display [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ekranda FPS göster

* **Komut:** `r_frametime_display [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Frame time (ms cinsinden) göster

#### A.3 Grafik Kalitesi

* **Komut:** `r_texture_quality [0-4]`
  * **Varsayılan:** 3
  * **Açıklama:** Doku kalitesi. 0: Kötü, 1: Düşük, 2: Orta, 3: Yüksek, 4: Ultra

* **Komut:** `r_model_quality [0-4]`
  * **Varsayılan:** 3
  * **Açıklama:** Model detay seviyesi (LOD). 0: Çok düşük, 4: Ultra detay

* **Komut:** `r_shadow_quality [0-4]`
  * **Varsayılan:** 2
  * **Açıklama:** Gölge kalitesi. 0: Kapalı, 1: Düşük (256x256), 2: Orta (1024x1024), 3: Yüksek (2048x2048), 4: Ultra (4096x4096)

* **Komut:** `r_shadow_distance [100-5000]`
  * **Varsayılan:** 1000
  * **Açıklama:** Gölge mesafe sınırı (metre cinsinden)

* **Komut:** `r_ao_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Ambient Occlusion (AO) etkinleştir

* **Komut:** `r_ao_quality [0-3]`
  * **Varsayılan:** 1
  * **Açıklama:** AO kalitesi. 0: Kapalı, 1: SSAO, 2: HBAO+, 3: GTAO

#### A.4 Anti-Aliasing (Kenar Yumuşatma)

* **Komut:** `r_aa_mode [0-3]`
  * **Varsayılan:** 1
  * **Açıklama:** Anti-aliasing modu. 0: Kapalı, 1: FXAA (hızlı), 2: SMAA (orta), 3: TAA (yüksek kalite)

* **Komut:** `r_aa_quality [0-2]`
  * **Varsayılan:** 1
  * **Açıklama:** Anti-aliasing kalitesi. 0: Düşük, 1: Orta, 2: Yüksek

* **Komut:** `r_msaa_samples [0/2/4/8/16]`
  * **Varsayılan:** 4
  * **Açıklama:** MSAA örnek sayısı (0 = Kapalı)

#### A.5 Kamera ve FOV

* **Komut:** `r_fov [60-130]`
  * **Varsayılan:** 90
  * **Açıklama:** Field of View (görüş açısı) derece cinsinden

* **Komut:** `r_fov_weapon [60-110]`
  * **Varsayılan:** 75
  * **Açıklama:** Silah sırasında FOV (weapon bob etkisi)

* **Komut:** `r_viewmodel_fov [60-120]`
  * **Varsayılan:** 70
  * **Açıklama:** 1. kişi silah modeli FOV

* **Komut:** `r_motion_blur [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Motion blur efekti etkinleştir

* **Komut:** `r_motion_blur_strength [0.0-2.0]`
  * **Varsayılan:** 0.5
  * **Açıklama:** Motion blur şiddeti

#### A.6 Işık ve Post-Processing

* **Komut:** `r_bloom_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Bloom (parlaklık yayılması) efekti

* **Komut:** `r_bloom_intensity [0.0-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Bloom şiddeti

* **Komut:** `r_gamma [0.5-2.5]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Ekran parlaklığı (gamma düzeltme)

* **Komut:** `r_brightness [0.5-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Genel parlaklık

* **Komut:** `r_contrast [0.5-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Kontrast seviyesi

#### A.7 Mesafe ve Draw Distance

* **Komut:** `r_draw_distance [500-10000]`
  * **Varsayılan:** 3000
  * **Açıklama:** Grafik render mesafesi (metre)

* **Komut:** `r_lod_bias [-2.0 - 2.0]`
  * **Varsayılan:** 0.0
  * **Açıklama:** LOD önyargı (düşük = daha detaylı, yüksek = daha az detaylı)

* **Komut:** `r_culling_distance [100-15000]`
  * **Varsayılan:** 5000
  * **Açıklama:** Frustum culling mesafe limiti

---

### B. Ağ ve Bağlantı (Networking & Connectivity)

#### B.1 Bağlantı Bilgisi

* **Komut:** `net_ping`
  * **Açıklama:** Mevcut ping (ms) göster

* **Komut:** `net_packetloss [0.0-100.0]`
  * **Varsayılan:** 0.0
  * **Açıklama:** Paket kaybı simulasyonu (%) - Geliştirici testi için

* **Komut:** `net_latency [0-1000]`
  * **Varsayılan:** 0
  * **Açıklama:** Yapay latency ekleme (ms) - Lag test için

* **Komut:** `net_stats [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ağ istatistiklerini ekranda göster (download/upload)

* **Komut:** `net_graph [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Gerçek zamanlı ağ grafiği göster

#### B.2 Bağlantı Ayarları

* **Komut:** `net_rate [8000-1000000]`
  * **Varsayılan:** 128000
  * **Açıklama:** Sunucuya gönderilen bayt/saniye oranı

* **Komut:** `net_cmdrate [20-100]`
  * **Varsayılan:** 60
  * **Açıklama:** Komut gönderme hızı (paket/saniye)

* **Komut:** `net_updaterate [10-66]`
  * **Varsayılan:** 32
  * **Açıklama:** Sunucudan güncelleme alım hızı (paket/saniye)

* **Komut:** `net_interpolate [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Hareket interpolasyonu etkinleştir (daha pürüzsüz görüntü)

* **Komut:** `net_interpolation_amount [0.0-1.0]`
  * **Varsayılan:** 0.5
  * **Açıklama:** İnterpolasyon gücü

#### B.3 Bağlantı Yönetimi

* **Komut:** `connect [IP:Port]`
  * **Açıklama:** Sunucuya bağlan. Örn: `connect 192.168.1.1:27015`

* **Komut:** `disconnect`
  * **Açıklama:** Sunucu bağlantısını kes

* **Komut:** `reconnect`
  * **Açıklama:** Son bağlanılan sunucuya yeniden bağlan

* **Komut:** `status`
  * **Açıklama:** Bağlantı durumunu ve oyuncu bilgisini göster

#### B.4 Lag Compensation

* **Komut:** `net_lag_compensate [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Server-side lag compensation etkinleştir

* **Komut:** `net_lerp [0.0-0.5]`
  * **Varsayılan:** 0.015
  * **Açıklama:** Interpolation miktarı (saniye)

* **Komut:** `cl_predict [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Client-side prediction etkinleştir (daha responsive kontrol)

* **Komut:** `cl_predict_correct [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Prediction hatasını düzelt

---

### C. Oynanış ve Girdi (Gameplay & Input)

#### C.1 Girdi Hassasiyeti

* **Komut:** `m_sensitivity [0.1-5.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Fare hassasiyeti (düşük = daha yavaş, yüksek = daha hızlı)

* **Komut:** `m_sensitivity_ads [0.1-5.0]`
  * **Varsayılan:** 0.75
  * **Açıklama:** Nişan alırken (ADS) fare hassasiyeti

* **Komut:** `m_accel [0.0-1.0]`
  * **Varsayılan:** 0.0
  * **Açıklama:** Fare akselerasyonu (0 = devre dışı)

* **Komut:** `m_rawmouse [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Raw mouse input (daha doğru)

* **Komut:** `m_filter [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Fare input filtreleme

* **Komut:** `m_invert_y [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Dikey eksen tersine çevir

* **Komut:** `m_pitch [0.022-0.5]`
  * **Varsayılan:** 0.022
  * **Açıklama:** Dikey görüş hassasiyeti

* **Komut:** `m_yaw [0.022-0.5]`
  * **Varsayılan:** 0.022
  * **Açıklama:** Yatay görüş hassasiyeti

#### C.2 Tuş Ataması (Keybind)

* **Komut:** `bind [key] [command]`
  * **Açıklama:** Tuşa komut ata. Örn: `bind w +forward`, `bind MOUSE1 +attack`

* **Komut:** `unbind [key]`
  * **Açıklama:** Tuş atamasını sil

* **Komut:** `unbindall`
  * **Açıklama:** Tüm tuş atamalarını sil

* **Komut:** `bindlist`
  * **Açıklama:** Tüm tuş atamalarını listele

#### C.3 HUD ve Görüntü

* **Komut:** `hud_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** HUD (arayüz) göster/gizle

* **Komut:** `hud_scale [0.5-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** HUD boyutu

* **Komut:** `hud_opacity [0.0-1.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** HUD saydamlığı

* **Komut:** `hud_health_display [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Can göstergesi göster

* **Komut:** `hud_ammo_display [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Mermi sayacı göster

* **Komut:** `hud_minimap [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Mini harita göster

* **Komut:** `hud_scoreboard [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Skor tablosunu göster

#### C.4 Crosshair Ayarları

* **Komut:** `cl_crosshair_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Crosshair göster

* **Komut:** `cl_crosshair_style [0-5]`
  * **Varsayılan:** 0
  * **Açıklama:** Crosshair stili. 0: Statik, 1: Dinamik (spread), 2: Daire, 3: Dot, 4: T-Style, 5: Özel

* **Komut:** `cl_crosshair_size [5-50]`
  * **Varsayılan:** 20
  * **Açıklama:** Crosshair boyutu

* **Komut:** `cl_crosshair_color [R G B]`
  * **Varsayılan:** 0 255 0
  * **Açıklama:** Crosshair rengi (RGB). Örn: `cl_crosshair_color 255 0 0` (kırmızı)

* **Komut:** `cl_crosshair_alpha [0.0-1.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Crosshair saydamlığı

* **Komut:** `cl_crosshair_outline [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Crosshair konturunu göster

#### C.5 Oyun Mekaniği

* **Komut:** `cl_jump_height [0.5-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Zıplama yüksekliği (çarpan)

* **Komut:** `cl_move_speed [0.5-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Hareket hızı (çarpan)

* **Komut:** `cl_crouch_speed [0.0-1.0]`
  * **Varsayılan:** 0.5
  * **Açıklama:** Eğilme hızı (çarpan)

---

### D. Ses (Audio)

#### D.1 Ses Seviyesi

* **Komut:** `snd_master_volume [0.0-1.0]`
  * **Varsayılan:** 0.8
  * **Açıklama:** Ana ses seviyesi

* **Komut:** `snd_music_volume [0.0-1.0]`
  * **Varsayılan:** 0.5
  * **Açıklama:** Müzik ses seviyesi

* **Komut:** `snd_effects_volume [0.0-1.0]`
  * **Varsayılan:** 0.9
  * **Açıklama:** Efekt ses seviyesi (silah sesleri, adım sesleri vb.)

* **Komut:** `snd_ui_volume [0.0-1.0]`
  * **Varsayılan:** 0.7
  * **Açıklama:** UI ses seviyesi (buton, menu vb.)

* **Komut:** `snd_voice_volume [0.0-1.0]`
  * **Varsayılan:** 0.85
  * **Açıklama:** Sesli iletişim ses seviyesi

#### D.2 Ses Teknolojisi

* **Komut:** `snd_3d_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** 3D ses etkinleştir

* **Komut:** `snd_doppler_effect [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Doppler efekti (hareket eden seslerde pitch değişikliği)

* **Komut:** `snd_reverb_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Akustik reverb (yankı) efekti

* **Komut:** `snd_spatial_quality [0-3]`
  * **Varsayılan:** 2
  * **Açıklama:** Spatial audio kalitesi. 0: Mono, 1: Stereo, 2: 5.1, 3: 7.1

#### D.3 Performans ve Konfigürasyon

* **Komut:** `snd_channels [16-512]`
  * **Varsayılan:** 128
  * **Açıklama:** Eş zamanlı ses kanalı sayısı

* **Komut:** `snd_quality [0-2]`
  * **Varsayılan:** 2
  * **Açıklama:** Ses kalitesi. 0: Düşük (11kHz), 1: Orta (22kHz), 2: Yüksek (44kHz+)

* **Komut:** `snd_hrtf [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** HRTF (Head-Related Transfer Function) 3D ses

* **Komut:** `snd_stats [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ses motor istatistiklerini göster

---

### E. Hile ve Geliştirici Araçları (Cheats & Debugging) - sv_cheats 1 Gerektirir

#### E.1 Hile Etkinleştirme

* **Komut:** `sv_cheats [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Hile komutlarını etkinleştir (Sunucu admini tarafından) - **Çoğu hile komutu bunu gerektirir!**

#### E.2 Ölümsüzlük ve Modlar

* **Komut:** `god [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ölümsüzlük modu - hasar almaz

* **Komut:** `buddha [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Buddha modu - 1 HP'ye kadar hasar alabilir ama ölmez

* **Komut:** `noclip [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Duvarlardan geçme ve uçma - harita keşfi için kullanışlı

* **Komut:** `notarget [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Düşman botlar seni görmez

#### E.3 Silah ve Mermi

* **Komut:** `give [item_name]`
  * **Açıklama:** Oyuncuya eşya ver. Örn: `give rifle_m4`, `give grenade`, `give armor`

* **Komut:** `give_ammo [weapon] [amount]`
  * **Açıklama:** Silah mermi ver. Örn: `give_ammo rifle_m4 999`

* **Komut:** `impulse 101`
  * **Açıklama:** Tüm silahları ve mermiyi ver

* **Komut:** `sv_infinite_ammo [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Sınırsız mermi - hiç mermi tüketme

* **Komut:** `sv_gravity [0.0-2.0]`
  * **Varsayılan:** 1.0
  * **Açıklama:** Yer çekimi kuvveti (çarpan)

#### E.4 Görselleştirme ve Debug

* **Komut:** `sv_hitbox_debug [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Hitboxları göster (NPC/oyuncu vücut bölgelerine ait kutular)

* **Komut:** `sv_hitbox_color [R G B A]`
  * **Varsayılan:** 255 0 0 128
  * **Açıklama:** Hitbox rengi (RGBA)

* **Komut:** `sv_wireframe [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Wireframe modu (kafes görünümü)

* **Komut:** `sv_skeleton_debug [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Karakter iskeleti göster

* **Komut:** `sv_trace_rays [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Raycasting ışınlarını visualize et

#### E.5 Bot ve NPC

* **Komut:** `bot_add [team] [difficulty] [name]`
  * **Açıklama:** Bot ekle. Örn: `bot_add ct easy BotName`, `bot_add t hard BOT2`

* **Komut:** `bot_remove [bot_name]`
  * **Açıklama:** Bot sil

* **Komut:** `bot_removeall`
  * **Açıklama:** Tüm botları sil

* **Komut:** `bot_difficulty [0-3]`
  * **Varsayılan:** 1
  * **Açıklama:** Bot zorluk. 0: Çok Kolay, 1: Kolay, 2: Orta, 3: Zor

* **Komut:** `bot_aggressiveness [0.0-1.0]`
  * **Varsayılan:** 0.5
  * **Açıklama:** Bot saldırganlığı

* **Komut:** `bot_accuracy [0.0-1.0]`
  * **Varsayılan:** 0.7
  * **Açıklama:** Bot atış doğruluğu

#### E.6 Mermi İzleri ve Efektler

* **Komut:** `sv_bullet_trails [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Mermi trajectorylerini çizgi olarak göster

* **Komut:** `sv_blood_enabled [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Kan efektlerini göster

* **Komut:** `sv_impact_particles [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Mermi çarpma parçacıklarını göster

---

### F. Sistem ve Profilleme (System & Profiling)

#### F.1 Temel Sistem Komutları

* **Komut:** `quit`
  * **Açıklama:** Oyundan çık

* **Komut:** `exit`
  * **Açıklama:** Oyundan çık (alias)

* **Komut:** `clear`
  * **Açıklama:** Konsolu temizle

* **Komut:** `echo [text]`
  * **Açıklama:** Metni konsola yazdır. Örn: `echo Debug message`

#### F.2 Bilgi ve İstatistik

* **Komut:** `sys_info`
  * **Açıklama:** Sistem bilgilerini göster (OS, GPU, CPU, RAM)

* **Komut:** `sys_cpu_usage [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ekranda CPU kullanımı göster (%)

* **Komut:** `sys_memory_usage [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ekranda RAM kullanımı göster (MB)

* **Komut:** `sys_gpu_usage [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Ekranda GPU kullanımı göster (%)

* **Komut:** `sys_frame_analysis [0/1]`
  * **Varsayılan:** 0
  * **Açıklama:** Her frame için detaylı analiz göster

#### F.3 Profilleme

* **Komut:** `profile_start [name]`
  * **Açıklama:** Profilleme başlat. Örn: `profile_start rendering`

* **Komut:** `profile_stop [name]`
  * **Açıklama:** Profillemeyi durdur ve sonuçları göster

* **Komut:** `profile_reset`
  * **Açıklama:** Tüm profil verilerini sıfırla

* **Komut:** `profile_results`
  * **Açıklama:** Son profil sonuçlarını göster

* **Komut:** `sys_benchmark`
  * **Açıklama:** Performans karşılaştırması çalıştır (CPU/GPU)

#### F.4 Loglama

* **Komut:** `log_enable [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Dosyaya loglama etkinleştir/devre dışı bırak

* **Komut:** `log_file [filename]`
  * **Varsayılan:** game.log
  * **Açıklama:** Log dosyası adı

* **Komut:** `log_level [0-4]`
  * **Varsayılan:** 2
  * **Açıklama:** Log seviyesi. 0: Kritik, 1: Hata, 2: Uyarı, 3: Info, 4: Debug

* **Komut:** `log_show_timestamp [0/1]`
  * **Varsayılan:** 1
  * **Açıklama:** Log'lara zaman damgası ekle

#### F.5 Konfigürasyon

* **Komut:** `save_config [filename]`
  * **Açıklama:** Ayarları dosyaya kaydet. Örn: `save_config my_settings.cfg`

* **Komut:** `load_config [filename]`
  * **Açıklama:** Ayarları dosyadan yükle. Örn: `load_config my_settings.cfg`

* **Komut:** `reset_config`
  * **Açıklama:** Tüm ayarları varsayılana sıfırla

* **Komut:** `cvarlist`
  * **Açıklama:** Tüm değişkenleri listele

* **Komut:** `help [command]`
  * **Açıklama:** Komut hakkında yardım göster. Örn: `help r_resolution`

---

## 3. Örnek Kullanım Senaryosu: FPS Düşüşü Analizi

### Senaryo: "Oyun orta harita açılırken FPS'imiz 60'dan 30'a düşüyor ve performans sorununu tespit etmek istiyorum."

#### Adım 1: Mevcut Durumu Kontrol Et
```
r_fps_display 1           (FPS göstergesini aç)
sys_cpu_usage 1           (CPU kullanımını izle)
sys_gpu_usage 1           (GPU kullanımını izle)
r_frametime_display 1     (Her frame'in harcadığı zamanı gör)
```

#### Adım 2: Grafik Kalitesini Aşamalı Olarak Düşür
```
r_shadow_quality 1        (Gölgeleri düşük kaliteyp ayarla)
r_texture_quality 2       (Dokuları düşür)
r_aa_mode 1               (AA'ı FXAA'ya ayarla)
r_bloom_enabled 0         (Bloom'u kapat)
```
**Sonuç:** Eğer FPS 50'ye çıktıysa, problem gölge/doku render'ıdır.

#### Adım 3: Draw Distance Kontrolü
```
r_draw_distance 2000      (Mesafeyi azalt)
r_shadow_distance 500     (Gölge mesafesini azalt)
```
**Sonuç:** Eğer FPS 55'e çıktıysa, problem belki çok sayıda nesnenin render'ı.

#### Adım 4: Detaylı Debug Bilgisi Topla
```
sv_cheats 1               (Hile modunu aç)
sys_frame_analysis 1      (Frame analizi başlat)
net_stats 1               (Ağ istatistiklerini kontrol et)
```

#### Adım 5: Profilleme Yap
```
profile_start rendering
(5 saniye oyun play et)
profile_stop rendering
profile_results           (Sonuçları gör: Hangi rendering işlemi en fazla zaman alıyor?)
```

#### Adım 6: Hitbox/Wireframe Analizi
```
sv_hitbox_debug 1         (Çarışma kutularını göster - Belki hitbox hesaplaması pahalı?)
sv_wireframe 1            (Mesh sayısını kontrol et)
```

#### Adım 7: Ağ Sorununu Test Et
```
net_latency 100           (Yapay 100ms latency ekle - FPS düşer mi?)
net_packetloss 5.0        (5% paket kaybı simüle et)
```
**Sonuç:** Eğer FPS hala 30 ise, sorun server-side işlemlerinden biri.

#### Adım 8: Bot Test (Dinamik Nesneler)
```
bot_add ct easy Bot1
bot_add ct easy Bot2
bot_add ct easy Bot3      (3 bot ekle - FPS'e etkisi ne?)
```

#### Adım 9: Bulguları Analiz Et
```
sys_info                  (Sistem kapasitesini gözden geçir)
save_config debug_session.cfg  (Debugging oturumunu kaydet)
```

### Özet:
- **CPU Sorunu:** Frame analysis'te CPU zaman yüksekse → AI, fizik veya ağ kodunda optimization gerekli
- **GPU Sorunu:** GPU zaman yüksekse → Shadow quality, draw distance, texture quality düşür
- **Ağ Sorunu:** Net stats'ta high packet loss veya latency görülürse → Server sync sorunları
- **LOD Sorunu:** Wireframe'de mesh count çok yüksekse → LOD ayarları uyarla

---

## 4. Komut Kategorilerine Göz Kısaca Bakış

| Kategori | Prefix | Amaç | Örnek Komut |
|----------|--------|------|-------------|
| **Rendering** | `r_` | Grafik kalitesi ve performans | `r_shadow_quality 2` |
| **Ağ** | `net_`, `cl_` | Bağlantı ve veri aktarımı | `net_ping`, `net_rate 128000` |
| **Girdi** | `m_`, `cl_` | Kontrol ve hassasiyet | `m_sensitivity 1.5` |
| **Ses** | `snd_` | Audio ayarları | `snd_master_volume 0.8` |
| **Hile/Debug** | `sv_`, `bot_` | Geliştirici araçları (sv_cheats gerekli) | `god 1`, `bot_add ct easy` |
| **Sistem** | `sys_` | Performans profilleme | `sys_cpu_usage 1` |

---

## Lisanslı Komutlar (Admin/Server Tarafı)

Aşağıdaki komutlar **sadece server admini** tarafından çalıştırılabilir:

- `sv_cheats 1/0` - Hile modunu aç/kapat
- `bot_add`, `bot_remove` - Bot yönetimi
- `sv_gravity` - Yer çekimi değişikliği
- `kick [player]` - Oyuncu atar
- `ban [player]` - Oyuncu engelle
- `server_time_limit [minutes]` - Maç süresi
- `server_round_limit [rounds]` - Tur sayısı sınırı

---

**Son Güncelleme:** Ocak 2026
**Versiyon:** 1.0 - FPS Engine Console Commands Reference

