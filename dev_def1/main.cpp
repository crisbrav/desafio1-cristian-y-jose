#include <iostream>
#include <fstream>
using namespace std;

void* my_memcpy(void* dst, const void* src, unsigned long n) {
    unsigned char* d = (unsigned char*)dst;
    const unsigned char* s = (const unsigned char*)src;
    for (unsigned long i = 0; i < n; ++i) d[i] = s[i];
    return dst;
}

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

char* decompress_lz78(const unsigned char* data, unsigned long size) {
    if (!data || size < 3) return nullptr;

    unsigned long maxEntries = (size / 3) + 8;
    char** dict = new char*[maxEntries];
    unsigned long* dlen = new unsigned long[maxEntries];
    for (unsigned long i = 0; i < maxEntries; ++i) { dict[i] = 0; dlen[i] = 0; }

    unsigned long dictCount = 0, pos = 0, totalLen = 0;

    while (pos + 2 < size) {
        unsigned char hi = data[pos];
        unsigned char lo = data[pos + 1];
        unsigned short idx = (unsigned short)((hi << 8) | lo); // big-endian
        unsigned char c = data[pos + 2];
        pos += 3;

        if (idx == 0) {
            char* s = new char[2];
            s[0] = (char)c; s[1] = '\0';
            dict[dictCount] = s; dlen[dictCount] = 1;
            totalLen += 1; dictCount++;
        } else {
            if ((unsigned long)idx > dictCount) {
                for (unsigned long i=0;i<dictCount;++i) delete[] dict[i];
                delete[] dict; delete[] dlen;
                return nullptr;
            }
            unsigned long plen = dlen[idx - 1];
            char* s = new char[plen + 2]; // +1 char + '\0'
            my_memcpy(s, dict[idx - 1], plen);
            s[plen] = (char)c; s[plen + 1] = '\0';
            dict[dictCount] = s; dlen[dictCount] = plen + 1;
            totalLen += (plen + 1); dictCount++;
        }
        if (dictCount >= maxEntries - 2) break;
    }

    if (totalLen == 0) {
        delete[] dlen; delete[] dict;
        return nullptr;
    }

    char* out = new char[totalLen + 1];
    unsigned long op = 0;
    for (unsigned long i = 0; i < dictCount; ++i) {
        my_memcpy(out + op, dict[i], dlen[i]);
        op += dlen[i];
        delete[] dict[i];
    }
    out[op] = '\0';
    delete[] dict; delete[] dlen;
    return out;
}




int main() {

    return 0;
}
