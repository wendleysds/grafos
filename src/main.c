#include <lib/shell.h>
#include <lib/id.h>
#include <stdlib.h>

int main(){
	reset_id();
	iniciar_shell();
}

static int limpar(int argc, char** argv){
	return system("clear");
}

REGISTRAR_COMANDO(limpar, "Limpa o terminal", limpar);