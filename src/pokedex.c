#include "pokedex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct pokedex {
    struct pokemon *pokemones;  // Lista principal para almacenamiento
    size_t cantidad;
    size_t capacidad;
};

#define CAPACIDAD_INICIAL 10
#define MAX_NOMBRE 255 // Tamaño máximo permitido para nombres

bool convertir_tipo(const char *tipo_str, tipo_pokemon *tipo);

bool validar_nombre(const char *nombre) {
    if (!nombre || strlen(nombre) == 0) return false; // Nombre no puede ser nulo o vacío

    bool contiene_letra = false; // Indica si el nombre tiene al menos una letra
    for (size_t i = 0; i < strlen(nombre); i++) {
        if (!isalnum(nombre[i]) && nombre[i] != '-' && nombre[i] != ' ') {
            fprintf(stderr, "Advertencia: Nombre contiene caracteres inválidos: %s\n", nombre);
            return false; // Nombre inválido si contiene caracteres prohibidos
        }
        if (isalpha(nombre[i])) contiene_letra = true; // Detecta si el nombre tiene al menos una letra
    }

    if (!contiene_letra) {
        fprintf(stderr, "Advertencia: Nombre inválido porque no contiene letras: %s\n", nombre);
        return false;
    }

    return true; // Nombre válido
}

bool leer_linea_pokemon(const char *linea, struct pokemon *p) {
    if (!linea || !p) return false;

    char *nombre = malloc(MAX_NOMBRE);
    if (!nombre) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nombre.\n");
        return false;
    }

    char tipo_str[20] = "";

    int leidos = sscanf(linea, "%u;%254[^;];%19[^;];%u;%u;%u",
                        &p->id, nombre, tipo_str, &p->fuerza, &p->destreza, &p->inteligencia);

    // Validación para líneas mal formateadas
    if (leidos != 6) {
        fprintf(stderr, "Advertencia: Campos incompletos o inválidos en la línea: %s\n", linea);
        free(nombre);
        return false;
    }

    // Validación para nombres largos
    if (strlen(nombre) >= MAX_NOMBRE) {
        fprintf(stderr, "Advertencia: Nombre demasiado largo (más de %d caracteres): %s\n", MAX_NOMBRE - 1, nombre);
        free(nombre);
        return false;
    }

    // Validar que el nombre sea alfanumérico y contenga al menos una letra
    if (!validar_nombre(nombre)) {
        free(nombre);
        return false;
    }

    // Validar que el tipo sea válido
    if (!convertir_tipo(tipo_str, &p->tipo)) {
        fprintf(stderr, "Advertencia: Tipo inválido '%s' en la línea: %s\n", tipo_str, linea);
        free(nombre);
        return false;
    }

    // Validación de estadísticas
    if (p->id <= 0 || p->fuerza <= 0 || p->destreza <= 0 || p->inteligencia <= 0 ||
        p->fuerza > 999 || p->destreza > 999 || p->inteligencia > 999) {
        fprintf(stderr, "Advertencia: Estadísticas inválidas en la línea: %s\n", linea);
        free(nombre);
        return false;
    }

    // Asignar el nombre válido
    p->nombre = nombre;
    return true;
}



static pokedex_t *pokedex_crear() {
    pokedex_t *pokedex = malloc(sizeof(pokedex_t));
    if (!pokedex) return NULL;

    pokedex->pokemones = malloc(CAPACIDAD_INICIAL * sizeof(struct pokemon));
    if (!pokedex->pokemones) {
        free(pokedex);
        return NULL;
    }

    pokedex->cantidad = 0;
    pokedex->capacidad = CAPACIDAD_INICIAL;
    return pokedex;
}

bool convertir_tipo(const char *tipo_str, tipo_pokemon *tipo) {
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

bool es_pokemon_valido(const struct pokemon *p) {
    if (!p) return false;

    // Validar que las estadísticas sean válidas (ID, fuerza, destreza e inteligencia mayores a 0)
    if (p->id <= 0 || p->fuerza <= 0 || p->destreza <= 0 || p->inteligencia <= 0) {
        fprintf(stderr, "Advertencia: Pokémon inválido con ID %u (estadísticas negativas o cero).\n", p->id);
        return false;
    }

    // Validar que las estadísticas no excedan el máximo permitido (999)
    if (p->fuerza > 999 || p->destreza > 999 || p->inteligencia > 999) {
        fprintf(stderr, "Advertencia: Pokémon con ID %u tiene estadísticas fuera de rango.\n", p->id);
        return false;
    }

    // Pokémon válido si pasa todas las validaciones
    return true;
}


static bool pokedex_agregar_pokemon(pokedex_t *pokedex, struct pokemon pokemon) {
    if (pokedex->cantidad >= pokedex->capacidad) {
        size_t nueva_capacidad = pokedex->capacidad * 2;
        struct pokemon *nueva_lista = realloc(pokedex->pokemones, nueva_capacidad * sizeof(struct pokemon));
        if (!nueva_lista) return false;

        pokedex->pokemones = nueva_lista;
        pokedex->capacidad = nueva_capacidad;
    }

    pokedex->pokemones[pokedex->cantidad++] = pokemon;
    return true;
}

pokedex_t *pokedex_abrir(const char *archivo) {
    if (!archivo || strlen(archivo) == 0) {
        fprintf(stderr, "Error: Nombre de archivo inválido.\n");
        return NULL;
    }

    FILE *f = fopen(archivo, "r");
    if (!f) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'.\n", archivo);
        return NULL;
    }

    pokedex_t *pokedex = pokedex_crear();
    if (!pokedex) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la Pokédex.\n");
        fclose(f);
        return NULL;
    }

    char linea[512];
    size_t pokemones_cargados = 0;

    while (fgets(linea, sizeof(linea), f)) {
        struct pokemon p;

        // Validar que la línea tenga al menos 6 campos separados por `;`
        int separadores = 0;
        for (size_t i = 0; i < strlen(linea); i++) {
            if (linea[i] == ';') separadores++;
        }
        if (separadores < 5) {
            fprintf(stderr, "Advertencia: Línea con campos insuficientes descartada: %s\n", linea);
            continue; // Ignorar esta línea
        }

        // Intentar leer la línea
        if (!leer_linea_pokemon(linea, &p)) {
            fprintf(stderr, "Advertencia: Línea inválida: %s\n", linea);

            if (pokemones_cargados == 0) {
                pokedex_destruir(pokedex);
                fclose(f);
                return NULL;
            }

            continue; // Ignorar esta línea
        }

        // Validar el Pokémon
        if (!es_pokemon_valido(&p)) {
            fprintf(stderr, "Advertencia: Pokémon inválido descartado.\n");
            if (pokemones_cargados == 0) {
                pokedex_destruir(pokedex);
                fclose(f);
                return NULL;
            }
            continue;
        }

        // Agregar el Pokémon válido
        if (!pokedex_agregar_pokemon(pokedex, p)) {
            fprintf(stderr, "Error: No se pudo agregar el Pokémon.\n");
            pokedex_destruir(pokedex);
            fclose(f);
            return NULL;
        }

        pokemones_cargados++;
    }

    fclose(f);

    if (pokemones_cargados == 0) {
        fprintf(stderr, "Advertencia: La Pokédex está vacía.\n");
        pokedex_destruir(pokedex);
        return NULL;
    }

    return pokedex;
}



unsigned pokedex_cantidad_pokemones(pokedex_t *pokedex) {
    if (!pokedex) return 0;
    return (unsigned)pokedex->cantidad;
}

const struct pokemon *pokedex_buscar_pokemon_nombre(pokedex_t *pokedex, const char *nombre) {
    if (!pokedex || !nombre) return NULL;

    for (size_t i = 0; i < pokedex->cantidad; i++) {
        if (strcmp(pokedex->pokemones[i].nombre, nombre) == 0) {
            return &pokedex->pokemones[i];
        }
    }
    return NULL;
}

const struct pokemon *pokedex_buscar_pokemon_id(pokedex_t *pokedex, unsigned id) {
    if (!pokedex) return NULL;

    for (size_t i = 0; i < pokedex->cantidad; i++) {
        if (pokedex->pokemones[i].id == id) {
            return &pokedex->pokemones[i];
        }
    }
    return NULL;
}

unsigned pokedex_iterar_pokemones(pokedex_t *pokedex, enum modo_iteracion modo,
                                  bool (*funcion)(struct pokemon *, void *),
                                  void *ctx) {
    if (!pokedex || !funcion) return 0;

    unsigned iterados = 0;

    for (size_t i = 0; i < pokedex->cantidad; i++) {
        for (size_t j = i + 1; j < pokedex->cantidad; j++) {
            if ((modo == ITERAR_NOMBRE && strcmp(pokedex->pokemones[i].nombre, pokedex->pokemones[j].nombre) > 0) ||
                (modo == ITERAR_ID && pokedex->pokemones[i].id > pokedex->pokemones[j].id)) {
                struct pokemon temp = pokedex->pokemones[i];
                pokedex->pokemones[i] = pokedex->pokemones[j];
                pokedex->pokemones[j] = temp;
            }
        }
    }

    for (size_t i = 0; i < pokedex->cantidad; i++) {
        iterados++;
        if (!funcion(&pokedex->pokemones[i], ctx))
            break;
    }

    return iterados;
}

void pokedex_destruir(pokedex_t *pokedex) {
    if (!pokedex) return;

    for (size_t i = 0; i < pokedex->cantidad; i++) {
        free((char *)pokedex->pokemones[i].nombre);
    }

    free(pokedex->pokemones);
    free(pokedex);
}