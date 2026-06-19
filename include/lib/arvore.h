#ifndef ARVORE_H
#define ARVORE_H

#include <list.h>

struct node {
    unsigned int id;
    int valor;
    struct node *pai;
    struct node *direita, *esquerda;

    struct list_head list;
};

struct arvore {
    int tipo;

    struct list_head nos;

    struct node *raiz;
};

struct arvore* criar_arvore_binaria(void);
void destruir_arvore_binaria(struct arvore* arvore);
int adicionar_arvore_binaria(struct arvore* arvore, int valor) ;
struct node* procurar_arvore_binaria(struct arvore* arvore, int valor);
int remover_arvore_binaria(struct arvore* arvore, int valor);

#endif
