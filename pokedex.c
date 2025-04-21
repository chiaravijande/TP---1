#include "pokedex.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pokedex {
	struct pokemon *pokemones;
	size_t cantidad;
	size_t capacidad;
	struct pokemon **ordenados_por_id;
	struct pokemon **ordenados_por_nombre;
};

#define CAPACIDAD_INICIAL 10

bool convertir_tipo(const char *tipo_str, tipo_pokemon *tipo);

pokedex_t *pokedex_crear()
{
	pokedex_t *pokedex = malloc(sizeof(pokedex_t));
	if (!pokedex) {
		return NULL;
	}

	pokedex->pokemones = malloc(CAPACIDAD_INICIAL * sizeof(struct pokemon));
	if (!pokedex->pokemones) {
		free(pokedex);
		return NULL;
	}

	pokedex->cantidad = 0;
	pokedex->capacidad = CAPACIDAD_INICIAL;

	pokedex->ordenados_por_id = NULL;
	pokedex->ordenados_por_nombre = NULL;

	return pokedex;
}

char *leer_nombre(FILE *f)
{
	size_t capacidad = 16;
	size_t longitud = 0;
	char *nombre = malloc(capacidad);
	if (!nombre) {
		return NULL;
	}

	int c;
	while ((c = fgetc(f)) != EOF) {
		if (c == ';')
			break;

		if (longitud + 1 >= capacidad) {
			capacidad *= 2;
			char *nuevo_nombre = realloc(nombre, capacidad);
			if (!nuevo_nombre) {
				free(nombre);
				return NULL;
			}
			nombre = nuevo_nombre;
		}

		nombre[longitud++] = (char)c;
	}

	if (longitud == 0 && c == EOF) {
		free(nombre);
		return NULL;
	}

	nombre[longitud] = '\0';
	return nombre;
}

bool leer_linea_pokemon(FILE *f, struct pokemon *p)
{
	if (!f || !p)
		return false;

	if (fscanf(f, "%u;", &p->id) != 1)
		return false;

	char *nombre = leer_nombre(f);
	if (!nombre) {
		free(nombre);
		return false;
	}

	p->nombre = nombre;

	char tipo_str[20];
	if (fscanf(f, "%19[^;];%u;%u;%u\n", tipo_str, &p->fuerza, &p->destreza,
		   &p->inteligencia) != 4) {
		free((void *)p->nombre);
		return false;
	}

	if (!convertir_tipo(tipo_str, &p->tipo)) {
		free((void *)p->nombre);
		return false;
	}

	if (p->fuerza <= 0 || p->destreza <= 0 || p->inteligencia <= 0) {
		free((void *)p->nombre);
		return false;
	}

	return true;
}

bool convertir_tipo(const char *tipo_str, tipo_pokemon *tipo)
{
	if (strcmp(tipo_str, "A") == 0)
		*tipo = TIPO_AGUA;
	else if (strcmp(tipo_str, "F") == 0)
		*tipo = TIPO_FUEGO;
	else if (strcmp(tipo_str, "P") == 0)
		*tipo = TIPO_PLANTA;
	else if (strcmp(tipo_str, "R") == 0)
		*tipo = TIPO_ROCA;
	else if (strcmp(tipo_str, "E") == 0)
		*tipo = TIPO_ELECTRICO;
	else if (strcmp(tipo_str, "N") == 0)
		*tipo = TIPO_NORMAL;
	else if (strcmp(tipo_str, "L") == 0)
		*tipo = TIPO_LUCHA;
	else
		return false;

	return true;
}

bool es_pokemon_valido(const struct pokemon *p)
{
	if (!p)
		return false;

	if (p->id <= 0 || p->fuerza <= 0 || p->destreza <= 0 ||
	    p->inteligencia <= 0) {
		return false;
	}

	if (p->fuerza > 999 || p->destreza > 999 || p->inteligencia > 999) {
		return false;
	}

	return true;
}

bool pokedex_agregar_pokemon(pokedex_t *pokedex, struct pokemon pokemon)
{
	if (pokedex->cantidad >= pokedex->capacidad) {
		if (!pokemon.nombre) {
			return false;
		}

		size_t nueva_capacidad = pokedex->capacidad * 2;
		struct pokemon *nueva_lista =
			realloc(pokedex->pokemones,
				nueva_capacidad * sizeof(struct pokemon));
		if (!nueva_lista) {
			free((void *)pokemon.nombre);
			return false;
		}
		pokedex->pokemones = nueva_lista;
		pokedex->capacidad = nueva_capacidad;
	}

	char *nombre_copia = malloc(strlen(pokemon.nombre) + 1);
	if (!nombre_copia) {
		free((void *)pokemon.nombre);
		return false;
	}
	strcpy(nombre_copia, pokemon.nombre);
	free((void *)pokemon.nombre);
	pokemon.nombre = nombre_copia;

	pokedex->pokemones[pokedex->cantidad++] = pokemon;
	return true;
}

int comparar_por_id(const void *a, const void *b)
{
	const struct pokemon *pokemon_a = *(const struct pokemon **)a;
	const struct pokemon *pokemon_b = *(const struct pokemon **)b;

	return (pokemon_a->id > pokemon_b->id) -
	       (pokemon_a->id < pokemon_b->id);
}

int comparar_por_nombre(const void *a, const void *b)
{
	const struct pokemon *pokemon_a = *(const struct pokemon **)a;
	const struct pokemon *pokemon_b = *(const struct pokemon **)b;

	if (!pokemon_a || !pokemon_a->nombre || !pokemon_b ||
	    !pokemon_b->nombre) {
		return 0;
	}

	return strcmp(pokemon_a->nombre, pokemon_b->nombre);
}

void ordenar_pokemones(struct pokemon **vector, size_t cantidad,
		       int (*comparador)(const void *, const void *))
{
	for (size_t i = 0; i < cantidad - 1; i++) {
		for (size_t j = i + 1; j < cantidad; j++) {
			if (comparador(&vector[i], &vector[j]) > 0) {
				if (!vector[i] || !vector[j]) {
					continue;
				}
				struct pokemon *temp = vector[i];
				vector[i] = vector[j];
				vector[j] = temp;
			}
		}
	}
}

void inicializar_vectores_ordenados(pokedex_t *pokedex)
{
	if (!pokedex || !pokedex->pokemones) {
		return;
	}

	pokedex->ordenados_por_id =
		malloc(sizeof(struct pokemon *) * pokedex->cantidad);
	pokedex->ordenados_por_nombre =
		malloc(sizeof(struct pokemon *) * pokedex->cantidad);

	if (!pokedex->ordenados_por_id || !pokedex->ordenados_por_nombre) {
		return;
	}

	for (size_t i = 0; i < pokedex->cantidad; i++) {
		pokedex->ordenados_por_id[i] = &pokedex->pokemones[i];
		pokedex->ordenados_por_nombre[i] = &pokedex->pokemones[i];
	}

	ordenar_pokemones(pokedex->ordenados_por_id, pokedex->cantidad,
			  comparar_por_id);
	ordenar_pokemones(pokedex->ordenados_por_nombre, pokedex->cantidad,
			  comparar_por_nombre);
}

pokedex_t *pokedex_abrir(const char *archivo)
{
	if (!archivo || strlen(archivo) == 0) {
		return NULL;
	}

	FILE *f = fopen(archivo, "r");
	if (!f) {
		return NULL;
	}

	pokedex_t *pokedex = pokedex_crear();
	if (!pokedex) {
		fclose(f);
		return NULL;
	}

	bool verdadero = true;

	while ((!feof(f)) && verdadero) {
		struct pokemon p = { 0 };

		if (!leer_linea_pokemon(f, &p)) {
			verdadero = false;
		} else if (!pokedex_agregar_pokemon(pokedex, p)) {
			free((void *)p.nombre);
			verdadero = false;
		}
	}

	fclose(f);

	if (pokedex->cantidad > 0) {
		inicializar_vectores_ordenados(pokedex);
	} else {
		pokedex->ordenados_por_id = NULL;
		pokedex->ordenados_por_nombre = NULL;
		pokedex_destruir(pokedex);
		return NULL;
	}

	return pokedex;
}

unsigned pokedex_cantidad_pokemones(pokedex_t *pokedex)
{
	if (!pokedex)
		return 0;
	return (unsigned)pokedex->cantidad;
}

const struct pokemon *pokedex_buscar_pokemon_nombre(pokedex_t *pokedex,
						    const char *nombre)
{
	if (!pokedex || !nombre)
		return NULL;

	bool verdadero = true;
	const struct pokemon *pokemon_encontrado = NULL;
	size_t i = 0;

	while ((i < pokedex->cantidad) && verdadero) {
		if (strcmp(pokedex->pokemones[i].nombre, nombre) == 0) {
			pokemon_encontrado = &pokedex->pokemones[i];
			verdadero = false;
		}
		i++;
	}

	return pokemon_encontrado;
}

const struct pokemon *pokedex_buscar_pokemon_id(pokedex_t *pokedex, unsigned id)
{
	if (!pokedex)
		return NULL;

	bool verdadero = true;
	const struct pokemon *pokemon_encontrado = NULL;
	size_t i = 0;

	while ((i < pokedex->cantidad) && verdadero) {
		if (pokedex->pokemones[i].id == id) {
			pokemon_encontrado = &pokedex->pokemones[i];
			verdadero = false;
		}
		i++;
	}

	return pokemon_encontrado;
}

unsigned pokedex_iterar_pokemones(pokedex_t *pokedex, enum modo_iteracion modo,
				  bool (*funcion)(struct pokemon *, void *),
				  void *ctx)
{
	if (!pokedex || !funcion)
		return 0;

	struct pokemon **vector_ordenado =
		(modo == ITERAR_ID) ? pokedex->ordenados_por_id :
				      pokedex->ordenados_por_nombre;
	if (!vector_ordenado) {
		return 0;
	}

	unsigned iterados = 0;

	for (size_t i = 0; i < pokedex->cantidad; i++) {
		struct pokemon *actual = vector_ordenado[i];
		if (!actual) {
			continue;
		}

		iterados++;
		if (!funcion(actual, ctx)) {
			break;
		}
	}

	return iterados;
}

void pokedex_destruir(pokedex_t *pokedex)
{
	if (!pokedex)
		return;

	for (size_t i = 0; i < pokedex->cantidad; i++) {
		if (pokedex->pokemones[i].nombre) {
			free((void *)pokedex->pokemones[i].nombre);
			pokedex->pokemones[i].nombre = NULL;
		}
	}

	if (pokedex->pokemones) {
		free(pokedex->pokemones);
	}

	if (pokedex->ordenados_por_id) {
		free(pokedex->ordenados_por_id);
	}

	if (pokedex->ordenados_por_nombre) {
		free(pokedex->ordenados_por_nombre);
	}

	free(pokedex);
}
