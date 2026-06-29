#ifndef ARVORE_GENEALOGICA_H
#define ARVORE_GENEALOGICA_H

#include <lib/arvore.h>

/* Funções para acessar parentesco. */

/* Pai */
static inline struct node* _pai(struct node* node){
	return node->pai;
}

/* Avo */
static inline struct node* _avo(struct node* node){
	if(!_pai(node)) return NULL;
	return _pai(node)->pai;
}

/* Tio */
static inline struct node* _tio(struct node* node){
	struct node* avo = _avo(node);
	if(!avo) return NULL;

	if(_pai(node) == avo->direita)
		return avo->esquerda;

	return avo->direita;
}

/* Primo Distante */
static inline struct node* _prD(struct node* node){
	struct node* tio = _tio(node);
	if(!tio) return NULL;

	if(_pai(node)->direita == node)
		return tio->esquerda;

	return tio->direita;
}

/* Primo Proximo */
static inline struct node* _prP(struct node* node){
	struct node* tio = _tio(node);
	if(!tio) return NULL;

	if(_pai(node)->direita == node)
		return tio->direita;

	return tio->esquerda;
}

/* Irmão */
static inline struct node* _irm(struct node* node){
	if(!_pai(node)) return NULL;

	struct node* pai = _pai(node);

	if(node == pai->esquerda)
		return pai->direita;

	return pai->esquerda;
}

/* Subrinho Distante */
static inline struct node* _suD(struct node* node){
	struct node* irmao = _irm(node);
	if(!irmao) return NULL;

	if(_pai(node)->direita == node)
		return irmao->esquerda;

	return irmao->direita;
}

/* Subrinho Proximo */
static inline struct node* _suP(struct node* node){
	struct node* irmao = _irm(node);
	if(!irmao) return NULL;

	if(_pai(node)->direita == node)
		return irmao->direita;

	return irmao->esquerda;
}

#endif

