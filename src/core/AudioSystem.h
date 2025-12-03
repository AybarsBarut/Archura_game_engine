#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Archura {

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

private:
    AudioSystem() = default;
    ~AudioSystem() = default;

    std::string m_CurrentMusicAlias;
    int m_NextSoundID = 0;
};

} // namespace Archura
