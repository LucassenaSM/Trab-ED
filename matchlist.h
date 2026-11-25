#ifndef MATCHLIST_H
#define MATCHLIST_H

#include "match.h"

// TAD que representa uma lista encadeada de partidas.
// A lista passa a ser dona das partidas inseridas e libera todas no ml_free.
typedef struct matchlist MatchList;
typedef struct node Node;

// Cria e retorna uma lista encadeada vazia (ou NULL se falhar a alocação).
MatchList *ml_create(void);

// Insere uma partida no final da lista, mantendo a ordem em que foram lidas.
// Não faz nada se ml ou m forem NULL.
void ml_append(MatchList *ml, Match *m);

// Retorna a quantidade de partidas na lista (0 se ml for NULL).
int ml_size(MatchList *ml);

// Retorna a partida na posição i (começando do 0), ou NULL se o índice for inválido.
Match *ml_get(MatchList *ml, int i);

/*
 * Procura e retorna a partida com a maior diferença de gols (goleada).
 * Se houver empate na diferença, retorna a primeira que apareceu na lista.
 * Retorna NULL se a lista estiver vazia.
 */
Match *ml_biggest_victory(MatchList *ml);

// Libera toda a memória da lista: os nós, cada Match armazenada e a struct MatchList.
void ml_free(MatchList *ml);

#endif