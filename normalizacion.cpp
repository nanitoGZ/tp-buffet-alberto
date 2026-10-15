#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct ComandaHistorica {
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const int K = 5;

void encriptar(const char* origen, char* destino) {
    int i = 0;
    while (origen[i] != '\0' && i < 19) {
        destino[i] = origen[i] + K;
        i++;
    }
    destino[i] = '\0';
}

void descontarStock(int codigo, int cantidad) {
    FILE* fInv = fopen("inventario.dat", "rb+");
    if (!fInv) return;

    fseek(fInv, 0, SEEK_END);
    long cantProds = ftell(fInv) / sizeof(Producto);

    long pri = 0, ult = cantProds - 1;
    bool encontrado = false;
    Producto prod;

    while (pri <= ult && !encontrado) {
        long med = (pri + ult) / 2;
        fseek(fInv, med * sizeof(Producto), SEEK_SET);

        if (fread(&prod, sizeof(Producto), 1, fInv) == 1) {
            if (prod.codigo == codigo) {
                encontrado = true;
                prod.stockActual -= cantidad;
                fseek(fInv, -(long)sizeof(Producto), SEEK_CUR);
                fwrite(&prod, sizeof(Producto), 1, fInv);
            } else if (codigo > prod.codigo) {
                pri = med + 1;
            } else {
                ult = med - 1;
            }
        } else {
            break;
        }
    }
    fclose(fInv);
}

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
    FILE* fHist = fopen("comandas_historicas.dat", "rb");
    if (!fHist) {
        cout << "No se pudo abrir comandas_historicas.dat\n";
        return 1;
    }

    Mozo mozos[100];
    int cantMozos = 0;
    char fechas[365][11];
    int cantFechas = 0;

    ComandaHistorica ch;

    // Pasada 1: armar lista de mozos y actualizar inventario
    while (fread(&ch, sizeof(ComandaHistorica), 1, fHist) == 1) {
        int pos = -1;
        for (int i = 0; i < cantMozos; i++) {
            if (strcmp(mozos[i].nombre, ch.nombreMozo) == 0) {
                pos = i;
                break;
            }
        }

        if (pos == -1) {
            mozos[cantMozos].idMozo = cantMozos + 1;
            strcpy(mozos[cantMozos].nombre, ch.nombreMozo);
            mozos[cantMozos].totalComision = ch.comision;

            char clavePlana[20];
            sprintf(clavePlana, "%d", cantMozos + 1);
            encriptar(clavePlana, mozos[cantMozos].password);

            cantMozos++;
        } else {
            mozos[pos].totalComision += ch.comision;
        }

        descontarStock(ch.codigoProducto, ch.cantidad);
    }

    // Guardar mozos.dat
    FILE* fMozos = fopen("mozos.dat", "wb");
    if (fMozos) {
        for (int i = 0; i < cantMozos; i++) {
            fwrite(&mozos[i], sizeof(Mozo), 1, fMozos);
        }
        fclose(fMozos);
    }

    // Pasada 2: repartir ventas en las planillas por día
    fseek(fHist, 0, SEEK_SET);

    while (fread(&ch, sizeof(ComandaHistorica), 1, fHist) == 1) {
        int idM = 0;
        for (int i = 0; i < cantMozos; i++) {
            if (strcmp(mozos[i].nombre, ch.nombreMozo) == 0) {
                idM = mozos[i].idMozo;
                break;
            }
        }

        bool yaEstaFecha = false;
        for (int i = 0; i < cantFechas; i++) {
            if (strcmp(fechas[i], ch.fecha) == 0) {
                yaEstaFecha = true;
                break;
            }
        }
        if (!yaEstaFecha) {
            strcpy(fechas[cantFechas], ch.fecha);
            cantFechas++;
        }

        char nombreDia[64];
        sprintf(nombreDia, "comandas_%s.dat", ch.fecha);

        FILE* fDia = fopen(nombreDia, "ab");
        if (fDia) {
            Comanda c;
            c.idMozo = idM;
            c.codigoProducto = ch.codigoProducto;
            c.cantidad = ch.cantidad;
            c.comision = ch.comision;

            fwrite(&c, sizeof(Comanda), 1, fDia);
            fclose(fDia);
        }
    }
    fclose(fHist);

    // Ordenar todas las planillas creadas
    for (int i = 0; i < cantFechas; i++) {
        char nombreDia[64];
        sprintf(nombreDia, "comandas_%s.dat", fechas[i]);
        ordenarPlanilla(nombreDia);
    }

    cout << "Normalizacion completada con exito.\n";
    return 0;
}
