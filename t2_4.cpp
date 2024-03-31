#include <iostream>
#include <future>
#include <algorithm>
#include <numeric>
#include <cstddef>
#include <cstdint>
#include <vector>

// 4 Summa asynkronisilla funktiokutsuilla (1p)
// Sama kuin tehtävä 1, mutta käytä toteutukseen asynkronisia funktiokutsuja.

int main() {
    using Element = std::uint64_t;
    constexpr std::size_t ELEMENT_COUNT = 100'000;
    
    std::vector<Element> elements(ELEMENT_COUNT);
    std::iota(elements.begin(), elements.end(), 1);
    
    using Iterator = decltype(elements)::const_iterator;
    auto sumRange = [](Iterator begin, Iterator end) -> Element {
        return std::accumulate(begin, end, static_cast<Element>(0));
    };
    
    const std::size_t threadCount = std::max(std::thread::hardware_concurrency(), 1u);
    std::vector<std::future<std::uint64_t>> futures;
    futures.reserve(threadCount);
    
    const std::size_t elementsPerThread = elements.size() / threadCount;
    const std::size_t remainder = elements.size() % threadCount;
    
    {
        Iterator end = elements.begin();
        for (std::size_t i = 0; i < threadCount; ++i) {
            Iterator begin = end;
            end += elementsPerThread;
            if (i < remainder) {
                end++;
            }
            
            futures.push_back(std::async(std::launch::async, sumRange, begin, end));
        }
    }
    
    std::int64_t sum = 0;
    for (std::future<std::uint64_t>& future : futures) {
        sum += future.get();
    }
    
    std::cout << "Kokonaissumma on " << sum << std::endl;
    
    return 0;
}
