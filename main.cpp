#include <iostream>
#include <fstream>
#include <string>
using namespace std;

/**
 * Función: lz78_descomprimir
 * Descripción: Descomprime un arreglo de pares (índice, símbolo) usando LZ78.
 * Parámetros:
 *   - indices: arreglo de enteros que representan los índices de referencia
 *   - simbolos: arreglo de caracteres que representan los símbolos agregados
 *   - n: cantidad de pares (longitud de los arreglos indices y simbolos)
 *   - tam_descomprimido: variable por referencia para devolver tamaño del resultado
 * Retorno:
 *   - puntero a arreglo dinámico con el contenido descomprimido
 */
unsigned char* lz78_descomprimir(const int* indices, const unsigned char* simbolos, int n, long &tam_descomprimido) {
    string* diccionario = new string[n]; // diccionario dinámico
    int diccSize = 0;
    string resultado;

    for (int i = 0; i < n; i++) {
        string entrada;

        if (indices[i] > 0 && indices[i] <= diccSize) {
            entrada = diccionario[indices[i] - 1];
        }

        entrada += simbolos[i];
        resultado += entrada;
        diccionario[diccSize++] = entrada;
    }

    delete[] diccionario;

    tam_descomprimido = resultado.size();
    unsigned char* salida = new unsigned char[tam_descomprimido];

    for (long i = 0; i < tam_descomprimido; i++) {
        salida[i] = resultado[i];
    }

    return salida;
}

int main() {
    ifstream entrada("entrada.txt");
    if (!entrada) {
        cerr << "Error: no se pudo abrir entrada.txt" << endl;
        return 1;
    }

    int n;
    entrada >> n; // cantidad de pares

    int* indices = new int[n];
    unsigned char* simbolos = new unsigned char[n];

    for (int i = 0; i < n; i++) {
        entrada >> indices[i] >> simbolos[i];
    }
    entrada.close();

    long tam_descomprimido;
    unsigned char* resultado = lz78_descomprimir(indices, simbolos, n, tam_descomprimido);

    ofstream salida("salida.txt");
    if (!salida) {
        cerr << "Error: no se pudo abrir salida.txt" << endl;
        delete[] indices;
        delete[] simbolos;
        delete[] resultado;
        return 1;
    }

    for (long i = 0; i < tam_descomprimido; i++) {
        salida << resultado[i];
    }
    salida.close();

    cout << "Descompresion completada. Resultado guardado en salida.txt" << endl;

    delete[] indices;
    delete[] simbolos;
    delete[] resultado;

    return 0;
}


