#include <cstddef>
#include <exception>
#include <iostream>

// 16 Poikkeukset (1p)
// Varaa niin paljon (tai niin monta kertaa) muistia new:llä, että muistinvaraus ei onnistu.
// Mikä poikkeus tästä aiheutuu (ota poikkeus kiinni)? Miten paljon muistia tuli varata?
// Pitäisikö muistinvarauksen epäonnistumiseen varautua ohjelmoitaessa?

int main() {
    
    try {
        while (true) {
            int* allocation = new int[1'000'000];
        }
    }
    catch(const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}


// 17 Poikkeukset (1p)
// Tutustu poikkeusten käytöstä käytyyn keskusteluun:
// https://gamedev.stackexchange.com/questions/46424/try-catch-or-ifs-for-error-handling-in-c
// Kannattaako poikkeuksia käyttää C++:ssa? Miksi tai miksi ei?

// V: Yleinen mielipide on, että poikkeuksia ei kannata käyttää C++:ssa. Niitä kannattaa välttää mm.
// virhealttiuden (helppo kirjoittaa vahingossa epäturvallista koodia), vaihtelevan kääntäjä tuen
// (osa kääntäjistä ei tue niitä kunnolla, osa ei ollenkaan) ja suorityskyky haittojen vuoksi.
