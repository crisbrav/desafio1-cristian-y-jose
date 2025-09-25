#include <iostream>
using namespace std;
    /**
 * Función: rle_descomprimir
 * Descripción: Descomprime un arreglo de bytes RLE.
 * Parámetros:
 *   - datos: arreglo de bytes ya desencriptados (XOR + rotación)
 *   - tam: tamaño del arreglo
 *   - tam_descomprimido: variable por referencia para devolver tamaño del resultado
 * Retorno:
 *   - puntero a arreglo dinámico con el contenido descomprimido
 */
    unsigned char* rle_descomprimir(const unsigned char* datos, long tam, long &tam_descomprimido) {
    // Calcular tamaño total del mensaje descomprimido
    long total = 0;
    for (long i = 0; i + 1 < tam; i += 2) {
        total += datos[i]; // primer byte = count
    }

    // Reservar memoria dinámica para el resultado
    unsigned char* resultado = new unsigned char[total];

    // Llenar el resultado con los valores repetidos
    long pos = 0;
    for (long i = 0; i + 1 < tam; i += 2) {
        unsigned char count = datos[i];   // cantidad de repeticiones
        unsigned char valor = datos[i + 1]; // valor a repetir
        for (int j = 0; j < count; j++) {
            resultado[pos++] = valor;
        }
    }

    tam_descomprimido = total; // devolver tamaño total
    return resultado;
}

// Ejemplo de uso desde main
int main() {
    // Supongamos que ya tenemos los datos desencriptados
    unsigned char datosRLE[] = {3, 'A', 2, 'B', 4, 'C'}; // ejemplo RLE
    long tam = sizeof(datosRLE);
    long tam_descomprimido;

    unsigned char* resultado = rle_descomprimir(datosRLE, tam, tam_descomprimido);

    // Imprimir resultado
    cout << "Mensaje descomprimido: ";
    for (long i = 0; i < tam_descomprimido; i++) {
        cout << resultado[i];
    }
    cout << endl;

    delete[] resultado; // liberar memoria
    return 0;
}