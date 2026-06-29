#include "arvore_genealogica.h"
#include <lib/arvore.h>
#include <lib/id.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*
 * Cria um novo no.
 *
 * Armazena o valor informadas pelo usuário
 * dentro do novo nó.
 *
 * O nó recebe:
 *   - ID único
 *   - Cor vermelha
 *   - Lista de parentes vazias
 *
 * Retorna:
 *   Ponteiro para o nó criado.
 *   NULL em caso de falha de alocação.
 */
static inline struct node* alloca_no(int valor){
	struct node* node = malloc(sizeof(struct node));
	if(node){
		memset(node, 0x0, sizeof(struct node));
		node->valor = valor;
		node->id = next_id();
		node->cor = VERMELHO;
		INIT_LIST_HEAD(&node->lista);
	}

	return node;
}

static int adicionar_arvore_rb(struct arvore* arvore, int valor) {
	return -1;
}

static int remover_arvore_rb(struct arvore* arvore, int valor){
	struct node* target = arvore_procurar(arvore, valor);
	if(!target) return 1;

	return -1;
}

const struct arvore_operacoes arvore_rb_ops = {
	.inserir = adicionar_arvore_rb,
	.remover = remover_arvore_rb
};
