#include <lib/grafo.h>
#include <lib/shell.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static LIST_HEAD(grafos);

static struct grafo* selecionado = NULL;

static inline struct grafo* procurar_grafo(const char* nome){
	struct grafo* grafo;
	list_for_each_entry(grafo, &grafos, lista){
		if(strcmp(grafo->nome, nome) == 0){
			return grafo;
		}
	}

	return NULL;
} 

static int listar_grafos(void){
	struct grafo* pos;

	if(list_empty(&grafos)){
		printf("Nenhum grafo criado! veja \"ajuda grafo\"\n");
		return 1;
	}

	int i = 1;
	printf("Grafos (selecionado, posicao, nome, atributos):\n");
	list_for_each_entry(pos, &grafos, lista){

		if(pos == selecionado){
			printf(" * ");
		}else{
			printf("   ");
		}

		printf("%d: \"%s\"; ", i++, pos->nome);

		if(!(pos->flags & FLAG_PONDERADO)){
			printf("Não ");
		}

		printf("PO; ");

		if(!(pos->flags & FLAG_DIRECIONADO)){
			printf("Não ");
		}

		printf("DI; ");

		if(!(pos->flags & FLAG_MULTIGRAFO)){
			printf("Não ");
		}

		printf("MU; ");

		printf("\n");
	}

	return 0;
}

static int grafo_criar(int argc, char** argv){
	uint8_t flags = 0;
	const char* nome = argv[2];
	struct grafo* grafo;

	list_for_each_entry(grafo, &grafos, lista){
		if(strcmp(grafo->nome, nome) == 0){
			printf("Já possui um grafo com este nome!\n");
			return 1;
		}
	}

	for(int i = 3; i < argc; i++){
		const char* tipo = argv[i];

		if(strcmp(tipo, "PO") == 0){
			flags |= FLAG_PONDERADO;
			continue;
		}

		if(strcmp(tipo, "DI") == 0){
			flags |= FLAG_DIRECIONADO;
			continue;
		}

		if(strcmp(tipo, "MU") == 0){
			flags |= FLAG_MULTIGRAFO;
			continue;
		}

		printf("flag não reconhecida \"%s\"! veja \"ajuda grafo\"\n", tipo);
		return 1;
	}

	grafo = criar_grafo(nome, flags);
	if(!grafo){
		perror("Erro ao alocar grafo!\n");
		return 1;
	}

	printf("grafo \"%s\" criado com sucesso!\n", grafo->nome);

	if(selecionado == NULL){
		printf("Automaticamente selecionado \"%s\", veja \"ajuda grafo\"\n", grafo->nome);
		selecionado = grafo;
	}

	list_add_tail(&grafo->lista, &grafos);

	return 0;
}

static inline int grafo_selecionar(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: grafo selecionar <nome>\n");
		return 1;
	}

	const char* alvo = argv[2];
	struct grafo* grafo;

	if(strcmp(selecionado->nome, alvo) == 0){
		printf("Grafo já selecionado\n");
		return 0;
	}

	grafo = procurar_grafo(alvo);
	if(grafo){
		printf("Selecionado \"%s\"\n", grafo->nome);
		selecionado = grafo;
		return 0;
	}

	printf("Grafo \"%s\" não encontrado!\n", alvo);
	return 1;
}

static inline int grafo_destruir(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: grafo destruir <nome>\n");
		return 1;
	}

	const char* alvo = argv[2];
	struct grafo* grafo;

	list_for_each_entry(grafo, &grafos, lista){
		if(strcmp(grafo->nome, alvo) == 0){
			list_remove(&grafo->lista);

			if(selecionado == grafo){
				printf("Grafo em operação removido! selecione outro\n");
				selecionado = NULL;
			}

			destruir_grafo(grafo);
			printf("Grafo \"%s\" destruido!\n", alvo);
			return 0;
		}
	}

	printf("Grafo \"%s\" não encontrado!\n", alvo);
	return 1;
}

static int vertice_subcomando(int argc, char** argv){
	if(argc < 5){
		printf("Uso: grafo vertice <criar|remover> [nomes]\n");
		return 1;
	}

	const char* operacao = argv[2];

	if(strcmp(operacao, "criar") == 0){
		for(int i = 3; i < argc; i++){
			const char* nome = argv[i];
			struct vertice* vertice = procura_vertice(selecionado, nome);

			if(vertice){
				printf("Vertice \"%s\" ja existe!\n", nome);
				continue;
			}

			if(criar_vertice(selecionado, nome) == NULL){
				printf("Falha ao criar vertice \"%s\"!\n", nome);
				return 1;
			}

			printf("Vertice \"%s\" criado com sucesso!\n", nome);
		}

		return 0;
	}

	if(strcmp(operacao, "remover") == 0){
		for(int i = 3; i < argc; i++){
			const char* nome = argv[i];
			struct vertice* vertice = procura_vertice(selecionado, nome);

			if(!vertice){
				printf("Vertice \"%s\" não encontrada!\n", nome);
				continue;
			}

			destruir_vertice(selecionado, vertice);

			printf("Vertice \"%s\" removido com sucesso!\n", nome);
		}

		return 0;
	}

	printf("Operação \"%s\" nao encontrada! veja \"ajuda grafo\"\n", operacao);
	return 1;
}

static int aresta_subcomando(int argc, char** argv){
	if(argc < 5){
		if(!(selecionado->flags & FLAG_PONDERADO) && argc != 5){
			printf("Uso: grafo aresta <criar|remover> <origem> <destino> <peso?>\n");
			return 1;
		}
	}

	const char* operacao = argv[2];
	const char* de = argv[3];
	const char* para = argv[4];

	struct vertice* de_v = procura_vertice(selecionado, de);
	if(!de_v) {
		printf("Vertice de origem \"%s\" não encontrado!\n", de);
		return 1;
	}

	struct vertice* para_v = procura_vertice(selecionado, para);
	if(!para_v) {
		printf("Vertice de origem \"%s\" não encontrado!\n", de);
		return 1;
	}

	if(strcmp(operacao, "criar") == 0){
		if(argc != 5 && argc != 6){
			printf("Uso: grafo aresta criar <nome de> <nome para> [peso]\n");
			return 1;
		}

		int peso = 0;

		if(argc == 5 && selecionado->flags & FLAG_PONDERADO){
			printf("Peso não fornecido!\n");
			return 1;
		}

		if(argc == 6){
			char* fim;
			peso = strtol(argv[5], &fim, 10);
			if(*fim != '\0'){
				printf("Peso invalido \"%s\"!\n", argv[5]);
				return 1;
			}
		}

		if(grafo_adicionar_aresta(selecionado, de_v, para_v, peso) != 0){
			printf("Falha ao criar aresta de \"%s\" para \"%s\"!\n", de, para);
			return 1;
		}

		printf("Aresta de \"%s\" para \"%s\" criada com sucesso!\n", de, para);
		return 0;
	}

	if(strcmp(operacao, "remover") == 0){
		if(argc != 5){
			printf("Uso: grafo aresta remover <nome de> <nome para>\n");
			return 1;
		}

		if(grafo_remove_aresta(selecionado, de_v, para_v) != 0){
			printf("Falha ao remover aresta de \"%s\" para \"%s\"!\n", de, para);
			return 1;
		}

		printf("Aresta de \"%s\" para \"%s\" removida com sucesso!\n", de, para);
		return 0;
	}

	printf("Operação \"%s\" nao encontrada! veja \"ajuda grafo\"\n", operacao);
	return 1;
}

static int grafo_visualizar(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: grafo visualizar <?nome> <tipo>\n");
		return 1;
	}

	struct grafo* grafo = NULL;
	const char* tipo = NULL;

	if(argc == 3){
		grafo = selecionado;
		tipo = argv[2];
	}else {
		grafo = procurar_grafo(argv[2]);
		tipo = argv[3];
	}

	if(grafo == NULL){
		printf("Grafo não encontrado!");
		return 1;
	}

	if(strcmp(tipo, "vertices") == 0){
		struct vertice* vertice;
		printf("Vertices do grafo \"%s\" (id, nome) \n", grafo->nome);
		list_for_each_entry(vertice, &grafo->vertices, nos){
			printf("  %d; %s\n", vertice->id, vertice->nome);
		}
		return 0;
	}

	if(strcmp(tipo, "lista") == 0){
		mostrar_grafo_lista(grafo);
		return 0;
	}

	if(strcmp(tipo, "matriz") == 0){
		mostrar_grafo_matriz(grafo);
		return 0;
	}

	if(strcmp(tipo, "imagem") == 0){
		return 1;
	}

	printf("Opção \"%s\" nao encontrada! veja \"ajuda grafo\"\n", tipo);
	return 1;
}

static int grafo(int argc, char** argv){
	if(argc < 2){
		return listar_grafos();
	}

	const char* subcomando = argv[1];

	if(strcmp(subcomando, "criar") == 0){
		return grafo_criar(argc, argv);
	}

	if(strcmp(subcomando, "selecionar") == 0){
		return grafo_selecionar(argc, argv);
	}

	if(strcmp(subcomando, "destruir") == 0){
		return grafo_destruir(argc, argv);
	}

	if(strcmp(subcomando, "visualizar") == 0){
		return grafo_visualizar(argc, argv);
	}

	if(selecionado == NULL){
		printf("Nenhum grafo selecionado! veja \"ajuda grafo\"\n");
		return 1;
	}

	if(strcmp(subcomando, "vertice") == 0){
		return vertice_subcomando(argc, argv);
	}

	if(strcmp(subcomando, "aresta") == 0){
		return aresta_subcomando(argc, argv);
	}
	
	/* Atalhos */
	if(strcmp(subcomando, "lista") == 0){
		mostrar_grafo_lista(selecionado);
		return 0;
	}

	if(strcmp(subcomando, "matriz") == 0){
		mostrar_grafo_matriz(selecionado);
		return 0;
	}

	printf("Subcomando \"%s\" não encontrado! veja \"ajuda grafo\"\n", subcomando);
	return 1;
}

REGISTRAR_COMANDO(
	grafo,
	"Principal comando de manipulacao de grafos\n"
	"Uso: grafo <SUBCOMANDO> [ARGUMENTOS]\n"
	"Subcomandos:\n"
	" - criar <nome> {PO} {DI} {MU}\n"
	"    Cria um novo grafo com o nome e atributos, sendo eles:\n"
	"      PO: Ponderado\n"
	"      DI: Direcionado\n"
	"      MU: Multigrafo\n"
	"\n"
	"    Um grafo sem atributos será:\n"
	"      Não ponderado, não direcionado, não suportará multigrafo\n"
	"\n"
	"    Exemplo de grafo direcionado, ponderado, não multigrafo\n"
	"      > grafo criar meugrafo PO DI\n"
	"\n"
	" - visualizar <?nome> <tipo>\n"
	"    Exibe o grafo em um formato, sendo eles:\n"
	"      vertices\n"
	"      lista\n"
	"      matriz\n"
	"      imagem\n"
	"    OBS:\n"
	"       Para opção de imagem deve ter a ferramenta \"Graphviz\" baixado em sua maquina\n"
	"       Caso o nome for vazio, selecionará o atual em operação\n"
	"\n"
	" - selecionar <nome>\n"
	"    Seleciona um grafo para operacoes\n"
	"\n"
	" - destruir <nome>\n"
	"    Remove um grafo existente\n"
	"\n"
	" - vertice <operação> [nomes]\n"
	"    Operações relacionada a vertices, sendo eles\n"
	"      criar\n"
	"      remover\n"
	"\n"
	" - aresta <operação> <origem> <destino> <?peso>\n"
	"    Operações relacionada a aresta, sendo eles\n"
	"      criar\n"
	"      remover\n"
	"\n"
	"Atalhos/Apelidos:\n"
	" > grafo lista --> grafo visualizar <selecioando> lista\n"
	" > grafo matriz --> grafo visualizar <selecioando> matriz\n"
	"\n"
	"Se nenhum subcomando for informado, ele lista os grafos atuais.\n"
	"O grafo selecionado e usado por comandos de vertices e arestas.",
	grafo
);

/*


static int mostrar_grafo(int argc, char** argv){return 1;}
*/