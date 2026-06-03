#include <lib/grafo.h>
#include <list.h>

/*
 * Identificador global utilizado para gerar IDs únicos
 * para cada vértice criado.
 */
static unsigned int next_id = 0;

/*
 * Estrutura principal do grafo.
 *
 * flags:
 *   Configurações do grafo (direcionado, ponderado, etc.).
 *
 * vertices:
 *   Lista contendo todos os vértices pertencentes ao grafo.
 */
struct grafo {
  uint8_t flags;
  struct list_head vertices;
};

/*
 * Representa uma aresta saindo de um vértice.
 *
 * peso:
 *   Peso/custo associado à conexão.
 *
 * para:
 *   Vértice de destino.
 *
 * nos:
 *   Nó utilizado para encadear a aresta na lista
 *   de arestas do vértice de origem.
 */
struct aresta {
  int peso;
  struct vertice *para;
  struct list_head nos;
};

/*
 * Representa um vértice do grafo.
 *
 * id:
 *   Identificador único.
 *
 * nome:
 *   Nome legível do vértice.
 *
 * arestas:
 *   Lista de arestas de saída deste vértice.
 *
 * nos:
 *   Nó utilizado para encadear o vértice na lista
 *   global de vértices do grafo.
 */
struct vertice {
  unsigned int id;
  char nome[MAXIMO_VERTICE_NOME];
  struct list_head arestas;
  struct list_head nos;
};
