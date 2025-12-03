#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <windows.h>

namespace fs = std::filesystem;

// Komut calistirma yardimcisi
int RunCommand(const std::string& command) {
    return std::system(command.c_str());
}

// Konsol renkleri
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int main() {
    SetConsoleTitle("Archura Engine Launcher");
    
    // Cyan
    SetColor(11);
    std::cout << "=== Archura Engine Launcher (C++) ===" << std::endl;
    SetColor(7); // Reset

    bool needsBuild = false;
    std::string exePath = "build/bin/Release/ArchuraEngine.exe";

    // 1. Baslangic Kontrolu
    if (!fs::exists(exePath)) {
        SetColor(14); // Yellow
        std::cout << "Oyun dosyasi bulunamadi. Ilk kurulum yapiliyor..." << std::endl;
        SetColor(7);
        needsBuild = true;
    }

    // 2. Guncelleme Kontrolu
    std::cout << "Guncellemeler kontrol ediliyor..." << std::endl;
    
    // Git fetch
    if (RunCommand("git fetch origin") == 0) {
        // Versiyon karsilastirma (Basitce git status ile)
        // Daha detayli kontrol icin git rev-parse kullanilabilir ama
        // basitlik adina 'git status' ciktisinda 'behind' var mi bakiyoruz.
        
        // Bu kisim C++'da system() ile ciktiyi almak zor oldugu icin
        // basitce her zaman 'git pull' deniyoruz. Eger guncel ise zaten bir sey yapmaz.
        // Ancak bu her acilista internet gerektirir.
        // O yuzden sadece internet varsa pull yapalim.
        
        SetColor(14); // Yellow
        std::cout << "Senkronizasyon saglaniyor..." << std::endl;
        SetColor(7);
        
        int pullResult = RunCommand("git pull");
        if (pullResult == 0) {
            // Eger pull basarili olduysa ve degisiklik varsa build gerekebilir.
            // Bunu anlamanin en kolay yolu, pull ciktisini kontrol etmek ama system() ile zor.
            // Guvenli tarafta kalmak icin: Eger exe yoksa veya kullanici isterse build yapalim.
            // Simdilik otomatik build'i sadece exe yoksa yapiyoruz.
            // Veya her zaman cmake --build yapabiliriz, degisiklik yoksa hizli gecer.
            needsBuild = true; 
        }
    } else {
        SetColor(12); // Red
        std::cout << "Guncelleme sunucusuna erisilemedi. Cevrimdisi mod." << std::endl;
        SetColor(7);
    }

    // 3. Derleme
    // Her zaman build denemesi yapmak iyidir, degisiklik yoksa saniyeler surer.
    SetColor(14); // Yellow
    std::cout << "Derleme durumu kontrol ediliyor..." << std::endl;
    SetColor(7);

    if (!fs::exists("build")) {
        fs::create_directory("build");
        SetColor(11); // Cyan
        std::cout << "CMake Konfigurasyonu..." << std::endl;
        SetColor(7);
        if (RunCommand("cd build && cmake ..") != 0) {
            SetColor(12); // Red
            std::cout << "CMake Hatasi!" << std::endl;
            system("pause");
            return -1;
        }
    }

    SetColor(11); // Cyan
    std::cout << "Derleniyor (Release)..." << std::endl;
    SetColor(7);
    
    // Sadece ArchuraEngine hedefini derle, Launcher kendini derlemeye calisirsa hata verir (cunku calisiyor)
    if (RunCommand("cmake --build build --config Release --target ArchuraEngine") != 0) {
        SetColor(12); // Red
        std::cout << "Derleme Hatasi!" << std::endl;
        system("pause");
        return -1;
    }

    // 4. Baslatma
    if (fs::exists(exePath)) {
        SetColor(10); // Green
        std::cout << "Oyun baslatiliyor..." << std::endl;
        SetColor(7);
        
        // Oyunu baslat
        // system() kullanirsak launcher kapanana kadar bekler.
        // ShellExecute veya start komutu ile ayri process acalim.
        std::string startCmd = "start \"\" \"" + exePath + "\"";
        RunCommand(startCmd);
    } else {
        SetColor(12); // Red
        std::cout << "HATA: Oyun dosyasi olusturulamadi!" << std::endl;
        system("pause");
    }

    return 0;
}
