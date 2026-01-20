#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Archura {

class Scene;
class Camera;

/**
 * @brief Windows MCI tabanli basit Ses Sistemi (MP3 destegi icin)
 * 
 * Harici kutuphane kullanmadan MP3 calmak icin Windows Multimedia API (winmm) kullanir.
 */
class AudioSystem {
public:
    static AudioSystem& Get() {
        static AudioSystem instance;
        return instance;
    }

    // Sistemi baslat
    void Init();
    
    // Sistemi kapat
    void Shutdown();

    /**
     * @brief Arkaplan muzigi cal (MP3)
     * @param filename assets/audio/ klasorundeki dosya adi
     * @param loop Donguye alinsin mi?
     */
    void PlayMusic(const std::string& filename, bool loop = true);

    /**
     * @brief Ses efekti cal (MP3/WAV)
     * @param filename assets/audio/ klasorundeki dosya adi
     */
    void PlayOneShot(const std::string& filename);

    // Muziği durdur
    void StopMusic();

    // 3D Audio Update Loop
    void Update(Scene* scene, Camera* camera);

    // Play/Stop AudioSource
    void Play(struct AudioSource* source, const std::string& alias);
    void Stop(struct AudioSource* source);

    // Ses Ayarlari (0.0f - 1.0f)
    void SetMasterVolume(float volume);
    void SetMusicVolume(float volume);
    void SetSFXVolume(float volume);

    float GetMasterVolume() const { return m_MasterVolume; }
    float GetMusicVolume() const { return m_MusicVolume; }
    float GetSFXVolume() const { return m_SFXVolume; }

private:
    AudioSystem();
    ~AudioSystem() = default;

    void ApplyVolume(const std::string& alias, float volume);

    std::string m_CurrentMusicAlias;
    int m_NextSoundID = 0;

    float m_MasterVolume = 1.0f;
    float m_MusicVolume = 1.0f;
    float m_SFXVolume = 1.0f;
};

} // namespace Archura
