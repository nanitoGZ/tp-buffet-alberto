#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

const float TASA_COMISION = 0.10f;
const int K = 5;

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

void encriptar(const char* origen, char* destino) {
    int i = 0;
    while (origen[i] != '\0' && i < 19) {
        destino[i] = origen[i] + K;
        i++;
    }
    destino[i] = '\0';
}

bool validarMozo(int idMozo, const char* pass, Mozo& mozoOut) {
    if (idMozo <= 0) return false;

    FILE* f = fopen("mozos.dat", "rb");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long totalMozos = ftell(f) / sizeof(Mozo);
    long pos = idMozo - 1; // PUP

    if (pos < 0 || pos >= totalMozos) {
        fclose(f);
        return false;
    }

    fseek(f, pos * sizeof(Mozo), SEEK_SET);
    Mozo m;
    int leido = fread(&m, sizeof(Mozo), 1, f);
    fclose(f);

    if (leido == 1) {
        char passEnc[20];
        encriptar(pass, passEnc);
        if (strcmp(m.password, passEnc) == 0) {
            mozoOut = m;
            return true;
        }
    }
    return false;
}

void sumarComisionMozo(int idMozo, float comision) {
    FILE* f = fopen("mozos.dat", "rb+");
    if (!f) return;

    long pos = idMozo - 1; // PUP
    fseek(f, pos * sizeof(Mozo), SEEK_SET);

    Mozo m;
    if (fread(&m, sizeof(Mozo), 1, f) == 1) {
        m.totalComision += comision;
        fseek(f, -(long)sizeof(Mozo), SEEK_CUR);
        fwrite(&m, sizeof(Mozo), 1, f);
    }
    fclose(f);
}

bool descontarStock(int codigo, int cantidad, float& precioUnitario) {
    FILE* f = fopen("inventario.dat", "rb+");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Producto);

    long pri = 0, ult = n - 1;
    bool exito = false;
    Producto prod;

    while (pri <= ult && !exito) {
        long med = (pri + ult) / 2;
        fseek(f, med * sizeof(Producto), SEEK_SET);

        if (fread(&prod, sizeof(Producto), 1, f) == 1) {
            if (prod.codigo == codigo) {
                if (prod.stockActual >= cantidad) {
                    prod.stockActual -= cantidad;
                    precioUnitario = prod.precio;
                    fseek(f, -(long)sizeof(Producto), SEEK_CUR);
                    fwrite(&prod, sizeof(Producto), 1, f);
                    exito = true;
                } else {
                    cout << "Error: Stock insuficiente. Hay: " << prod.stockActual << endl;
                    fclose(f);
                    return false;
                }
            } else if (codigo > prod.codigo) {
                pri = med + 1;
            } else {
                ult = med - 1;
            }
        } else {
            break;
        }
    }

    fclose(f);
    if (!exito) {
        cout << "Error: Producto no encontrado.\n";
    }
    return exito;
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
    char fecha[11];
    cout << "Ingrese la fecha del dia (DD-MM-AAAA): ";
    cin >> fecha;

    char nombreArchivo[64];
    sprintf(nombreArchivo, "comandas_%s.dat", fecha);

    int idMozo;
    cout << "\nNumero de mozo (0 para salir): ";
    cin >> idMozo;

    while (idMozo > 0) {
        char pass[20];
        cout << "Clave: ";
        cin >> pass;

        Mozo mozoActivo;
        if (!validarMozo(idMozo, pass, mozoActivo)) {
            cout << "Acceso denegado: Mozo inexistente o clave incorrecta.\n";
        } else {
            cout << "Mozo autenticado: " << mozoActivo.nombre << endl;

            int codProd, cant;
            cout << "Codigo de producto: ";
            cin >> codProd;
            cout << "Cantidad: ";
            cin >> cant;

            if (cant > 0) {
                float precioUnitario = 0.0f;
                if (descontarStock(codProd, cant, precioUnitario)) {
                    float comision = cant * precioUnitario * TASA_COMISION;
                    sumarComisionMozo(idMozo, comision);

                    FILE* fDia = fopen(nombreArchivo, "ab");
                    if (fDia) {
                        Comanda c;
                        c.idMozo = idMozo;
                        c.codigoProducto = codProd;
                        c.cantidad = cant;
                        c.comision = comision;

                        fwrite(&c, sizeof(Comanda), 1, fDia);
                        fclose(fDia);
                        cout << "Venta cargada con exito. Comision: $" << comision << endl;
                    }
                }
            } else {
                cout << "Cantidad invalida.\n";
            }
        }

        cout << "\nNumero de mozo (0 para salir): ";
        cin >> idMozo;
    }

    ordenarPlanilla(nombreArchivo);
    cout << "Planilla " << nombreArchivo << " actualizada y ordenada.\n";
    return 0;
}
