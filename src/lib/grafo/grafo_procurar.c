#include <lib/grafo.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Estrutura de dados Pilha (Stack) para auxiliar na busca.
 * Armazena ponteiros genéricos (void*) para flexibilidade.
 *
 * data:
 *   Array de ponteiros
 *
 * top:
 *   Índice do próximo elemento livre.
 *
 * capacity:
 *   Capacidade total da pilha.
 */
typedef struct {
	void** data;
	size_t top;
	size_t capacity;
} stack_t;

/*
 * Insere um elemento no topo da pilha.
 * Retorna 0 em caso de sucesso, 1 se estiver cheia.
 */
static int push(stack_t *s, void* data) {
	if(s->top >= s->capacity) return 1;

	s->data[s->top++] = data;
	return 0;
}

/*
 * Remove e retorna o elemento do topo da pilha.
 * Retorna NULL se a pilha estiver vazia.
 */
static void* pop(stack_t *s) {
	if(s->top == 0) return NULL;

	return s->data[--s->top];
}

/*
 * Verifica se a pilha está vazia.
 */
static int stack_empty(stack_t *s) {
	return s->top == 0;
}

/*
 * Realiza uma busca em profundidade (Depth-First Search - DFS)
 * a partir de um vértice de origem até encontrar o destino.
 *
 * Utiliza a pilha para explorar os ramos do grafo o mais
 * fundo possível antes de retroceder (backtracking).
 *
 * Complexidade:
 *   O(V + E), onde V é o número de vértices e E o número de arestas.
 *
 * Retorna:
 * 0 se o destino foi encontrado.
 * 1 se o destino não é alcançável.
 * -1 especificamente em stack overflow.
 */
int grafo_buscar_em_profundidade(struct grafo* grafo, struct vertice* origem, struct vertice* destino){
	/* Prepara memória para a pilha baseada no número de vértices */
	void* stack_data[grafo->quantidade_vertice];

	struct vertice* vertice;

	stack_t stack = {
		.data = stack_data,
		.capacity = grafo->quantidade_vertice
	};

	int res;

	/* Inicializa a busca empilhando a origem e marcando como visitado */
	push(&stack, origem);
	origem->visitado = 1;

	while(!stack_empty(&stack)){
		struct vertice* atual = pop(&stack);

		/* Exibe o vértice atual visitado */
		printf("[%d] %s\n", atual->id, atual->nome);

		/* Chegamos? */
		if(atual == destino){
			res = 0;
			goto out;
		}

		/* Itera sobre todos os vizinhos do vértice atual */
		struct aresta* aresta;
		list_for_each_entry(aresta, &atual->arestas, nos) {
		    struct vertice *vizinho = aresta->para;

			/* Se vizinho não foi visitado, marca ele como visitado e empilha para exploração futura */
		    if (!vizinho->visitado) {
				vizinho->visitado = 1;

				/* Verifica se a pilha não estourou após a operação */
				if(push(&stack, vizinho)){
					res = -1;
					goto out;
				};
		    }
		}
	}

	/* Caso o loop termine sem encontrar o destino */
	res = 1;

out:

	/* Reseta a flag 'visitado' de todos os vértices do grafo para
	  permitir futuras buscas no mesmo grafo. */
	list_for_each_entry(vertice, &grafo->vertices, nos){
		vertice->visitado = 0;
	}

	return res;
}
