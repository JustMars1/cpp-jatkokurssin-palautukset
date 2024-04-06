#include <ostream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <string_view>
#include <string>
#include <thread>
#include <algorithm>
#include <cstddef>
#include <utility>

#include <atomic>
#include <vector>

// 8 Log_Ptr, osa 1 (1 piste) - TEHTÄVÄT 8-11 OVAT MINIPROJEKTI 2 (VOI TEHDÄ PAREISSA)
// Tutustu standardikirjaston smart pointereihin, esim.:
// · http://umich.edu/~eecs381/handouts/C++11_smart_ptrs.pdf (kommentoituna myös Omassa)
// · http://www.informit.com/articles/article.aspx?p=2085179 (kannattaa kokeilla esimerkin
// koodinpätkiä)
// · https://www.codeproject.com/Articles/15351/Implementing-a-simple-smart-pointer-in-c (smart
// pointer -luokan esimerkkitoteutus)
// Voit kokeilla shared_ptr:a vaikka seuraavasti:
// Laadi jokin yksinkertainen luokka, jonka konstruktori ja destruktori tulostavat jotain. Anna luokasta luotu olio shared_ptr:n huolehdittavaksi. Välitä shared_ptr parametrina johonkin funktioon, palauta shared_ptr funktion paluuarvona ja tee shared_ptr:ien välisiä sijoituksia. Kiinnitä huomiota, missä vaiheessa luomasi olio tuhotaan. Mikä on reference count ohjelman missäkin vaiheessa?
// Suunnittele ja toteuta luokka Log_Ptr. Luokalle annetaan huolehdittavaksi joku toinen olio, joka on varattu heapista. Viitatun olion käytöstä kirjoitetaan rivejä lokitiedostoon (esim. tiedostoon ”log_ptr.txt” tai suoraan konsoliin).
// Log_Ptr:lla tulee olla seuraavat ominaisuudet (kirjoita testiohjelma):
// · Log_Ptr ylläpitää osoitinta viitattuun olioon. Olio voi olla minkä tahansa tyyppinen (template-
// luokka).
// · Log_Ptr:lle annetaan viitattava olio konstruktorin parametrina. Konstruktori kirjoittaa vastaavan rivin lokitiedostoon:
// <aikaleima> omistajuus siirretty <viitatun olion muistiosoite>
// · Viitattu olio tuhotaan Log_Ptr:n destruktorissa. Destruktori kirjoittaa vastaavan rivin lokitiedostoon:
// <aikaleima> olio tuhottu <viitatun olion muistiosoite>
// · Log_Ptr:n sijoitusoperaattorin ja kopiokonstrukorin käyttö estetään (vihje:
// https://www.geeksforgeeks.org/explicitly-defaulted-deleted-functions-c-11/)

// 9 Log_Ptr, osa 2 (1 piste)
// Lisää Log_Ptr-luokkaan seuraavat ominaisuudet:
// · Lisää luokkaan -> operaattori, jonka kautta viitattuun olioon pääsee käsiksi. Operaattori kirjoittaa vastaavan rivin lokitiedostoon:
// <aikaleima> operator-> <viitatun olion muistiosoite>
// · Lisää luokkaan * operaattori, jonka kautta viitattuun olioon pääsee käsiksi. Operaattori kirjoittaa vastaavan rivin lokitiedostoon:
// <aikaleima> operator* <viitatun olion muistiosoite>

// 10 Log_Ptr, osa 3 (2 pistettä)
// Lisää Log_Ptr-luokkaan reference counting -mekanismi. Viitattu olio siis tuhotaan vasta, kun count = 0. Toteuta ainakin sijoitusoperaattori ja kopiokonstruktori. Muista kirjoittaa myös sopivat lokiviestit.

// 11 Log_Ptr, osa 4 (1 piste)
// Tee Log_Ptr-luokasta ”säiekestävä” (thread-safe). Reference counting -mekanismin tulee toimia, vaikka Log_Ptr-olion kopioita käytettäisiin samaan aikaan eri säikeistä.

template <typename T>
class Log_Ptr {
public:
    Log_Ptr(T* t) : Log_Ptr(t, &std::cout) {}
    Log_Ptr(T* t, std::ostream* output) : t(t), control_block(new Control_Block(output, t)) {
        control_block->log_thread_safe("omistajuus siirretty", t);
        control_block->retain(t);
    }
    
    Log_Ptr(const Log_Ptr& other) : t(other.t), control_block(other.control_block) {
        control_block->retain(t);
    }
    
    ~Log_Ptr() {
        control_block->release(t);
    }
    
    Log_Ptr& operator=(const Log_Ptr& other) {
        control_block->release(t);
        
        t = other.t;
        control_block = other.control_block;
        control_block->retain(t);
        return *this;
    }
    
    T* operator->() {
        control_block->log_thread_safe("operator->", t);
        return t;
    }
    
    const T* operator->() const {
        control_block->log_thread_safe("operator->", t);
        return t;
    }
    
    T& operator*() {
        control_block->log_thread_safe("operator*", t);
        return *t;
    }
    
    const T& operator*() const {
        control_block->log_thread_safe("operator*", t);
        return *t;
    }
private:
    class Control_Block {
    public:
        Control_Block(std::ostream* output, const T* t) : output(output), count(0) {}
        
        void retain(const T* t) {
            std::lock_guard lock(mutex);
            count++;
            log_count(t);
        }
        
        void release(T* t) {
            bool should_destruct;
            {
                std::lock_guard lock(mutex);
                count--;
                log_count(t);
                should_destruct = count == 0;
            }
            
            if (should_destruct) {
                delete t;
                log("olio tuhottu", t);
                delete this;
            }
        }
        
        void log_thread_safe(std::string_view&& msg, const T* t) {
            std::lock_guard lock(mutex);
            log(std::move(msg), t);
        }
    private:
        std::ostream* output;
        std::mutex mutex;
        std::size_t count;
        
        void log_count(const T* t) {
            std::string msg = "count = " + std::to_string(count);
            log(msg, t);
        }
        
        void log(std::string_view&& msg, const T* t) {
            std::time_t current_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now());
            std::size_t padding = std::max(20 - msg.length(), static_cast<std::string::size_type>(0));
            *output << std::put_time(std::localtime(&current_time), "<%FT%T%z> ") << msg << std::string(padding, ' ') <<" <" << t << ">" << std::endl;
        }
    };
    
    T* t;
    Control_Block* control_block;
};

int main() {
    Log_Ptr<std::string> a(new std::string("123"));
    std::cout << *a << std::endl;
    
    Log_Ptr<std::string> b(new std::string("Temp"));
    std::cout << *b << std::endl;
    b = a;
    std::cout << *b << std::endl;
    b->push_back('4');
    std::cout << *a << std::endl;
    
    std::atomic<bool> wait = true;
    auto lambda = [a, &wait]() {
        while (wait) {}
        for (std::size_t i = 0; i < 10; i++) {
            Log_Ptr<std::string> c = a;
        }
    };
    
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < 100; i++) {
        threads.emplace_back(lambda);
    }
    
    wait = false;
    
    for (std::thread& thread : threads) {
        thread.join();
    }
    
    return 0;
}
