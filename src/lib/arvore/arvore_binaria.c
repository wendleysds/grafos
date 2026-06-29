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
static int adicionar_arvore_bst(struct arvore* arvore, int valor) {
    struct node* novo = alloca_no(valor);
    if(!novo) return 1;

    if(!arvore->raiz){
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

    return 0;
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
static int remover_arvore_bst(struct arvore* arvore, int valor){
    struct node* alvo = arvore_procurar(arvore, valor);
    if(!alvo) return 1;

    /* Caso 1 e 2: um ou nenhum filho */
    if (!alvo->esquerda) {
        arvore_transplantar(arvore, alvo, alvo->direita);
        goto out;
    } else if (!alvo->direita) {
        arvore_transplantar(arvore, alvo, alvo->esquerda);
        goto out;
    }

    /* Caso 3: dois filhos */
    /* Encontra manualmente para evitar o comportamento de subir na árvore */
    struct node *sucessor = subarvore_menor_valor(alvo->direita);

    if (sucessor->pai != alvo) {
        /* Mover o sucessor para o lugar do seu filho direito */
        arvore_transplantar(arvore, sucessor, sucessor->direita);
        sucessor->direita = alvo->direita;
        if (sucessor->direita) {
            sucessor->direita->pai = sucessor;
        }
    }

    /* Substituir alvo por sucessor */
    arvore_transplantar(arvore, alvo, sucessor);
    sucessor->esquerda = alvo->esquerda;
    if (sucessor->esquerda) {
        sucessor->esquerda->pai = sucessor;
    }

out:
    list_remove(&alvo->lista);
    free(alvo);
    return 0;
}

const struct arvore_operacoes arvore_bst_ops = {
    .inserir = adicionar_arvore_bst,
    .remover = remover_arvore_bst
};

