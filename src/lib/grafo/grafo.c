#include <lib/grafo.h>
#include <lib/id.h>
#include <list.h>
#include <stdlib.h>
#include <string.h>

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
struct grafo *criar_grafo(const char nome[MAXIMO_VERTICE_NOME], uint8_t flags) {
	struct grafo *grafo = malloc(sizeof(struct grafo));

	if (grafo) {
		memset(grafo, 0x0, sizeof(struct grafo));
		grafo->flags = flags;
		INIT_LIST_HEAD(&grafo->vertices);
		INIT_LIST_HEAD(&grafo->lista);
	}

	return grafo;
}

/*
 * Destrói todas as vértices de um grafo e sua
 * própria estrutura
 */
void destruir_grafo(struct grafo* grafo){
	struct vertice* vertice;
	struct vertice* tmp;
	list_for_each_entry_safe(vertice, tmp, &grafo->vertices, nos){
		destruir_vertice(grafo, vertice);
	}
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
struct vertice *criar_vertice(struct grafo *grafo, const char vertice_nome[MAXIMO_VERTICE_NOME]) {
	struct vertice *vertice = malloc(sizeof(struct vertice));

	if (vertice) {
		memset(vertice, 0x0, sizeof(struct vertice));
		vertice->id = next_id();

		strncpy(vertice->nome, vertice_nome, MAXIMO_VERTICE_NOME - 1);
		vertice->nome[MAXIMO_VERTICE_NOME - 1] = '\0';

		INIT_LIST_HEAD(&vertice->arestas);
		INIT_LIST_HEAD(&vertice->nos);

		list_add_tail(&vertice->nos, &grafo->vertices);
		grafo->quantidade_vertice++;
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
int grafo_adicionar_aresta(struct grafo *grafo, struct vertice *de, struct vertice *para, int peso) {
	struct aresta *aresta;
	
	if (!(grafo->flags & FLAG_MULTIGRAFO)) {
		struct aresta *tmp;

		list_for_each_entry(tmp, &de->arestas, nos) {
			if (tmp->para == para) {
				return 1;
			}
		}
	}

	if(de == para){
		return 1;
	}

	aresta = malloc(sizeof(*aresta));

	if (!aresta) {
		return 1;
	}

	aresta->peso = peso;
	aresta->para = para;
	INIT_LIST_HEAD(&aresta->nos);
	list_add_tail(&aresta->nos, &de->arestas);

	if (!(grafo->flags & FLAG_DIRECIONADO)) {
		struct aresta *rev = malloc(sizeof(*rev));

		if (!rev) {
			list_remove(&aresta->nos);
			free(aresta);
			return 1;
		}

		rev->peso = peso;
		rev->para = de;
		INIT_LIST_HEAD(&rev->nos);
		list_add_tail(&rev->nos, &para->arestas);
	}

	return 0;
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
	int encontrado = 0;

	list_for_each_entry_safe(aresta, tmp, &de->arestas, nos) {
		if (aresta->para == para) {
			list_remove(&aresta->nos);
			free(aresta);
			encontrado = 1;
			break;
		}
	}

	if(!(grafo->flags & FLAG_DIRECIONADO)){

		if(!encontrado) return 1;
		encontrado = 0;

		/* Grafos não direcionados remove apenas para -> de tambem */
		list_for_each_entry_safe(aresta, tmp, &para->arestas, nos) {
			if (aresta->para == de) {
				list_remove(&aresta->nos);
				free(aresta);
				encontrado = 1;
				break;
			}
		}
	}

	return !encontrado;
}

struct vertice* procura_vertice(struct grafo* grafo, const char vertice_nome[MAXIMO_VERTICE_NOME]){
	struct vertice* vertice;
	list_for_each_entry(vertice, &grafo->vertices, nos){
		if(strncmp(vertice->nome, vertice_nome, MAXIMO_VERTICE_NOME) == 0){
			return vertice;
		}
	}

	return NULL;
}

/*
 * Remove todas as arestas que apontam para o vértice informado.
 *
 * Exemplo:
 *
 * A -> B
 * C -> B
 *
 * Ao isolar B:
 *
 * A -> NULL
 * C -> NULL
 */
static void isola_vertice(struct grafo *grafo, struct vertice *vertice) {
  struct vertice *v;

  list_for_each_entry(v, &grafo->vertices, nos) {
    grafo_remove_aresta(grafo, v, vertice);
  }
}

/*
 * Remove e libera todas as arestas de saída
 * pertencentes ao vértice.
 */
static void libera_todas_arestas(struct vertice *vertice) {
  struct aresta *aresta;
  struct aresta *tmp;

  list_for_each_entry_safe(aresta, tmp, &vertice->arestas, nos) {
    list_remove(&aresta->nos);
    free(aresta);
  }
}

/*
 * Remove um vértice do grafo.
 *
 * Processo:
 *
 * 1. Remove todas as arestas que apontam para ele.
 * 2. Remove todas as suas arestas de saída.
 * 3. Remove o vértice da lista do grafo.
 * 4. Libera a memória do vertice.
 */
void destruir_vertice(struct grafo *grafo, struct vertice *vertice) {
  isola_vertice(grafo, vertice);
  libera_todas_arestas(vertice);

  list_remove(&vertice->nos);

  grafo->quantidade_vertice--;

  free(vertice);
}
