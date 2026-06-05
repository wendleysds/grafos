#include <lib/grafo.h>
#include <list.h>
#include <stdlib.h>
#include <stdio.h>

static struct grafo grafo;


static inline struct vertice* tenta_criar_vertice(char* vertice_nome){
	struct vertice* vertice = criar_vertice(&grafo, vertice_nome);
	if(!vertice){
		printf("Falha ao alocar vertice \"%s\"!", vertice_nome);
		exit(EXIT_FAILURE);
	}

	return vertice;
}

static inline void tenta_adicionar_aresta(struct vertice *de, struct vertice *para){
	if(grafo_adicionar_aresta(&grafo, de, para, 0)){
		printf("Falha ao adicionar aresta \"%p\" --> \"%p\"!", de, para);
		exit(EXIT_FAILURE);
	}
}

int main(){
	grafo.flags = FLAG_DIRECIONADO;
	INIT_LIST_HEAD(&grafo.vertices);

	struct vertice* verticeA = tenta_criar_vertice("A");
	struct vertice* verticeB = tenta_criar_vertice("B");
	struct vertice* verticeC = tenta_criar_vertice("C");

	tenta_adicionar_aresta(verticeA, verticeB);
	tenta_adicionar_aresta(verticeA, verticeC);

	tenta_adicionar_aresta(verticeB, verticeC);
	tenta_adicionar_aresta(verticeC, verticeA);

	struct list_head* pos;
	struct vertice* vertice;

	list_for_each(pos, &grafo.vertices){
		vertice = list_entry(pos, struct vertice, nos);
		printf("%s", vertice->nome);

		struct list_head* pos_aresta;
		struct aresta* aresta;

		if(list_empty(&vertice->arestas)){
			printf(" --> (vazio)\n");
			continue;
		}

		list_for_each(pos_aresta, &vertice->arestas){
			aresta = list_entry(pos_aresta, struct aresta, nos);
			printf(" --> %s", aresta->para->nome);
		}

		printf("\n");
	}

	return EXIT_SUCCESS;
}