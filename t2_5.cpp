#include <cstdint>
#include <cstddef>
#include <vector>
#include <numeric>
#include <algorithm>
#include <execution>
#include <chrono>
#include <iostream>

// 5 Taulukon käsittely standardikirjaston avulla (1p)
// Luo N alkion kokoinen (suuri) taulukko, johon on talletettu luvut 0...N-1. Kirjoita ohjelma, joka käy kasvattamassa jokaisen alkion arvoa yhdellä:
// - käytä std::for_each-algoritmia
// - kokeile algoritmille erilaisia suorismääreitä:
//      - std::execution::seq
//      - std::execution::par
//      - std::execution::par_unseq
// Havaitsetko näin yksinkertaisessa tehtävässä suorituskykyeroja eri suoritusmääreiden välillä? Mitä eri suoritusmääreet tarkoittavat?
// Huomaa, että tehtävä vaatii vähintään C++17-version.

int main() {
    using Number = std::uint64_t;
    constexpr std::size_t N = 100'000;
    
    std::vector<Number> numbers(N);
    std::iota(numbers.begin(), numbers.end(), 0);
    
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<double, std::milli>;
    using TimePoint = Clock::time_point;
    
    {
        TimePoint start_time = Clock::now();
        std::for_each(std::execution::seq, numbers.begin(), numbers.end(), [](Number& num) {
            num += 1;
        });
        TimePoint end_time = Clock::now();
        std::cout << "seq       : " << Duration(end_time - start_time).count() << " ms\n";
    }
    
    {
        TimePoint start_time = Clock::now();
        std::for_each(std::execution::par, numbers.begin(), numbers.end(), [](Number& num) {
            num += 1;
        });
        TimePoint end_time = Clock::now();
        std::cout << "par       : " << Duration(end_time - start_time).count() << " ms\n";
    }
    
    {
        TimePoint start_time = Clock::now();
        std::for_each(std::execution::par_unseq, numbers.begin(), numbers.end(), [](Number& num) {
            num += 1;
        });
        TimePoint end_time = Clock::now();
        std::cout << "par_unseq : " << Duration(end_time - start_time).count() << " ms\n";
    }
}

// seq       : 0.956 ms
// par       : 0.877 ms
// par_unseq : 0.385 ms
