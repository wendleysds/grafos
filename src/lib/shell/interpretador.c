#include <lib/shell.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAXIMO_ARGUMENTOS 8

static char* skip_blank(char* p){
	while(*p && isblank(*p)) p++;
	return p;
}

static int separar_argumentos(char* line, char** argv, int max){
	int argc = 0;

	while(*line && argc < max){
		line = skip_blank(line);
		if(*line == '\0') break;

		argv[argc++] = line;

		while(*line && !isblank(*line)) line++;
		if(*line){
			*line = '\0';
			line++;
		}
	}

	return argc;
}

/*
 * Mostra todos os comandos cadastrados no
 * array de ponteiros definidos pelo linker.
 */
static void listar_comandos(){
	for(const command_t* const* cmd = __start_commands;
		cmd < __stop_commands;
		cmd++){

		printf("  %s\n", (*cmd)->cmd_str);
	}
}

/*
 * Procura um comando dentro do array de ponteiros
 * definidos pelo linker.
 *
 * Retorna:
 *   O pointeiro da estrutura do comando.
 *   NULL caso não encontrar.
 */
static const command_t* encontrar_comando(const char* nome){
	for(const command_t* const* cmd = __start_commands;
		cmd < __stop_commands;
		cmd++){
		
		if(strcmp((*cmd)->cmd_str , nome) == 0){
			return *cmd;
		}
	}

	return NULL;
}

/* Analisa e executa a linha de comando fornecida.
 * Retorna:
 *   O código de saída do comando executado
 */
int executar_comando(char* linha){
	char* argv[MAXIMO_ARGUMENTOS];
	int argc = separar_argumentos(linha, argv, MAXIMO_ARGUMENTOS);

	if(argc == 0) return 0;

	const command_t* cmd = encontrar_comando(argv[0]);
	if(!cmd){
		printf("Comando desconhecido: %s\n", argv[0]);
		return 1;
	}

	return cmd->handler(argc, argv);
}

static int ajuda(int argc, char** argv){
	if(argc < 2 || !argv[1]) {
		printf("ajuda [EXPRESSAO]\n");
		printf("Comandos disponíveis:\n");
		listar_comandos();
		return 0;
	}

	const command_t* cmd = encontrar_comando(argv[1]);
	if(!cmd){
		printf("Comando não encontrado: %s\n", argv[1]);
		return 1;
	}

	printf("%s\n", cmd->help_str ? 
		cmd->help_str : 
		"Sem informação de ajudar para esse comando!"
	);

	return 0;
}

REGISTRAR_COMANDO(
	ajuda,
	"Ajuda o usuário mostrando os comandos disponíveis e oque eles fazem\n"
	"Em relação aos argumentos\n"
	"  [...]: lista\n"
	"  <...>: texto/string\n"
	"  {...}: item/opcão/adicional\n"
	"uso: ajuda [EXPRESSAO]",
	ajuda
);
