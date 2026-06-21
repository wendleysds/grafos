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
    struct node *pai;
    struct node *direita, *esquerda;

    struct list_head lista;
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
 *
 * lista:
 *   Para uma lista global de árvores.
 */
struct arvore {
    int tipo;
    struct node *raiz;
    struct list_head nos;
	struct list_head lista;
};

struct arvore* criar_arvore_binaria(void);
void destruir_arvore_binaria(struct arvore* arvore);
int adicionar_arvore_binaria(struct arvore* arvore, int valor);

struct node* procurar_arvore_binaria(struct arvore* arvore, int valor);
int remover_arvore_binaria(struct arvore* arvore, int valor);

struct node* subarvore_maior_valor(struct node* subarvore);
struct node* subarvore_menor_valor(struct node* subarvore);

struct node* subarvore_antecessor(struct node* subarvore);
struct node* subarvore_sucessor(struct node* subarvore);

#endif
