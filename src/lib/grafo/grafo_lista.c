#include <lib/grafo.h>
#include <stdio.h>

void mostrar_grafo_lista(struct grafo* grafo){
	struct vertice* vertice;
	list_for_each_entry(vertice, &grafo->vertices, nos){
		printf("%s", vertice->nome);

		struct aresta* aresta;
		list_for_each_entry(aresta, &vertice->arestas, nos){

			printf(" --> ");

			if(grafo->flags & FLAG_PONDERADO){
				printf("(%s, %d)", aresta->para->nome, aresta->peso);
			}else{
				printf("%s", aresta->para->nome);
			}
		}

		printf(" --> NULL\n");
	}
}
