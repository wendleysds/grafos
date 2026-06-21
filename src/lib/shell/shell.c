#include <lib/shell.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <list.h>

#define TAMANHO_DA_LINHA 128

static int resultado = 0;

static LIST_HEAD(apelidos);

struct apelito {
	const char* alias;
	const char* cmd;
	struct list_head list;
};

static struct apelito *apelido_find(const char *alias){
	struct apelito *a;

	list_for_each_entry(a, &apelidos, list){
		if(strcmp(a->alias, alias) == 0)
			return a;
	}

	return NULL;
}

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

		// lida com apelidos
		struct apelito *a;
		list_for_each_entry(a, &apelidos, list) {
			size_t alias_len = strlen(a->alias);
			size_t cmd_len   = strlen(a->cmd);

			if (strncmp(buffer, a->alias, alias_len) != 0)
				continue;

			if (buffer[alias_len] != '\0' &&
				buffer[alias_len] != ' ')
				continue;

			char *resto = buffer + alias_len;
			size_t resto_len = strlen(resto);

			if (cmd_len + resto_len + 1 > TAMANHO_DA_LINHA)
				continue; /* não cabe */

			memmove(
				buffer + cmd_len,
				resto,
				resto_len + 1  /* inclui null char */
			);

			memcpy(buffer, a->cmd, cmd_len);
			break;
		}

		resultado = executar_comando(buffer);
	}
}

static int apelido(int argc, char **argv){
	struct apelito *a;

	switch(argc){
	case 1:
		list_for_each_entry(a, &apelidos, list){
			printf("%s='%s'\n", a->alias, a->cmd);
		}
		return 0;

	case 2:
		a = apelido_find(argv[1]);
		if(!a)
			return 1;

		printf("%s='%s'\n", a->alias, a->cmd);
		return 0;

	case 3:
		a = apelido_find(argv[1]);

		if(a){
			char *cmd = strdup(argv[2]);
			if(!cmd)
				return 1;

			free((void *)a->cmd);
			a->cmd = cmd;
			return 0;
		}

		a = malloc(sizeof(*a));
		if(!a)
			return 1;

		a->alias = strdup(argv[1]);
		a->cmd   = strdup(argv[2]);

		if(!a->alias || !a->cmd){
			free((void *)a->alias);
			free((void *)a->cmd);
			free(a);
			return 1;
		}

		list_add_tail(&a->list, &apelidos);
		return 0;

	default:
		return 1;
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

REGISTRAR_COMANDO(
	apelido,
	"Cria um apelido para um comando\n"
	"uso: apelido <nome> <comando>"
	"\n"
	"Casos de uso:\n"
	"   > apelido\n"
	"       lista todos os apelidos\n"
	"   > apelido <nome>\n"
	"       mostra o <comando> do <nome>\n"
	"   > apelido <nome> <comando>\n"
	"       cria/atualiza apelido <nome> com <comando>",
	apelido
);
