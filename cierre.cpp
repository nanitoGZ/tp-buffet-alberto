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

struct ArchivoApareo {
    FILE* f;
    Comanda actual;
    bool fin;
};

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

    ArchivoApareo fuentes[7];
    int cantDias = 0;

    char fecha[15];
    cout << "Ingrese fecha a incluir (DD-MM-AAAA) o FIN para terminar: ";
    cin >> fecha;

    while (strcmp(fecha, "FIN") != 0 && strcmp(fecha, "fin") != 0 && cantDias < 7) {
        char nombreDia[64];
        sprintf(nombreDia, "comandas_%s.dat", fecha);

        FILE* fDia = fopen(nombreDia, "rb");
        if (!fDia) {
            cout << "Planilla no encontrada: " << nombreDia << endl;
        } else {
            fuentes[cantDias].f = fDia;
            if (fread(&fuentes[cantDias].actual, sizeof(Comanda), 1, fDia) == 1) {
                fuentes[cantDias].fin = false;
            } else {
                fuentes[cantDias].fin = true;
            }
            cantDias++;
            cout << "Planilla agregada al apareo: " << nombreDia << endl;
        }

        cout << "Ingrese fecha a incluir (DD-MM-AAAA) o FIN para terminar: ";
        cin >> fecha;
    }

    if (cantDias == 0) {
        cout << "No se ingresaron planillas validas.\n";
        fclose(fSemana);
        return 0;
    }

    while (true) {
        int indiceMenor = -1;

        for (int i = 0; i < cantDias; i++) {
            if (!fuentes[i].fin) {
                if (indiceMenor == -1 || fuentes[i].actual.idMozo < fuentes[indiceMenor].actual.idMozo) {
                    indiceMenor = i;
                }
            }
        }

        if (indiceMenor == -1) {
            break;
        }

        fwrite(&fuentes[indiceMenor].actual, sizeof(Comanda), 1, fSemana);

        if (fread(&fuentes[indiceMenor].actual, sizeof(Comanda), 1, fuentes[indiceMenor].f) != 1) {
            fuentes[indiceMenor].fin = true;
        }
    }

    for (int i = 0; i < cantDias; i++) {
        fclose(fuentes[i].f);
    }
    fclose(fSemana);

    cout << "Semana unificada exitosamente mediante apareo en " << nombreSemana << ".\n";
    return 0;
}
