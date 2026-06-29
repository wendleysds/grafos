#include "arvore_genealogica.h"
#include <lib/arvore.h>
#include <lib/id.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define E_VERMELHO(no) (no == NULL ? 0 : (no)->cor == VERMELHO)
#define E_PRETO(no) (!E_VERMELHO(no))

/*
 * Cria um novo no.
 *
 * Armazena o valor informadas pelo usuário
 * dentro do novo nó.
 *
 * O nó recebe:
 * - ID único
 * - Cor vermelha
 * - Lista de parentes vazias
 *
 * Retorna:
 * Ponteiro para o nó criado.
 * NULL em caso de falha de alocação.
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


/*
 * Rotaciona a arvore para a esquerda.
 *
 *  x               y
 *   \             / \
 *    y    -->    x   C
 *   / \         / \
 *  B   C       A   B
 */
void rotacao_esquerda(struct arvore* arvore, struct node* x){
	struct node* y = x->direita;

	x->direita = y->esquerda;

	if(y->esquerda){
		y->esquerda->pai = x;
	}

	y->pai = x->pai;

	if(!x->pai){
		arvore->raiz = y;
	}else if(x == x->pai->esquerda){
		x->pai->esquerda = y;
	}else {
		x->pai->direita = y;
	}

	y->esquerda = x;
	x->pai = y;
}

/*
 * Rotaciona a árvore para a direita. (espelho da esquerda)
 *
 *      y           x
 *     /           / \
 *    x    -->    A   y
 *   / \         / \
 *  A   B       B   C
 */
void rotacao_direita(struct arvore* arvore, struct node* y){
	struct node* x = y->esquerda;

	y->esquerda = x->direita;

	if(x->direita){
		x->direita->pai = y;
	}

	x->pai = y->pai;

	if(!y->pai){
		arvore->raiz = x;
	}else if(y == y->pai->direita){
		y->pai->direita = x;
	}else{
		y->pai->esquerda = x;
	}

	x->direita = y;
	y->pai = x;
}

/* Corrige as propriedades da árvore rubro-negra após uma inserção.
  O novo nó é inserido inicialmente como VERMELHO, o que pode gerar
  uma violação caso seu pai também seja vermelho. */
void correcao_cores_inserir(struct arvore* arvore, struct node* no){
	while (E_VERMELHO(no->pai)) {
		struct node* avo = _avo(no);
		struct node* tio = _tio(no);

		/*
		 * Caso 1: O tio também é vermelho.
		 * A solução é apenas recolorir:
		 * - Pai e tio tornam-se PRETOS.
		 * - Avô torna-se VERMELHO.
		 */
		if (E_VERMELHO(tio)) {
			no->pai->cor = PRETO;
			tio->cor = PRETO;
			avo->cor = VERMELHO;

			/* Continua verificando a partir do avô. Pois pode ter 
			   acontecido uma violação*/
			no = avo;
			continue;
		}


		/* CASOS 2 e 3 (tio é preto) */

		/* Pai está à esquerda do avô. */
		if (no->pai == avo->esquerdo) {

			/* Caso 2 (Esquerda-Direita) */
			if (no == no->pai->direita) {
				no = no->pai;
				rotacao_esquerda(arvore, no);
			}

			/* Caso 3  (Esquerda-Esquerda)*/
			/* Recolore pai e avô e realiza rotação à direita no avô. */
			no->pai->cor = PRETO;
			avo->cor = VERMELHO;
			rotacao_direita(arvore, avo);

		} else { /* Pai está à direita do avô. */

			/* Os casos são simétricos aos anteriores.*/
			if (no == no->pai->esquerda) {
				no = no->pai;
				rotacao_direita(arvore, no);
			}

			no->pai->cor = PRETO;
			avo->cor = VERMELHO;
			rotacao_esquerda(arvore, avo);
		}
	}

	/* A raiz é sempre preta.*/
	arvore->raiz->cor = PRETO;
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
