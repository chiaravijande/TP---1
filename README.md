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

Para realizar estas acciones, sigue los siguientes pasos:
	1.Lee el archivo csv:
	-Se abre el archivo especificado y se lee línea por línea.

	-Cada línea contiene información sobre un Pokémon (ID, nombre, tipo, fuerza, inteligencia, destreza).

	-Si el archivo está vacío o no se puede abrir, el programa muestra un mensaje de error y devuelve NULL, indicando que no se pudo cargar la Pokédex.


	2.Valida y carga los datos:
	- Valida que haya 6 elementos, que tanto el ID, como la fuerza, la inteligencia y destreza sean numeros positivos, que el nombre empiece con mayúscula y esté compuesto solo de letras (sin espacios), y que el tipo sea alguna de las siguientes letras (en mayúscula): A, F, P, R, E, N y L, que corresponden al tipo agua, fuego, planta, roca, eléctrico, normal y lucha respectivamente.

	-Los Pokémon inválidos son ignorados (y ni bien se encuentra uno se detiene la lectura del archivo), pero los Pokémon válidos se almacenan en la Pokédex utilizando memoria dinámica.


	3-Ordenamiento de Pokémon:
	-Se ordenan los Pokémon por ID o por nombre. Este ordenamiento se realiza al inicializar la Pokédex y los vectores se reutilizan en búsquedas y recorridos.

	-Se necesitan respectivos ordenamientos para poder iterar fácilmente sobre la Pokédex.


	4-Iteración sobre los Pokémon:
	-El programa permite recorrer todos los Pokémon en orden alfabético o por ID.

	-El usuario puede proporcionar una función que define qué hacer con cada Pokémon durante la iteración (por ejemplo, imprimir los nombres o sumar atributos).


	5-Búsquedas eficientes:
	-Estas búsquedas retornan un puntero al Pokémon encontrado o NULL si no hay coincidencias.


	6-Liberación de memoria:

	-Cuando ya no se necesita la Pokédex, el programa libera toda la memoria asignada, incluyendo los nombres de los Pokémon, los vectores ordenados y la estructura principal.

	-Las validaciones aseguran que los punteros no sean liberados dos veces ni accedidos cuando están sin inicializar.


2. Estructuras Desarrolladas Para El Funcionamiento Del Programa
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

-Lee línea por línea hasta encontrar un error o el final del archivo, usando funciones auxiliares para que sea una función modularizada.

-Valida los datos en cada línea antes de almacenarlos, asegurándose de procesar solo Pokémon válidos.

-Ignora Pokémon inválidos, pero carga los Pokémon validos que encontró hasta el momento.

-Si el archivo está vacío o no contiene Pokémon válidos, la función destruye la Pokédex creada y devuelve NULL.

-Devuelve un puntero a la estructura Pokédex completamente inicializada si se cargaron Pokémon exitosamente.

3.2. pokedex_buscar_pokemon_nombre() y pokedex_buscar_pokemon_id()
-Permiten buscar Pokémon por nombre o ID.

-Realizan una búsqueda secuencial, iterando por la lista de Pokémon en tiempo O(n), dado el diseño de la estructura de datos.

-Devuelven un puntero constante al Pokémon encontrado para garantizar que los datos sean tratados como inmutables.

-Si no se encuentra el Pokémon, devuelven NULL, indicando que no hay coincidencias con el criterio proporcionado.

-Estas funciones son esenciales para interactuar con los datos almacenados en la Pokédex y permiten verificar la integridad y disponibilidad de los Pokémon cargados. Básicamente, asegura que cada Pokémon tiene sus atributos completos y correctos, siguiendo las reglas establecidas en el código para validarlos antes de incluirlos en la Pokédex.

3.3. pokedex_iterar_pokemones()
-Permite recorrer la Pokédex en orden alfabético o por ID, aplicando una función a cada Pokémon. La función que se le pasa como parámetro decide qué hacer con cada Pokémon (ejemplo: imprimirlos con su nombre y sumar su fuerza).

- Puede recorrer la Pokédex de forma ordenada, ya sea por ID o por nombre, gracias a vectores preordenados generados previamente en la función auxiliar inicializar_vectores_ordenados, lo que asegura que el recorrido se realiza en tiempo O(n), tal como se pide en los comentarios de pokedex.h.

-El recorrido puede detenerse en cualquier punto si la función que se le pasa como parámetro retorna false, lo que permite un control flexible de la iteración.

3.4. pokedex_destruir()
-Libera toda la memoria utilizada por la Pokédex.

-Primero, recorre cada Pokémon almacenado y libera de forma individual los nombres que fueron asignados dinámicamente.

-A continuación, libera la lista principal de Pokémon y los vectores ordenados (ordenados_por_id y ordenados_por_nombre), si estos fueron inicializados.

-Finalmente, libera la estructura principal de la Pokédex.

-Incluye verificaciones para evitar dobles liberaciones o accesos indebidos a punteros no inicializados, asegurando una gestión segura y eficiente de la memoria.


