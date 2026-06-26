#include <lib/arvore.h>
#include <lib/shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct contexto {
	char* nome;
	struct arvore* arvore;
	struct list_head lista;
};

static struct contexto* selecionado = NULL;

static LIST_HEAD(contextos);

static inline void escreve_no(struct node* no){
	if (no->pai)
		printf("     (%d : %d)\n", no->pai->id, no->pai->valor);
	else
		printf("       NIL\n");

	printf("        |\n");
	printf("        v\n");

	printf("     (%d : %d)\n", no->id, no->valor);

	printf("      /   \\\n");
	printf("     v     v\n");

	if (no->esquerda)
		printf("(%d : %d)", no->esquerda->id, no->esquerda->valor);
	else
		printf("  NULL");

	printf("    ");

	if (no->direita)
		printf("(%d : %d)", no->direita->id, no->direita->valor);
	else
		printf("NULL");

	printf("\n");
}

static struct contexto* contexto_procurar(const char* nome){
	struct contexto* arvore;
	list_for_each_entry(arvore, &contextos, lista){
		if(strcmp(arvore->nome, nome) == 0){
			return arvore;
		}
	}

	return NULL;
}

static int listar_contextos(void){
	struct contexto* pos;

	if(list_empty(&contextos)){
		printf("Nenhuma árvore criada! veja \"ajuda arvore\"\n");
		return 1;
	}

	int i = 1;
	printf("Árvore (selecionado, posicao, nome):\n");
	list_for_each_entry(pos, &contextos, lista){

		if(pos == selecionado){
			printf(" * ");
		}else{
			printf("   ");
		}

		printf("%d: \"%s\"\n", i++, pos->nome);
	}

	return 0;
}

static int criar_arvore(int argc, char** argv){
	if(argc < 3){
		printf("Uso: arvore criar <nome>\n");
		return 1;
	}

	struct contexto* contexto = malloc(sizeof(struct contexto));
	if(!contexto){
		perror("Erro ao alocar árvore!\n");
		return 1;
	}

	struct arvore* arvore = arvore_criar(ARVORE_BINARIA);
	if(!arvore){
		free(contexto);
		perror("Erro ao alocar árvore!\n");
		return 1;
	}

	char* nome = strdup(argv[2]);
	if(!nome){
		free(contexto);
		free(arvore);
		perror("Erro ao alocar árvore!\n");
		return 1;
	}
	
	contexto->nome = nome;
	contexto->arvore = arvore;
	INIT_LIST_HEAD(&contexto->lista);

	list_add(&contexto->lista, &contextos);

	printf("Árvore \"%s\" criada com sucesso!\n", contexto->nome);

	if(selecionado == NULL){
		printf("Automaticamente selecionado \"%s\", veja \"ajuda arvore\"\n", contexto->nome);
		selecionado = contexto;
	}

	return 0;
}

static int selecionar_arvore(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: arvore selecionar <nome>\n");
		return 1;
	}

	const char* alvo = argv[2];
	struct contexto* contexto;

	if(strcmp(selecionado->nome, alvo) == 0){
		printf("Árvore já selecionada\n");
		return 0;
	}

	contexto = contexto_procurar(alvo);
	if(contexto){
		printf("Selecionado \"%s\"\n", contexto->nome);
		selecionado = contexto;
		return 0;
	}

	printf("Árvore \"%s\" não encontrada!\n", alvo);
	return 1;
}

static int destruir_arvore(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: arvore destruir <nome>\n");
		return 1;
	}

	const char* nome = argv[2];
	struct contexto* alvo = contexto_procurar(nome);
	if(!alvo){
		printf("Árvore \"%s\" não encontrada!\n", nome);
		return 1;
	}

	list_remove(&alvo->lista);

	arvore_destruir(alvo->arvore);
	free(alvo->nome);
	free(alvo);

	if(selecionado == alvo){
		printf("Árvore \"%s\" (em operação) destruido!\n", nome);
		selecionado = NULL;
	}else{
		printf("Árvore \"%s\" destruido!\n", nome);
	}

	return 0;
}

static int inserir_arvore(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: arvore inserir [valores]\n");
		return 1;
	}

	for(int i = 2; i < argc; i++){
		const char* valor_str = argv[i];
		int valor;

		if(sscanf(valor_str, "%d", &valor) < 1){
			printf("Falha ao converter \"%s\" para inteiro!\n", valor_str);
			continue;
		}

		int erro = arvore_inserir(selecionado->arvore, valor);
	
		if(erro){
			struct node* tmp = arvore_procurar(selecionado->arvore, valor);
			if(tmp){
				printf("Valor \"%d\" duplicado!\n", valor);
			}else{
				printf("Erro ao inserir o valor na árvore\n");
			}

			continue;
		}

		printf("Valor \"%d\" inserido!\n", valor);
	}

	return 0;
}

static int remover_arvore(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: arvore remover [valores]\n");
		return 1;
	}

	for(int i = 3; i < argc; i++){
		const char* valor_str = argv[i];
		int valor;

		if(sscanf(valor_str, "%d", &valor) < 1){
			printf("Falha ao converter \"%s\" para inteiro!", valor_str);
			continue;
		}

		int erro = arvore_remover(selecionado->arvore, valor);
	
		if(erro){
			printf("Valor \"%d\" não encontrado\n", valor);
			continue;
		}

		printf("Valor \"%d\" removido!\n", valor);
	}

	return 0;
}

static int procurar_na_arvore(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: arvore procurar <valor>\n");
		return 1;
	}

	const char* valor_str = argv[2];
	int valor;

	if(sscanf(valor_str, "%d", &valor) < 1){
		printf("Falha ao converter \"%s\" para inteiro!", valor_str);
		return 1;
	}

	struct node* no = arvore_procurar(selecionado->arvore, valor);

	if(!no){
		printf("Valor \"%d\" não encontrado!", valor);
		return 1;
	}

	printf("Valor encontrado! dados do no (id:valor)\n");
	escreve_no(no);

	return 1;
}

static int visualizar_arvore(int argc, char** argv){
	if(argc < 3) {
		printf("Uso: arvore visualizar <?nome> <formato>\n");
		return 1;
	}

	struct contexto* contexto = NULL;
	const char* tipo = NULL;
	if(argc == 3){
		contexto = selecionado;
		tipo = argv[2];
	}else {
		contexto = contexto_procurar(argv[2]);
		tipo = argv[3];
	}

	if(contexto == NULL){
		printf("Árvore não encontrada!");
		return 1;
	}

	enum tipo_ordem ordem;

	if(strcmp(tipo, "pre-ordem") == 0){
		ordem = PRE_ORDEM;
	}

	else if(strcmp(tipo, "pos-ordem") == 0){
		ordem = POS_ORDEM;
	}

	else if(strcmp(tipo, "em-ordem") == 0){
		ordem = EM_ORDEM;
	}

	else if(strcmp(tipo, "imagem") == 0){
		return 1;
	}

	else {
		printf("Opção \"%s\" nao encontrada! veja \"ajuda arvore\"\n", tipo);
		return 1;
	}

	return arvore_mostrar(contexto->arvore, ordem);
}

static int arvore(int argc, char** argv){
	if(argc < 2){
		listar_contextos();
		return 0;
	}

	const char* subcomando = argv[1];

	if(strcmp(subcomando, "criar") == 0){
		return criar_arvore(argc, argv);
	}

	if(strcmp(subcomando, "selecionar") == 0){
		return selecionar_arvore(argc, argv);
	}

	if(strcmp(subcomando, "destruir") == 0){
		return destruir_arvore(argc, argv);
	}

	if(selecionado == NULL){
		printf("Nenhuma árvore selecionada! veja \"ajuda arvore\"\n");
		return 1;
	}

	if(strcmp(subcomando, "visualizar") == 0){
		return visualizar_arvore(argc, argv);
	}

	if(strcmp(subcomando, "inserir") == 0){
		return inserir_arvore(argc, argv);
	}

	if(strcmp(subcomando, "procurar") == 0){
		return procurar_na_arvore(argc, argv);
	}

	if(strcmp(subcomando, "remover") == 0){
		return remover_arvore(argc, argv);
	}

	printf("Ação \"%s\" não encontrado! veja \"ajuda arvore\"\n", subcomando);
	return 1;
}

REGISTRAR_COMANDO(
	arvore,
	"Principal comando de manipulacao de árvores\n"
	"Uso: grafo <AÇÃO> [ARGUMENTOS]\n"
	"Ações:\n"
	" - criar <nome>\n"
	"    Cria uma nova árvore vazia\n"
	"\n"
	" - selecionar <nome>\n"
	"    Seleciona uma árvore para operacoes\n"
	"\n"
	" - destruir <nome>\n"
	"    Destroi uma árvore existente\n"
	"\n"
	" - visualizar <?nome> <formato>\n"
	"    Exibe a árvore em um formato, sendo eles:\n"
	"      pre-ordem\n"
	"      pos-ordem\n"
	"      em-ordem\n"
	"      imagem\n"
	"    OBS:\n"
	"       Para opção de imagem deve ter a ferramenta \"Graphviz\" baixado em sua maquina\n"
	"       Caso o nome for vazio, selecionará o atual em operação\n"
	"\n"
	" - inserir [valores]\n"
	"    Insere valores na árvore.\n"
	"\n"
	" - proourar <valor>\n"
	"    Procura um valor na árvore. Caso encontrado exibe informações do nó encontrado\n"
	"\n"
	" - remover [valores]\n"
	"    Remove valores na árvore\n"
	"\n"
	"Se nenhuma ação for informada, ele lista os árvore atuais.\n"
	"A árvore selecionado e utilizada pelas ações.",
	arvore
);
