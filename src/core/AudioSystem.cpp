#include "AudioSystem.h"
#include <windows.h>
#include <iostream>
#include <filesystem>

// Link with winmm.lib
#pragma comment(lib, "winmm.lib")

namespace Archura {

AudioSystem::AudioSystem() {
    // Varsayilan ses seviyeleri
    m_MasterVolume = 1.0f;
    m_MusicVolume = 1.0f;
    m_SFXVolume = 1.0f;
}

void AudioSystem::Init() {

}

void AudioSystem::Shutdown() {
    StopMusic();
    // Tum sesleri kapat
    mciSendString("close all", NULL, 0, NULL);
}

void AudioSystem::PlayMusic(const std::string& filename, bool loop) {
    StopMusic();

    std::string path = "assets/audio/" + filename;
    if (!std::filesystem::exists(path)) {
        std::cerr << "Audio file not found: " << path << std::endl;
        return;
    }

    // Alias olustur (Music)
    std::string alias = "Music";
    std::string cmdOpen = "open \"" + path + "\" type mpegvideo alias " + alias;
    
    MCIERROR err = mciSendString(cmdOpen.c_str(), NULL, 0, NULL);
    if (err != 0) {
        char buffer[128];
        mciGetErrorString(err, buffer, 128);
        std::cerr << "MCI Error (Open): " << buffer << std::endl;
        return;
    }

    std::string cmdPlay = "play " + alias;
    if (loop) {
        cmdPlay += " repeat";
    }

    // Ses seviyesini ayarla
    ApplyVolume(alias, m_MusicVolume);

    mciSendString(cmdPlay.c_str(), NULL, 0, NULL);
    m_CurrentMusicAlias = alias;
    

}

void AudioSystem::PlayOneShot(const std::string& filename) {
    std::string path = "assets/audio/" + filename;
    if (!std::filesystem::exists(path)) {
        // Hata mesaji yazdirma (cok sik olabilir)
        return;
    }

    // Benzersiz alias (SFX_0, SFX_1, ...)
    std::string alias = "SFX_" + std::to_string(m_NextSoundID++);
    
    // Dosyayi ac
    std::string cmdOpen = "open \"" + path + "\" type mpegvideo alias " + alias;
    mciSendString(cmdOpen.c_str(), NULL, 0, NULL);

    // Cal ve bitince otomatik kapat (notify kullanilabilir ama basitlik icin wait yapmiyoruz)
    // MCI'da "auto close" yok, bu yuzden surekli yeni alias acmak bellek sizintisina yol acabilir.
    // Ancak basit kullanim icin "play" komutu yeterli.
    // Daha iyi bir yontem: "play alias from 0 notify" ve callback ile kapatmak.
    // Simdilik basit "fire and forget" yapiyoruz, ama alias'lari temizlemedigimiz icin uzun surede sorun olabilir.
    // Cozum: Sadece belirli sayida kanal kullanmak veya kisa sesler icin WAV kullanmak.
    // MP3 icin MCI biraz agir.
    
    // Pratik cozum: Cal ve kapat komutunu ayni anda veremeyiz.
    // Bu yuzden sadece "play" diyoruz. 
    
    // Ses seviyesini ayarla
    ApplyVolume(alias, m_SFXVolume);

    std::string cmdPlay = "play " + alias;
    mciSendString(cmdPlay.c_str(), NULL, 0, NULL);
}

void AudioSystem::StopMusic() {
    if (!m_CurrentMusicAlias.empty()) {
        std::string cmd = "close " + m_CurrentMusicAlias;
        mciSendString(cmd.c_str(), NULL, 0, NULL);
        m_CurrentMusicAlias.clear();
    }
}

void AudioSystem::SetMasterVolume(float volume) {
    m_MasterVolume = volume;
    // Muzik sesini guncelle
    if (!m_CurrentMusicAlias.empty()) {
        ApplyVolume(m_CurrentMusicAlias, m_MusicVolume);
    }
}

void AudioSystem::SetMusicVolume(float volume) {
    m_MusicVolume = volume;
    if (!m_CurrentMusicAlias.empty()) {
        ApplyVolume(m_CurrentMusicAlias, m_MusicVolume);
    }
}

void AudioSystem::SetSFXVolume(float volume) {
    m_SFXVolume = volume;
}

void AudioSystem::ApplyVolume(const std::string& alias, float channelVolume) {
    // MCI volume range: 0 to 1000
    int finalVol = static_cast<int>(m_MasterVolume * channelVolume * 1000.0f);
    if (finalVol < 0) finalVol = 0;
    if (finalVol > 1000) finalVol = 1000;

    std::string cmd = "setaudio " + alias + " volume to " + std::to_string(finalVol);
    mciSendString(cmd.c_str(), NULL, 0, NULL);
}

} // namespace Archura
