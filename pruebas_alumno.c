#include "pa2m.h"
#include "src/pokedex.h"
#include <string.h>

#define ARCHIVO_PRUEBA "ejemplos/pokedex.csv"

bool verificar_orden_nombre(struct pokemon *pokemon_actual, void *ctx) {
    struct pokemon **anterior = (struct pokemon **)ctx;

    if (*anterior && strcmp((*anterior)->nombre, pokemon_actual->nombre) > 0) {
        return false;  // No est� en orden alfab�tico.
    }

    *anterior = pokemon_actual;
    return true;  // Sigue en orden.
}


void pokedex_abrir_lee_todos_los_pokemon_y_devuelve_la_cantidad_correcta() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  pa2m_afirmar(pokedex != NULL, "La pokedex no es nula");
  size_t cantidad = pokedex_cantidad_pokemones(pokedex);
  pa2m_afirmar(cantidad == 9,
               "La pokedex tiene 9 pokemones (valor obtenido: %d)", cantidad);
  pokedex_destruir(pokedex);
}

void pokedex_abrir_con_archivo_inexistente_devuelve_NULL() {
  pokedex_t *pokedex = pokedex_abrir("archivo_que_no_existe.csv");
  pa2m_afirmar(pokedex == NULL, "Abrir un archivo inexistente devuelve NULL");
}

void pokedex_buscar_pokemon_existente_por_nombre() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  const struct pokemon *pikachu =
      pokedex_buscar_pokemon_nombre(pokedex, "Pikachu");
  pa2m_afirmar(pikachu != NULL, "Se encuentra a Pikachu en la pokedex");
  pa2m_afirmar(pikachu->id == 25, "Pikachu tiene el ID correcto (25)");
  pokedex_destruir(pokedex);
}

void pokedex_buscar_pokemon_inexistente_por_nombre() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  const struct pokemon *mewtwo =
      pokedex_buscar_pokemon_nombre(pokedex, "Mewtwo");
  pa2m_afirmar(mewtwo == NULL, "Mewtwo no est� en la pokedex");
  pokedex_destruir(pokedex);
}

void pokedex_buscar_pokemon_existente_por_id() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  const struct pokemon *pokemon = pokedex_buscar_pokemon_id(pokedex, 4);
  pa2m_afirmar(pokemon != NULL, "Se encuentra a Charmander en la pokedex");
  pa2m_afirmar(pokemon->id == 4, "Charmander tiene el ID correcto (4)");
  pokedex_destruir(pokedex);
}

void pokedex_buscar_pokemon_inexistente_por_id() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  const struct pokemon *pokemon = pokedex_buscar_pokemon_id(pokedex, 9999);
  pa2m_afirmar(pokemon == NULL, "No hay ning�n Pok�mon con ID 9999");
  pokedex_destruir(pokedex);
}

bool contar_pokemon(struct pokemon *p, void *ctx) {
  (*(int *)ctx)++;
  return true;
}

void pokedex_iterar_pokemon_por_id() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  int contador = 0;
  unsigned iterados = pokedex_iterar_pokemones(pokedex, ITERAR_ID, contar_pokemon, &contador);

  pa2m_afirmar(iterados == 9, "Se iteraron los 9 Pok�mon por ID");
  pa2m_afirmar(contador == 9, "El contador tambi�n registr� 9 Pok�mon");

  pokedex_destruir(pokedex);
}

void pokedex_iterar_pokemon_por_nombre() {
  pokedex_t *pokedex = pokedex_abrir(ARCHIVO_PRUEBA);
  struct pokemon *anterior = NULL;

  unsigned iterados = pokedex_iterar_pokemones(
      pokedex, ITERAR_NOMBRE, verificar_orden_nombre, &anterior);

  pa2m_afirmar(iterados > 0, "Iterar por nombre devuelve mayor a 0");

  pokedex_destruir(pokedex);
}

void pokedex_abrir_ignora_pokemon_con_stats_negativas() {
  pa2m_nuevo_grupo("Pruebas con stats negativas");
  pokedex_t *pokedex = pokedex_abrir("ejemplos/archivo_stats_negativas.csv");
  pa2m_afirmar(pokedex != NULL, "La pok�dex se abre incluso si hay stats negativas.");
  pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 0,
               "No se cargan Pok�mon con stats negativas.");
  pokedex_destruir(pokedex);
}

void pokedex_abrir_ignora_pokemon_con_nombres_invalidos() {
  pa2m_nuevo_grupo("Pruebas con nombres inv�lidos");
  pokedex_t *pokedex = pokedex_abrir("ejemplos/archivo_nombres_invalidos.csv");
  pa2m_afirmar(pokedex != NULL, "La pok�dex se abre incluso si hay nombres inv�lidos.");
  pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 0,
               "No se cargan Pok�mon con nombres no alfanum�ricos.");
  pokedex_destruir(pokedex);
}

void pokedex_abrir_acepta_pokemon_con_nombres_numericos() {
  pa2m_nuevo_grupo("Pruebas con nombres num�ricos");
  pokedex_t *pokedex = pokedex_abrir("ejemplos/archivo_nombres_numericos.csv");
  pa2m_afirmar(pokedex != NULL, "La pok�dex se abre con nombres num�ricos.");
  pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 1,
               "Se carg� un Pok�mon con nombre num�rico.");
  const struct pokemon *pokemon = pokedex_buscar_pokemon_nombre(pokedex, "12345");
  pa2m_afirmar(pokemon != NULL && strcmp(pokemon->nombre, "12345") == 0,
               "El Pok�mon con nombre num�rico se encuentra correctamente.");
  pokedex_destruir(pokedex);
}

void pruebas_archivo_vacio() {
  pa2m_nuevo_grupo("Pruebas con archivo vac�o");
  pokedex_t *pokedex = pokedex_abrir("ejemplos/archivo_vacio.csv");
  pa2m_afirmar(pokedex == NULL,
               "La pok�dex no se carga si el archivo est� vac�o.");
  if (pokedex)
    pokedex_destruir(pokedex);
}

void pruebas_archivo_con_errores() {
  pa2m_nuevo_grupo("Pruebas con archivo con errores");
  pokedex_t *pokedex = pokedex_abrir("ejemplos/archivo_errores.csv");
  pa2m_afirmar(pokedex != NULL,
               "La pok�dex se carga parcialmente si el archivo tiene errores.");
  pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) > 0,
               "Se cargaron Pok�mon v�lidos a pesar de los errores.");
  pokedex_destruir(pokedex);
}

void pruebas_archivo_grande() {
  pa2m_nuevo_grupo("Pruebas con archivo grande");
  pokedex_t *pokedex = pokedex_abrir("ejemplos/archivo_grande.csv");
  pa2m_afirmar(pokedex != NULL,
               "La pok�dex se carga correctamente desde un archivo grande.");
  pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) > 100,
               "La pok�dex contiene m�s de 100 Pok�mon.");
  pokedex_destruir(pokedex);
}

int main() {
  pa2m_nuevo_grupo("Pruebas de apertura de archivo");
  pokedex_abrir_lee_todos_los_pokemon_y_devuelve_la_cantidad_correcta();
  pokedex_abrir_con_archivo_inexistente_devuelve_NULL();
  pruebas_archivo_grande();
  pruebas_archivo_con_errores();
  pruebas_archivo_vacio();

  pa2m_nuevo_grupo("Pruebas de stats y nombres inv�lidos");
  pokedex_abrir_ignora_pokemon_con_stats_negativas();
  pokedex_abrir_ignora_pokemon_con_nombres_invalidos();
  pokedex_abrir_acepta_pokemon_con_nombres_numericos();


  pa2m_nuevo_grupo("Pruebas de b�squeda");
  pokedex_buscar_pokemon_existente_por_nombre();
  pokedex_buscar_pokemon_inexistente_por_nombre();
  pokedex_buscar_pokemon_existente_por_id();
  pokedex_buscar_pokemon_inexistente_por_id();

  pa2m_nuevo_grupo("Pruebas de iteraci�n");
  pokedex_iterar_pokemon_por_id();
  pokedex_iterar_pokemon_por_nombre();
  return pa2m_mostrar_reporte();
}


