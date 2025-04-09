#include "src/pokedex.h"
#include <stdio.h>
#include <string.h>

_Bool mostrar_pokemon_y_sumar_fuerza(struct pokemon *pokemon, void *aux) {
    if (!pokemon)
        return false;

    printf("Id: %u | Nombre: %s | Tipo: %d | Fuerza: %u | Destreza: %u | "
           "Inteligencia: %u\n",
           pokemon->id, pokemon->nombre, pokemon->tipo, pokemon->fuerza,
           pokemon->destreza, pokemon->inteligencia);

    if (aux) {
        *(unsigned int *)aux += pokemon->fuerza;
    }

    return true;
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Uso incorrecto: %s <archivo_pokedex.csv>\n", argv[0]);
        return 1;
    }

    pokedex_t *pokedex = pokedex_abrir(argv[1]);
    if (!pokedex) {
        printf("Error al abrir el archivo '%s'.\n", argv[1]);
        return 1;
    }

    printf("La Pokédex tiene %u Pokémon.\n", pokedex_cantidad_pokemones(pokedex));

    unsigned fuerza_total = 0;

    printf("\nPokémon ordenados por Nombre:\n");
    pokedex_iterar_pokemones(pokedex, ITERAR_NOMBRE,
                             mostrar_pokemon_y_sumar_fuerza, &fuerza_total);

    printf("\nFuerza total de todos los Pokémon: %u\n", fuerza_total);

    printf("\nPokémon ordenados por ID:\n");
    pokedex_iterar_pokemones(pokedex, ITERAR_ID, mostrar_pokemon_y_sumar_fuerza,
                             NULL);

    printf("\nBuscando Pokémon por Nombre:\n");
    const struct pokemon *pikachu =
        pokedex_buscar_pokemon_nombre(pokedex, "Pikachu");
    if (pikachu) {
        printf("Pikachu encontrado: ID %u, Tipo %d\n", pikachu->id, pikachu->tipo);
    } else {
        printf("Pikachu no encontrado.\n");
    }

    const struct pokemon *mewtwo = pokedex_buscar_pokemon_nombre(pokedex, "Mewtwo");
    if (mewtwo) {
        printf("ERROR: Se encontró a Mewtwo, pero no debería estar.\n");
    } else {
        printf("Mewtwo no está en la Pokédex, como era esperado.\n");
    }

    printf("\nBuscando Pokémon por ID:\n");
    unsigned id_buscar = 4;
    const struct pokemon *pokemon_id = pokedex_buscar_pokemon_id(pokedex, id_buscar);
    if (pokemon_id) {
        printf("Pokémon con ID %u encontrado: %s\n", pokemon_id->id, pokemon_id->nombre);
    } else {
        printf("No se encontró un Pokémon con ID %u.\n", id_buscar);
    }

    id_buscar = 9999;
    pokemon_id = pokedex_buscar_pokemon_id(pokedex, id_buscar);
    if (!pokemon_id) {
        printf("No hay ningún Pokémon con ID %u, como se esperaba.\n", id_buscar);
    } else {
        printf("ERROR: Se encontró un Pokémon con ID %u, pero no debería estar.\n", id_buscar);
    }

    pokedex_destruir(pokedex);
    return 0;
}
