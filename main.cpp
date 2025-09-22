#include <iostream>
#include <fstream>
using namespace std;

// Función para comprimir RLE simple
unsigned char* rle_comprimir(unsigned char* datos, long tam, long &tam_comprimido) {
    // Reservar memoria temporal grande (en el peor caso sin compresión)
    unsigned char* temp = new unsigned char[tam * 2];
    long pos = 0;

    for (long i = 0; i < tam; ) {
        unsigned char valor = datos[i];
        unsigned char count = 1;

        // Contar repeticiones hasta 255
        while (i + count < tam && datos[i + count] == valor && count < 255) {
            count++;
        }

        temp[pos++] = count;
        temp[pos++] = valor;
        i += count;
    }

    tam_comprimido = pos;

    // Crear buffer exacto y copiar
    unsigned char* comprimido = new unsigned char[tam_comprimido];
    for (long i = 0; i < tam_comprimido; i++) {
        comprimido[i] = temp[i];
    }

    delete[] temp;
    return comprimido;
}

// Función para descomprimir RLE simple
unsigned char* rle_descomprimir(unsigned char* datos, long tam, long &tam_descomprimido) {
    // Calcular tamaño total
    long total = 0;
    for (long i = 0; i + 1 < tam; i += 2) {
        total += datos[i];
    }

    unsigned char* resultado = new unsigned char[total];
    long pos = 0;
    for (long i = 0; i + 1 < tam; i += 2) {
        unsigned char count = datos[i];
        unsigned char valor = datos[i + 1];
        for (int j = 0; j < count; j++) {
            resultado[pos++] = valor;
        }
    }

    tam_descomprimido = total;
    return resultado;
}

// Función para detectar si el archivo parece RLE
bool esRLE(unsigned char* datos, long tam) {
    if (tam % 2 != 0) return false;
    for (long i = 0; i + 1 < tam; i += 2) {
        if (datos[i] == 0) return false; // un count 0 no es válido
    }
    return true;
}

int main() {
    char archivoEntrada[100], archivoSalida[100];
    cout << "Archivo de entrada: ";
    cin >> archivoEntrada;
    cout << "Archivo de salida: ";
    cin >> archivoSalida;

    ifstream fin(archivoEntrada, ios::binary);
    if (!fin) {
        cout << "No se pudo abrir el archivo de entrada.\n";
        return 1;
    }

    fin.seekg(0, ios::end);
    long tam = fin.tellg();
    fin.seekg(0, ios::beg);

    unsigned char* datos = new unsigned char[tam];
    fin.read((char*)datos, tam);
    fin.close();

    ofstream fout(archivoSalida, ios::binary);
    if (!fout) {
        cout << "No se pudo crear el archivo de salida.\n";
        delete[] datos;
        return 1;
    }

    const unsigned char clave = 0x5A; // clave fija para ejemplo

    if (esRLE(datos, tam)) {
        // Descomprimir
        long tam_descomprimido;
        unsigned char* descomprimido = rle_descomprimir(datos, tam, tam_descomprimido);
        fout.write((char*)descomprimido, tam_descomprimido);
        delete[] descomprimido;
        cout << "Archivo estaba comprimido. Se descomprimió.\n";
    } else {
        // Comprimir
        long tam_comprimido;
        unsigned char* comprimido = rle_comprimir(datos, tam, tam_comprimido);
        // Adjuntar clave al inicio del archivo
        fout << "CLAVE: 0x" << hex << (int)clave << "\n";
        fout.write((char*)comprimido, tam_comprimido);
        delete[] comprimido;
        cout << "Archivo no estaba comprimido. Se comprimió y se adjuntó la clave.\n";
    }

    delete[] datos;
    fout.close();

    cout << "Proceso completado. Archivo generado: " << archivoSalida << endl;
    return 0;
}

