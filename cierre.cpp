#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

void ordenarPlanilla(const char* nombreArchivo) {
    FILE* f = fopen(nombreArchivo, "rb");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long cant = ftell(f) / sizeof(Comanda);
    fseek(f, 0, SEEK_SET);

    if (cant <= 1) {
        fclose(f);
        return;
    }

    Comanda* arr = new Comanda[cant];
    fread(arr, sizeof(Comanda), cant, f);
    fclose(f);

    for (int i = 0; i < cant - 1; i++) {
        for (int j = 0; j < cant - i - 1; j++) {
            if (arr[j].idMozo > arr[j + 1].idMozo) {
                Comanda aux = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = aux;
            }
        }
    }

    f = fopen(nombreArchivo, "wb");
    if (f) {
        fwrite(arr, sizeof(Comanda), cant, f);
        fclose(f);
    }
    delete[] arr;
}

int main() {
    char idSemana[20];
    cout << "Ingrese identificador de semana (ej: s1-06): ";
    cin >> idSemana;

    char nombreSemana[64];
    sprintf(nombreSemana, "comandas_semana_%s.dat", idSemana);

    FILE* fSemana = fopen(nombreSemana, "wb");
    if (!fSemana) {
        cout << "No se pudo crear el archivo semanal.\n";
        return 1;
    }

    char fecha[15];
    cout << "Ingrese fecha a incluir (DD-MM-AAAA) o FIN para terminar: ";
    cin >> fecha;

    while (strcmp(fecha, "FIN") != 0 && strcmp(fecha, "fin") != 0) {
        char nombreDia[64];
        sprintf(nombreDia, "comandas_%s.dat", fecha);

        FILE* fDia = fopen(nombreDia, "rb");
        if (!fDia) {
            cout << "Planilla no encontrada: " << nombreDia << endl;
        } else {
            Comanda c;
            int count = 0;
            while (fread(&c, sizeof(Comanda), 1, fDia) == 1) {
                fwrite(&c, sizeof(Comanda), 1, fSemana);
                count++;
            }
            fclose(fDia);
            cout << "Se agregaron " << count << " comandas de " << nombreDia << endl;
        }

        cout << "Ingrese fecha a incluir (DD-MM-AAAA) o FIN para terminar: ";
        cin >> fecha;
    }

    fclose(fSemana);
    ordenarPlanilla(nombreSemana);

    cout << "Semana unificada en " << nombreSemana << " ordenada por mozo.\n";
    return 0;
}
