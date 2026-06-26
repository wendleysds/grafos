#ifndef ARVORE_H
#define ARVORE_H

#include <list.h>

/*
 * Representa um nó da árvore.
 *
 * id:
 *   Identificador único.
 *
 * valor:
 *   Valor único do nó.
 *
 * cor:
 *   Cor do nó.
 *
 * pai:
 *   Pai do nó, caso for nulo ele e a raiz
 *
 * direita/esquerda:
 *   Folhas do nó
 *
 * lista:
 *   Para encadeiar o nó diretamente na
 *   lista da arvore.
 */
struct node {
    unsigned int id;
    int valor;
    int cor;

    struct node *pai;
    struct node *direita, *esquerda;
    struct list_head lista;
};

enum tipo_arvore {
    ARVORE_BINARIA,
    ARVORE_RUBRO_NEGRA
};

/*
 * Estrutura principal da árvore.
 *
 * tipo:
 *   Tipo da árvore(Binaria, RB, B).
 *
 * raiz:
 *   Raiz da árvore.
 *
 * pai:
 *   Pai do nó, caso for nulo ele e a raiz
 *
 * nos:
 *   Nós pertecentes a árvore
 */
struct arvore {
    enum tipo_arvore tipo;
    struct node *raiz;
    const struct arvore_operacoes* ops;
    struct list_head nos;
};

struct arvore_operacoes {
    int (*inserir)(struct arvore*, int);
    int (*remover)(struct arvore*, int);
};

enum tipo_ordem {
    POS_ORDEM,
    PRE_ORDEM,
    EM_ORDEM
};

struct arvore* arvore_criar(enum tipo_arvore tipo);

void arvore_destruir(struct arvore* arvore);
int arvore_inserir(struct arvore* arvore, int valor);
int arvore_remover(struct arvore* arvore, int valor);

struct node* arvore_procurar(struct arvore* arvore, int valor);

struct node* subarvore_maior_valor(struct node* subarvore);
struct node* subarvore_menor_valor(struct node* subarvore);
void arvore_transplantar(struct arvore *arvore, struct node *u, struct node *v);

struct node* subarvore_antecessor(struct node* subarvore);
struct node* subarvore_sucessor(struct node* subarvore);

int arvore_mostrar(struct arvore* arvore, enum tipo_ordem ordem);

#endif
