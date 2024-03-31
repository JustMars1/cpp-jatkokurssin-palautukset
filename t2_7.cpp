#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <iostream>

// 7 Työjono säikeille (3p) - MINIPROJEKTI 1
// Suunnittele ja toteuta luokka TaskQueue (”työjono”). Työjonoon voidaan lisätä ulkopuolelta työtehtäviä, joita sitten
// työjonoon kuuluvat säikeet suorittavat sitä mukaan kuin ehtivät. Luokalla tulee olla seuraavat ominaisuudet:
//      · Konstruktori TaskQueue(int nof_threads)
//        Luo työjonon, ja halutun määrän säikeitä töiden suorittamiseen.
//      · Jäsenfunktio void addJob(Game_Task task)
//        Funktio lisää työjonoon uuden työn. Työ on jokin laskennallinen tehtävä, ks. tehtävä 3. Voit
//        välittää työn sellaisenaan (ei siis referenssinä tai osoittimena).
//        Funktio palaa kutsujalle välittömästi, se ei siis jää odottamaan työn valmistumista.
//      · Destruktori
//        Destruktori tyhjentää työjonon ja odottaa, että kaikki käynnissä olevan työt ovat valmistuneet. Sen jälkeen destruktori
//        herättää kaikki säikeet niin, että ne lopettavat toimintansa. Destruktori palaa vasta sitten, kun säikeitä ei enää ole
//        suorituksessa. Destruktorin ollessa suorituksessa uusia töitä ei voi enää lisätä.
// Huomioita toteutuksesta:
//      · Säikeet odottavat töitä ”unilla”, ehtomuuttujan (conditional variable) avulla. Säikeet eivät siis tee ”busy wait”:itä.
//        addJob signaloi säikeitä saapuneesta työstä (notify_one).
//      · Käytä töiden tietorakenteena std::queue. Ehtona säikeen jatkamiselle signaloinnin jälkeen on se, jono ei ole tyhjä
//        (.size() > 0).
//      · Joudut suojaamaan std::queue:n käsittelyn mutexilla, ks. työtilan esimerkkikoodi.
//      · Kun säie on saanut työn suoritettua, sen on syytä tarkistaa, olisiko jonossa uusi työ sen suoritettavaksi, eikä mennä
//        suoraan odottamaan ehtomuuttujan signaalia. Huomaatko miksi?

class Game_Task {
public:
    void perform() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
    }
};

class TaskQueue {
public:
    TaskQueue(int nof_threads) : is_destructing(false), number_of_completed_tasks(0) {
        threads.reserve(nof_threads);
        for (int i = 0; i < nof_threads; i++) {
            threads.emplace_back(&TaskQueue::TaskLoop, this);
        }
    }
    
    ~TaskQueue() {
        is_destructing = true;
        tasks_cv.notify_all();
        
        for (std::thread& thread : threads) {
            thread.join();
        }
    }
    
    void addJob(Game_Task task) {
        {
            std::lock_guard lock(tasks_mutex);
            tasks.push(task);
        }
        tasks_cv.notify_one();
    }
    
    int get_number_of_completed_tasks() { return number_of_completed_tasks; } // extra
private:
    std::vector<std::thread> threads;
    std::queue<Game_Task> tasks;
    
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::atomic<bool> is_destructing;
    
    std::atomic<int> number_of_completed_tasks; // extra
    
    void TaskLoop() {
        while (true) {
            thread_local Game_Task task;
            {
                std::unique_lock lock(tasks_mutex);
                tasks_cv.wait(lock, [this]() {
                    return !tasks.empty() || is_destructing;
                });
                
                if (is_destructing) {
                    return;
                }
                
                task = tasks.front();
                tasks.pop();
            }
            
            task.perform();
            number_of_completed_tasks++;
        }
    }
};

int main() {
    TaskQueue task_queue(std::max(std::thread::hardware_concurrency(), static_cast<unsigned>(1)));
    
    constexpr size_t NUMBER_OF_TASKS = 100;
    
    for (size_t i = 0; i < NUMBER_OF_TASKS; i++) {
        task_queue.addJob(Game_Task());
    }
    
    while (task_queue.get_number_of_completed_tasks() < NUMBER_OF_TASKS) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
        std::cout << task_queue.get_number_of_completed_tasks() << "/" << NUMBER_OF_TASKS << std::endl;
    }
    std::cout << "Completed all tasks\n";
}
