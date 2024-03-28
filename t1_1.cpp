#include <iostream>
#include <thread>
#include <algorithm>
#include <numeric>
#include <atomic>
#include <cstdint>

#include <vector>   // lisätty palautuksen jälkeen
#include <stddef.h> // lisätty palautuksen jälkeen

// 1 Summan laskeminen rinnakkaisesti (1p)
// Kirjoita ohjelma, joka laskee (suuren) taulukon (esim. vektorin) elementtien summan käyttäen useita säikeitä. Jaa
// taulukko yhtä suuriin osiin, anna jokainen osa eri säikeelle ja anna niiden laskea oman osansa summa. Lopuksi yhdistä
// kaikkien säikeiden tulokset saadaksesi kokonaissumman. Pääohjelma luo säikeet ja odottaa niiden päättymistä. Varmista,
// että tulos on oikein.
// Luo säikeet std::thread-luokan avulla.

int main() {
    using Element = std::uint64_t;
    constexpr size_t ELEMENT_COUNT = 100'000;
    
    std::vector<Element> elements(ELEMENT_COUNT);
    std::iota(elements.begin(), elements.end(), 1);
    std::atomic<Element> sum = 0;
    
    using Iterator = decltype(elements)::const_iterator;
    auto sumRange = [&sum](Iterator begin, Iterator end) {
        sum.fetch_add(std::accumulate(begin, end, static_cast<Element>(0)), std::memory_order_relaxed);
    };
    
    const size_t threadCount = std::max(std::thread::hardware_concurrency(), 1u);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    
    const size_t elementsPerThread = elements.size() / threadCount;
    for (size_t i = 0; i < threadCount - 1; ++i) {
        
        Iterator begin = elements.begin() + i * elementsPerThread;
        Iterator end = begin + elementsPerThread;
        threads.emplace_back(sumRange, begin, end);
    }
    
    // Lasketaan viimeinen osuus odetellessa muita
    sumRange(elements.begin() + (threadCount - 1) * elementsPerThread, elements.end());
    
    for (std::thread& thread : threads) {
        thread.join();
    }
    
    std::cout << "Kokonaissumma on " << sum << std::endl;
    
    return 0;
}
