#include <lib/grafo.h>
#include <list.h>
#include <stdlib.h>
#include <string.h>

/*
 * Identificador global utilizado para gerar IDs únicos
 * para cada vértice criado.
 */
static unsigned int next_id = 0;

/*
 * Estrutura principal do grafo.
 *
 * flags:
 *   Configurações do grafo (direcionado, ponderado, etc.).
 *
 * vertices:
 *   Lista contendo todos os vértices pertencentes ao grafo.
 */
struct grafo {
	uint8_t flags;
	struct list_head vertices;
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


/*
 * Cria um novo grafo vazio.
 *
 * Inicializa a lista de vértices e armazena as flags
 * informadas pelo usuário.
 *
 * Retorna:
 *   Ponteiro para o grafo criado.
 *   NULL em caso de falha de alocação.
 */
struct grafo *criar_grafo(uint8_t flags) {
	struct grafo *grafo = malloc(sizeof(struct grafo));

	if (grafo) {
		grafo->flags = flags;
		INIT_LIST_HEAD(&grafo->vertices);
	}

	return grafo;
}

/*
 * Cria um novo vértice e o adiciona ao grafo.
 *
 * O vértice recebe:
 *   - ID único
 *   - Nome fornecido pelo usuário
 *   - Lista de arestas vazia
 *
 * Retorna:
 *   Ponteiro para o vértice criado.
 *   NULL em caso de erro.
 */
struct vertice *criar_vertice(struct grafo *grafo, char vertice_nome[MAXIMO_VERTICE_NOME]) {
	struct vertice *vertice = malloc(sizeof(struct vertice));

	if (vertice) {
		vertice->id = next_id++;

		strncpy(vertice->nome, vertice_nome, MAXIMO_VERTICE_NOME - 1);
		vertice->nome[MAXIMO_VERTICE_NOME - 1] = '\0';

		INIT_LIST_HEAD(&vertice->arestas);
		INIT_LIST_HEAD(&vertice->nos);

		list_add_tail(&vertice->nos, &grafo->vertices);
	}

	return vertice;
}