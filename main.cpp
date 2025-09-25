#include <iostream>
#include <fstream>
using namespace std;

void* my_memcpy(void* dst, const void* src, unsigned long n) {
    unsigned char* d = (unsigned char*)dst;
    const unsigned char* s = (const unsigned char*)src;
    for (unsigned long i = 0; i < n; ++i) d[i] = s[i];
    return dst;
}


//funcion que me da la longitud de una cadena terminada en \0
unsigned long my_strlen(const char* s) {
    if (!s) return 0;
    unsigned long n = 0; while (s[n] != '\0') ++n; return n;
}

//funcion para comparar cadenas caracter a caraceter
int my_strncmp(const char* a, const char* b, unsigned long n) {
    for (unsigned long i = 0; i < n; ++i) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (ca != cb) return (ca < cb) ? -1 : 1;
        if (ca == '\0') return 0;
    }
    return 0;
}

//convierte numero a cadena
void utoa_dec(unsigned long v, char* out) {
    char tmp[32];
    int p = 0;
    if (v == 0) { out[0] = '0'; out[1] = '\0'; return; }
    while (v > 0) { unsigned long d = v % 10; tmp[p++] = (char)('0' + d); v /= 10; }
    for (int i = 0; i < p; ++i) out[i] = tmp[p - 1 - i];
    out[p] = '\0';
}

unsigned char* read_file_binary(const char* path, unsigned long &size_out) {
    size_out = 0;
    fstream f(path, ios::in | ios::binary);
    if (!f.good()) return nullptr;
    f.seekg(0, ios::end);
    std::streamoff len = f.tellg();
    if (len <= 0) { f.close(); return nullptr; }
    f.seekg(0, ios::beg);
    unsigned long size = (unsigned long)len;
    unsigned char* buf = new unsigned char[size];
    f.read((char*)buf, (std::streamsize)size);
    f.close();
    size_out = size;
    return buf;
}
char* read_file_text(const char* path) {
    fstream f(path, ios::in);
    if (!f.good()) return nullptr;
    f.seekg(0, ios::end);
    std::streamoff len = f.tellg();
    if (len < 0) { f.close(); return nullptr; }
    f.seekg(0, ios::beg);
    unsigned long n = (unsigned long)len;
    char* buf = new char[n + 1];
    f.read(buf, (std::streamsize)n);
    buf[n] = '\0';
    f.close();
    return buf;
}
bool write_text_file(const char* path, const char* data) {
    fstream f(path, ios::out | ios::binary);
    if (!f.good()) return false;
    unsigned long n = my_strlen(data);
    f.write(data, (std::streamsize)n);
    f.close();
    return true;
}

// funcion para buscar si la pista esta en el texto descomprimido
bool contains_subcadena(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    unsigned long nlen = my_strlen(needle);
    if (nlen == 0) return true;
    const char* p = haystack;
    while (*p) {
        if (my_strncmp(p, needle, nlen) == 0) return true;
        ++p;
    }
    return false;
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
