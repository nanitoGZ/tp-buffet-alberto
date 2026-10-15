# TP Integrador — Buffet Albert Einstein

## Grupo: 21
## Integrantes: Ignacio Tomas Gonzalez — nanitoGZ (Trabajo individual)

## Compilación y Ejecución

Compilar cada programa por separado con g++:
```bash
g++ -O2 -o normalizacion normalizacion.cpp
g++ -O2 -o ventas ventas.cpp
g++ -O2 -o cierre cierre.cpp
g++ -O2 -o resumen resumen.cpp
```
Orden de ejecución:

cp datos/comandas_historicas.dat datos/inventario.dat .
./normalizacion
./ventas
./cierre
./resumen

## Explicación de los programas

### normalizacion.cpp
Este módulo organiza y adapta los datos del sistema anterior. Lee el archivo comandas_historicas.dat para identificar a todos los mozos sin repetirlos y crear el archivo mozos.dat, asignándoles a cada uno un número de ID correlativo a partir de 1 (1, 2, 3...) y sumando la comisión que ya traían acumulada. A su vez, genera para cada mozo una contraseña inicial con su propio número de ID y la guarda encriptada sumándole 5 a cada carácter (cifrado César). Al mismo tiempo, actualiza las unidades en inventario.dat descontando las ventas históricas mediante una búsqueda rápida aprovechando que los productos ya están ordenados por código. Luego, en una segunda pasada, reparte cada venta histórica en su archivo diario correspondiente (comandas_dd-mm-aaaa.dat) y deja cada una de esas planillas diarias ordenadas por ID de mozo.

### ventas.cpp
Permite la carga de ventas de una jornada. Al iniciar, pide la fecha del día una sola vez para abrir o crear el archivo comandas_dd-mm-aaaa.dat. En cada venta solicita el ID del mozo y su clave; a la clave ingresada le suma 5 por carácter y la compara con la guardada en mozos.dat. Como los mozos tienen IDs continuos del 1 al N, el programa calcula su posición exacta y va directo al registro en disco sin necesidad de recorrer todo el archivo. Una vez autenticado el mozo, pide el código de producto y la cantidad. Busca el artículo en inventario.dat, valida que haya stock suficiente, descuenta las unidades en el archivo y calcula la comisión del 10%. Guarda la comanda al final de la planilla del día, suma la comisión ganada al mozo en mozos.dat y, al finalizar la carga (ingresando 0 como ID de mozo), deja toda la planilla de ese día ordenada por ID de mozo.

### cierre.cpp
Se encarga de unificar las ventas de la semana en un solo lugar. Pide el identificador de la semana (por ejemplo, s1-06) para nombrar el archivo semanal comandas_semana_sX-mm.dat. Luego solicita una por una las fechas de los días que integraron esa semana hasta que se ingresa la palabra FIN. El programa abre cada archivo diario, copia todas sus comandas dentro del archivo semanal y, al terminar de procesar todas las fechas, ordena el archivo semanal completo por ID de mozo para que quede preparado para el informe de cierre.

### resumen.cpp
Genera el informe final del buffet a partir de la planilla semanal. Solicita el identificador de la semana a consultar y verifica que el archivo exista y contenga datos. Luego, mediante un corte de control, va agrupando las ventas de cada mozo mientras recorre el archivo, consultando mozos.dat para mostrar por pantalla el nombre real de cada uno, el total de productos que vendió en la semana y el dinero que le corresponde cobrar de comisión. Al terminar el listado, imprime el total acumulado de productos vendidos en todo el buffet durante esa semana.
