#include <string>
#include <iostream>
#include <utility>
#include <vector>

// 13 move-semantiikka (1p)
// Kirjoita ohjelma, joka luo kaksi std::string merkkijonoa, s1 ja s2.
// Sijoita s1:n arvoksi jokin merkkijono, esim. ”kissa”. s2 on aluksi tyhjä.

// a) Sijoita s1:n arvoksi s2. Tutki debuggerilla tilannetta ennen tai jälkeen sijoituksen.
// Yritä löytää std::string-olioiden sisältä osoitin varsinaiseen merkkijonodataan. Mitä huomaat?

// b) Sijoita s1:n arvoksi s2, mutta muuta s2 ensin rvalue-referenssiksi (std::move).
// Tutki debuggerilla tilannetta ennen tai jälkeen sijoituksen. Yritä löytää std::string-olioiden sisältä osoitin varsinaiseen merkkijonodataan. Mitä huomaat?

// c) Luo vielä merkkijonovektori (std::vector). Lisää luo std::string s3, jonka arvona on ”koira”. Lisää s3 vektoriin (push_back) normaalisti,
// ja vaihtoehtoisesti muuttamalla s3 ensin rvalue- referenssiksi (std::move). Tutki sijoituksia debuggerilla. Mitä huomaat?


int main() {
    /*
    std::string s1 = "kissakissakissakissakis"; // 23 + \0 = 24 = 3 bytes
    */
    std::string s1 = "kissakissakissakissaki"; // 22 + \0 = 23 = 2 bytes 7 bits
    std::string s2;
    
    // a)
    std::cout << "a)\n";
    std::cout << "before\n";
    std::cout << "s1 " << static_cast<const void*>(s1.c_str()) << std::endl;
    std::cout << "s2 " << static_cast<const void*>(s2.c_str()) << std::endl;
    s1 = s2;
    std::cout << "after\n";
    std::cout << "s1 " << static_cast<const void*>(s1.c_str()) << std::endl;
    std::cout << "s2 " << static_cast<const void*>(s2.c_str()) << std::endl;
    
    // b)
    std::cout << "\nb)\n";
    std::cout << "before move\n";
    std::cout << "s1 " << static_cast<const void*>(s1.c_str()) << std::endl;
    std::cout << "s2 " << static_cast<const void*>(s2.c_str()) << std::endl;
    s1 = std::move(s2);
    std::cout << "after move\n";
    std::cout << "s1 " << static_cast<const void*>(s1.c_str()) << std::endl;
    std::cout << "s2 " << static_cast<const void*>(s2.c_str()) << std::endl;
    
    // c)
    
    /*
    std::string s3 = "koirakoirakoirakoirakoi"; // 23 + \0 = 24 = 3 bytes
    */
    
    std::string s3 = "koirakoirakoirakoirako"; // 22 + \0 = 23 = 2 bytes 7 bits
    std::vector<std::string> strings;
    std::cout << "\nc)\n";

    std::cout << "before\n";
    std::cout << "s3 " << static_cast<const void*>(s3.c_str()) << std::endl;
    strings.push_back(s3);
    std::cout << "after\n";
    std::cout << "s3 " << static_cast<const void*>(s3.c_str()) << std::endl;
    
    std::cout << "\nbefore move\n";
    std::cout << "s3 " << static_cast<const void*>(s3.c_str()) << std::endl;
    strings.push_back(std::move(s3));
    std::cout << "after move\n";
    std::cout << "s3 " << static_cast<const void*>(s3.c_str()) << std::endl;
}
