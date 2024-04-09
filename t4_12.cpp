#include <iostream>
#include <vector>
#include <algorithm>

// 12 Lambda-funktiot (2p)
// Sinulla on roolipelissä kokonaislukuvektori (std::vector) kokonaislukuja, jotka kuvaavat vihollisten hitpointseja (HP).
// Onnistuneen loitsusi ansiosta jokaista HP:ta tulisi vähentää 100:lla (tai asettaa 0:aan, jos HP on tällä hetkellä 100 tai alempi).
// Käytä vähennyksen toteutukseen std::for_each -algoritmia. for_each haluaa saada alkioille tehtävän operaation parametrina;
// toteuta se seuraavilla eri tavoilla:
// a. erillisenä funktiona
// b. funktio-objektina (tutustu tarvittaessa)
// c. lambda-funktiona
// d. nimettynä lambda-funktiona
// Järjestä lopuksi vektori suurimmasta pienimpään käyttäen std::sort-funktiota.
// Anna järjestyskriteeri (laskeva järjestys) sort:lle lambda-funktiona.

void decrease_hp_function(int& hp) {
    hp = std::max(0, hp - 100);
}

class decrease_hp_function_object {
public:
    void operator()(int& hp) const {
        hp = std::max(0, hp - 100);
    }
};

int main() {
    std::vector<int> enemy_hps { 200, 6400, 3200, 800, 400, 1600, 50, 100, 12800 };
    
    // a.
    std::for_each(enemy_hps.begin(), enemy_hps.end(), decrease_hp_function);
    
    // b.
    std::for_each(enemy_hps.begin(), enemy_hps.end(), decrease_hp_function_object());
    
    // c.
    std::for_each(enemy_hps.begin(), enemy_hps.end(), [](int& hp) {
        hp = std::max(0, hp - 100);
    });
    
    // d.
    auto decrease_hp_named_lambda_function = [](int& hp) {
        hp = std::max(0, hp - 100);
    };
    std::for_each(enemy_hps.begin(), enemy_hps.end(), decrease_hp_named_lambda_function);
    
    std::sort(enemy_hps.begin(), enemy_hps.end(), [](int a, int b) {
        return a > b;
    });
    
    
    for (int hp : enemy_hps) {
        std::cout << hp << " ";
    }
    
    return 0;
}
