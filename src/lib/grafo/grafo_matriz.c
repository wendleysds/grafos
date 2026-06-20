#include <lib/grafo.h>
#include <stdio.h>

void mostrar_grafo_matriz(struct grafo* grafo){
	struct vertice *linha;
	struct vertice *coluna;

	/* Cabeçalho */
	printf("# ");
	list_for_each_entry(linha, &grafo->vertices, nos){
		printf("%s ", linha->nome);
	}
	printf("\n"); 

	list_for_each_entry(linha, &grafo->vertices, nos){
		printf("%s ", linha->nome);

		list_for_each_entry(coluna, &grafo->vertices, nos) {
			int valor = 0;

			struct aresta *aresta;
			list_for_each_entry(aresta, &linha->arestas, nos){
				if(aresta->para == coluna){
					if(grafo->flags & FLAG_PONDERADO)
						valor = aresta->peso;
					else
						valor = 1;
	
					break;
				}
			}

			printf("%d ", valor);
		}

		printf("\n"); 
	}
}
