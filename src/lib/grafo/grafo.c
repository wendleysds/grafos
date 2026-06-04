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

/*
 * Cria uma aresta entre dois vértices.
 *
 * de   -> vértice de origem
 * para -> vértice de destino
 *
 * Exemplo:
 *
 * A ----> B
 *
 * É criada uma nova estrutura aresta dentro da
 * lista de arestas de A.
 *
 * Retorna:
 *   0 em caso de sucesso.
 *   1 em caso de falha.
 */
int grafo_adicionar_aresta(struct vertice *de, struct vertice *para, int peso) {
	struct aresta *aresta = malloc(sizeof(struct aresta));

	if (aresta) {
		aresta->peso = peso;
		aresta->para = para;

		INIT_LIST_HEAD(&aresta->nos);

		list_add_tail(&aresta->nos, &de->arestas);

		return 0;
	}

	return 1;
}

/*
 * Remove uma aresta específica.
 *
 * Procura uma conexão:
 *
 * de -> para
 *
 * Caso encontrada:
 *   - remove da lista
 *   - libera memória
 *
 * Complexidade:
 *   O(E)
 *
 * onde E é o número de arestas de saída
 * do vértice de origem.
 *
 * Retorna:
 *   0 se removida.
 *   1 se não encontrada.
 */
int grafo_remove_aresta(struct vertice *de, struct vertice *para) {
	struct aresta *aresta;
	struct aresta *tmp;

	list_for_each_entry_safe(aresta, tmp, &de->arestas, nos) {
		if (aresta->para == para) {
			list_remove(&aresta->nos);
			free(aresta);
			return 0;
		}
	}

	return 1;
}