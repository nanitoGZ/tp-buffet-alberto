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

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

void obtenerNombreMozo(int idMozo, char* dest) {
    strcpy(dest, "Desconocido");
    if (idMozo <= 0) return;

    FILE* f = fopen("mozos.dat", "rb");
    if (!f) return;

    long pos = idMozo - 1; // PUP
    fseek(f, 0, SEEK_END);
    long total = ftell(f) / sizeof(Mozo);

    if (pos >= 0 && pos < total) {
        fseek(f, pos * sizeof(Mozo), SEEK_SET);
        Mozo m;
        if (fread(&m, sizeof(Mozo), 1, f) == 1) {
            strcpy(dest, m.nombre);
        }
    }
    fclose(f);
}

int main() {
    char idSemana[20];
    cout << "Ingrese identificador de semana a consultar (ej: s1-06): ";
    cin >> idSemana;

    char nombreSemana[64];
    sprintf(nombreSemana, "comandas_semana_%s.dat", idSemana);

    FILE* f = fopen(nombreSemana, "rb");
    if (!f) {
        cout << "No se pudo abrir " << nombreSemana << endl;
        return 1;
    }

    cout << "\n========================================================" << endl;
    cout << "           RESUMEN SEMANAL - BUFFET ALBERT EINSTEIN     " << endl;
    cout << "========================================================" << endl;

    Comanda r;
    int leido = fread(&r, sizeof(Comanda), 1, f);
    int totalProdBuffet = 0;
    float totalComBuffet = 0.0f;

    // Corte de control canonico de catedra
    while (leido == 1) {
        int mozoActual = r.idMozo;
        int cantMozo = 0;
        float comMozo = 0.0f;

        while (leido == 1 && r.idMozo == mozoActual) {
            cantMozo += r.cantidad;
            comMozo += r.comision;
            leido = fread(&r, sizeof(Comanda), 1, f);
        }

        char nombre[50];
        obtenerNombreMozo(mozoActual, nombre);

        cout << "Mozo " << mozoActual << " (" << nombre << "):" << endl;
        cout << "  - Productos vendidos: " << cantMozo << endl;
        cout << "  - Comision a pagar:   $" << comMozo << endl;
        cout << "--------------------------------------------------------" << endl;

        totalProdBuffet += cantMozo;
        totalComBuffet += comMozo;
    }

    fclose(f);

    cout << "TOTALES GENERALES DEL BUFFET EN LA SEMANA:" << endl;
    cout << "  - Total productos vendidos: " << totalProdBuffet << endl;
    cout << "  - Total comisiones a pagar: $" << totalComBuffet << endl;
    cout << "========================================================" << endl;

    return 0;
}
