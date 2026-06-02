#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/*
 * container_of(ptr, type, member)
 *
 * Converte um ponteiro para um membro de uma estrutura em um ponteiro
 * para a estrutura que contém esse membro.
 *
 * Exemplo:
 *
 * struct pessoa {
 *     char* nome;
 *     struct list_head list;
 * };
 *
 * struct list_head *node = ...;
 * struct pessoa *task = container_of(node, struct pessoa, list);
 *
 * Funcionamento:
 * 1. Obtém o endereço do membro.
 * 2. Calcula o offset do membro dentro da estrutura.
 * 3. Subtrai esse offset do endereço do membro.
 * 4. O resultado é o endereço da estrutura pai.
 *
 * Esta macro é a base de toda a implementação da lista encadeada.
 */
#define container_of(ptr, type, member) ({                  \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/*
 * list_entry(ptr, type, member)
 *
 * Converte um nó da lista (struct list_head) para a estrutura
 * que o contém.
 *
 * É apenas um alias para container_of() que torna o código
 * mais legível quando usado com listas.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/*
 * list_for_each(pos, head)
 *
 * Itera sobre todos os nós da lista.
 *
 * pos deve ser do tipo struct list_head *.
 *
 * Exemplo:
 *
 * struct list_head *pos;
 *
 * list_for_each(pos, &pessoas) {
 *     ...
 * }
 *
 * Não converte automaticamente para o tipo do objeto armazenado.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/*
 * list_for_each_entry(pos, head, member)
 *
 * Itera diretamente sobre os objetos armazenados na lista.
 *
 * pos deve ser um ponteiro para a estrutura que contém
 * o membro "member".
 *
 * Exemplo:
 *
 * struct pessoa *pessoa;
 *
 * list_for_each_entry(task, &pessoa, list) {
 *     printk("%s\n", pessoa->nome);
 * }
 *
 * Internamente converte cada struct list_head para sua
 * estrutura pai usando list_entry().
 */
#define list_for_each_entry(pos, head, member)                    \
	for (pos = list_entry((head)->next, typeof(*pos), member);    \
		&pos->member != (head);                                   \
		pos = list_entry(pos->member.next, typeof(*pos), member))

/*
 * list_for_each_entry_safe(pos, n, head, member)
 *
 * Variante segura para remoção durante a iteração.
 *
 * O próximo elemento é salvo antecipadamente em "n",
 * permitindo remover o elemento atual sem invalidar
 * a travessia da lista.
 *
 * Exemplo:
 *
 * struct pessoa *pessoa;
 * struct pessoa *tmp;
 *
 * list_for_each_entry_safe(pessoa, tmp, &pessoas, list) {
 *     if (pessoa->nome == NULL)
 *         list_remove(&pessoa->list);
 * }
 *
 * Sem esta versão "safe", remover o elemento atual pode
 * quebrar a iteração.
 */
#define list_for_each_entry_safe(pos, n, head, member)                \
	for (pos = list_entry((head)->next, typeof(*pos), member),        \
		n = list_entry(pos->member.next, typeof(*pos), member);       \
		&pos->member != (head);                                       \
		pos = n, n = list_entry(n->member.next, typeof(*n), member))

/*
 * LIST_HEAD_INIT(name)
 *
 * Inicializador estático para uma lista vazia.
 *
 * A lista é circular, então next e prev apontam
 * para o próprio cabeçalho.
 *
 * Exemplo:
 *
 * struct list_head pessoas = LIST_HEAD_INIT(pessoas);
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/*
 * LIST_HEAD(name)
 *
 * Declara e inicializa uma lista vazia.
 *
 * Exemplo:
 *
 * LIST_HEAD(pessoas);
 */
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/*
 * INIT_LIST_HEAD(ptr)
 *
 * Inicializa dinamicamente uma lista vazia.
 *
 * Deve ser usado quando a lista é alocada em tempo
 * de execução.
 *
 * Exemplo:
 *
 * struct list_head list;
 * INIT_LIST_HEAD(&list);
 */
#define INIT_LIST_HEAD(ptr) \
    do { (ptr)->next = (ptr); (ptr)->prev = (ptr); } while (0)

/*
 * Nó básico da lista duplamente encadeada circular.
 *
 * Esta implementação não armazena dados diretamente.
 * Cada estrutura incorpora um list_head como membro.
 */
struct list_head {
	struct list_head *next, *prev;
};

/*
 * list_add(new, head)
 *
 * Insere "new" logo após "head".
 *
 * Se "head" for o cabeçalho da lista,
 * a inserção ocorre no início.
 *
 * Complexidade: O(1)
 */
static inline void list_add(struct list_head *new, struct list_head *head) {
	new->next = head->next;
	new->prev = head;
	head->next->prev = new;
	head->next = new;
}

/*
 * list_remove(entry)
 *
 * Remove um elemento da lista.
 *
 * Após a remoção, o nó passa a apontar para si mesmo.
 * Isso ajuda a evitar corrupção causada por remoções
 * acidentais repetidas.
 *
 * Complexidade: O(1)
 */
static inline void list_remove(struct list_head *entry) {
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
	entry->next = entry;
	entry->prev = entry;
}

/*
 * list_add_head(new, head)
 *
 * Insere um elemento no início da lista.
 *
 * Equivalente a list_add().
 *
 * Complexidade: O(1)
 */
static inline void list_add_head(struct list_head *new, struct list_head *head){
	list_add(new, head);
}

/*
 * list_add_tail(new, head)
 *
 * Insere um elemento no final da lista.
 *
 * Como a lista é circular, inserir após head->prev
 * significa inserir antes do cabeçalho.
 *
 * Complexidade: O(1)
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head){
	list_add(new, head->prev);
}

/*
 * list_empty(head)
 *
 * Verifica se a lista está vazia.
 *
 * Uma lista vazia possui:
 *
 *     head->next == head
 *
 * Retorna:
 *   1 -> vazia
 *   0 -> possui elementos
 */
static inline int list_empty(struct list_head *head){
	return head->next == head;
}

#endif