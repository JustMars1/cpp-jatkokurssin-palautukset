#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <future>

// 6 Pelin laskentaa asynkronisilla funktiokutsuilla (1p)
// Sama kuin tehtävä 3, mutta käytä rinnakkaistamiseen asynkronisia funktiokutsuja.
// Yksinkertaisuuden vuoksi samanaikaisten kutsujen määrää ei tarvitse rajoittaa.

int main() {
    using namespace std::chrono_literals;
    
    class Game_Task {
    public:
        virtual void perform() = 0;
    };
    
    class Run_AI : public Game_Task {
    public:
        void perform() override {
            std::this_thread::sleep_for(8ms);
        }
    };
    
    class Update_World : public Game_Task {
    public:
        void perform() override {
            std::this_thread::sleep_for(16ms);
        }
    };
    
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<double, std::milli>;
    using TimePoint = Clock::time_point;
    
    constexpr std::size_t NUMBER_OF_RUN_AI_TASKS = 50;
    constexpr std::size_t NUMBER_OF_UPDATE_WORLD_TASKS = 50;
    
    std::vector<Game_Task*> tasks;
    
    for (std::size_t i = 0; i < NUMBER_OF_RUN_AI_TASKS; i++) {
        tasks.push_back(new Run_AI);
    }
    
    for (std::size_t i = 0; i < NUMBER_OF_UPDATE_WORLD_TASKS; i++) {
        tasks.push_back(new Update_World);
    }
    
    const std::size_t number_of_tasks = tasks.size();
    
    double duration_ms;
    {
        using Iterator = decltype(tasks)::iterator;
        TimePoint start_time = Clock::now();
        
        const std::size_t thread_count = std::max(std::thread::hardware_concurrency(), 1u);
        const std::size_t elements_per_thread = number_of_tasks / thread_count;
        const std::size_t remainder = number_of_tasks % thread_count;
        
        Iterator begin = tasks.begin();
        auto performTasks = [begin, elements_per_thread, remainder](std::size_t rangeIndex) {
            Iterator range_begin = begin + rangeIndex * elements_per_thread + std::min(rangeIndex, remainder);
            Iterator range_end = begin + (rangeIndex + 1) * elements_per_thread + std::min((rangeIndex + 1), remainder);
            for (Iterator task_iterator = range_begin;  task_iterator != range_end; ++task_iterator) {
                (*task_iterator)->perform();
            }
        };
        
        std::vector<std::future<void>> futures;
        futures.reserve(thread_count);
        for (std::size_t i = 0; i < thread_count; ++i) {
            futures.push_back(std::async(std::launch::async, performTasks, i));
        }
        
        for (std::future<void>& future : futures) {
            future.wait();
        }
        
        TimePoint end_time = Clock::now();
        duration_ms = Duration(end_time - start_time).count();
    }
    std::cout << "Duration: " << duration_ms << " ms\n";
    
    return 0;
}
