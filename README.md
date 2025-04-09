<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA LISTA

## Alumno: Chiara Vijande - 111771 - cvijande@fi.uba.ar

- Para compilar: 
gcc -o tp1 tp1.c src/pokedex.c -I. -I./src -lm
gcc -o pruebas pruebas_alumno.c src/pokedex.c -I. -I./src -lm

- Para ejecutar:
./tp1 ejemplos/pokedex.csv
./pruebas

- Para ejecutar con valgrind:
valgrind --leak-check=full --track-origins=yes ./tp1 ejemplos/pokedex.csv
valgrind --leak-check=full --track-origins=yes ./pruebas

---

##  Funcionamiento

1. Funcionamiento General
El programa implementa una Pokédex, una estructura que permite almacenar pokemones y las características de los mismos. 
Concretamente, se encarga de:

-Leer un archivo CSV con información de los Pokémones.

-Guardar los Pokémones en una estructura de datos adecuada.

-Permitir búsquedas por nombre e ID.

-Iterar sobre la Pokédex ordenando por nombre o ID.

-Liberar correctamente la memoria utilizada.

2. Estructuras Desarrolladas
2.1. Estructura pokemon:

struct pokemon {
    unsigned id; 
    const char *nombre;
    tipo_pokemon tipo;
    unsigned fuerza;
    unsigned destreza;
    unsigned inteligencia;
};
-Cada pokemon almacena la información básica de un Pokémon:

    -Id: Número identificador único.

    -Nombre: Nombre del Pokémon.

    -Tipo: Tipo (agua, fuego, roca, etc.).

    -Atributos del Pokémon: fuerza, destreza, inteligencia.

El nombre se guarda como const char * porque se lee de un archivo, y es mejor no modificarlo accidentalmente.

2.2. Estructura pokedex_t
Esta estructura almacena todos los Pokémon cargados en la Pokédex. No está definida en el .h, ya que se maneja de forma opaca (abstracta). Esto significa que su estructura interna no está expuesta en el archivo de cabecera (pokedex.h). Es decir, el usuario del módulo no sabe ni necesita saber cómo está implementada la Pokédex internamente; solo puede interactuar con ella a través de las funciones públicas del .h.

3. Funcionamiento de las Funciones Principales
3.1. pokedex_abrir()
Esta función:

-Abre el archivo CSV pasado como argumento.

-Lee línea por línea hasta encontrar un error o el final del archivo.

-Parsea los datos y los almacena en la Pokédex.

-Ignora Pokémon inválidos.

-Si no pudo leer ningún Pokémon, devuelve NULL.

-Se decidió ignorar los Pokémon inválidos en lugar de abortar la carga completa.

3.2. pokedex_buscar_pokemon_nombre() y pokedex_buscar_pokemon_id()
-Permiten buscar Pokémon por nombre o ID.

-Se decidió usar búsqueda secuencial porque la complejidad del TP exige O(n) en tiempo y O(1) en espacio.

3.3. pokedex_iterar_pokemones()
-Permite recorrer la Pokédex en orden alfabético o por ID, aplicando una función a cada Pokémon.

-Usa un algoritmo de ordenación O(n) para mantener la eficiencia pedida en el enunciado.

-La función que se le pasa como parámetro decide qué hacer con cada Pokémon (ejemplo: imprimirlos con su nombre y sumar su fuerza).

3.4. pokedex_destruir()
-Libera toda la memoria utilizada por la Pokédex.

-Se decidió que se debe recorrer y liberar cada Pokémon individualmente antes de liberar la estructura principal.


