#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

// 14 Kopiokonstruktori ja sijoitusoperaattori (1p)
// Toteuta luokka Big_Data, jonka konstruktorin parametrina annetaan varattavan ”bufferin” koko tavuina. 
// Konstruktori varaa muistin malloc:lla, ja destruktori vapauttaa muistin.
// Toteuta luokalle kopiokonstruktori ja sijoitusoperaattori, niin että ao. ohjelmarivit toimivat korrektisti.
// Eri oliot eivät siis saa osoittaa samaan bufferiin, vaan tarpeen tullen bufferin sisältö kopioidaan toisen olion bufferiin. 
// Testaa.
//           Big_Data a(1024);
//           Big_Data b(1024);
//           a = a;
//           a = b;
//           BigData c(a);

// 15 Move-konstruktori ja move-sijoitusoperaattori (1p)
// Toteuta Big_Data-luokalle move-konstruktori ja move-sijoitusoperaattori.
// Nämä saavat parametrina rvalue-referenssin, ”varastavat” sen sisällön ja sen jälkeen ”resetoivat” alkuperäisen rvaluen.
// Testaa toimintaa esim. seuraavilla ohjelmariveillä:
//           Big_Data a(Big_Data(1024));
//           Big_Data b(1024);
//           b = Big_Data(1024);
//           BigData c(std::move(a));

class Big_Data {
public:
    Big_Data(std::size_t size) : size(size) {
        data = static_cast<std::byte*>(std::malloc(size * sizeof(std::byte)));
    }
    
    ~Big_Data() {
        std::free(data);
    }
    
    Big_Data(const Big_Data& other) : size(other.size) {
        data = static_cast<std::byte*>(std::malloc(size * sizeof(std::byte)));
        std::memcpy(data, other.data, size);
    }
    
    Big_Data& operator=(const Big_Data& other) {
        if (size != other.size) {
            std::free(data);
            size = other.size;
            data = static_cast<std::byte*>(std::malloc(size * sizeof(std::byte)));
        }
        std::memcpy(data, other.data, size);
        return *this;
    }
    
    Big_Data(Big_Data&& other) : size(other.size), data(other.data) {
        other.data = nullptr;
        other.size = 0;
    }
    
    Big_Data& operator=(Big_Data&& other) {
        std::free(data);
        size = other.size;
        data = other.data;
        other.data = nullptr;
        other.size = 0;
        
        return *this;
    }
    
    std::byte* get_data() {
        return data;
    }
private:
    std::byte* data;
    std::size_t size;
};

int main() {
    {
        Big_Data a(1024);
        Big_Data b(1024);
        a = a;
        a = b;
        Big_Data c(a);
        
        std::cout << "14.\n";
        std::cout << "a.data: " << a.get_data() << std::endl;
        std::cout << "b.data: " << b.get_data() << std::endl;
        std::cout << "c.data: " << c.get_data() << std::endl;
    }
    
    {
        Big_Data a(Big_Data(1024));
        Big_Data b(1024);
        b = Big_Data(1024);
        Big_Data c(std::move(a));
        
        std::cout << "\n15.\n";
        std::cout << "a.data: " << a.get_data() << std::endl;
        std::cout << "b.data: " << b.get_data() << std::endl;
        std::cout << "c.data: " << c.get_data() << std::endl;
    }
}
