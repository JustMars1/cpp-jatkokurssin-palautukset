/*
 
 Harjoitus 3
 
 Optimoi suorien tunnistus. Älä muuta kääntäjäoptioita.
 Pidä ohjelma yksisäikeisenä.
 
 Vertaa alkuperäistä ja optimoitua versiota:
 
 - kuinka paljon enemmän muistia optimoitu versio kuluttaa?
 - kuinka paljon nopeammin optimoitu versio toimii?
 
 Käännös: gcc straight.c -o straight
 
 */

#include <stdio.h>
#include <time.h>
#include <cstdint>

constexpr int SIZE = 1 << 16;
int is_straight_lookup_table[SIZE] = { 0 };
bool is_quadsorfullhouse_lookup_table[SIZE] = { 0 };

inline int is_straight(int *k);

inline std::int16_t hand_to_key(int k[5]);
inline int is_straight_lookup(int *k);

inline bool is_quadsorfullhouse(int k[5]);
inline int is_quadsorfullhouse_lookup(int k[5]);

void test_straight();
void test_quadsorfullhouse();

int main()
{
    for (int a = 2; a < 15; ++a) {
        for (int b = 2; b < 15; ++b) {
            for (int c = 2; c < 15; ++c) {
                for (int d = 2; d < 15; ++d) {
                    for (int e = 2; e < 15; ++e) {
                        int k[5] = { a, b, c, d, e };
                        is_straight_lookup_table[hand_to_key(k)] = is_straight(k);
                        is_quadsorfullhouse_lookup_table[hand_to_key(k)] = is_quadsorfullhouse(k);
                    }
                }
            }
        }
    }
    
    test_straight();
    test_quadsorfullhouse();
    
    return 0;
}

void test_straight() {
    /*
     
     Käydään läpi kaikki viiden kortin permutaatiot, ja lasketaan
     kuin monta suoraa (värisuoraa tai tavallista) löytyy.
     
     Ks. Wikipedia http://en.wikipedia.org/wiki/Poker_probability :
     
     Royal flush     4      combinations
     Straight flush  36     combinations
     Straight        10,200 combinations
     
     Koska 5 korttia voi olla 5 x 4 x 3 x 2 x 1 = 120 järjestyksessä,
     pitäisi suoria löytyä (10200 + 36 + 4) x 120 = 1228800 kappaletta.
     
     */
    int k0, k1, k2, k3, k4; /* Pakan kortit, 0..51*/
    int k[5]; /* Korttien numeroarvot, 2..14*/
    int suoria; /* Suorien lkm*/
    clock_t t1, t2;
    
    t1 = clock();
    suoria = 0;
    for (k0 = 0; k0 < 52; ++k0) {
        for (k1 = 0; k1 < 52; ++k1) { if (k1==k0)continue;
            for (k2 = 0; k2 < 52; ++k2) { if (k2==k0||k2==k1)continue;
                for (k3 = 0; k3 < 52; ++k3) { if (k3==k0||k3==k1||k3==k2)continue;
                    for (k4 = 0; k4 < 52; ++k4) { if (k4==k0||k4==k1||k4==k2||k4==k3)continue;
                        
                        /*
                         Lasketaan korttien numeroarvot ja
                         tarkistetaan onko suora.
                         */
                        k[0] = (k0 % 13) + 2;
                        k[1] = (k1 % 13) + 2;
                        k[2] = (k2 % 13) + 2;
                        k[3] = (k3 % 13) + 2;
                        k[4] = (k4 % 13) + 2;
                        if (is_straight_lookup(k))
                            ++suoria;
                    }}}}}
    t2 = clock();
    printf("Suoria     : %d kpl (oikea arvo 1228800)\n", suoria);
    printf("Aikaa kului: %.1fs\n", (t2 - t1) / (float)CLOCKS_PER_SEC);
}

void test_quadsorfullhouse() {
    int k0, k1, k2, k3, k4; /* Pakan kortit, 0..51*/
    int k[5]; /* Korttien numeroarvot, 2..14*/
    int nelosia_ja_tayskasia; /* Nelosien ja tayskasien lkm*/
    clock_t t1, t2;
    
    t1 = clock();
    nelosia_ja_tayskasia = 0;
    for (k0 = 0; k0 < 52; ++k0) {
        for (k1 = 0; k1 < 52; ++k1) { if (k1==k0)continue;
            for (k2 = 0; k2 < 52; ++k2) { if (k2==k0||k2==k1)continue;
                for (k3 = 0; k3 < 52; ++k3) { if (k3==k0||k3==k1||k3==k2)continue;
                    for (k4 = 0; k4 < 52; ++k4) { if (k4==k0||k4==k1||k4==k2||k4==k3)continue;
                        
                        /*
                         Lasketaan korttien numeroarvot ja
                         tarkistetaan onko neloset tai tayskasi.
                         */
                        k[0] = (k0 % 13) + 2;
                        k[1] = (k1 % 13) + 2;
                        k[2] = (k2 % 13) + 2;
                        k[3] = (k3 % 13) + 2;
                        k[4] = (k4 % 13) + 2;
                        if (is_quadsorfullhouse_lookup(k))
                            ++nelosia_ja_tayskasia;
                    }}}}}
    t2 = clock();
    printf("Nelosia ja tayskasia     : %d kpl (oikea arvo 524160)\n", nelosia_ja_tayskasia);
    printf("Aikaa kului              : %.1fs\n", (t2 - t1) / (float)CLOCKS_PER_SEC);
}


/*
 Parametrina 5 pelikortin numeroarvoa k, 2..14 (ei välttämättä
 numerojärjesteyksessä).
 
 Palauttaa 1, jos numeroarvot voidaan järjestää siten, että
 ne muodostavat suoran (esim. 4,5,6,7,8).
 
 Huom! Ässää (arvo 14) voidaan käyttää suorissa myös arvona 1
 (esim. 1,2,3,4,5).
 */
inline int is_straight(int *k)
{
    for (int i = 1; i < 5; ++i) {
        int tmp = k[i];
        int j = i - 1;
        
        while (j > -1 && tmp < k[j]) {
            k[j + 1] = k[j];
            --j;
        }
        k[j + 1] = tmp;
    }
    
    for (int i = 1; i < 4; ++i) {
        if (k[i - 1] + 1 != k[i]) {
            return 0;
        }
    }
    
    if (k[3] + 1 == k[4] || (k[0] == 2 && k[4] == 14)) {
        return 1;
    }
    
    return 0;
}

inline std::int16_t hand_to_key(int k[5]) {
    std::int16_t key = 0;
    key |= static_cast<std::int16_t>(1) << k[0];
    key |= static_cast<std::int16_t>(1) << k[1];
    key |= static_cast<std::int16_t>(1) << k[2];
    key |= static_cast<std::int16_t>(1) << k[3];
    key |= static_cast<std::int16_t>(1) << k[4];
    return key;
}

inline int is_straight_lookup(int *k) {
    return is_straight_lookup_table[hand_to_key(k)];
}

inline bool is_quadsorfullhouse(int k[5]) {
    int first = k[0];
    int second = 0;
    for (int i = 1; i < 5; i++) {
        if (k[i] == first || k[i] == second) {
            continue;
        }
        else if (second == 0) {
            second = k[i];
        }
        else {
            return false;
        }
    }
    return true;
}

inline int is_quadsorfullhouse_lookup(int k[5]) {
    return is_quadsorfullhouse_lookup_table[hand_to_key(k)];
}
