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
 * Caso for um grafo não direcionado:
 *
 * A <----> B
 *
 * É criada uma nova estrutura aresta dentro da
 * lista de arestas de A. Caso não direcionado
 * cria-se uma estrutura na lista de arestas de B.
 *
 * Retorna:
 *   0 em caso de sucesso.
 *   1 em caso de falha.
 */
int grafo_adicionar_aresta(struct grafo* grafo, struct vertice* de, struct vertice* para, int peso) {
	struct aresta *aresta = malloc(sizeof(struct aresta));

	if(grafo->flags & FLAG_DIRECIONADO){
		if (aresta) {
			aresta->peso = peso;
			aresta->para = para;

			INIT_LIST_HEAD(&aresta->nos);

			list_add_tail(&aresta->nos, &de->arestas);

			return 0;
		}
	}else{
		/* Para grafos não direcionados, criamos a aresta em ambos os sentidos */
		struct aresta *aresta_reversa = malloc(sizeof(struct aresta));

		if (aresta && aresta_reversa) {
			/* A -> B */
			aresta->peso = peso;
			aresta->para = para;
			INIT_LIST_HEAD(&aresta->nos);
			list_add_tail(&aresta->nos, &de->arestas);

			/* B -> A */
			aresta_reversa->peso = peso;
			aresta_reversa->para = de;
			INIT_LIST_HEAD(&aresta_reversa->nos);
			list_add_tail(&aresta_reversa->nos, &para->arestas);

			return 0;
		}

		if (aresta) {
			free(aresta);
		}

		if (aresta_reversa) {
			free(aresta_reversa);
		}
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
 * E uma conexão(grafo não direcionados):
 *
 * de -> para
 * para -> de
 *
 * Caso encontrada:
 *   - remove da lista
 *   - libera memória
 *
 * Complexidade:
 *   O(E)
 *
 * Complexidade total para grafos não direcionados:
 *   O(E_de + E_para)
 *
 * onde E é o número de arestas de saída
 * do vértice de origem.
 *
 * Retorna:
 *   0 se removida.
 *   1 se não encontrada.
 */
int grafo_remove_aresta(struct grafo* grafo, struct vertice* de, struct vertice* para) {
	struct aresta *aresta;
	struct aresta *tmp;

	if(grafo->flags & FLAG_DIRECIONADO){
		/* Grafos direcionados: remover apenas de -> para */
		list_for_each_entry_safe(aresta, tmp, &de->arestas, nos) {
			if (aresta->para == para) {
				list_remove(&aresta->nos);
				free(aresta);
				return 0;
			}
		}
	}else{
		list_for_each_entry_safe(aresta, tmp, &de->arestas, nos) {
			if (aresta->para == para) {
				list_remove(&aresta->nos);
				free(aresta);
				break;
			}
		}

		list_for_each_entry_safe(aresta, tmp, &para->arestas, nos) {
			if (aresta->para == de) {
				list_remove(&aresta->nos);
				free(aresta);
				return 0;
			}
		}
	}

	return 1;
}