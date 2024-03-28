#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <cmath>

#include <vector>   // lisätty palautuksen jälkeen
#include <stddef.h> // lisätty palautuksen jälkeen

// 3 Rinnakkaista pelin laskentaa (2p)
// Oletetaan, että pelin suorittamille toimenpiteille (tekoälyn ajaminen, pelimaailman päivitys, jne.) on määritelty
// yhteinen yliluokka Game_Task. Luokassa on määritelty puhdas virtuaalifunktio perform(), joka suorittaa ko. toimenpiteen:
// virtual void Game_Task::perform() = 0;
// Kaikki toimenpiteet ovat toisistaan riippumattomia (eivät käytä samaa dataa tms.). Peli ylläpitää vektoria
// toimenpiteistä. Toimenpiteet suoritetaan peräkkäin silmukassa:
// std::vector<Game_Task*> tasks;
// ...
// int number_of_tasks = tasks.size();
// for (int i = 0; i < number_of_tasks; i++)
// {
//    task[i]->perform();
// }
// // Continue only after all tasks are complete!
// Hahmottele, miten em. silmukka voitaisiin rinnakkaistaa käyttäen std::thread -luokkaa. Etsi ohjelmallinen tapa
// selvittää, kuinka montaa samanaikaista säiettä laitteistosi pystyy suorittamaan fyysisesti; käytä
// rinnakkaistamiseen yksi säie vähemmän.
// Luo testaamista varten yliluokka Game_Task ja sille muutama aliluokka, joiden perform-funktio kuluttaa sopivasti
// prosessoriaikaa johonkin. Mittaa rinnakkaistamisesta saamasi suoritusaikahyöty.

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
    
    constexpr size_t NUMBER_OF_RUN_AI_TASKS = 50;
    constexpr size_t NUMBER_OF_UPDATE_WORLD_TASKS = 50;
    
    std::vector<Game_Task*> tasks;
    
    for (size_t i = 0; i < NUMBER_OF_RUN_AI_TASKS; i++) {
        tasks.push_back(new Run_AI);
    }
    
    for (size_t i = 0; i < NUMBER_OF_UPDATE_WORLD_TASKS; i++) {
        tasks.push_back(new Update_World);
    }
    
    size_t number_of_tasks = tasks.size();
    
    double single_thread_duration_ms;
    {
        Clock::time_point start_time = Clock::now();
        
        for (int i = 0; i < number_of_tasks; i++)
        {
            tasks[i]->perform();
        }
        
        Clock::time_point end_time = Clock::now();
        single_thread_duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    
    std::cout << "Single-thread duration : " << single_thread_duration_ms << " ms\n";
    
    double multi_thread_duration_ms;
    {
        using Iterator = decltype(tasks)::iterator;
        auto performTasks = [](Iterator begin, Iterator end) {
            for (Iterator task_iterator = begin;  task_iterator != end; ++task_iterator) {
                (*task_iterator)->perform();
            }
        };
        
        Clock::time_point start_time = Clock::now();
        
        const size_t number_of_threads = std::max(std::thread::hardware_concurrency(), 1u);
        std::vector<std::thread> threads;
        threads.reserve(number_of_threads);
        
        const size_t tasks_per_thread = number_of_tasks / number_of_threads;
        for (size_t i = 0; i < number_of_threads - 1; ++i) {
            
            Iterator begin = tasks.begin() + i * tasks_per_thread;
            Iterator end = begin + tasks_per_thread;
            threads.emplace_back(performTasks, begin, end);
        }
        
        // Lasketaan viimeinen osuus odetellessa muita
        performTasks(tasks.begin() + (number_of_threads - 1) * tasks_per_thread, tasks.end());
        
        for (std::thread& thread : threads) {
            thread.join();
        }
        
        Clock::time_point end_time = Clock::now();
        multi_thread_duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    std::cout << "Multi-thread duration  : " << multi_thread_duration_ms << " ms\n";
    
    double difference = single_thread_duration_ms - multi_thread_duration_ms;
    
    std::cout
    << "Multi-thread was " << std::abs(difference) << " ms " << (difference < 0.0 ? "slower" : "faster")
    << " (" << single_thread_duration_ms / multi_thread_duration_ms << "x)" << std::endl;
    
    return 0;
}
