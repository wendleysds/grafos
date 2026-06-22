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
        INIT_LIST_HEAD(&node->lista);
    }

    return node;
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
static inline void transplantar(struct arvore *arvore, struct node *u, struct node *v) {
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
 * Cria uma nova árvore binária de busca vazia.
 *
 * Retorna:
 *   Ponteiro para a arvore criado.
 *   NULL em caso de falha de alocação.
 */
struct arvore* criar_arvore_binaria(void){
    struct arvore* arvore = malloc(sizeof(struct arvore));
    if(arvore){
        memset(arvore, 0x0, sizeof(struct arvore));
        INIT_LIST_HEAD(&arvore->nos);
    }

    return arvore;
}

/*
 * Destroi a arvore interia, liberando seus nos
 * e a estrutura em si.
 **/
void destruir_arvore_binaria(struct arvore* arvore){
    struct node *tmp, *no;
    list_for_each_entry_safe(no, tmp, &arvore->nos, lista){
        free(no);
    }

    free(arvore);
}

/*
 * Adiciona um novo nó na árvore binária de busca.
 *
 * Oberservações:
 *  - Apenas valores não duplicados são inseridos
 *    caso contrario retorna falha.
 *
 * Retorna:
 *   0 em caso de sucesso.
 *   1 em caso de falha.
 */
int adicionar_arvore_binaria(struct arvore* arvore, int valor) {
    struct node* novo = alloca_no(valor);
    if(!novo) return 1;

    if(!arvore->raiz){
        arvore->raiz = novo;
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

    return 0;
}

/*
 * Procura um nó na árvore binária de busca
 * com o valor correspondente
 *
 * Retorna:
 *   Ponteiro para o nó encontrado.
 *   NULL em caso de não encontrar.
 */
struct node* procurar_arvore_binaria(struct arvore* arvore, int valor){
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
 * Remove um nó da árvore binária de busca.
 *
 * Casos tratados:
 * 1) Nó sem filhos:
 *   - remove diretamente e atualiza o ponteiro do pai.
 * 2) Nó com um filho:
 *   - substitui o nó pelo seu único filho.
 * 3) Nó com dois filhos:
 *   - encontra o sucessor (mínimo da subárvore direita),
 *     se necessário move o sucessor para ocupar o lugar
 *     do seu filho direito, e então substitui o alvo
 *     pelo sucessor.
 *
 * Retorna:
 *   0 em caso de sucesso.
 *   1 se o valor não for encontrado.
 */
int remover_arvore_binaria(struct arvore* arvore, int valor){
    struct node* alvo = procurar_arvore_binaria(arvore, valor);
    if(!alvo) return 1;

    /* Caso 1 e 2: um ou nenhum filho */
    if (!alvo->esquerda) {
        transplantar(arvore, alvo, alvo->direita);
        goto out;
    } else if (!alvo->direita) {
        transplantar(arvore, alvo, alvo->esquerda);
        goto out;
    }

    /* Caso 3: dois filhos */
    /* Encontra manualmente para evitar o comportamento de subir na árvore */
    struct node *sucessor = subarvore_menor_valor(alvo->direita);

    if (sucessor->pai != alvo) {
        /* Mover o sucessor para o lugar do seu filho direito */
        transplantar(arvore, sucessor, sucessor->direita);
        sucessor->direita = alvo->direita;
        if (sucessor->direita) {
            sucessor->direita->pai = sucessor;
        }
    }

    /* Substituir alvo por sucessor */
    transplantar(arvore, alvo, sucessor);
    sucessor->esquerda = alvo->esquerda;
    if (sucessor->esquerda) {
        sucessor->esquerda->pai = sucessor;
    }

out:
    list_remove(&alvo->lista);
    free(alvo);
    return 0;
}

