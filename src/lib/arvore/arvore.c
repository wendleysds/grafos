#include <lib/arvore.h>
#include <lib/id.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

extern const struct arvore_operacoes arvore_bst_ops;

/*
 * Cria uma nova árvore vazia.
 *
 * Retorna:
 *   Ponteiro para a arvore criado.
 *   NULL em caso de falha de alocação.
 */
struct arvore* arvore_criar(enum tipo_arvore tipo){
    struct arvore* arvore = malloc(sizeof(struct arvore));
    if(arvore){
        memset(arvore, 0x0, sizeof(struct arvore));
        arvore->tipo = tipo;
        INIT_LIST_HEAD(&arvore->nos);

        if(tipo == ARVORE_BINARIA)
            arvore->ops = &arvore_bst_ops;
        else {
            free(arvore);
            return NULL;
        }
    }

    return arvore;
}

/*
 * Destroi a árvore interia, liberando seus nos
 * e a estrutura em si.
 */
void arvore_destruir(struct arvore* arvore){
    struct node *tmp, *no;
    list_for_each_entry_safe(no, tmp, &arvore->nos, lista){
        free(no);
    }

    free(arvore);
}

/*
 * Adiciona um novo nó na árvore chamando sua função especifica.
 *
 * Oberservações:
 *  - Apenas valores não duplicados são inseridos
 *    caso contrario retorna falha.
 *
 * Retorna:
 *   0 em caso de sucesso.
 *   1 em caso de falha.
 */
int arvore_inserir(struct arvore* arvore, int valor) {
    if(!arvore->ops) return 1;
    if(!arvore->ops->inserir) return 1;

    if(arvore_procurar(arvore, valor)) return 1;

    return arvore->ops->inserir(arvore, valor);
}

/*
 * Encontra o nó com maior valor indo
 * pela direita da subárvore
 *
 * Retorna:
 *   Nó com maior valor
 *
 * Complexidade:
 *   O(h)
 */
struct node* subarvore_maior_valor(struct node* subarvore){
    struct node *anterior, *atual = subarvore;
    while(atual){
        anterior = atual;
        atual = atual->direita;
    }

    return anterior;
}

/*
 * Encontra o nó com menor valor indo
 * pela esquerda da subárvore
 *
 * Retorna:
 *   Nó com menor valor
 *
 * Complexidade:
 *   O(h)
 */
struct node* subarvore_menor_valor(struct node* subarvore){
    struct node *anterior, *atual = subarvore;
    while(atual){
        anterior = atual;
        atual = atual->esquerda;
    }

    return anterior;
}

/*
 * Encontra o antecessor de um nó na árvore binária de busca.
 *
 * Se a subárvore tiver filho à esquerda, o antecessor é o
 * maior valor da subárvore esquerda (máximo à direita).
 *
 * Caso contrário, sobe-se pelo ancestral de pais até encontrar
 * um ancestre do qual o nó corrente esteja na subárvore direita.
 *
 * Retorno:
 *   Ponteiro para o nó antecessor.
 *   NULL se não houver.
 *
 * Complexidade:
 *   O(N)
 */
struct node* subarvore_antecessor(struct node* subarvore){
    if(subarvore->esquerda){
        /* O predecessor é o maior da subárvore esquerda */
        return subarvore_maior_valor(subarvore->esquerda);
    }else{
        /* Sobe na árvore até encontrar um pai em que o atual seja filho direito */
        struct node* atual = subarvore;
        while(atual->pai){
            if(atual->pai->direita == atual){
                /* O pai é o antecessor */
                return atual->pai;
            }

            atual = atual->pai;
        }
    }

    /* Não existe antecessor (nó mais à esquerda da árvore) */
    return NULL;
}

/*
 * Encontra o sucessor de um nó na árvore binária de busca.
 *
 * Se a subárvore tiver filho à direita, o sucessor é o
 * menor valor da subárvore direita (mínimo à esquerda).
 *
 * Caso contrário, sobe-se pelo ancestral de pais até encontrar
 * um ancestre do qual o nó corrente esteja na subárvore esquerda.
 *
 * Retorno:
 *   Ponteiro para o nó sucessor.
 *   NULL se não houver.
 *
 * Complexidade:
 *   O(N)
 */
struct node* subarvore_sucessor(struct node* subarvore){
    if(subarvore->direita){
        /* O sucessor é o menor da subárvore direita */
        return subarvore_menor_valor(subarvore->direita);
    }else{
        /* Sobe na árvore até encontrar um pai em que o atual seja filho esquerdo */
        struct node* atual = subarvore;
        while(atual->pai){
            if(atual->pai->esquerda == atual){
                /* O pai é o sucessor */
                return atual->pai;
            }

            atual = atual->pai;
        }
    }

    /* Não existe sucessor (nó mais à direita da árvore) */
    return NULL;
}

/*
 * Substitui uma subárvore por outra.
 * u = nó substituído
 * v = nova raiz daquela subárvore
 *
 * De:
 *   pai
 *    |
 *    U
 *
 * Para:
 *   pai
 *    |
 *    V
 *
 * Alguns casos:
 *
 * 1. Se U for a raiz da árvore:
 *   - V torna-se a nova raiz.
 *
 * 2. Se não U for a raiz da árvore:
 *   - Atualiza o ponteiro do pai de U para apontar para V.
 *
 * 3. Se V existir:
 *   - Atualiza o ponteiro pai de V para o antigo pai de U.
 *
 * Complexidade:
 *   O(1)
 */
void arvore_transplantar(struct arvore *arvore, struct node *u, struct node *v) {
    if (!u->pai) {
        arvore->raiz = v;
    } else if (u->pai->direita == u) {
        u->pai->direita = v;
    } else {
        u->pai->esquerda = v;
    }

    if (v)  {
        v->pai = u->pai;
    }
}

/*
 * Procura um nó na árvore com o valor correspondente.
 *
 * Retorna:
 *   Ponteiro para o nó encontrado.
 *   NULL em caso de não encontrar.
 */
struct node* arvore_procurar(struct arvore* arvore, int valor){
    struct node* atual = arvore->raiz;
    while(atual){
        if(atual->valor == valor){
            return atual;
        }

        if(atual->valor > valor){
            atual = atual->esquerda;
        }else{
            atual = atual->direita;
        }
    }

    return NULL;
}

/*
 * Remove um nó da árvore chamando sua função especifica.
 *
 * Retorna:
 *   0 em caso de sucesso.
 *   1 se o valor não for encontrado.
 */
int arvore_remover(struct arvore* arvore, int valor){
    if(!arvore->ops) return 1;
    if(!arvore->ops->inserir) return 1;

    return arvore->ops->remover(arvore, valor);
}

static void pre_ordem(struct node* no){
    if(no){
        printf("%d, ", no->valor);
        pre_ordem(no->esquerda);
        pre_ordem(no->direita);
    }
}

static void pos_ordem(struct node* no){
    if(no){
        pos_ordem(no->esquerda);
        printf("%d, ", no->valor);
        pos_ordem(no->direita);
    }
}

static void em_ordem(struct node* no){
    if(no){
        em_ordem(no->esquerda);
        em_ordem(no->direita);
        printf("%d, ", no->valor);
    }
}

int arvore_mostrar(struct arvore* arvore, enum tipo_ordem ordem){
    printf("[");

    if(!arvore->raiz){
        return 1;
    }

    switch(ordem){
        case POS_ORDEM: 
            pos_ordem(arvore->raiz);
            break;
        case PRE_ORDEM: 
            pre_ordem(arvore->raiz);
            break;
        case EM_ORDEM:
            em_ordem(arvore->raiz);
            break;
    }

    printf("\b\b]\n");
    return 0;
}

