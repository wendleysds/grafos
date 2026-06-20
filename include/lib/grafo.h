#ifndef GRAFO_H
#define GRAFO_H

#include <stdint.h>
#include <list.h>

#define MAXIMO_VERTICE_NOME 32

#define BIT(x) (1UL << x)

/* Flags bitwise */
#define FLAG_PONDERADO   BIT(1)
#define FLAG_DIRECIONADO BIT(2)
#define FLAG_MULTIGRAFO  BIT(3)

/*
 * Estrutura principal do grafo.
 *
 * flags:
 *   Configurações do grafo (direcionado, ponderado, etc.).
 *
 * nome:
 *   Nome do grafo.
 *
 * vertices:
 *   Lista contendo todos os vértices pertencentes ao grafo.
 *
 * lista:
 *   Lista contendo todos os grafos
 */
struct grafo {
	uint8_t flags;
	char nome[MAXIMO_VERTICE_NOME];
	struct list_head vertices;
	struct list_head lista;
};

/*
 * Representa uma aresta saindo de um vértice.
 *
 * peso:
 *   Peso/custo associado à conexão.
 *
 * para:
 *   Vértice de destino.
 *
 * nos:
 *   Nó utilizado para encadear a aresta na lista
 *   de arestas do vértice de origem.
 */
struct aresta {
	int peso;
	struct vertice *para;
	struct list_head nos;
};

/*
 * Representa um vértice do grafo.
 *
 * id:
 *   Identificador único.
 *
 * nome:
 *   Nome legível do vértice.
 *
 * arestas:
 *   Lista de arestas de saída deste vértice.
 *
 * nos:
 *   Nó utilizado para encadear o vértice na lista
 *   global de vértices do grafo.
 */
struct vertice {
	unsigned int id;
	char nome[MAXIMO_VERTICE_NOME];
	struct list_head arestas;
	struct list_head nos;
};

struct grafo* criar_grafo(const char nome[MAXIMO_VERTICE_NOME], uint8_t flags);
void destruir_grafo(struct grafo* grafo);

struct vertice* criar_vertice(struct grafo* grafo, const char vertice_nome[MAXIMO_VERTICE_NOME]);
struct vertice* procura_vertice(struct grafo* grafo, const char vertice_nome[MAXIMO_VERTICE_NOME]);
void destruir_vertice(struct grafo* grafo, struct vertice* vertice);

int grafo_adicionar_aresta(struct grafo* grafo, struct vertice* de, struct vertice* para, int peso);
int grafo_remove_aresta(struct grafo* grafo, struct vertice* de, struct vertice* para);

void mostrar_grafo_lista(struct grafo* grafo);
void mostrar_grafo_matriz(struct grafo* grafo);
void mostrar_grafo_imagem(struct grafo* grafo);

#endif
