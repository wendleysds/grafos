#include <lib/shell.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define TAMANHO_DA_LINHA 128

static int resultado = 0;

/* 
 * Inicia o loop interativo da shell. Esta função não retorna.
 */
void noreturn iniciar_shell(){
	char buffer[TAMANHO_DA_LINHA];

	printf("Escreva \"ajuda\" para listar os comandos disponiveis.\n");

	while(1){
		printf("> ");

		/* Le a entrada do usuario como seguranda de tamanho */
		char* r = fgets(buffer, TAMANHO_DA_LINHA, stdin);

		/* fgets retorna null em caso de erro */
		if(!r){
			if(errno == 0) exit(0);

			perror("Erro ao ler entrada!");
			continue;
		}

		/* substitue a nova linha que o fgtes coloca no final do buffer
			por um caracter nulo */
		int len = strcspn(buffer, "\n");
		buffer[len] = '\0';

		/* caso especial: escreve o codigo de saida do ultimo comando */
		if(strcmp(buffer, "$?") == 0){
			printf("%d\n", resultado);
			continue;
		}

		resultado = executar_comando(buffer);
	}
}

static int echo(int argc, char** argv){
	for(int i = 1; i < argc; i++){
		printf("%s ", argv[i]);
	}

	printf("\n");

	return 0;
}

static int sair(int argc, char** argv){
	exit(0);
}

REGISTRAR_COMANDO(
	exit,
	"Finaliza o programa",
	sair
);

REGISTRAR_COMANDO(
	echo,
	"Escreve os argumentos na saida\n"
	"uso: echo [QUALQUER COISA]",
	echo
);
