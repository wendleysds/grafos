#ifndef GRAFO_H
#define GRAFO_H

#include <stdint.h>

#define MAXIMO_VERTICE_NOME 32

#define BIT(x) (1UL << x)

/* Flags bitwise */
#define FLAG_PONDERADO   BIT(1)
#define FLAG_DIRECIONADO BIT(2)

struct grafo;
struct vertice;

struct grafo* criar_grafo(uint8_t flags);
void destruir_grafo(struct grafo* grafo);

struct vertice* criar_vertice(struct grafo* grafo, char vertice_nome[MAXIMO_VERTICE_NOME]);
struct vertice* procura_vertice(struct grafo* grafo, char vertice_nome[MAXIMO_VERTICE_NOME]);
void destruir_vertice(struct grafo* grafo, struct vertice* vertice);

int grafo_adicionar_aresta(struct vertice* de, struct vertice* para, int peso);
int grafo_remove_aresta(struct vertice* de, struct vertice* para);

#endif