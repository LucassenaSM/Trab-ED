#ifndef TEAMLIST_H
#define TEAMLIST_H

#include "team.h"

/*
 * TAD que representa uma lista duplamente encadeada de times.
 * Pode ser usada como lista principal (que libera os times no tl_free) ou como
 * lista auxiliar ordenada compartilhando os mesmos ponteiros (liberada com tl_free_nodes).
 */
typedef struct teamlist TeamList;
typedef struct node Node;

// Cria e retorna uma lista duplamente encadeada vazia (ou NULL se faltar memória).
TeamList *tl_create(void);

// Insere um time no final da lista, mantendo a ordem de inserção.
// Não faz nada se tl ou t forem NULL.
void tl_append(TeamList *tl, Team *t);

/*
 * Insere um time na lista já mantendo a ordem de classificação do campeonato:
 * 1. Maior pontuação (PG)
 * 2. Maior número de vitórias (V)
 * 3. Maior saldo de gols (S)
 * 4. Maior número de gols marcados (GM)
 * 5. Menor ID (desempate final)
 */
void tl_insert_ordered(TeamList *l, Team *t);

// Retorna a quantidade de times na lista (0 se tl for NULL).
int tl_size(TeamList *tl);

// Retorna o time na posição i (começando do 0), ou NULL se o índice for inválido.
Team *tl_get(TeamList *tl, int i);

/*
 * Retorna o time com mais gols marcados (melhor ataque), ou NULL se a lista estiver vazia.
 * Assume que tl já está ordenada pela classificação para desempatar a favor do melhor colocado.
 */
Team *tl_best_attack(TeamList *tl);

/*
 * Retorna o time com menos gols sofridos (melhor defesa), ou NULL se a lista estiver vazia.
 * Assume que tl já está ordenada pela classificação para desempatar a favor do melhor colocado.
 */
Team *tl_best_defense(TeamList *tl);

// Libera a lista, todos os nós e também os structs Team armazenados (usar só na lista principal).
void tl_free(TeamList *tl);

// Libera apenas os nós e a struct da lista, sem dar free nos times (evita double-free em listas auxiliares).
void tl_free_nodes(TeamList *tl);

#endif