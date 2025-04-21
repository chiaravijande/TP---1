
#ifndef POKEDEX_H
#define POKEDEX_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  TIPO_AGUA,
  TIPO_FUEGO,
  TIPO_PLANTA,
  TIPO_ROCA,
  TIPO_ELECTRICO,
  TIPO_NORMAL,
  TIPO_LUCHA
} tipo_pokemon;

enum modo_iteracion { ITERAR_NOMBRE, ITERAR_ID };

struct pokemon {
  unsigned id;
  const char *nombre;
  tipo_pokemon tipo;
  unsigned fuerza;
  unsigned destreza;
  unsigned inteligencia;
};

typedef struct pokedex pokedex_t;

/**
 * Crea una nueva pokedex a partir de un archivo.
 *
 * Devuelve un puntero a la pokedex si se pudo abrir el archivo, o NULL en caso
 * contrario.
 *
 * Cada pokemon en el archivo debe tener el siguiente formato:
 * id;nombre;tipo;fuerza;destreza;inteligencia
 *
 * Al encontrar un pokemon inv�lido, se ignora y deja de leer del archivo.
 *
 * Se leen todos los pokemon v�lidos del archivo y se almacenan en la pokedex.
 * Si no se pudo leer ning�n pokemon v�lido, se devuelve NULL.
 *
 */
pokedex_t *pokedex_abrir(const char *archivo);

/**
 * Devuelve la cantidad de pokemones en la pokedex.
 */
unsigned pokedex_cantidad_pokemones(pokedex_t *pokedex);

/**
 * Busca un pokemon con el nombre especificado en la pokedex.
 *
 * Devuelve un puntero al pokemon si se encontr�, o NULL en caso contrario.
 */
const struct pokemon *pokedex_buscar_pokemon_nombre(pokedex_t *pokedex,
                                                    const char *nombre);

/**
 * Busca un pokemon con el id especificado en la pokedex.
 *
 * Devuelve un puntero al pokemon si se encontr�, o NULL en caso contrario.
 */
const struct pokemon *pokedex_buscar_pokemon_id(pokedex_t *pokedex,
                                                unsigned id);

/**
 * Invoca la funci�n especificada para cada pokemon en la pokedex.
 *
 * La funci�n se debe invocar con los pokemon ordenados seg�n el criterio de
 * iteracion dado (creciente por nombre o por id).
 *
 * La funci�n de iteraci�n recibe un puntero al pokemon y un puntero al contexto
 * y devuelve true si se debe seguir iterando, o false en caso contrario.
 *
 * Devuelve la cantidad de pokemones iterados.
 *
 * Restriccion del TP: implementar con complejidad O(n) en tiempo y O(1) en
 * espacio.
 */
unsigned pokedex_iterar_pokemones(pokedex_t *pokedex, enum modo_iteracion modo,
                                  bool (*funcion)(struct pokemon *, void *),
                                  void *ctx);

/**
 * Destruye la pokedex.
 */
void pokedex_destruir(pokedex_t *pokedex);

#endif // POKEDEX_H
