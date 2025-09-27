#include <iostream>
#include <fstream>
using namespace std;

unsigned long contar_chars(const char* s) {
    if (!s) return 0;
    unsigned long n = 0; while (s[n] != '\0') ++n; return n;
}
int comparar_n_chars(const char* a, const char* b, unsigned long n) {
    for (unsigned long i = 0; i < n; ++i) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (ca != cb) return (ca < cb) ? -1 : 1;
        if (ca == '\0') return 0;
    }
    return 0;
}
void* my_memcpy(void* dst, const void* src, unsigned long n) {
    unsigned char* d = (unsigned char*)dst;
    const unsigned char* s = (const unsigned char*)src;
    for (unsigned long i = 0; i < n; ++i) d[i] = s[i];
    return dst;
}

void numero_a_dec(unsigned long v, char* out) {
    char tmp[32]; int p = 0;
    if (v == 0) { out[0]='0'; out[1]='\0'; return; }
    while (v > 0) { unsigned long d = v % 10; tmp[p++] = (char)('0'+d); v /= 10; }
    for (int i=0;i<p;++i) out[i] = tmp[p-1-i];
    out[p]='\0';
}
char to_hex_nibble(unsigned int v) { v &= 0xF; return (v<10)?('0'+v):('A'+(v-10)); }
void byte_to_hex(unsigned char b, char* out) {
    out[0]='0'; out[1]='x'; out[2]=to_hex_nibble(b>>4); out[3]=to_hex_nibble(b); out[4]='\0';
}

unsigned char rotacion_der(unsigned char v, int r) {
    r &= 7; return (unsigned char)((v >> r) | (v << (8 - r)));
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
    unsigned long n = contar_chars(data);
    f.write(data, (std::streamsize)n);
    f.close();
    return true;
}

bool buscar_pista(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    unsigned long nlen = contar_chars(needle);
    if (nlen == 0) return true;
    const char* p = haystack;
    while (*p) { if (comparar_n_chars(p, needle, nlen) == 0) return true; ++p; }
    return false;
}

// Formato estricto: terna repetida [0x00][count][symbol]
char* decompresion_rle_terna(const unsigned char* data, unsigned long size) {
    if (!data || size < 3) return nullptr;
    unsigned long triples = size / 3;           // ignoramos resto si no múltiplo exacto
    if (triples == 0) return nullptr;

    // 1) validar y calcular longitud total
    unsigned long total = 0;
    for (unsigned long i = 0; i < triples; ++i) {
        unsigned char marker = data[3*i];
        if (marker != 0x00) return nullptr;     // no cumple el formato
        unsigned char cnt    = data[3*i + 1];
        total += (unsigned long)cnt;
    }
    if (total == 0) return nullptr;

    // 2) construir salida
    char* out = new char[total + 1];
    unsigned long op = 0;
    for (unsigned long i = 0; i < triples; ++i) {
        unsigned char cnt = data[3*i + 1];
        unsigned char sym = data[3*i + 2];
        for (unsigned long k = 0; k < (unsigned long)cnt; ++k) out[op++] = (char)sym;
    }
    out[op] = '\0';
    return out;
}

// Tripletas [prefijo_hi][prefijo_lo][char], índice 1-based, big-endian
char* decomprimir_lz78(const unsigned char* data, unsigned long size) {
    if (!data || size < 3) return nullptr;

    unsigned long maxEntries = (size / 3) + 8;
    char** dict = new char*[maxEntries];
    unsigned long* dlen = new unsigned long[maxEntries];
    for (unsigned long i=0;i<maxEntries;++i){ dict[i]=0; dlen[i]=0; }

    unsigned long dictCount=0, pos=0, totalLen=0;

    while (pos + 2 < size) {
        unsigned char hi = data[pos];
        unsigned char lo = data[pos+1];
        unsigned short idx = (unsigned short)((hi << 8) | lo); // big-endian
        unsigned char c = data[pos+2];
        pos += 3;

        if (idx == 0) {
            char* s = new char[2]; s[0]=(char)c; s[1]='\0';
            dict[dictCount]=s; dlen[dictCount]=1;
            totalLen += 1; dictCount++;
        } else {
            if ((unsigned long)idx > dictCount) {
                for (unsigned long i=0;i<dictCount;++i) delete[] dict[i];
                delete[] dict; delete[] dlen; return nullptr;
            }
            unsigned long plen = dlen[idx-1];
            char* s = new char[plen + 2];
            my_memcpy(s, dict[idx-1], plen);
            s[plen]=(char)c; s[plen+1]='\0';
            dict[dictCount]=s; dlen[dictCount]=plen+1;
            totalLen += (plen+1); dictCount++;
        }
        if (dictCount >= maxEntries - 2) break;
    }

    if (totalLen == 0) { delete[] dlen; delete[] dict; return nullptr; }

    char* out = new char[totalLen + 1];
    unsigned long op = 0;
    for (unsigned long i=0;i<dictCount;++i) { my_memcpy(out+op, dict[i], dlen[i]); op += dlen[i]; delete[] dict[i]; }
    out[op]='\0';
    delete[] dict; delete[] dlen;
    return out;
}

unsigned char* desencriptar_arch(const unsigned char* enc, unsigned long size, int rot_n, unsigned char K) {
    if (!enc || size == 0) return nullptr;
    unsigned char* dec = new unsigned char[size];
    for (unsigned long i=0;i<size;++i) {
        unsigned char bx = (unsigned char)(enc[i] ^ K);
        dec[i] = rotacion_der(bx, rot_n);
    }
    return dec;
}


void construccion_nom_arch(char* out, unsigned long cap, const char* prefix, int X, const char* ext) {
    if (!out || cap == 0) return;
    unsigned long p = 0;
    for (unsigned long i=0; prefix[i] != '\0' && p + 1 < cap; ++i) out[p++] = prefix[i];
    char numbuf[32]; numero_a_dec((unsigned long)X, numbuf);
    for (unsigned long i=0; numbuf[i] != '\0' && p + 1 < cap; ++i) out[p++] = numbuf[i];
    for (unsigned long i=0; ext[i] != '\0' && p + 1 < cap; ++i) out[p++] = ext[i];
    out[p]='\0';
}


bool write_report(const char* outName, int metodo, int rot, unsigned char key, const char* plain) {
    const char* h1="Metodo: "; const char* mR="RLE"; const char* mL="LZ78";
    const char* h2="\nRotacion (n): "; const char* h3="\nKey: "; const char* h4="\n\nMensaje original completo:\n";
    char rotbuf[32]; numero_a_dec((unsigned long)rot, rotbuf);
    char keybuf[8];  byte_to_hex(key, keybuf);

    unsigned long need = contar_chars(h1)+(metodo==1?contar_chars(mR):contar_chars(mL))+ contar_chars(h2)+contar_chars(rotbuf)+ contar_chars(h3)+contar_chars(keybuf)+ contar_chars(h4)+contar_chars(plain) + 2;

    char* buf = new char[need + 1];
    unsigned long p=0;
    const char* mtxt=(metodo==1?mR:mL);
    const char* parts[8] = { h1, mtxt, h2, rotbuf, h3, keybuf, h4, plain };
    for (int i=0;i<8;++i){ const char* s=parts[i]; for (unsigned long j=0; s[j] != '\0'; ++j) buf[p++]=s[j]; }
    buf[p++] = '\n'; buf[p]='\0';

    bool ok = write_text_file(outName, buf);
    delete[] buf;
    return ok;
}


void resolver_caso(int X) {
    char encName[64], pistaName[64], outName[64];
    construccion_nom_arch(encName, 64, "Encriptado", X, ".txt");
    construccion_nom_arch(pistaName, 64, "pista",      X, ".txt");
    construccion_nom_arch(outName,  64, "Salida",     X, ".txt");

    unsigned long encSize = 0;
    unsigned char* encBuf = read_file_binary(encName, encSize);
    if (!encBuf) { cout<<"No se pudo abrir "<<encName<<"\n"; return; }

    char* pista = read_file_text(pistaName);
    if (!pista) { cout<<"No se pudo abrir "<<pistaName<<"\n"; delete[] encBuf; return; }

    unsigned long plen = contar_chars(pista);
    char* pista_clean = new char[plen + 1];
    unsigned long pc = 0;
    for (unsigned long i=0;i<plen;++i){ char ch=pista[i]; if (ch=='\r'||ch=='\n') continue; pista_clean[pc++]=ch; }
    pista_clean[pc]='\0';

    bool found=false; char* final_plain=0; int best_rot=-1; unsigned char best_key=0; int metodo=0;

    for (int rot=1; rot<=7 && !found; ++rot) {
        for (int K=0; K<=255 && !found; ++K) {
            unsigned char* dec = desencriptar_arch(encBuf, encSize, rot, (unsigned char)K);
            if (!dec) continue;


            char* rle_t00 = decompresion_rle_terna(dec, encSize);
            if (rle_t00) {
                if (buscar_pista(rle_t00, pista_clean)) {
                    found = true; final_plain = rle_t00; best_rot = rot; best_key = (unsigned char)K; metodo = 1;
                    delete[] dec; break;
                } else { delete[] rle_t00; }
            }


            char* lz = decomprimir_lz78(dec, encSize);
            if (lz) {
                if (buscar_pista(lz, pista_clean)) {
                    found = true; final_plain = lz; best_rot = rot; best_key = (unsigned char)K; metodo = 2;
                    delete[] dec; break;
                } else { delete[] lz; }
            }

            delete[] dec;
        }
    }

    if (found && final_plain) {
        cout<<"Caso "<<X<<" resuelto.\n";
        cout<<"Metodo: "<<(metodo==1?"RLE":"LZ78")<<"\n";
        cout<<"Rotacion (n): "<<best_rot<<"\n";
        cout<<"Key: "; char kbuf[8]; byte_to_hex(best_key, kbuf); cout<<kbuf<<"\n";
        bool ok = write_report(outName, metodo, best_rot, best_key, final_plain);
        if (ok) cout<<"Resultado guardado en "<<outName<<"\n";
        else    cout<<"Resuelto, pero no se pudo escribir "<<outName<<"\n";
        delete[] final_plain;
    } else {
        cout<<"Caso "<<X<<" NO resuelto: no se encontro una combinacion (n,K,metodo) que contenga la pista.\n";
    }

    delete[] pista_clean; delete[] pista; delete[] encBuf;
}

int main() {
    cout << "por favor ingrese el numero de archivos a evaluar: ";
    int n = 0;
    if (!(cin >> n)) { cout << "Entrada invalida.\n"; return 1; }
    cin.ignore();

    for (int i=1;i<=n;++i) {
        cout << "Procesando caso " << i << "...\n";
        resolver_caso(i);
    }
    cout << "Fin\n";
    return 0;
}
