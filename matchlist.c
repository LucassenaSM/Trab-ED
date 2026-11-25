#include "matchlist.h"
#include "match.h"
#include <stdlib.h>

// Nó da lista duplamente encadeada
struct node {
    Match *match;
    struct node *next;
    struct node *prev;
};

// Guarda o início, o fim e o tamanho da lista para facilitar inserções no final
struct matchlist {
    Node *first;
    Node *last;
    int size;
};

MatchList *ml_create(void) {
    MatchList *ml = (MatchList *)malloc(sizeof(*ml));
    if (ml == NULL)
        return NULL;

    ml->first = NULL;
    ml->last = NULL;
    ml->size = 0;

    return ml;
}

void ml_append(MatchList *ml, Match *m) {
    if (ml == NULL || m == NULL)
        return;

    Node *new_node = (Node *)malloc(sizeof(*new_node));
    if (new_node == NULL) {
        // Se falhar ao criar o nó, libera a partida para não vazar memória
        match_free(m);
        return;
    }

    new_node->match = m;
    new_node->next = NULL;
    new_node->prev = ml->last;

    // Se a lista já tem elementos, liga o antigo último ao novo; senão, ele é o primeiro
    if (ml->size > 0)
        ml->last->next = new_node;
    else
        ml->first = new_node;

    ml->last = new_node;
    ml->size++;
}

int ml_size(MatchList *ml) {
    return ml != NULL ? ml->size : 0;
}

Match *ml_get(MatchList *ml, int i) {
    if (ml == NULL || i < 0 || i >= ml->size)
        return NULL;

    // Caminha na lista até chegar na posição i
    Node *current = ml->first;
    for (int j = 0; j < i; j++)
        current = current->next;

    return current->match;
}

Match *ml_biggest_victory(MatchList *ml) {
    if (ml == NULL || ml->first == NULL)
        return NULL;

    // Começa assumindo que a primeira partida tem a maior goleada
    Match *best_match = ml->first->match;
    int max_diff = abs(match_golsT1(best_match) - match_golsT2(best_match));

    Node *curr = ml->first->next;
    while (curr != NULL) {
        Match *m = curr->match;
        int diff = abs(match_golsT1(m) - match_golsT2(m));

        // Usa > (e não >=) para manter a primeira partida em caso de empate
        if (diff > max_diff) {
            best_match = m;
            max_diff = diff;
        }

        curr = curr->next;
    }

    return best_match;
}

void ml_free(MatchList *ml) {
    if (ml == NULL)
        return;

    Node *current = ml->first;
    while (current != NULL) {
        // Guarda o próximo nó antes de dar free no atual para não perder o encadeamento
        Node *next = current->next;
        match_free(current->match);
        free(current);
        current = next;
    }

    free(ml);
}