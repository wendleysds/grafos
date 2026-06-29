#include "arvore_genealogica.h"
#include <lib/arvore.h>
#include <lib/id.h>
#include <stdlib.h>
#include <string.h>
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
static void rotacao_esquerda(struct arvore* arvore, struct node* x){
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
static void rotacao_direita(struct arvore* arvore, struct node* y){
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
static void correcao_cores_inserir(struct arvore* arvore, struct node* no){
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

			// Continua verificando a partir do avô. 
			// Pois pode ter acontecido uma violação
			no = avo;
			continue;
		}


		/* CasoS 2 e 3 (tio é preto) */

		// Pai está à esquerda do avô.
		if (no->pai == avo->esquerda) {

			/* Caso 2 (Esquerda-Direita) */
			if (no == no->pai->direita) {
				no = no->pai;
				rotacao_esquerda(arvore, no);
			}

			// Caso 3  (Esquerda-Esquerda)
			// Recolore pai e avô e realiza rotação à direita no avô.
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
	struct node* novo = alloca_no(valor);
	if(!novo) return 1;

	if(!arvore->raiz){
		/* Raiz sempre é preta */
		novo->cor = PRETO;
		arvore->raiz = novo;
		list_add(&arvore->nos, &novo->lista);
		return 0;
	}

	struct node *atual = arvore->raiz, *anterior = NULL;
	while(atual){
		anterior = atual;
		if(atual->valor > valor){
			atual = atual->esquerda;
		}else if(valor > atual->valor){
			atual = atual->direita;
		}else{
			/* valor já existe: não inserir duplicatas */
			free(novo);
			return 1;
		}
	}

	if(anterior->valor > valor){
		anterior->esquerda = novo;
	}else{
		anterior->direita = novo;
	}

	novo->pai = anterior;
	list_add(&arvore->nos, &novo->lista);

	correcao_cores_inserir(arvore, novo);
	
	return 0;
}

/*
 * Corrige as propriedades da árvore rubro-negra após a remoção
 * de um nó preto. So para resolver o "duplo preto".
 */
static void correcao_cores_remover(struct arvore* arvore, struct node* x, struct node* parent){
	/*
	 * Enquanto o "duplo preto" não chegar à raiz e
	 * x continuar sendo considerado preto.
	 */
	while(x != arvore->raiz && E_PRETO(x)){
		// CASOS EM QUE x É FILHO ESQUERDO.
		if(parent && parent->esquerda == x){
			struct node* irmao = parent->direita;

			/*
			 * CASO 1
			 *
			 * O irmão é vermelho.
			 */
			if(E_VERMELHO(irmao)){
				irmao->cor = PRETO;
				parent->cor = VERMELHO;
				rotacao_esquerda(arvore, parent);

				// Novo irmão após a rotação.
				irmao = parent->direita;
			}

			/*
			 * CASO 2
			 *
			 * O irmão é preto e ambos os seus filhos são pretos.
			 */
			if(E_PRETO(irmao->esquerda) && E_PRETO(irmao->direita)){
				irmao->cor = VERMELHO;

				// O duplo preto sobe para o pai.
				x = parent;
				parent = x->pai;
			}else{

				/*
				 * CASO 3
				 *
				 * O irmão é preto.
				 * O sobrinho distante é preto.
				 * O sobrinho próximo é vermelho.
				 *
				 * Fazemos uma rotação no irmão para
				 * transformar este caso no Caso 4.
				 */
				if(E_PRETO(irmao->direita)){
					irmao->esquerda->cor = PRETO;
					irmao->cor = VERMELHO;

					rotacao_direita(arvore, irmao);

					irmao = parent->direita;
				}

				/*
				 * CASO 4
				 *
				 * O irmão é preto e o sobrinho distante é vermelho.
				 *
				 * Uma rotação no pai elimina o duplo preto.
				 */
				irmao->cor = parent->cor;
				parent->cor = PRETO;

				if(irmao->direita){
					irmao->direita->cor = PRETO;
				}

				rotacao_esquerda(arvore, parent);

				//  O problema foi finalmente resolvido :D
				x = arvore->raiz;
				break;
			}

		}else{
			/*
			 * CASOS ESPELHADOS
			 *
			 * Todas as operações são simétricas às anteriores,
			 * agora com x sendo filho direito.
			 */

			struct node* irmao =
				parent ? parent->esquerda : NULL;

			/*
			 * CASO 1 (espelhado)
			 */
			if(E_VERMELHO(irmao)){
				irmao->cor = PRETO;
				parent->cor = VERMELHO;
				rotacao_direita(arvore, parent);

				irmao = parent->esquerda;
			}

			/*
			 * CASO 2 (espelhado)
			 */
			if(E_PRETO(irmao->direita) && E_PRETO(irmao->esquerda)){
				irmao->cor = VERMELHO;
				x = parent;
				parent = x->pai;
			}else{
				/*
				 * CASO 3 (espelhado)
				 */
				if(E_PRETO(irmao->esquerda)){
					irmao->direita->cor = PRETO;
					irmao->cor = VERMELHO;

					rotacao_esquerda(arvore, irmao);
					irmao = parent->esquerda;
				}

				/*
				 * CASO 4 (espelhado)
				 */
				irmao->cor = parent->cor;
				parent->cor = PRETO;

				if(irmao->esquerda) {
					irmao->esquerda->cor = PRETO;
				}

				rotacao_direita(arvore, parent);

				x = arvore->raiz;
				break;
			}
		}
	}

	/*
	 * Ao final garantimos que o nó resultante seja preto.
	 *
	 * Isso também cobre o caso em que o duplo preto chegou
	 * até a raiz.
	 */
	if(x){
		x->cor = PRETO;
	}
}

static int remover_arvore_rb(struct arvore* arvore, int valor){
	struct node* alvo = arvore_procurar(arvore, valor);
	if(!alvo) return 1;

	// y será o nó que será fisicamente removido da árvore.
	// Na maioria dos casos y == alvo, exceto quando o alvo possui
	// dois filhos, onde seu sucessor é removido em seu lugar.
	struct node* y = alvo;

	enum cor cor_original = alvo->cor;

	// x será o filho que substituirá y.
	// Ele pode ser NULL.
	struct node* x = NULL;

	// Quando x for NULL não é possível acessar o pai, então o guardamos.
	struct node* parente = NULL;

	/*
	 * CASO 1:
	 * O nó não possui filho esquerdo.
	 *
	 * Basta substituir o nó por seu filho direito.
	 */
	if(!y->esquerda){
		x = y->direita;
		parente = y->pai;
		arvore_transplantar(arvore, y, y->direita);

	/*
	 * CASO 2:
	 * O nó não possui filho direito.
	 *
	 * Situação simétrica ao caso anterior.
	 */
	}else if(!y->direita){

		x = y->esquerda;
		parente = y->pai;

		arvore_transplantar(arvore, y, y->esquerda);

	/*
	 * CASO 3:
	 * O nó possui dois filhos.
	 */
	}else{
		// Encontra o sucessor em ordem.
		y = subarvore_menor_valor(alvo->direita);

		// O sucessor nunca possui filho esquerdo.
		x = y->direita;

		/*
		 * Existem dois cenários:
		 *
		 * 1) O sucessor é o próprio filho direito do alvo.
		 * 2) O sucessor está mais abaixo na árvore.
		 */
		if(y->pai == alvo){
			// O sucessor já está na posição correta.
			// Apenas registramos quem será o pai de x.
			parente = y;
		}else{
			// Remove o sucessor de sua posição original.
			parente = y->pai;
			arvore_transplantar(arvore, y, y->direita);

			// Liga a antiga subárvore direita do alvo
			// ao sucessor.
			y->direita = alvo->direita;
			if(y->direita){
				y->direita->pai = y;
			}
		}

		// Coloca o sucessor exatamente na posição do alvo.
		arvore_transplantar(arvore, alvo, y);

		// Liga a antiga subárvore esquerda do alvo.
		y->esquerda = alvo->esquerda;
		if(y->esquerda){
			y->esquerda->pai = y;
		}

		// O sucessor assume a cor do alvo.
		y->cor = alvo->cor;
	}

	/*
	 * Se um nó VERMELHO foi removido, nenhuma propriedade
	 * da árvore rubro-negra é violada. Caso contrario tem
	 * que resolver.
	 */
	if(cor_original == PRETO){
		correcao_cores_remover(arvore, x, parente);
	}

	list_remove(&alvo->lista);
	free(alvo);

	return 0;
}

const struct arvore_operacoes arvore_rb_ops = {
	.inserir = adicionar_arvore_rb,
	.remover = remover_arvore_rb
};
