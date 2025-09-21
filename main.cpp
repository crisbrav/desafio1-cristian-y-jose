#include <iostream>
#include <fstream>
using namespace std;


unsigned char ror8(unsigned char v, int r) {
    r &= 7; // 0..7
    return (unsigned char)((v >> r) | (v << (8 - r)));
}
unsigned char rol8(unsigned char v, int r) {
    r &= 7;
    return (unsigned char)((v << r) | (v >> (8 - r)));
}

unsigned char* try_decrypt(const unsigned char* enc, unsigned long size, int rot_n, unsigned char K) {
    if (!enc || size == 0) return nullptr;
    unsigned char* dec = new unsigned char[size];
    for (unsigned long i = 0; i < size; ++i) {
        unsigned char b  = enc[i];
        unsigned char bx = (unsigned char)(b ^ K);  // deshacer XOR
        unsigned char br = ror8(bx, rot_n);         // rotación inversa
        dec[i] = br;
    }
    return dec;
}

int main() {

    return 0;
}
