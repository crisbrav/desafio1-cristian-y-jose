#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

// ===== RLE =====
string RLE_encode(const char* texto) {
    string resultado = "";
    int n = strlen(texto);
    for (int i = 0; i < n; i++) {
        char simbolo = texto[i];
        int conteo = 1;
        while (i + 1 < n && texto[i + 1] == simbolo) {
            conteo++;
            i++;
        }
        resultado += to_string(conteo) + simbolo;
    }
    return resultado;
}

string RLE_decode(const char* comprimido) {
    string resultado = "";
    int n = strlen(comprimido);
    for (int i = 0; i < n;) {
        string numero = "";
        while (i < n && isdigit(comprimido[i])) {
            numero += comprimido[i];
            i++;
        }
        int conteo = stoi(numero);
        char simbolo = comprimido[i];
        i++;
        resultado.append(conteo, simbolo);
    }
    return resultado;
}

// ===== Rotación (Cifrado César) =====
string rotar(const string& texto, int desplazamiento) {
    string resultado = texto;
    for (char &c : resultado) {
        c = c + desplazamiento;
    }
    return resultado;
}

string desrotar(const string& texto, int desplazamiento) {
    string resultado = texto;
    for (char &c : resultado) {
        c = c - desplazamiento;
    }
    return resultado;
}

// ===== XOR =====
string aplicarXOR(const string& texto, char key) {
    string resultado = texto;
    for (char &c : resultado) {
        c = c ^ key;
    }
    return resultado;
}

int main() {
    // ======== PARÁMETROS ========
    int rotacion = 3;
    string keyInput;
    unsigned int keyValue;

    cout << "Ingrese la clave en hexadecimal (ejemplo: 0x5A): ";
    cin >> keyInput;

    // Convertir la cadena hexadecimal a número entero
    try {
        keyValue = stoi(keyInput, nullptr, 16);
    } catch (...) {
        cout << "Error: clave invalida. Use formato 0x??" << endl;
        return 1;
    }

    char key = static_cast<char>(keyValue);

    // ======== LECTURA DEL ARCHIVO ORIGINAL ========
    ifstream archivoEntrada("entrada.txt");
    if (!archivoEntrada) {
        cout << "Error: No se pudo abrir el archivo entrada.txt" << endl;
        return 1;
    }
    string contenido;
    getline(archivoEntrada, contenido);
    archivoEntrada.close();

    // ====== COMPRESIÓN + CIFRADO ======
    char* mensaje = new char[contenido.length() + 1];
    strcpy(mensaje, contenido.c_str());

    string comprimido = RLE_encode(mensaje);
    string rotado = rotar(comprimido, rotacion);
    string cifrado = aplicarXOR(rotado, key);

    ofstream archivoComprimido("comprimido.txt");
    archivoComprimido << cifrado;
    archivoComprimido.close();

    // ====== DESCIFRADO + DESCOMPRESIÓN ======
    string descifradoXOR = aplicarXOR(cifrado, key);
    string desrotado = desrotar(descifradoXOR, rotacion);
    string descomprimido = RLE_decode(desrotado.c_str());

    ofstream archivoDescomprimido("descomprimido.txt");
    archivoDescomprimido << descomprimido;
    archivoDescomprimido.close();

    delete[] mensaje;

    cout << "Proceso completado.\n";
    cout << "Archivo comprimido: comprimido.txt\n";
    cout << "Archivo descomprimido: descomprimido.txt\n";

    return 0;
}

