#include <iostream>
#include <thread>
#include <cstdint>
#include <mutex>

// 2 Tililtä nostaminen (1p)
// Kirjoita ohjelma, jossa on kaksi säiettä, ja jotka käsittelevät yhteistä pankkitiliä. Käytä säikeiden luomiseen
// std::thread-luokkaa. Ensimmäinen säie tallettaa rahaa tilille, kun taas toinen nostaa sitä. Nosto- ja talletustapahtumia
// tulisi olla paljon (tuhansia). Pääohjelma luo säikeet ja odottaa niiden päättymistä.
// Tarkista tapahtumien jälkeen, että tilin saldo on korrekti. Jos/kun saldo on väärä, käytä mutexia tapahtumien
// suojaamiseen. Muokkaa ratkaisua lopuksi niin, että mutexia käytetään std::lock_guard:n avulla, eksplisiittisten lock()-
// ja unlock()-kutsujen sijasta.

int main() {
    std::uint64_t saldo = 0;
    std::mutex saldoMutex;
    
    constexpr size_t TALLETUSTEN_MAARA = 1'000'000;
    constexpr size_t NOSTOJEN_MAARA = TALLETUSTEN_MAARA;
    
    std::thread talletusSaie([&saldo, &saldoMutex]() {
        for (size_t i = 0; i < TALLETUSTEN_MAARA; i++) {
            std::lock_guard<std::mutex> lukko(saldoMutex);
            saldo += 10;
        }
    });
    std::thread nostoSaie([&saldo, &saldoMutex]() {
        for (size_t i = 0; i < NOSTOJEN_MAARA; i++) {
            std::lock_guard<std::mutex> lukko(saldoMutex);
            saldo -= 10;
        }
    });
    
    talletusSaie.join();
    nostoSaie.join();
    
    std::cout << "Saldo: " << saldo << std::endl;
    
    return 0;
}
