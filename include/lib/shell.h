#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>
#include <stdnoreturn.h>

/* 
 * Tipo de função que implementa um comando da shell.
 * Recebe os argumentos já tokenizados e retorna um código de saída.
 */
typedef int (*command_func_t)(int argc, char** argv);

/* 
 * Representa um comando registrado na shell.
 * cmd_str: nome do comando usado pelo usuário.
 * help_str: descrição curta exibida em ajuda.
 * handler: função executada quando o comando é chamado.
 */
typedef struct command {
	const char* cmd_str;
	const char* help_str;
	command_func_t handler;
} command_t;

/*
 * Macro para registrar um comando na seção "commands" do binário.
 * O linker gera automaticamente os marcadores __start_commands e __stop_commands
 * que permitem iterar sobre todos os comandos disponíveis em tempo de execução.
 */
#define REGISTRAR_COMANDO(nome, str_ajuda, funcao) \
	static const command_t __cmd_##nome = { .cmd_str = #nome, .help_str = str_ajuda, .handler = funcao }; \
	static const command_t* const __cmd_ptr_##nome \
	__attribute__((section("commands"), used)) = &__cmd_##nome

/* 
 * Início e fim da array de ponteiros para comandos definidos na seção.
 * Estes símbolos são fornecidos pelo linker do gcc.
 */
extern const command_t* const __start_commands[];
extern const command_t* const __stop_commands[];

void noreturn iniciar_shell();
int executar_comando(char* line);

#endif