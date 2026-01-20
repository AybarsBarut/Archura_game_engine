#include "AudioSystem.h"

// Windows headers must come first with proper guards
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h> // Added mmsystem.h
#include <iostream>
#include <filesystem>
#include <vector> // Added vector
#include "../game/AudioSource.h"
#include "../game/AudioSource.h"
#include "../rendering/Camera.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../ecs/Component.h"
#include <glm/glm.hpp>

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

void AudioSystem::Update(Scene* scene, Camera* camera) {
    if (!scene || !camera) return;

    for (auto& entityPtr : scene->GetEntities()) {
        auto* source = entityPtr->GetComponent<AudioSource>();
        if (!source || !source->isPlaying) continue;

        auto* transform = entityPtr->GetComponent<Transform>();
        if (!transform) continue;

        // Calculate distance
        float distance = glm::distance(transform->position, camera->GetPosition());

        // Linear attenuation
        // minDistance: volume = max
        // maxDistance: volume = 0
        
        float volume = source->volume;
        
        if (distance < source->minDistance) {
            volume = source->volume;
        } else if (distance > source->maxDistance) {
            volume = 0.0f;
        } else {
            float pct = 1.0f - ((distance - source->minDistance) / (source->maxDistance - source->minDistance));
            volume = source->volume * pct;
        }
        
        // MCI uses volume 0-1000
        int finalVol = (int)(volume * 1000.0f);
        if (finalVol < 0) finalVol = 0;
        if (finalVol > 1000) finalVol = 1000;

        std::string alias = source->runtimeAlias;
        if (alias.empty()) {
            alias = std::string("sound_") + std::to_string(entityPtr->GetID());
            source->runtimeAlias = alias;
        }

        std::string cmd = "setaudio " + alias + " volume to " + std::to_string(finalVol);
        mciSendStringA(cmd.c_str(), NULL, 0, NULL);
    }
}

void AudioSystem::Play(AudioSource* source, const std::string& alias) {
    if (!source) return;

    std::string path = "assets/audio/" + source->clipName;
    if (!std::filesystem::exists(path)) return;

    // Close previous if any
    std::string cmdClose = "close " + alias;
    mciSendString(cmdClose.c_str(), NULL, 0, NULL);

    // Open
    std::string cmdOpen = "open \"" + path + "\" type mpegvideo alias " + alias;
    MCIERROR err = mciSendString(cmdOpen.c_str(), NULL, 0, NULL);
    
    if (err == 0) {
        std::string cmdPlay = "play " + alias;
        if (source->loop) cmdPlay += " repeat";
        mciSendString(cmdPlay.c_str(), NULL, 0, NULL);
        source->isPlaying = true;
    } else {
         std::cerr << "AudioSource Open Error: " << path << std::endl;
    }
}

void AudioSystem::Stop(AudioSource* source) {
    if (!source || source->runtimeAlias.empty()) return;

    std::string cmd = "stop " + source->runtimeAlias;
    mciSendStringA(cmd.c_str(), NULL, 0, NULL);
    source->isPlaying = false;
}

} // namespace Archura
